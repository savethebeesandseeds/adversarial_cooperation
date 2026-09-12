#include "ac/rps.h"

#include <stdio.h>
#include <string.h>

#include <sodium.h>

typedef enum {
    DEMO_HONEST = 0,
    DEMO_SELECTIVE_ABORT
} demo_mode;

static const char *status_name(ac_status status)
{
    switch (status) {
    case AC_OK:
        return "AC_OK";
    case AC_ERR_ARGUMENT:
        return "AC_ERR_ARGUMENT";
    case AC_ERR_LENGTH:
        return "AC_ERR_LENGTH";
    case AC_ERR_FORMAT:
        return "AC_ERR_FORMAT";
    case AC_ERR_CONTEXT:
        return "AC_ERR_CONTEXT";
    case AC_ERR_STATE:
        return "AC_ERR_STATE";
    case AC_ERR_DUPLICATE:
        return "AC_ERR_DUPLICATE";
    case AC_ERR_CONFLICT:
        return "AC_ERR_CONFLICT";
    case AC_ERR_INVALID_OPENING:
        return "AC_ERR_INVALID_OPENING";
    case AC_ERR_TERMINAL:
        return "AC_ERR_TERMINAL";
    case AC_ERR_CRYPTO:
        return "AC_ERR_CRYPTO";
    case AC_ERR_CAPACITY:
        return "AC_ERR_CAPACITY";
    default:
        return "unknown status";
    }
}

static const char *move_name(ac_rps_move move)
{
    switch (move) {
    case AC_RPS_MOVE_ROCK:
        return "rock";
    case AC_RPS_MOVE_PAPER:
        return "paper";
    case AC_RPS_MOVE_SCISSORS:
        return "scissors";
    default:
        return "invalid move";
    }
}

static const char *outcome_name(ac_rps_outcome outcome)
{
    switch (outcome) {
    case AC_RPS_OUTCOME_TIE:
        return "tie";
    case AC_RPS_OUTCOME_ALICE_WINS:
        return "Alice wins";
    case AC_RPS_OUTCOME_BOB_WINS:
        return "Bob wins";
    default:
        return "invalid outcome";
    }
}

static const char *state_name(ac_rps_state state)
{
    switch (state) {
    case AC_RPS_STATE_UNINITIALIZED:
        return "UNINITIALIZED";
    case AC_RPS_STATE_READY:
        return "READY";
    case AC_RPS_STATE_COMMIT_SENT:
        return "COMMIT_SENT";
    case AC_RPS_STATE_COMMIT_RECEIVED:
        return "COMMIT_RECEIVED";
    case AC_RPS_STATE_BOTH_COMMITTED:
        return "BOTH_COMMITTED";
    case AC_RPS_STATE_REVEAL_SENT:
        return "REVEAL_SENT";
    case AC_RPS_STATE_REVEAL_RECEIVED:
        return "REVEAL_RECEIVED";
    case AC_RPS_STATE_COMPLETE:
        return "COMPLETE";
    case AC_RPS_STATE_ABORTED_LOCAL:
        return "ABORTED_LOCAL";
    case AC_RPS_STATE_ABORTED_PEER:
        return "ABORTED_PEER";
    case AC_RPS_STATE_TIMED_OUT:
        return "TIMED_OUT";
    case AC_RPS_STATE_FAILED:
        return "FAILED";
    default:
        return "unknown state";
    }
}

static int parse_move(const char *text, ac_rps_move *move)
{
    if (strcmp(text, "rock") == 0) {
        *move = AC_RPS_MOVE_ROCK;
        return 1;
    }
    if (strcmp(text, "paper") == 0) {
        *move = AC_RPS_MOVE_PAPER;
        return 1;
    }
    if (strcmp(text, "scissors") == 0) {
        *move = AC_RPS_MOVE_SCISSORS;
        return 1;
    }
    return 0;
}

static void print_usage(const char *program)
{
    fprintf(stderr,
            "Usage: %s [--alice rock|paper|scissors] "
            "[--bob rock|paper|scissors] [--selective-abort]\n",
            program);
}

static int session_id_is_zero(const uint8_t session_id[AC_RPS_SESSION_ID_BYTES])
{
    uint8_t aggregate = 0U;
    size_t index;

    for (index = 0U; index < AC_RPS_SESSION_ID_BYTES; ++index) {
        aggregate = (uint8_t)(aggregate | session_id[index]);
    }
    return aggregate == 0U;
}

