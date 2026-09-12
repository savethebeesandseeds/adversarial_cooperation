#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ac/rps.h"
#include "test_support.h"

typedef struct {
    ac_rps_session alice;
    ac_rps_session bob;
    uint8_t session_id[AC_RPS_SESSION_ID_BYTES];
} rps_pair;

static void fill_sequence(uint8_t *out, size_t length, uint8_t first)
{
    size_t index;

    for (index = 0U; index < length; ++index) {
        out[index] = (uint8_t)(first + (uint8_t)index);
    }
}

static int init_pair(
    rps_pair *pair,
    ac_rps_move alice_move,
    ac_rps_move bob_move,
    uint8_t tag)
{
    ac_commitment_nonce alice_nonce;
    ac_commitment_nonce bob_nonce;

    fill_sequence(pair->session_id, sizeof pair->session_id, (uint8_t)(1U + tag));
    fill_sequence(alice_nonce.bytes, sizeof alice_nonce.bytes, (uint8_t)(0x20U + tag));
    fill_sequence(bob_nonce.bytes, sizeof bob_nonce.bytes, (uint8_t)(0x80U + tag));

    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &pair->alice,
            AC_RPS_ROLE_ALICE,
            alice_move,
            pair->session_id,
            (uint32_t)tag + 1U,
            &alice_nonce),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &pair->bob,
            AC_RPS_ROLE_BOB,
            bob_move,
            pair->session_id,
            (uint32_t)tag + 1U,
            &bob_nonce),
        AC_OK);
    ac_commitment_nonce_clear(&alice_nonce);
    ac_commitment_nonce_clear(&bob_nonce);
    return 0;
}

static int assert_state(
    const ac_rps_session *session,
    ac_rps_state expected)
{
    ac_rps_state actual = AC_RPS_STATE_UNINITIALIZED;

    AC_TEST_ASSERT_STATUS(ac_rps_get_state(session, &actual), AC_OK);
    AC_TEST_ASSERT_EQ_INT(actual, expected);
    return 0;
}

