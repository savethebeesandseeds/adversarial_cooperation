#include "ac/rps.h"

#include <string.h>

#include <sodium.h>

static const uint8_t ac_rps_protocol_id[AC_COMMITMENT_PROTOCOL_ID_BYTES] = {
    'A', 'C', '-', 'R', 'P', 'S', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

static const uint8_t ac_rps_message_magic[4] = {'A', 'C', 'R', 'P'};
static const uint8_t ac_rps_transcript_magic[4] = {'A', 'C', 'R', 'T'};

enum {
    AC_RPS_TRANSCRIPT_SLOT_ALICE_COMMIT = 0,
    AC_RPS_TRANSCRIPT_SLOT_BOB_COMMIT = 1,
    AC_RPS_TRANSCRIPT_SLOT_ALICE_REVEAL = 2,
    AC_RPS_TRANSCRIPT_SLOT_BOB_REVEAL = 3,
    AC_RPS_TRANSCRIPT_SLOT_ALICE_ABORT = 4,
    AC_RPS_TRANSCRIPT_SLOT_BOB_ABORT = 5
};

typedef struct {
    ac_rps_message_type type;
    ac_rps_role sender;
    ac_rps_role recipient;
    const uint8_t *payload;
    uint16_t payload_length;
} ac_rps_parsed_message;

_Static_assert(
    AC_RPS_TRANSCRIPT_MAX_BYTES ==
        AC_RPS_TRANSCRIPT_HEADER_BYTES +
        AC_RPS_TRANSCRIPT_SLOT_COUNT * AC_RPS_TRANSCRIPT_SLOT_HEADER_BYTES +
        2U * AC_RPS_COMMIT_MESSAGE_BYTES +
        2U * AC_RPS_REVEAL_MESSAGE_BYTES +
        2U * AC_RPS_ABORT_MESSAGE_BYTES,
    "RPS transcript size constant must match its canonical layout");

static void ac_write_u16be(uint8_t out[2], uint16_t value)
{
    out[0] = (uint8_t)(value >> 8);
    out[1] = (uint8_t)value;
}

static void ac_write_u32be(uint8_t out[4], uint32_t value)
{
    out[0] = (uint8_t)(value >> 24);
    out[1] = (uint8_t)(value >> 16);
    out[2] = (uint8_t)(value >> 8);
    out[3] = (uint8_t)value;
}

static uint16_t ac_read_u16be(const uint8_t in[2])
{
    return (uint16_t)(((uint16_t)in[0] << 8) | (uint16_t)in[1]);
}

static uint32_t ac_read_u32be(const uint8_t in[4])
{
    return ((uint32_t)in[0] << 24) |
           ((uint32_t)in[1] << 16) |
           ((uint32_t)in[2] << 8) |
           (uint32_t)in[3];
}

static int ac_rps_role_is_valid(ac_rps_role role)
{
    return role == AC_RPS_ROLE_ALICE || role == AC_RPS_ROLE_BOB;
}

static int ac_rps_move_is_valid(ac_rps_move move)
{
    return move == AC_RPS_MOVE_ROCK ||
           move == AC_RPS_MOVE_PAPER ||
           move == AC_RPS_MOVE_SCISSORS;
}

static int ac_rps_abort_reason_is_valid(ac_rps_abort_reason reason)
{
    return reason == AC_RPS_ABORT_UNSPECIFIED ||
           reason == AC_RPS_ABORT_APPLICATION_REQUEST ||
           reason == AC_RPS_ABORT_PROTOCOL_POLICY;
}

static int ac_rps_bytes_are_all_zero(const uint8_t *bytes, size_t length)
{
    uint8_t aggregate = 0U;
    size_t index;

    for (index = 0U; index < length; ++index) {
        aggregate = (uint8_t)(aggregate | bytes[index]);
    }
    return aggregate == 0U;
}

static int ac_rps_state_is_terminal(ac_rps_state state)
{
    return state == AC_RPS_STATE_COMPLETE ||
           state == AC_RPS_STATE_ABORTED_LOCAL ||
           state == AC_RPS_STATE_ABORTED_PEER ||
           state == AC_RPS_STATE_TIMED_OUT ||
           state == AC_RPS_STATE_FAILED;
}

static ac_status ac_rps_require_initialized(const ac_rps_session *session)
{
    if (session == NULL || session->internal_initialized != 1U) {
        return AC_ERR_ARGUMENT;
    }
    return AC_OK;
}

static ac_rps_role ac_rps_other_role(ac_rps_role role)
{
    return role == AC_RPS_ROLE_ALICE ? AC_RPS_ROLE_BOB : AC_RPS_ROLE_ALICE;
}

static void ac_rps_make_commitment_context(
    const ac_rps_session *session,
    ac_rps_role committer,
    ac_rps_role recipient,
    ac_commitment_context *context)
{
    memset(context, 0, sizeof(*context));
    memcpy(context->protocol_id,
           ac_rps_protocol_id,
           sizeof(context->protocol_id));
    context->protocol_version = AC_RPS_PROTOCOL_VERSION;
    memcpy(context->session_id,
           session->internal_session_id,
           sizeof(context->session_id));
    context->round = session->internal_round;
    context->committer_role = (uint8_t)committer;
    context->recipient_role = (uint8_t)recipient;
    context->payload_type = AC_RPS_COMMITMENT_PAYLOAD_TYPE_MOVE;
}

static int ac_rps_transcript_slot(
    ac_rps_role role,
    ac_rps_message_type type)
{
    if (type == AC_RPS_MESSAGE_COMMIT) {
        return role == AC_RPS_ROLE_ALICE
                   ? AC_RPS_TRANSCRIPT_SLOT_ALICE_COMMIT
                   : AC_RPS_TRANSCRIPT_SLOT_BOB_COMMIT;
    }
    if (type == AC_RPS_MESSAGE_REVEAL) {
        return role == AC_RPS_ROLE_ALICE
                   ? AC_RPS_TRANSCRIPT_SLOT_ALICE_REVEAL
                   : AC_RPS_TRANSCRIPT_SLOT_BOB_REVEAL;
    }
    return role == AC_RPS_ROLE_ALICE
               ? AC_RPS_TRANSCRIPT_SLOT_ALICE_ABORT
               : AC_RPS_TRANSCRIPT_SLOT_BOB_ABORT;
}

static void ac_rps_store_transcript_message(
    ac_rps_session *session,
    ac_rps_role sender,
    ac_rps_message_type type,
    const uint8_t *message,
    size_t message_length)
{
    int slot = ac_rps_transcript_slot(sender, type);

    memcpy(session->internal_transcript_messages[slot],
           message,
           message_length);
    session->internal_transcript_lengths[slot] = (uint16_t)message_length;
    session->internal_transcript_present[slot] = 1U;
}

static void ac_rps_wipe_unrevealed_local_secret(ac_rps_session *session)
{
    if (session->internal_local_reveal_sent == 0U &&
        session->internal_local_secret_cleared == 0U) {
        ac_commitment_nonce_clear(&session->internal_local_nonce);
        sodium_memzero(&session->internal_local_move,
                       sizeof(session->internal_local_move));
        session->internal_local_secret_cleared = 1U;
    }
}

static ac_status ac_rps_fail(ac_rps_session *session, ac_status reason)
{
    session->internal_failure_reason = reason;
    session->internal_state = AC_RPS_STATE_FAILED;
    ac_rps_wipe_unrevealed_local_secret(session);
    return reason;
}

static ac_status ac_rps_session_init_common(
    ac_rps_session *session,
    ac_rps_role local_role,
    ac_rps_move local_move,
    const uint8_t session_id[AC_RPS_SESSION_ID_BYTES],
    uint32_t round,
    const ac_commitment_nonce *fixed_nonce)
{
    ac_rps_session candidate;
    ac_commitment_context context;
    uint8_t payload;
    ac_status status;

    if (session == NULL || session_id == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (!ac_rps_role_is_valid(local_role) ||
        !ac_rps_move_is_valid(local_move)) {
        return AC_ERR_ARGUMENT;
    }
    if (round == 0U ||
        ac_rps_bytes_are_all_zero(session_id, AC_RPS_SESSION_ID_BYTES)) {
        return AC_ERR_CONTEXT;
    }

    memset(&candidate, 0, sizeof(candidate));
    memcpy(candidate.internal_session_id,
           session_id,
           sizeof(candidate.internal_session_id));
    candidate.internal_round = round;
    candidate.internal_local_role = local_role;
    candidate.internal_peer_role = ac_rps_other_role(local_role);
    candidate.internal_local_move = local_move;
    candidate.internal_peer_move = AC_RPS_MOVE_ROCK;
    candidate.internal_state = AC_RPS_STATE_READY;
    candidate.internal_failure_reason = AC_OK;
    candidate.internal_abort_reason = AC_RPS_ABORT_UNSPECIFIED;
    candidate.internal_initialized = 1U;

    ac_rps_make_commitment_context(
        &candidate, local_role, candidate.internal_peer_role, &context);
    payload = (uint8_t)local_move;
    if (fixed_nonce == NULL) {
        status = ac_commitment_create(
            &context,
            &payload,
            sizeof(payload),
            &candidate.internal_local_commitment,
            &candidate.internal_local_nonce);
    } else {
        memcpy(&candidate.internal_local_nonce,
               fixed_nonce,
               sizeof(candidate.internal_local_nonce));
        status = ac_commitment_compute_with_nonce(
            &context,
            &payload,
            sizeof(payload),
            &candidate.internal_local_nonce,
            &candidate.internal_local_commitment);
    }
    sodium_memzero(&context, sizeof(context));
    sodium_memzero(&payload, sizeof(payload));
    if (status != AC_OK) {
        sodium_memzero(&candidate, sizeof(candidate));
        return status;
    }

    sodium_memzero(session, sizeof(*session));
    memcpy(session, &candidate, sizeof(*session));
    sodium_memzero(&candidate, sizeof(candidate));
    return AC_OK;
}

ac_status ac_rps_session_init(
    ac_rps_session *session,
    ac_rps_role local_role,
    ac_rps_move local_move,
    const uint8_t session_id[AC_RPS_SESSION_ID_BYTES],
    uint32_t round)
{
    return ac_rps_session_init_common(
        session, local_role, local_move, session_id, round, NULL);
}

ac_status ac_rps_session_init_with_nonce_for_test(
    ac_rps_session *session,
    ac_rps_role local_role,
    ac_rps_move local_move,
    const uint8_t session_id[AC_RPS_SESSION_ID_BYTES],
    uint32_t round,
    const ac_commitment_nonce *nonce)
{
    if (nonce == NULL) {
        return AC_ERR_ARGUMENT;
    }
    return ac_rps_session_init_common(
        session, local_role, local_move, session_id, round, nonce);
}

static size_t ac_rps_encode_message(
    const ac_rps_session *session,
    ac_rps_message_type type,
    const uint8_t *payload,
    uint16_t payload_length,
    uint8_t *out)
{
    memcpy(out, ac_rps_message_magic, sizeof(ac_rps_message_magic));
    ac_write_u16be(out + 4U, AC_RPS_PROTOCOL_VERSION);
    memcpy(out + 6U,
           session->internal_session_id,
           AC_RPS_SESSION_ID_BYTES);
    ac_write_u32be(out + 38U, session->internal_round);
    out[42] = (uint8_t)session->internal_local_role;
    out[43] = (uint8_t)session->internal_peer_role;
    out[44] = (uint8_t)type;
    ac_write_u16be(out + 45U, payload_length);
    if (payload_length != 0U) {
        memcpy(out + AC_RPS_MESSAGE_HEADER_BYTES, payload, payload_length);
    }
    return AC_RPS_MESSAGE_HEADER_BYTES + (size_t)payload_length;
}

static ac_status ac_rps_prepare_output(
    const ac_rps_session *session,
    uint8_t *out,
    size_t out_capacity,
    size_t required,
    size_t *out_length)
{
    ac_status status;

    if (out_length == NULL) {
        return AC_ERR_ARGUMENT;
    }
    *out_length = 0U;
    status = ac_rps_require_initialized(session);
    if (status != AC_OK || out == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (out_capacity < required) {
        return AC_ERR_CAPACITY;
    }
    if (ac_rps_state_is_terminal(session->internal_state)) {
        return AC_ERR_TERMINAL;
    }
    return AC_OK;
}

ac_status ac_rps_make_commit(
    ac_rps_session *session,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_length)
{
    uint8_t encoded[AC_RPS_COMMIT_MESSAGE_BYTES];
    size_t encoded_length;
    ac_status status;

    status = ac_rps_prepare_output(
        session, out, out_capacity, sizeof(encoded), out_length);
    if (status != AC_OK) {
        return status;
    }
    if (session->internal_local_commit_sent != 0U ||
        (session->internal_state != AC_RPS_STATE_READY &&
         session->internal_state != AC_RPS_STATE_COMMIT_RECEIVED)) {
        return AC_ERR_STATE;
    }

    encoded_length = ac_rps_encode_message(
        session,
        AC_RPS_MESSAGE_COMMIT,
        session->internal_local_commitment.bytes,
        AC_COMMITMENT_DIGEST_BYTES,
        encoded);
    memcpy(out, encoded, encoded_length);
    ac_rps_store_transcript_message(
        session,
        session->internal_local_role,
        AC_RPS_MESSAGE_COMMIT,
        encoded,
        encoded_length);
    session->internal_local_commit_sent = 1U;
    session->internal_state = session->internal_peer_commit_received != 0U
                                  ? AC_RPS_STATE_BOTH_COMMITTED
                                  : AC_RPS_STATE_COMMIT_SENT;
    *out_length = encoded_length;
    sodium_memzero(encoded, sizeof(encoded));
    return AC_OK;
}

ac_status ac_rps_make_reveal(
    ac_rps_session *session,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_length)
{
    uint8_t payload[1U + AC_COMMITMENT_NONCE_BYTES];
    uint8_t encoded[AC_RPS_REVEAL_MESSAGE_BYTES];
    size_t encoded_length;
    ac_status status;

    status = ac_rps_prepare_output(
        session, out, out_capacity, sizeof(encoded), out_length);
    if (status != AC_OK) {
        return status;
    }
    if (session->internal_local_reveal_sent != 0U ||
        (session->internal_state != AC_RPS_STATE_BOTH_COMMITTED &&
         session->internal_state != AC_RPS_STATE_REVEAL_RECEIVED)) {
        return AC_ERR_STATE;
    }
    if (session->internal_local_secret_cleared != 0U) {
        return AC_ERR_STATE;
    }

    payload[0] = (uint8_t)session->internal_local_move;
    memcpy(payload + 1U,
           session->internal_local_nonce.bytes,
           AC_COMMITMENT_NONCE_BYTES);
    encoded_length = ac_rps_encode_message(
        session,
        AC_RPS_MESSAGE_REVEAL,
        payload,
        sizeof(payload),
        encoded);
    memcpy(out, encoded, encoded_length);
    ac_rps_store_transcript_message(
        session,
        session->internal_local_role,
        AC_RPS_MESSAGE_REVEAL,
        encoded,
        encoded_length);
    session->internal_local_reveal_sent = 1U;
    session->internal_state = session->internal_peer_reveal_received != 0U
                                  ? AC_RPS_STATE_COMPLETE
                                  : AC_RPS_STATE_REVEAL_SENT;
    *out_length = encoded_length;
    sodium_memzero(payload, sizeof(payload));
    sodium_memzero(encoded, sizeof(encoded));
    return AC_OK;
}

ac_status ac_rps_make_abort(
    ac_rps_session *session,
    ac_rps_abort_reason reason,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_length)
{
    uint8_t payload;
    uint8_t encoded[AC_RPS_ABORT_MESSAGE_BYTES];
    size_t encoded_length;
    ac_status status;

    status = ac_rps_prepare_output(
        session, out, out_capacity, sizeof(encoded), out_length);
    if (status != AC_OK) {
        return status;
    }
    if (!ac_rps_abort_reason_is_valid(reason)) {
        return AC_ERR_ARGUMENT;
    }

    payload = (uint8_t)reason;
    encoded_length = ac_rps_encode_message(
        session,
        AC_RPS_MESSAGE_ABORT,
        &payload,
        sizeof(payload),
        encoded);
    memcpy(out, encoded, encoded_length);
    ac_rps_store_transcript_message(
        session,
        session->internal_local_role,
        AC_RPS_MESSAGE_ABORT,
        encoded,
        encoded_length);
    session->internal_abort_reason = reason;
    session->internal_state = AC_RPS_STATE_ABORTED_LOCAL;
    ac_rps_wipe_unrevealed_local_secret(session);
    *out_length = encoded_length;
    sodium_memzero(encoded, sizeof(encoded));
    return AC_OK;
}

static ac_status ac_rps_parse_message(
    const ac_rps_session *session,
    const uint8_t *message,
    size_t message_length,
    ac_rps_parsed_message *parsed)
{
    uint16_t wire_version;
    uint32_t round;
    uint16_t payload_length;
    ac_rps_role sender;
    ac_rps_role recipient;
    ac_rps_message_type type;
    size_t required_length;

    if (message == NULL || parsed == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (message_length < AC_RPS_MESSAGE_HEADER_BYTES ||
        message_length > AC_RPS_MESSAGE_MAX_BYTES) {
        return AC_ERR_LENGTH;
    }
    if (memcmp(message,
               ac_rps_message_magic,
               sizeof(ac_rps_message_magic)) != 0) {
        return AC_ERR_FORMAT;
    }

    wire_version = ac_read_u16be(message + 4U);
    if (wire_version != (uint16_t)AC_RPS_PROTOCOL_VERSION) {
        return AC_ERR_FORMAT;
    }
    if (ac_rps_bytes_are_all_zero(message + 6U,
                                  AC_RPS_SESSION_ID_BYTES)) {
        return AC_ERR_FORMAT;
    }
    round = ac_read_u32be(message + 38U);
    if (round == 0U) {
        return AC_ERR_FORMAT;
    }

    sender = (ac_rps_role)message[42];
    recipient = (ac_rps_role)message[43];
    type = (ac_rps_message_type)message[44];
    payload_length = ac_read_u16be(message + 45U);
    if (!ac_rps_role_is_valid(sender) ||
        !ac_rps_role_is_valid(recipient) ||
        sender == recipient) {
        return AC_ERR_FORMAT;
    }
    if (type != AC_RPS_MESSAGE_COMMIT &&
        type != AC_RPS_MESSAGE_REVEAL &&
        type != AC_RPS_MESSAGE_ABORT) {
        return AC_ERR_FORMAT;
    }

    required_length = AC_RPS_MESSAGE_HEADER_BYTES + (size_t)payload_length;
    if (required_length != message_length) {
        return AC_ERR_LENGTH;
    }
    if ((type == AC_RPS_MESSAGE_COMMIT &&
         payload_length != (uint16_t)AC_COMMITMENT_DIGEST_BYTES) ||
        (type == AC_RPS_MESSAGE_REVEAL &&
         payload_length != (uint16_t)(1U + AC_COMMITMENT_NONCE_BYTES)) ||
        (type == AC_RPS_MESSAGE_ABORT && payload_length != 1U)) {
        return AC_ERR_LENGTH;
    }

    if (memcmp(message + 6U,
               session->internal_session_id,
               AC_RPS_SESSION_ID_BYTES) != 0 ||
        round != session->internal_round ||
        sender != session->internal_peer_role ||
        recipient != session->internal_local_role) {
        return AC_ERR_CONTEXT;
    }

    if (type == AC_RPS_MESSAGE_REVEAL &&
        !ac_rps_move_is_valid((ac_rps_move)message[47])) {
        return AC_ERR_FORMAT;
    }
    if (type == AC_RPS_MESSAGE_ABORT &&
        !ac_rps_abort_reason_is_valid((ac_rps_abort_reason)message[47])) {
        return AC_ERR_FORMAT;
    }

    parsed->type = type;
    parsed->sender = sender;
    parsed->recipient = recipient;
    parsed->payload = message + AC_RPS_MESSAGE_HEADER_BYTES;
    parsed->payload_length = payload_length;
    return AC_OK;
}

static ac_status ac_rps_receive_commit(
    ac_rps_session *session,
    const ac_rps_parsed_message *parsed,
    const uint8_t *message,
    size_t message_length)
{
    int same_digest;

    if (session->internal_peer_commit_received != 0U) {
        same_digest = sodium_memcmp(
            session->internal_peer_commitment.bytes,
            parsed->payload,
            AC_COMMITMENT_DIGEST_BYTES) == 0;
        return ac_rps_fail(
            session, same_digest ? AC_ERR_DUPLICATE : AC_ERR_CONFLICT);
    }
    if (session->internal_state != AC_RPS_STATE_READY &&
        session->internal_state != AC_RPS_STATE_COMMIT_SENT) {
        return ac_rps_fail(session, AC_ERR_STATE);
    }

    memcpy(session->internal_peer_commitment.bytes,
           parsed->payload,
           AC_COMMITMENT_DIGEST_BYTES);
    ac_rps_store_transcript_message(
        session,
        parsed->sender,
        parsed->type,
        message,
        message_length);
    session->internal_peer_commit_received = 1U;
    session->internal_state = session->internal_local_commit_sent != 0U
                                  ? AC_RPS_STATE_BOTH_COMMITTED
                                  : AC_RPS_STATE_COMMIT_RECEIVED;
    return AC_OK;
}

static ac_status ac_rps_receive_reveal(
    ac_rps_session *session,
    const ac_rps_parsed_message *parsed,
    const uint8_t *message,
    size_t message_length)
{
    ac_commitment_context context;
    ac_commitment_nonce nonce;
    ac_rps_move move;
    uint8_t payload;
    ac_status status;

    if (session->internal_peer_reveal_received != 0U) {
        return ac_rps_fail(session, AC_ERR_DUPLICATE);
    }
    if (session->internal_local_commit_sent == 0U ||
        session->internal_peer_commit_received == 0U ||
        (session->internal_state != AC_RPS_STATE_BOTH_COMMITTED &&
         session->internal_state != AC_RPS_STATE_REVEAL_SENT)) {
        return ac_rps_fail(session, AC_ERR_STATE);
    }

    move = (ac_rps_move)parsed->payload[0];
    payload = (uint8_t)move;
    memcpy(nonce.bytes,
           parsed->payload + 1U,
           sizeof(nonce.bytes));
    ac_rps_make_commitment_context(
        session, parsed->sender, parsed->recipient, &context);
    status = ac_commitment_verify(
        &context,
        &payload,
        sizeof(payload),
        &nonce,
        &session->internal_peer_commitment);
    sodium_memzero(&context, sizeof(context));
    sodium_memzero(&nonce, sizeof(nonce));
    sodium_memzero(&payload, sizeof(payload));
    if (status == AC_ERR_INVALID_OPENING) {
        return ac_rps_fail(session, status);
    }
    if (status != AC_OK) {
        return status;
    }

    session->internal_peer_move = move;
    ac_rps_store_transcript_message(
        session,
        parsed->sender,
        parsed->type,
        message,
        message_length);
    session->internal_peer_reveal_received = 1U;
    session->internal_state = session->internal_local_reveal_sent != 0U
                                  ? AC_RPS_STATE_COMPLETE
                                  : AC_RPS_STATE_REVEAL_RECEIVED;
    return AC_OK;
}

static ac_status ac_rps_receive_abort(
    ac_rps_session *session,
    const ac_rps_parsed_message *parsed,
    const uint8_t *message,
    size_t message_length)
{
    ac_rps_store_transcript_message(
        session,
        parsed->sender,
        parsed->type,
        message,
        message_length);
    session->internal_abort_reason =
        (ac_rps_abort_reason)parsed->payload[0];
    session->internal_state = AC_RPS_STATE_ABORTED_PEER;
    ac_rps_wipe_unrevealed_local_secret(session);
    return AC_OK;
}

ac_status ac_rps_receive(
    ac_rps_session *session,
    const uint8_t *message,
    size_t message_length)
{
    ac_rps_parsed_message parsed;
    ac_status status;

    status = ac_rps_require_initialized(session);
    if (status != AC_OK) {
        return status;
    }
    if (ac_rps_state_is_terminal(session->internal_state)) {
        return AC_ERR_TERMINAL;
    }

    memset(&parsed, 0, sizeof(parsed));
    status = ac_rps_parse_message(
        session, message, message_length, &parsed);
    if (status != AC_OK) {
        return status;
    }

    if (parsed.type == AC_RPS_MESSAGE_COMMIT) {
        return ac_rps_receive_commit(
            session, &parsed, message, message_length);
    }
    if (parsed.type == AC_RPS_MESSAGE_REVEAL) {
        return ac_rps_receive_reveal(
            session, &parsed, message, message_length);
    }
    return ac_rps_receive_abort(
        session, &parsed, message, message_length);
}

ac_status ac_rps_mark_timeout(ac_rps_session *session)
{
    ac_status status = ac_rps_require_initialized(session);

    if (status != AC_OK) {
        return status;
    }
    if (ac_rps_state_is_terminal(session->internal_state)) {
        return AC_ERR_TERMINAL;
    }
    session->internal_state = AC_RPS_STATE_TIMED_OUT;
    ac_rps_wipe_unrevealed_local_secret(session);
    return AC_OK;
}

ac_status ac_rps_get_state(
    const ac_rps_session *session,
    ac_rps_state *state)
{
    ac_status status = ac_rps_require_initialized(session);

    if (status != AC_OK || state == NULL) {
        return AC_ERR_ARGUMENT;
    }
    *state = session->internal_state;
    return AC_OK;
}

ac_status ac_rps_get_peer_move_if_revealed(
    const ac_rps_session *session,
    ac_rps_move *peer_move)
{
    ac_status status = ac_rps_require_initialized(session);

    if (status != AC_OK || peer_move == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (session->internal_peer_reveal_received == 0U) {
        return AC_ERR_STATE;
    }
    *peer_move = session->internal_peer_move;
    return AC_OK;
}

ac_status ac_rps_get_outcome(
    const ac_rps_session *session,
    ac_rps_outcome *outcome)
{
    ac_rps_move alice_move;
    ac_rps_move bob_move;
    unsigned difference;
    ac_status status = ac_rps_require_initialized(session);

    if (status != AC_OK || outcome == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (session->internal_state != AC_RPS_STATE_COMPLETE ||
        session->internal_local_reveal_sent == 0U ||
        session->internal_peer_reveal_received == 0U) {
        return AC_ERR_STATE;
    }

    if (session->internal_local_role == AC_RPS_ROLE_ALICE) {
        alice_move = session->internal_local_move;
        bob_move = session->internal_peer_move;
    } else {
        alice_move = session->internal_peer_move;
        bob_move = session->internal_local_move;
    }
    difference = ((unsigned)alice_move + 3U - (unsigned)bob_move) % 3U;
    *outcome = difference == 0U
                   ? AC_RPS_OUTCOME_TIE
                   : (difference == 1U
                          ? AC_RPS_OUTCOME_ALICE_WINS
                          : AC_RPS_OUTCOME_BOB_WINS);
    return AC_OK;
}

ac_status ac_rps_get_failure(
    const ac_rps_session *session,
    ac_status *failure_reason)
{
    ac_status status = ac_rps_require_initialized(session);

    if (status != AC_OK || failure_reason == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (session->internal_state != AC_RPS_STATE_FAILED) {
        return AC_ERR_STATE;
    }
    *failure_reason = session->internal_failure_reason;
    return AC_OK;
}

ac_status ac_rps_get_abort_reason(
    const ac_rps_session *session,
    ac_rps_abort_reason *abort_reason)
{
    ac_status status = ac_rps_require_initialized(session);

    if (status != AC_OK || abort_reason == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (session->internal_state != AC_RPS_STATE_ABORTED_LOCAL &&
        session->internal_state != AC_RPS_STATE_ABORTED_PEER) {
        return AC_ERR_STATE;
    }
    *abort_reason = session->internal_abort_reason;
    return AC_OK;
}

static size_t ac_rps_expected_slot_message_length(size_t slot)
{
    if (slot <= (size_t)AC_RPS_TRANSCRIPT_SLOT_BOB_COMMIT) {
        return AC_RPS_COMMIT_MESSAGE_BYTES;
    }
    if (slot <= (size_t)AC_RPS_TRANSCRIPT_SLOT_BOB_REVEAL) {
        return AC_RPS_REVEAL_MESSAGE_BYTES;
    }
    return AC_RPS_ABORT_MESSAGE_BYTES;
}

ac_status ac_rps_export_transcript(
    const ac_rps_session *session,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_length)
{
    uint8_t encoded[AC_RPS_TRANSCRIPT_MAX_BYTES];
    size_t required = AC_RPS_TRANSCRIPT_HEADER_BYTES;
    size_t offset;
    size_t slot;
    ac_status status;

    if (out_length == NULL) {
        return AC_ERR_ARGUMENT;
    }
    *out_length = 0U;
    status = ac_rps_require_initialized(session);
    if (status != AC_OK || out == NULL) {
        return AC_ERR_ARGUMENT;
    }

    for (slot = 0U; slot < AC_RPS_TRANSCRIPT_SLOT_COUNT; ++slot) {
        uint8_t present = session->internal_transcript_present[slot];
        uint16_t length = session->internal_transcript_lengths[slot];

        if (present > 1U ||
            (present == 0U && length != 0U) ||
            (present == 1U &&
             (size_t)length != ac_rps_expected_slot_message_length(slot))) {
            return AC_ERR_FORMAT;
        }
        required += AC_RPS_TRANSCRIPT_SLOT_HEADER_BYTES + (size_t)length;
    }
    if (required > sizeof(encoded)) {
        return AC_ERR_LENGTH;
    }
    if (out_capacity < required) {
        return AC_ERR_CAPACITY;
    }

    memcpy(encoded,
           ac_rps_transcript_magic,
           sizeof(ac_rps_transcript_magic));
    ac_write_u16be(encoded + 4U, 1U);
    memcpy(encoded + 6U,
           session->internal_session_id,
           AC_RPS_SESSION_ID_BYTES);
    ac_write_u32be(encoded + 38U, session->internal_round);
    encoded[42] = AC_RPS_TRANSCRIPT_SLOT_COUNT;
    offset = AC_RPS_TRANSCRIPT_HEADER_BYTES;

    for (slot = 0U; slot < AC_RPS_TRANSCRIPT_SLOT_COUNT; ++slot) {
        uint16_t length = session->internal_transcript_lengths[slot];

        encoded[offset] = (uint8_t)(slot + 1U);
        encoded[offset + 1U] = session->internal_transcript_present[slot];
        ac_write_u16be(encoded + offset + 2U, length);
        offset += AC_RPS_TRANSCRIPT_SLOT_HEADER_BYTES;
        if (length != 0U) {
            memcpy(encoded + offset,
                   session->internal_transcript_messages[slot],
                   length);
            offset += length;
        }
    }

    memcpy(out, encoded, required);
    *out_length = required;
    sodium_memzero(encoded, sizeof(encoded));
    return AC_OK;
}

void ac_rps_session_clear(ac_rps_session *session)
{
    if (session != NULL) {
        sodium_memzero(session, sizeof(*session));
    }
}