static int make_session_id(uint8_t session_id[AC_RPS_SESSION_ID_BYTES])
{
    if (sodium_init() < 0) {
        return 0;
    }
    do {
        randombytes_buf(session_id, AC_RPS_SESSION_ID_BYTES);
    } while (session_id_is_zero(session_id));
    return 1;
}

static void print_hex(const uint8_t *bytes, size_t length)
{
    size_t index;

    for (index = 0U; index < length; ++index) {
        printf("%02x", (unsigned)bytes[index]);
    }
}

static int require_ok(const char *operation, ac_status status)
{
    if (status == AC_OK) {
        return 1;
    }
    fprintf(stderr, "%s failed: %s\n", operation, status_name(status));
    return 0;
}

static ac_rps_outcome calculate_outcome(
    ac_rps_move alice_move,
    ac_rps_move bob_move)
{
    unsigned difference =
        ((unsigned)alice_move + 3U - (unsigned)bob_move) % 3U;

    if (difference == 0U) {
        return AC_RPS_OUTCOME_TIE;
    }
    return difference == 1U
               ? AC_RPS_OUTCOME_ALICE_WINS
               : AC_RPS_OUTCOME_BOB_WINS;
}

static int init_participants(
    ac_rps_session *alice,
    ac_rps_session *bob,
    ac_rps_move alice_move,
    ac_rps_move bob_move)
{
    uint8_t session_id[AC_RPS_SESSION_ID_BYTES];
    ac_status status;

    if (!make_session_id(session_id)) {
        fprintf(stderr, "libsodium initialization failed\n");
        return 0;
    }
    status = ac_rps_session_init(
        alice, AC_RPS_ROLE_ALICE, alice_move, session_id, 1U);
    if (!require_ok("initialize Alice", status)) {
        sodium_memzero(session_id, sizeof(session_id));
        return 0;
    }
    status = ac_rps_session_init(
        bob, AC_RPS_ROLE_BOB, bob_move, session_id, 1U);
    sodium_memzero(session_id, sizeof(session_id));
    if (!require_ok("initialize Bob", status)) {
        ac_rps_session_clear(alice);
        return 0;
    }
    return 1;
}

static int exchange_commitments(
    ac_rps_session *alice,
    ac_rps_session *bob)
{
    uint8_t message[AC_RPS_MESSAGE_MAX_BYTES];
    size_t message_length = 0U;
    ac_status status;

    status = ac_rps_make_commit(
        alice, message, sizeof(message), &message_length);
    if (!require_ok("Alice creates commitment", status)) {
        return 0;
    }
    printf("Alice commitment: ");
    print_hex(message + AC_RPS_MESSAGE_HEADER_BYTES,
              AC_COMMITMENT_DIGEST_BYTES);
    printf("\n");
    status = ac_rps_receive(bob, message, message_length);
    if (!require_ok("Bob accepts Alice commitment", status)) {
        sodium_memzero(message, sizeof(message));
        return 0;
    }

    status = ac_rps_make_commit(
        bob, message, sizeof(message), &message_length);
    if (!require_ok("Bob creates commitment", status)) {
        sodium_memzero(message, sizeof(message));
        return 0;
    }
    printf("Bob commitment:   ");
    print_hex(message + AC_RPS_MESSAGE_HEADER_BYTES,
              AC_COMMITMENT_DIGEST_BYTES);
    printf("\n");
    status = ac_rps_receive(alice, message, message_length);
    sodium_memzero(message, sizeof(message));
    return require_ok("Alice accepts Bob commitment", status);
}

static int transcripts_match(
    const ac_rps_session *alice,
    const ac_rps_session *bob)
{
    uint8_t alice_transcript[AC_RPS_TRANSCRIPT_MAX_BYTES];
    uint8_t bob_transcript[AC_RPS_TRANSCRIPT_MAX_BYTES];
    size_t alice_length = 0U;
    size_t bob_length = 0U;
    ac_status status;
    int equal;

    status = ac_rps_export_transcript(
        alice,
        alice_transcript,
        sizeof(alice_transcript),
        &alice_length);
    if (!require_ok("export Alice transcript", status)) {
        return 0;
    }
    status = ac_rps_export_transcript(
        bob,
        bob_transcript,
        sizeof(bob_transcript),
        &bob_length);
    if (!require_ok("export Bob transcript", status)) {
        sodium_memzero(alice_transcript, sizeof(alice_transcript));
        return 0;
    }

    equal = alice_length == bob_length &&
            memcmp(alice_transcript, bob_transcript, alice_length) == 0;
    printf("Canonical transcripts: %s (%lu bytes)\n",
           equal ? "equal" : "different",
           (unsigned long)alice_length);
    sodium_memzero(alice_transcript, sizeof(alice_transcript));
    sodium_memzero(bob_transcript, sizeof(bob_transcript));
    return equal;
}