static ac_rps_outcome expected_outcome(
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

static int export_and_compare_transcripts(
    const ac_rps_session *alice,
    const ac_rps_session *bob,
    size_t expected_length)
{
    uint8_t alice_transcript[AC_RPS_TRANSCRIPT_MAX_BYTES];
    uint8_t bob_transcript[AC_RPS_TRANSCRIPT_MAX_BYTES];
    size_t alice_length = 0U;
    size_t bob_length = 0U;

    AC_TEST_ASSERT_STATUS(
        ac_rps_export_transcript(
            alice,
            alice_transcript,
            sizeof alice_transcript,
            &alice_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_export_transcript(
            bob,
            bob_transcript,
            sizeof bob_transcript,
            &bob_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(alice_length, expected_length);
    AC_TEST_ASSERT_EQ_INT(bob_length, expected_length);
    AC_TEST_ASSERT_MEMORY(alice_transcript, bob_transcript, alice_length);
    return 0;
}

static uint16_t read_u16be(const uint8_t bytes[2])
{
    return (uint16_t)(((uint16_t)bytes[0] << 8) | (uint16_t)bytes[1]);
}

static int assert_transcript_slot(
    const uint8_t *transcript,
    size_t transcript_length,
    size_t *offset,
    uint8_t expected_slot_id,
    const uint8_t *expected_message,
    size_t expected_message_length)
{
    uint8_t expected_present = expected_message == NULL ? 0U : 1U;
    uint16_t encoded_length;

    AC_TEST_ASSERT(*offset <= transcript_length);
    AC_TEST_ASSERT(transcript_length - *offset >=
                   AC_RPS_TRANSCRIPT_SLOT_HEADER_BYTES);
    AC_TEST_ASSERT_EQ_INT(transcript[*offset], expected_slot_id);
    AC_TEST_ASSERT_EQ_INT(transcript[*offset + 1U], expected_present);
    encoded_length = read_u16be(transcript + *offset + 2U);
    AC_TEST_ASSERT_EQ_INT(encoded_length, expected_message_length);
    *offset += AC_RPS_TRANSCRIPT_SLOT_HEADER_BYTES;
    AC_TEST_ASSERT(*offset <= transcript_length);
    AC_TEST_ASSERT(transcript_length - *offset >= expected_message_length);
    if (expected_message_length != 0U) {
        AC_TEST_ASSERT_MEMORY(
            transcript + *offset,
            expected_message,
            expected_message_length);
    }
    *offset += expected_message_length;
    return 0;
}

static int assert_complete_transcript_layout(
    const ac_rps_session *session,
    const uint8_t *alice_commit,
    size_t alice_commit_length,
    const uint8_t *bob_commit,
    size_t bob_commit_length,
    const uint8_t *alice_reveal,
    size_t alice_reveal_length,
    const uint8_t *bob_reveal,
    size_t bob_reveal_length)
{
    uint8_t transcript[AC_RPS_TRANSCRIPT_MAX_BYTES];
    const uint8_t *messages[AC_RPS_TRANSCRIPT_SLOT_COUNT] = {
        alice_commit,
        bob_commit,
        alice_reveal,
        bob_reveal,
        NULL,
        NULL
    };
    const size_t lengths[AC_RPS_TRANSCRIPT_SLOT_COUNT] = {
        alice_commit_length,
        bob_commit_length,
        alice_reveal_length,
        bob_reveal_length,
        0U,
        0U
    };
    size_t transcript_length = 0U;
    size_t offset = AC_RPS_TRANSCRIPT_HEADER_BYTES;
    size_t slot;

    AC_TEST_ASSERT_STATUS(
        ac_rps_export_transcript(
            session,
            transcript,
            sizeof transcript,
            &transcript_length),
        AC_OK);
    for (slot = 0U; slot < AC_RPS_TRANSCRIPT_SLOT_COUNT; ++slot) {
        AC_TEST_ASSERT_EQ_INT(
            assert_transcript_slot(
                transcript,
                transcript_length,
                &offset,
                (uint8_t)(slot + 1U),
                messages[slot],
                lengths[slot]),
            0);
    }
    AC_TEST_ASSERT_EQ_INT(offset, transcript_length);
    return 0;
}

static int play_alice_first(
    rps_pair *pair,
    ac_rps_outcome expected)
{
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t alice_reveal[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_reveal[AC_RPS_MESSAGE_MAX_BYTES];
    size_t alice_commit_length = 0U;
    size_t bob_commit_length = 0U;
    size_t alice_reveal_length = 0U;
    size_t bob_reveal_length = 0U;
    ac_rps_outcome alice_outcome = AC_RPS_OUTCOME_TIE;
    ac_rps_outcome bob_outcome = AC_RPS_OUTCOME_TIE;

    AC_TEST_ASSERT_STATUS(
        ac_rps_get_outcome(&pair->alice, &alice_outcome),
        AC_ERR_STATE);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_outcome(&pair->bob, &bob_outcome),
        AC_ERR_STATE);

    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair->alice,
            alice_commit,
            sizeof alice_commit,
            &alice_commit_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(alice_commit_length, AC_RPS_COMMIT_MESSAGE_BYTES);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair->bob,
            bob_commit,
            sizeof bob_commit,
            &bob_commit_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(bob_commit_length, AC_RPS_COMMIT_MESSAGE_BYTES);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair->bob, alice_commit, alice_commit_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair->alice, bob_commit, bob_commit_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair->alice, AC_RPS_STATE_BOTH_COMMITTED), 0);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair->bob, AC_RPS_STATE_BOTH_COMMITTED), 0);

    AC_TEST_ASSERT_STATUS(
        ac_rps_make_reveal(
            &pair->alice,
            alice_reveal,
            sizeof alice_reveal,
            &alice_reveal_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair->bob, alice_reveal, alice_reveal_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_outcome(&pair->bob, &bob_outcome),
        AC_ERR_STATE);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_reveal(
            &pair->bob,
            bob_reveal,
            sizeof bob_reveal,
            &bob_reveal_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair->alice, bob_reveal, bob_reveal_length),
        AC_OK);

    AC_TEST_ASSERT_EQ_INT(alice_reveal_length, AC_RPS_REVEAL_MESSAGE_BYTES);
    AC_TEST_ASSERT_EQ_INT(bob_reveal_length, AC_RPS_REVEAL_MESSAGE_BYTES);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_outcome(&pair->alice, &alice_outcome),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_outcome(&pair->bob, &bob_outcome),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(alice_outcome, expected);
    AC_TEST_ASSERT_EQ_INT(bob_outcome, expected);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair->alice, AC_RPS_STATE_COMPLETE), 0);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair->bob, AC_RPS_STATE_COMPLETE), 0);

    AC_TEST_ASSERT_EQ_INT(
        export_and_compare_transcripts(
            &pair->alice,
            &pair->bob,
            AC_RPS_TRANSCRIPT_HEADER_BYTES +
                AC_RPS_TRANSCRIPT_SLOT_COUNT *
                    AC_RPS_TRANSCRIPT_SLOT_HEADER_BYTES +
                2U * AC_RPS_COMMIT_MESSAGE_BYTES +
                2U * AC_RPS_REVEAL_MESSAGE_BYTES),
        0);
    AC_TEST_ASSERT_EQ_INT(
        assert_complete_transcript_layout(
            &pair->alice,
            alice_commit,
            alice_commit_length,
            bob_commit,
            bob_commit_length,
            alice_reveal,
            alice_reveal_length,
            bob_reveal,
            bob_reveal_length),
        0);
    return 0;
}

static int test_all_nine_outcomes(void)
{
    unsigned alice_value;
    unsigned bob_value;
    uint8_t tag = 0U;

    for (alice_value = 0U; alice_value < 3U; ++alice_value) {
        for (bob_value = 0U; bob_value < 3U; ++bob_value) {
            rps_pair pair;
            ac_rps_move alice_move = (ac_rps_move)alice_value;
            ac_rps_move bob_move = (ac_rps_move)bob_value;

            AC_TEST_ASSERT_EQ_INT(
                init_pair(&pair, alice_move, bob_move, tag), 0);
            AC_TEST_ASSERT_EQ_INT(
                play_alice_first(
                    &pair,
                    expected_outcome(alice_move, bob_move)),
                0);
            ac_rps_session_clear(&pair.alice);
            ac_rps_session_clear(&pair.bob);
            ++tag;
        }
    }
    return 0;
}

static int test_reverse_legal_interleaving(void)
{
    rps_pair pair;
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t alice_reveal[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_reveal[AC_RPS_MESSAGE_MAX_BYTES];
    size_t alice_commit_length = 0U;
    size_t bob_commit_length = 0U;
    size_t alice_reveal_length = 0U;
    size_t bob_reveal_length = 0U;
    ac_rps_outcome outcome;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(
            &pair,
            AC_RPS_MOVE_SCISSORS,
            AC_RPS_MOVE_PAPER,
            30U),
        0);

    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair.bob,
            bob_commit,
            sizeof bob_commit,
            &bob_commit_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.alice, bob_commit, bob_commit_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_COMMIT_RECEIVED), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair.alice,
            alice_commit,
            sizeof alice_commit,
            &alice_commit_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, alice_commit, alice_commit_length),
        AC_OK);

    AC_TEST_ASSERT_STATUS(
        ac_rps_make_reveal(
            &pair.bob,
            bob_reveal,
            sizeof bob_reveal,
            &bob_reveal_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.alice, bob_reveal, bob_reveal_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_REVEAL_RECEIVED), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_reveal(
            &pair.alice,
            alice_reveal,
            sizeof alice_reveal,
            &alice_reveal_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, alice_reveal, alice_reveal_length),
        AC_OK);

    AC_TEST_ASSERT_STATUS(ac_rps_get_outcome(&pair.alice, &outcome), AC_OK);
    AC_TEST_ASSERT_EQ_INT(outcome, AC_RPS_OUTCOME_ALICE_WINS);
    AC_TEST_ASSERT_STATUS(ac_rps_get_outcome(&pair.bob, &outcome), AC_OK);
    AC_TEST_ASSERT_EQ_INT(outcome, AC_RPS_OUTCOME_ALICE_WINS);
    AC_TEST_ASSERT_EQ_INT(
        export_and_compare_transcripts(
            &pair.alice,
            &pair.bob,
            AC_RPS_TRANSCRIPT_HEADER_BYTES +
                AC_RPS_TRANSCRIPT_SLOT_COUNT *
                    AC_RPS_TRANSCRIPT_SLOT_HEADER_BYTES +
                2U * AC_RPS_COMMIT_MESSAGE_BYTES +
                2U * AC_RPS_REVEAL_MESSAGE_BYTES),
        0);
    return 0;
}