static int run_honest(ac_rps_move alice_move, ac_rps_move bob_move)
{
    ac_rps_session alice;
    ac_rps_session bob;
    uint8_t message[AC_RPS_MESSAGE_MAX_BYTES];
    size_t message_length = 0U;
    ac_rps_outcome alice_outcome;
    ac_rps_outcome bob_outcome;
    ac_status status;
    int success = 0;

    memset(&alice, 0, sizeof(alice));
    memset(&bob, 0, sizeof(bob));
    if (!init_participants(
            &alice, &bob, alice_move, bob_move) ||
        !exchange_commitments(&alice, &bob)) {
        goto cleanup;
    }

    status = ac_rps_get_outcome(&alice, &alice_outcome);
    if (status != AC_ERR_STATE) {
        fprintf(stderr,
                "Alice outcome became available before reveal: %s\n",
                status_name(status));
        goto cleanup;
    }

    status = ac_rps_make_reveal(
        &alice, message, sizeof(message), &message_length);
    if (!require_ok("Alice creates reveal", status)) {
        goto cleanup;
    }
    printf("Alice now reveals: %s\n", move_name(alice_move));
    status = ac_rps_receive(&bob, message, message_length);
    if (!require_ok("Bob validates Alice reveal", status)) {
        goto cleanup;
    }

    status = ac_rps_make_reveal(
        &bob, message, sizeof(message), &message_length);
    if (!require_ok("Bob creates reveal", status)) {
        goto cleanup;
    }
    printf("Bob now reveals:   %s\n", move_name(bob_move));
    status = ac_rps_receive(&alice, message, message_length);
    if (!require_ok("Alice validates Bob reveal", status)) {
        goto cleanup;
    }

    if (!require_ok("Alice obtains completed outcome",
                    ac_rps_get_outcome(&alice, &alice_outcome)) ||
        !require_ok("Bob obtains completed outcome",
                    ac_rps_get_outcome(&bob, &bob_outcome))) {
        goto cleanup;
    }
    if (alice_outcome != bob_outcome) {
        fprintf(stderr, "participants derived different outcomes\n");
        goto cleanup;
    }
    printf("Completed result: %s\n", outcome_name(alice_outcome));
    if (!transcripts_match(&alice, &bob)) {
        fprintf(stderr, "honest completed transcripts did not match\n");
        goto cleanup;
    }
    success = 1;

cleanup:
    sodium_memzero(message, sizeof(message));
    ac_rps_session_clear(&alice);
    ac_rps_session_clear(&bob);
    return success;
}