static int test_reveal_gate_and_atomic_capacity_failure(void)
{
    rps_pair pair;
    uint8_t output[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
    size_t output_length = 99U;
    size_t commit_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 40U), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_reveal(
            &pair.alice,
            output,
            sizeof output,
            &output_length),
        AC_ERR_STATE);
    AC_TEST_ASSERT_EQ_INT(output_length, 0U);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_READY), 0);

    output_length = 99U;
    (void)memset(output, 0xa5, sizeof output);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair.alice,
            output,
            AC_RPS_COMMIT_MESSAGE_BYTES - 1U,
            &output_length),
        AC_ERR_CAPACITY);
    AC_TEST_ASSERT_EQ_INT(output_length, 0U);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_READY), 0);

    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair.alice,
            alice_commit,
            sizeof alice_commit,
            &commit_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_reveal(
            &pair.alice,
            output,
            sizeof output,
            &output_length),
        AC_ERR_STATE);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_COMMIT_SENT), 0);
    return 0;
}

static int test_wire_encodings_and_abort_round_trip(void)
{
    rps_pair pair;
    uint8_t abort_message[AC_RPS_MESSAGE_MAX_BYTES];
    size_t abort_length = 0U;
    ac_rps_abort_reason abort_reason = AC_RPS_ABORT_UNSPECIFIED;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_PAPER, AC_RPS_MOVE_ROCK, 50U), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_abort_reason(&pair.alice, &abort_reason),
        AC_ERR_STATE);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &pair.alice,
            AC_RPS_ABORT_APPLICATION_REQUEST,
            abort_message,
            sizeof abort_message,
            &abort_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(abort_length, AC_RPS_ABORT_MESSAGE_BYTES);
    AC_TEST_ASSERT_MEMORY(abort_message, "ACRP", 4U);
    AC_TEST_ASSERT_EQ_INT(abort_message[4], 0U);
    AC_TEST_ASSERT_EQ_INT(abort_message[5], AC_RPS_PROTOCOL_VERSION);
    AC_TEST_ASSERT_MEMORY(abort_message + 6U, pair.session_id, sizeof pair.session_id);
    AC_TEST_ASSERT_EQ_INT(abort_message[42], AC_RPS_ROLE_ALICE);
    AC_TEST_ASSERT_EQ_INT(abort_message[43], AC_RPS_ROLE_BOB);
    AC_TEST_ASSERT_EQ_INT(abort_message[44], AC_RPS_MESSAGE_ABORT);
    AC_TEST_ASSERT_EQ_INT(abort_message[45], 0U);
    AC_TEST_ASSERT_EQ_INT(abort_message[46], 1U);
    AC_TEST_ASSERT_EQ_INT(
        abort_message[47], AC_RPS_ABORT_APPLICATION_REQUEST);

    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, abort_message, abort_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_ABORTED_LOCAL), 0);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.bob, AC_RPS_STATE_ABORTED_PEER), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_abort_reason(&pair.alice, &abort_reason),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        abort_reason, AC_RPS_ABORT_APPLICATION_REQUEST);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_abort_reason(&pair.bob, &abort_reason),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        abort_reason, AC_RPS_ABORT_APPLICATION_REQUEST);
    AC_TEST_ASSERT_EQ_INT(
        export_and_compare_transcripts(
            &pair.alice,
            &pair.bob,
            AC_RPS_TRANSCRIPT_HEADER_BYTES +
                AC_RPS_TRANSCRIPT_SLOT_COUNT *
                    AC_RPS_TRANSCRIPT_SLOT_HEADER_BYTES +
                AC_RPS_ABORT_MESSAGE_BYTES),
        0);
    return 0;
}

static int test_transcript_header_and_capacity(void)
{
    rps_pair pair;
    uint8_t transcript[AC_RPS_TRANSCRIPT_MAX_BYTES];
    size_t length = 99U;
    size_t expected_empty_length =
        AC_RPS_TRANSCRIPT_HEADER_BYTES +
        AC_RPS_TRANSCRIPT_SLOT_COUNT * AC_RPS_TRANSCRIPT_SLOT_HEADER_BYTES;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_ROCK, 60U), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_export_transcript(
            &pair.alice,
            transcript,
            expected_empty_length - 1U,
            &length),
        AC_ERR_CAPACITY);
    AC_TEST_ASSERT_EQ_INT(length, 0U);

    AC_TEST_ASSERT_STATUS(
        ac_rps_export_transcript(
            &pair.alice,
            transcript,
            sizeof transcript,
            &length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(length, expected_empty_length);
    AC_TEST_ASSERT_MEMORY(transcript, "ACRT", 4U);
    AC_TEST_ASSERT_EQ_INT(transcript[4], 0U);
    AC_TEST_ASSERT_EQ_INT(transcript[5], 1U);
    AC_TEST_ASSERT_MEMORY(transcript + 6U, pair.session_id, sizeof pair.session_id);
    AC_TEST_ASSERT_EQ_INT(transcript[42], AC_RPS_TRANSCRIPT_SLOT_COUNT);
    return 0;
}

static int test_initialization_validation_and_clear(void)
{
    ac_rps_session session;
    ac_commitment_nonce nonce;
    uint8_t session_id[AC_RPS_SESSION_ID_BYTES];
    uint8_t zero_session_id[AC_RPS_SESSION_ID_BYTES] = {0U};
    ac_rps_state state;

    fill_sequence(session_id, sizeof session_id, 1U);
    fill_sequence(nonce.bytes, sizeof nonce.bytes, 0x40U);
    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &session,
            (ac_rps_role)0,
            AC_RPS_MOVE_ROCK,
            session_id,
            1U,
            &nonce),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &session,
            AC_RPS_ROLE_ALICE,
            (ac_rps_move)3,
            session_id,
            1U,
            &nonce),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &session,
            AC_RPS_ROLE_ALICE,
            AC_RPS_MOVE_ROCK,
            zero_session_id,
            1U,
            &nonce),
        AC_ERR_CONTEXT);
    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &session,
            AC_RPS_ROLE_ALICE,
            AC_RPS_MOVE_ROCK,
            session_id,
            0U,
            &nonce),
        AC_ERR_CONTEXT);
    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &session,
            AC_RPS_ROLE_ALICE,
            AC_RPS_MOVE_ROCK,
            session_id,
            1U,
            NULL),
        AC_ERR_ARGUMENT);

    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &session,
            AC_RPS_ROLE_ALICE,
            AC_RPS_MOVE_ROCK,
            session_id,
            1U,
            &nonce),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(assert_state(&session, AC_RPS_STATE_READY), 0);
    ac_rps_session_clear(&session);
    AC_TEST_ASSERT_ZERO(&session, sizeof session);
    AC_TEST_ASSERT_STATUS(ac_rps_get_state(&session, &state), AC_ERR_ARGUMENT);
    return 0;
}

int main(void)
{
    ac_test_suite suite = {0U, 0U};

    AC_TEST_RUN(suite, test_all_nine_outcomes);
    AC_TEST_RUN(suite, test_reverse_legal_interleaving);
    AC_TEST_RUN(suite, test_reveal_gate_and_atomic_capacity_failure);
    AC_TEST_RUN(suite, test_wire_encodings_and_abort_round_trip);
    AC_TEST_RUN(suite, test_transcript_header_and_capacity);
    AC_TEST_RUN(suite, test_initialization_validation_and_clear);
    return ac_test_finish(&suite);
}