static int run_selective_abort(ac_rps_move alice_move, ac_rps_move bob_move)
{
    ac_rps_session alice;
    ac_rps_session bob;
    uint8_t message[AC_RPS_MESSAGE_MAX_BYTES];
    size_t message_length = 0U;
    ac_rps_move bob_observed_alice_move;
    ac_rps_move alice_observed_bob_move;
    ac_rps_outcome bob_private_outcome;
    ac_rps_outcome unavailable_outcome;
    ac_rps_state alice_state;
    ac_rps_state bob_state;
    ac_status status;
    int success = 0;

    memset(&alice, 0, sizeof(alice));
    memset(&bob, 0, sizeof(bob));
    if (!init_participants(
            &alice, &bob, alice_move, bob_move) ||
        !exchange_commitments(&alice, &bob)) {
        goto cleanup;
    }

    status = ac_rps_make_reveal(
        &alice, message, sizeof(message), &message_length);
    if (!require_ok("Alice creates first reveal", status)) {
        goto cleanup;
    }
    printf("Alice reveals first: %s\n", move_name(alice_move));
    status = ac_rps_receive(&bob, message, message_length);
    if (!require_ok("Bob validates Alice reveal", status)) {
        goto cleanup;
    }
    status = ac_rps_get_peer_move_if_revealed(
        &bob, &bob_observed_alice_move);
    if (!require_ok("Bob reads accepted Alice move", status)) {
        goto cleanup;
    }
    bob_private_outcome = calculate_outcome(
        bob_observed_alice_move, bob_move);
    printf("Bob can now derive privately: %s\n",
           outcome_name(bob_private_outcome));

    status = ac_rps_get_outcome(&bob, &unavailable_outcome);
    printf("Bob's shared-outcome API before his reveal: %s\n",
           status_name(status));
    if (status != AC_ERR_STATE) {
        fprintf(stderr, "outcome API should remain gated before Bob reveals\n");
        goto cleanup;
    }

    status = ac_rps_make_abort(
        &bob,
        AC_RPS_ABORT_APPLICATION_REQUEST,
        message,
        sizeof(message),
        &message_length);
    if (!require_ok("Bob emits abort instead of reveal", status)) {
        goto cleanup;
    }
    status = ac_rps_receive(&alice, message, message_length);
    if (!require_ok("Alice records Bob abort", status)) {
        goto cleanup;
    }

    status = ac_rps_get_peer_move_if_revealed(
        &alice, &alice_observed_bob_move);
    printf("Alice asks for Bob's withheld move: %s\n", status_name(status));
    if (status != AC_ERR_STATE) {
        fprintf(stderr, "Alice unexpectedly learned Bob's withheld move\n");
        goto cleanup;
    }
    status = ac_rps_get_outcome(&alice, &unavailable_outcome);
    printf("Alice asks for a completed outcome: %s\n", status_name(status));
    if (status != AC_ERR_STATE) {
        fprintf(stderr, "Alice unexpectedly obtained an incomplete result\n");
        goto cleanup;
    }

    if (!require_ok("read Alice terminal state",
                    ac_rps_get_state(&alice, &alice_state)) ||
        !require_ok("read Bob terminal state",
                    ac_rps_get_state(&bob, &bob_state))) {
        goto cleanup;
    }
    printf("Terminal local states: Alice=%s, Bob=%s\n",
           state_name(alice_state), state_name(bob_state));
    if (alice_state != AC_RPS_STATE_ABORTED_PEER ||
        bob_state != AC_RPS_STATE_ABORTED_LOCAL) {
        fprintf(stderr, "unexpected selective-abort terminal states\n");
        goto cleanup;
    }
    if (!transcripts_match(&alice, &bob)) {
        fprintf(stderr, "delivered selective-abort transcripts did not match\n");
        goto cleanup;
    }
    printf("No game completed. Bob learned the result first and could withhold "
           "his opening; this protocol does not provide fairness.\n");
    success = 1;

cleanup:
    sodium_memzero(message, sizeof(message));
    ac_rps_session_clear(&alice);
    ac_rps_session_clear(&bob);
    return success;
}

int main(int argc, char **argv)
{
    ac_rps_move alice_move = AC_RPS_MOVE_ROCK;
    ac_rps_move bob_move = AC_RPS_MOVE_SCISSORS;
    demo_mode mode = DEMO_HONEST;
    int index;

    for (index = 1; index < argc; ++index) {
        if (strcmp(argv[index], "--alice") == 0) {
            if (index + 1 >= argc ||
                !parse_move(argv[index + 1], &alice_move)) {
                print_usage(argv[0]);
                return 2;
            }
            ++index;
        } else if (strcmp(argv[index], "--bob") == 0) {
            if (index + 1 >= argc ||
                !parse_move(argv[index + 1], &bob_move)) {
                print_usage(argv[0]);
                return 2;
            }
            ++index;
        } else if (strcmp(argv[index], "--selective-abort") == 0) {
            mode = DEMO_SELECTIVE_ABORT;
        } else if (strcmp(argv[index], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            print_usage(argv[0]);
            return 2;
        }
    }

    printf("EDUCATIONAL DEMONSTRATION -- NOT PRODUCTION-SAFE\n");
    printf("Authenticated, integrity-protected transport is assumed and is not "
           "implemented here.\n");
    printf("Fair exchange and guaranteed output are absent. This one process "
           "receives both moves and is not an isolation boundary.\n\n");

    if (mode == DEMO_SELECTIVE_ABORT) {
        return run_selective_abort(alice_move, bob_move) ? 0 : 1;
    }
    return run_honest(alice_move, bob_move) ? 0 : 1;
}
