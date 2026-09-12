#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ac/rps.h"
#include "test_support.h"

enum {
    WIRE_OFFSET_VERSION = 4,
    WIRE_OFFSET_SESSION = 6,
    WIRE_OFFSET_ROUND = 38,
    WIRE_OFFSET_SENDER = 42,
    WIRE_OFFSET_RECIPIENT = 43,
    WIRE_OFFSET_TYPE = 44,
    WIRE_OFFSET_LENGTH = 45,
    WIRE_OFFSET_PAYLOAD = 47
};

typedef struct {
    ac_rps_session alice;
    ac_rps_session bob;
    uint8_t session_id[AC_RPS_SESSION_ID_BYTES];
    uint32_t round;
} adversarial_pair;

static void fill_sequence(uint8_t *out, size_t length, uint8_t first)
{
    size_t index;

    for (index = 0U; index < length; ++index) {
        out[index] = (uint8_t)(first + (uint8_t)index);
    }
}

static int init_pair(
    adversarial_pair *pair,
    ac_rps_move alice_move,
    ac_rps_move bob_move,
    uint8_t tag)
{
    ac_commitment_nonce alice_nonce;
    ac_commitment_nonce bob_nonce;

    fill_sequence(pair->session_id, sizeof pair->session_id, (uint8_t)(tag + 1U));
    pair->round = (uint32_t)tag + 1U;
    fill_sequence(alice_nonce.bytes, sizeof alice_nonce.bytes, (uint8_t)(0x11U + tag));
    fill_sequence(bob_nonce.bytes, sizeof bob_nonce.bytes, (uint8_t)(0x91U + tag));
    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &pair->alice,
            AC_RPS_ROLE_ALICE,
            alice_move,
            pair->session_id,
            pair->round,
            &alice_nonce),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &pair->bob,
            AC_RPS_ROLE_BOB,
            bob_move,
            pair->session_id,
            pair->round,
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
    ac_rps_state state = AC_RPS_STATE_UNINITIALIZED;

    AC_TEST_ASSERT_STATUS(ac_rps_get_state(session, &state), AC_OK);
    AC_TEST_ASSERT_EQ_INT(state, expected);
    return 0;
}

static int assert_failed(
    const ac_rps_session *session,
    ac_status expected_reason)
{
    ac_status reason = AC_OK;

    AC_TEST_ASSERT_EQ_INT(assert_state(session, AC_RPS_STATE_FAILED), 0);
    AC_TEST_ASSERT_STATUS(ac_rps_get_failure(session, &reason), AC_OK);
    AC_TEST_ASSERT_EQ_INT(reason, expected_reason);
    return 0;
}

static int export_transcript(
    const ac_rps_session *session,
    uint8_t out[AC_RPS_TRANSCRIPT_MAX_BYTES],
    size_t *length)
{
    AC_TEST_ASSERT_STATUS(
        ac_rps_export_transcript(
            session,
            out,
            AC_RPS_TRANSCRIPT_MAX_BYTES,
            length),
        AC_OK);
    return 0;
}

static int expect_receive_rejected_without_mutation(
    ac_rps_session *receiver,
    const uint8_t *message,
    size_t message_length,
    ac_status expected_status,
    ac_rps_state expected_state)
{
    uint8_t before[AC_RPS_TRANSCRIPT_MAX_BYTES];
    uint8_t after[AC_RPS_TRANSCRIPT_MAX_BYTES];
    ac_rps_session before_session;
    size_t before_length = 0U;
    size_t after_length = 0U;

    (void)memcpy(&before_session, receiver, sizeof before_session);
    AC_TEST_ASSERT_EQ_INT(export_transcript(receiver, before, &before_length), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(receiver, message, message_length),
        expected_status);
    AC_TEST_ASSERT_EQ_INT(assert_state(receiver, expected_state), 0);
    AC_TEST_ASSERT_EQ_INT(export_transcript(receiver, after, &after_length), 0);
    AC_TEST_ASSERT_EQ_INT(after_length, before_length);
    AC_TEST_ASSERT_MEMORY(after, before, before_length);
    AC_TEST_ASSERT_MEMORY(receiver, &before_session, sizeof before_session);
    return 0;
}

static int make_commit_messages(
    adversarial_pair *pair,
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES],
    size_t *alice_commit_length,
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES],
    size_t *bob_commit_length)
{
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair->alice,
            alice_commit,
            AC_RPS_MESSAGE_MAX_BYTES,
            alice_commit_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair->bob,
            bob_commit,
            AC_RPS_MESSAGE_MAX_BYTES,
            bob_commit_length),
        AC_OK);
    return 0;
}

static int make_both_committed(
    adversarial_pair *pair,
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES],
    size_t *alice_commit_length,
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES],
    size_t *bob_commit_length)
{
    AC_TEST_ASSERT_EQ_INT(
        make_commit_messages(
            pair,
            alice_commit,
            alice_commit_length,
            bob_commit,
            bob_commit_length),
        0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair->bob, alice_commit, *alice_commit_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair->alice, bob_commit, *bob_commit_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair->alice, AC_RPS_STATE_BOTH_COMMITTED), 0);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair->bob, AC_RPS_STATE_BOTH_COMMITTED), 0);
    return 0;
}

static int make_alice_reveal_fixture(
    adversarial_pair *pair,
    uint8_t reveal[AC_RPS_MESSAGE_MAX_BYTES],
    size_t *reveal_length,
    uint8_t tag)
{
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    size_t alice_commit_length = 0U;
    size_t bob_commit_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(
            pair,
            AC_RPS_MOVE_ROCK,
            AC_RPS_MOVE_SCISSORS,
            tag),
        0);
    AC_TEST_ASSERT_EQ_INT(
        make_both_committed(
            pair,
            alice_commit,
            &alice_commit_length,
            bob_commit,
            &bob_commit_length),
        0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_reveal(
            &pair->alice,
            reveal,
            AC_RPS_MESSAGE_MAX_BYTES,
            reveal_length),
        AC_OK);
    return 0;
}

static int test_every_message_truncation_prefix(void)
{
    adversarial_pair fixture;
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t alice_reveal[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t alice_abort[AC_RPS_MESSAGE_MAX_BYTES];
    size_t alice_commit_length = 0U;
    size_t bob_commit_length = 0U;
    size_t alice_reveal_length = 0U;
    size_t alice_abort_length = 0U;
    size_t prefix;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&fixture, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 2U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_commit_messages(
            &fixture,
            alice_commit,
            &alice_commit_length,
            bob_commit,
            &bob_commit_length),
        0);
    for (prefix = 0U; prefix < alice_commit_length; ++prefix) {
        adversarial_pair receiver;

        AC_TEST_ASSERT_EQ_INT(
            init_pair(&receiver, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 2U), 0);
        AC_TEST_ASSERT_EQ_INT(
            expect_receive_rejected_without_mutation(
                &receiver.bob,
                alice_commit,
                prefix,
                AC_ERR_LENGTH,
                AC_RPS_STATE_READY),
            0);
        ac_rps_session_clear(&receiver.alice);
        ac_rps_session_clear(&receiver.bob);
    }
    ac_rps_session_clear(&fixture.alice);
    ac_rps_session_clear(&fixture.bob);

    AC_TEST_ASSERT_EQ_INT(
        make_alice_reveal_fixture(
            &fixture,
            alice_reveal,
            &alice_reveal_length,
            3U),
        0);
    for (prefix = 0U; prefix < alice_reveal_length; ++prefix) {
        adversarial_pair receiver;
        uint8_t receiver_alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
        uint8_t receiver_bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
        size_t receiver_alice_commit_length = 0U;
        size_t receiver_bob_commit_length = 0U;

        AC_TEST_ASSERT_EQ_INT(
            init_pair(&receiver, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_SCISSORS, 3U), 0);
        AC_TEST_ASSERT_EQ_INT(
            make_both_committed(
                &receiver,
                receiver_alice_commit,
                &receiver_alice_commit_length,
                receiver_bob_commit,
                &receiver_bob_commit_length),
            0);
        AC_TEST_ASSERT_EQ_INT(
            expect_receive_rejected_without_mutation(
                &receiver.bob,
                alice_reveal,
                prefix,
                AC_ERR_LENGTH,
                AC_RPS_STATE_BOTH_COMMITTED),
            0);
        ac_rps_session_clear(&receiver.alice);
        ac_rps_session_clear(&receiver.bob);
    }
    ac_rps_session_clear(&fixture.alice);
    ac_rps_session_clear(&fixture.bob);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&fixture, AC_RPS_MOVE_PAPER, AC_RPS_MOVE_ROCK, 4U), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &fixture.alice,
            AC_RPS_ABORT_UNSPECIFIED,
            alice_abort,
            sizeof alice_abort,
            &alice_abort_length),
        AC_OK);
    for (prefix = 0U; prefix < alice_abort_length; ++prefix) {
        adversarial_pair receiver;

        AC_TEST_ASSERT_EQ_INT(
            init_pair(&receiver, AC_RPS_MOVE_PAPER, AC_RPS_MOVE_ROCK, 4U), 0);
        AC_TEST_ASSERT_EQ_INT(
            expect_receive_rejected_without_mutation(
                &receiver.bob,
                alice_abort,
                prefix,
                AC_ERR_LENGTH,
                AC_RPS_STATE_READY),
            0);
        ac_rps_session_clear(&receiver.alice);
        ac_rps_session_clear(&receiver.bob);
    }
    return 0;
}

static int test_length_and_framing_rejections(void)
{
    adversarial_pair fixture;
    adversarial_pair receiver;
    uint8_t commit[AC_RPS_MESSAGE_MAX_BYTES + 1U];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t mutated[AC_RPS_MESSAGE_MAX_BYTES + 1U];
    size_t commit_length = 0U;
    size_t bob_commit_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&fixture, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 8U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_commit_messages(
            &fixture,
            commit,
            &commit_length,
            bob_commit,
            &bob_commit_length),
        0);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&receiver, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 8U), 0);
    (void)memcpy(mutated, commit, commit_length);
    mutated[commit_length] = 0U;
    AC_TEST_ASSERT_EQ_INT(
        expect_receive_rejected_without_mutation(
            &receiver.bob,
            mutated,
            commit_length + 1U,
            AC_ERR_LENGTH,
            AC_RPS_STATE_READY),
        0);
    ac_rps_session_clear(&receiver.alice);
    ac_rps_session_clear(&receiver.bob);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&receiver, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 8U), 0);
    (void)memset(mutated, 0, sizeof mutated);
    (void)memcpy(mutated, commit, commit_length);
    AC_TEST_ASSERT_EQ_INT(
        expect_receive_rejected_without_mutation(
            &receiver.bob,
            mutated,
            AC_RPS_MESSAGE_MAX_BYTES + 1U,
            AC_ERR_LENGTH,
            AC_RPS_STATE_READY),
        0);
    ac_rps_session_clear(&receiver.alice);
    ac_rps_session_clear(&receiver.bob);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&receiver, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 8U), 0);
    (void)memcpy(mutated, commit, commit_length);
    mutated[WIRE_OFFSET_LENGTH] = 0U;
    mutated[WIRE_OFFSET_LENGTH + 1U] = 31U;
    AC_TEST_ASSERT_EQ_INT(
        expect_receive_rejected_without_mutation(
            &receiver.bob,
            mutated,
            commit_length,
            AC_ERR_LENGTH,
            AC_RPS_STATE_READY),
        0);
    ac_rps_session_clear(&receiver.alice);
    ac_rps_session_clear(&receiver.bob);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&receiver, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 8U), 0);
    (void)memcpy(mutated, commit, commit_length);
    mutated[WIRE_OFFSET_TYPE] = AC_RPS_MESSAGE_ABORT;
    AC_TEST_ASSERT_EQ_INT(
        expect_receive_rejected_without_mutation(
            &receiver.bob,
            mutated,
            commit_length,
            AC_ERR_LENGTH,
            AC_RPS_STATE_READY),
        0);
    return 0;
}

static int test_header_and_context_mutations(void)
{
    adversarial_pair fixture;
    uint8_t commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t mutated[AC_RPS_MESSAGE_MAX_BYTES];
    size_t commit_length = 0U;
    size_t bob_commit_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&fixture, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 12U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_commit_messages(
            &fixture,
            commit,
            &commit_length,
            bob_commit,
            &bob_commit_length),
        0);

#define EXPECT_COMMIT_MUTATION(expected_status)                               \
    do {                                                                      \
        adversarial_pair receiver;                                            \
        AC_TEST_ASSERT_EQ_INT(                                                \
            init_pair(                                                        \
                &receiver,                                                    \
                AC_RPS_MOVE_ROCK,                                             \
                AC_RPS_MOVE_PAPER,                                            \
                12U),                                                         \
            0);                                                               \
        AC_TEST_ASSERT_EQ_INT(                                                \
            expect_receive_rejected_without_mutation(                         \
                &receiver.bob,                                                \
                mutated,                                                      \
                commit_length,                                                \
                (expected_status),                                            \
                AC_RPS_STATE_READY),                                          \
            0);                                                               \
        ac_rps_session_clear(&receiver.alice);                                \
        ac_rps_session_clear(&receiver.bob);                                  \
    } while (0)

    (void)memcpy(mutated, commit, commit_length);
    mutated[0] ^= 1U;
    EXPECT_COMMIT_MUTATION(AC_ERR_FORMAT);

    (void)memcpy(mutated, commit, commit_length);
    mutated[WIRE_OFFSET_VERSION + 1U] = 2U;
    EXPECT_COMMIT_MUTATION(AC_ERR_FORMAT);

    (void)memcpy(mutated, commit, commit_length);
    mutated[WIRE_OFFSET_TYPE] = 0xffU;
    EXPECT_COMMIT_MUTATION(AC_ERR_FORMAT);

    (void)memcpy(mutated, commit, commit_length);
    mutated[WIRE_OFFSET_SENDER] = 0U;
    EXPECT_COMMIT_MUTATION(AC_ERR_FORMAT);

    (void)memcpy(mutated, commit, commit_length);
    mutated[WIRE_OFFSET_RECIPIENT] = 0U;
    EXPECT_COMMIT_MUTATION(AC_ERR_FORMAT);

    (void)memcpy(mutated, commit, commit_length);
    mutated[WIRE_OFFSET_RECIPIENT] = AC_RPS_ROLE_ALICE;
    EXPECT_COMMIT_MUTATION(AC_ERR_FORMAT);

    (void)memcpy(mutated, commit, commit_length);
    (void)memset(mutated + WIRE_OFFSET_SESSION, 0, AC_RPS_SESSION_ID_BYTES);
    EXPECT_COMMIT_MUTATION(AC_ERR_FORMAT);

    (void)memcpy(mutated, commit, commit_length);
    (void)memset(mutated + WIRE_OFFSET_ROUND, 0, 4U);
    EXPECT_COMMIT_MUTATION(AC_ERR_FORMAT);

    (void)memcpy(mutated, commit, commit_length);
    mutated[WIRE_OFFSET_SESSION] ^= 0x80U;
    EXPECT_COMMIT_MUTATION(AC_ERR_CONTEXT);

    (void)memcpy(mutated, commit, commit_length);
    mutated[WIRE_OFFSET_ROUND + 3U] ^= 1U;
    EXPECT_COMMIT_MUTATION(AC_ERR_CONTEXT);

    (void)memcpy(mutated, commit, commit_length);
    mutated[WIRE_OFFSET_SENDER] = AC_RPS_ROLE_BOB;
    mutated[WIRE_OFFSET_RECIPIENT] = AC_RPS_ROLE_ALICE;
    EXPECT_COMMIT_MUTATION(AC_ERR_CONTEXT);

#undef EXPECT_COMMIT_MUTATION
    return 0;
}

static int test_invalid_move_and_abort_reason(void)
{
    adversarial_pair pair;
    uint8_t reveal[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t abort_message[AC_RPS_MESSAGE_MAX_BYTES];
    size_t reveal_length = 0U;
    size_t abort_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        make_alice_reveal_fixture(&pair, reveal, &reveal_length, 16U), 0);
    reveal[WIRE_OFFSET_PAYLOAD] = 3U;
    AC_TEST_ASSERT_EQ_INT(
        expect_receive_rejected_without_mutation(
            &pair.bob,
            reveal,
            reveal_length,
            AC_ERR_FORMAT,
            AC_RPS_STATE_BOTH_COMMITTED),
        0);
    ac_rps_session_clear(&pair.alice);
    ac_rps_session_clear(&pair.bob);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 17U), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &pair.alice,
            AC_RPS_ABORT_UNSPECIFIED,
            abort_message,
            sizeof abort_message,
            &abort_length),
        AC_OK);
    abort_message[WIRE_OFFSET_PAYLOAD] = 0U;
    AC_TEST_ASSERT_EQ_INT(
        expect_receive_rejected_without_mutation(
            &pair.bob,
            abort_message,
            abort_length,
            AC_ERR_FORMAT,
            AC_RPS_STATE_READY),
        0);
    return 0;
}

static int test_duplicate_and_conflicting_commitments(void)
{
    adversarial_pair pair;
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t conflicting[AC_RPS_MESSAGE_MAX_BYTES];
    size_t alice_commit_length = 0U;
    size_t bob_commit_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 20U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_commit_messages(
            &pair,
            alice_commit,
            &alice_commit_length,
            bob_commit,
            &bob_commit_length),
        0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, alice_commit, alice_commit_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, alice_commit, alice_commit_length),
        AC_ERR_DUPLICATE);
    AC_TEST_ASSERT_EQ_INT(assert_failed(&pair.bob, AC_ERR_DUPLICATE), 0);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 21U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_commit_messages(
            &pair,
            alice_commit,
            &alice_commit_length,
            bob_commit,
            &bob_commit_length),
        0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, alice_commit, alice_commit_length),
        AC_OK);
    (void)memcpy(conflicting, alice_commit, alice_commit_length);
    conflicting[WIRE_OFFSET_PAYLOAD] ^= 1U;
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, conflicting, alice_commit_length),
        AC_ERR_CONFLICT);
    AC_TEST_ASSERT_EQ_INT(assert_failed(&pair.bob, AC_ERR_CONFLICT), 0);
    return 0;
}

static int test_replay_and_reflection_are_context_errors(void)
{
    adversarial_pair sender;
    adversarial_pair receiver;
    uint8_t commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t reflected[AC_RPS_MESSAGE_MAX_BYTES];
    size_t commit_length = 0U;
    size_t bob_commit_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&sender, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 24U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_commit_messages(
            &sender,
            commit,
            &commit_length,
            bob_commit,
            &bob_commit_length),
        0);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&receiver, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 25U), 0);
    AC_TEST_ASSERT_EQ_INT(
        expect_receive_rejected_without_mutation(
            &receiver.bob,
            commit,
            commit_length,
            AC_ERR_CONTEXT,
            AC_RPS_STATE_READY),
        0);

    AC_TEST_ASSERT_STATUS(
        ac_rps_session_init_with_nonce_for_test(
            &receiver.bob,
            AC_RPS_ROLE_BOB,
            AC_RPS_MOVE_PAPER,
            sender.session_id,
            sender.round + 1U,
            &(ac_commitment_nonce){{
                1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U,
                9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U,
                17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U,
                25U, 26U, 27U, 28U, 29U, 30U, 31U, 32U
            }}),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        expect_receive_rejected_without_mutation(
            &receiver.bob,
            commit,
            commit_length,
            AC_ERR_CONTEXT,
            AC_RPS_STATE_READY),
        0);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&receiver, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 24U), 0);
    (void)memcpy(reflected, commit, commit_length);
    reflected[WIRE_OFFSET_SENDER] = AC_RPS_ROLE_BOB;
    reflected[WIRE_OFFSET_RECIPIENT] = AC_RPS_ROLE_ALICE;
    AC_TEST_ASSERT_EQ_INT(
        expect_receive_rejected_without_mutation(
            &receiver.bob,
            reflected,
            commit_length,
            AC_ERR_CONTEXT,
            AC_RPS_STATE_READY),
        0);
    return 0;
}

static int test_premature_reveal_is_protocol_failure(void)
{
    adversarial_pair fixture;
    adversarial_pair receiver;
    uint8_t reveal[AC_RPS_MESSAGE_MAX_BYTES];
    size_t reveal_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        make_alice_reveal_fixture(&fixture, reveal, &reveal_length, 28U), 0);
    AC_TEST_ASSERT_EQ_INT(
        init_pair(&receiver, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_SCISSORS, 28U), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&receiver.bob, reveal, reveal_length),
        AC_ERR_STATE);
    AC_TEST_ASSERT_EQ_INT(assert_failed(&receiver.bob, AC_ERR_STATE), 0);
    return 0;
}

static int test_invalid_openings_and_unopenable_commitment(void)
{
    unsigned mutation;

    for (mutation = 0U; mutation < 3U; ++mutation) {
        adversarial_pair pair;
        uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
        uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
        uint8_t alice_reveal[AC_RPS_MESSAGE_MAX_BYTES];
        uint8_t before[AC_RPS_TRANSCRIPT_MAX_BYTES];
        uint8_t after[AC_RPS_TRANSCRIPT_MAX_BYTES];
        size_t alice_commit_length = 0U;
        size_t bob_commit_length = 0U;
        size_t alice_reveal_length = 0U;
        size_t before_length = 0U;
        size_t after_length = 0U;

        AC_TEST_ASSERT_EQ_INT(
            init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_SCISSORS, (uint8_t)(32U + mutation)),
            0);
        AC_TEST_ASSERT_EQ_INT(
            make_commit_messages(
                &pair,
                alice_commit,
                &alice_commit_length,
                bob_commit,
                &bob_commit_length),
            0);
        if (mutation == 2U) {
            alice_commit[WIRE_OFFSET_PAYLOAD + 7U] ^= 1U;
        }
        AC_TEST_ASSERT_STATUS(
            ac_rps_receive(&pair.bob, alice_commit, alice_commit_length),
            AC_OK);
        AC_TEST_ASSERT_STATUS(
            ac_rps_receive(&pair.alice, bob_commit, bob_commit_length),
            AC_OK);
        AC_TEST_ASSERT_STATUS(
            ac_rps_make_reveal(
                &pair.alice,
                alice_reveal,
                sizeof alice_reveal,
                &alice_reveal_length),
            AC_OK);
        if (mutation == 0U) {
            alice_reveal[WIRE_OFFSET_PAYLOAD] = AC_RPS_MOVE_PAPER;
        } else if (mutation == 1U) {
            alice_reveal[WIRE_OFFSET_PAYLOAD + 1U + 5U] ^= 1U;
        }

        AC_TEST_ASSERT_EQ_INT(
            export_transcript(&pair.bob, before, &before_length), 0);
        AC_TEST_ASSERT_STATUS(
            ac_rps_receive(&pair.bob, alice_reveal, alice_reveal_length),
            AC_ERR_INVALID_OPENING);
        AC_TEST_ASSERT_EQ_INT(
            assert_failed(&pair.bob, AC_ERR_INVALID_OPENING), 0);
        AC_TEST_ASSERT_EQ_INT(
            export_transcript(&pair.bob, after, &after_length), 0);
        AC_TEST_ASSERT_EQ_INT(after_length, before_length);
        AC_TEST_ASSERT_MEMORY(after, before, before_length);
    }
    return 0;
}

static int test_duplicate_reveal_and_abort(void)
{
    adversarial_pair pair;
    uint8_t reveal[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t abort_message[AC_RPS_MESSAGE_MAX_BYTES];
    size_t reveal_length = 0U;
    size_t abort_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        make_alice_reveal_fixture(&pair, reveal, &reveal_length, 38U), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, reveal, reveal_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, reveal, reveal_length),
        AC_ERR_DUPLICATE);
    AC_TEST_ASSERT_EQ_INT(assert_failed(&pair.bob, AC_ERR_DUPLICATE), 0);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 39U), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &pair.alice,
            AC_RPS_ABORT_PROTOCOL_POLICY,
            abort_message,
            sizeof abort_message,
            &abort_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, abort_message, abort_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, abort_message, abort_length),
        AC_ERR_TERMINAL);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.bob, AC_RPS_STATE_ABORTED_PEER), 0);
    return 0;
}

static int test_abort_after_commit_and_timeout_missing_reveal(void)
{
    adversarial_pair pair;
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t alice_reveal[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t abort_message[AC_RPS_MESSAGE_MAX_BYTES];
    size_t alice_commit_length = 0U;
    size_t bob_commit_length = 0U;
    size_t alice_reveal_length = 0U;
    size_t abort_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_PAPER, AC_RPS_MOVE_SCISSORS, 42U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_both_committed(
            &pair,
            alice_commit,
            &alice_commit_length,
            bob_commit,
            &bob_commit_length),
        0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &pair.bob,
            AC_RPS_ABORT_APPLICATION_REQUEST,
            abort_message,
            sizeof abort_message,
            &abort_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.alice, abort_message, abort_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.bob, AC_RPS_STATE_ABORTED_LOCAL), 0);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_ABORTED_PEER), 0);
    AC_TEST_ASSERT_EQ_INT(pair.bob.internal_local_secret_cleared, 1U);
    AC_TEST_ASSERT_ZERO(
        &pair.bob.internal_local_nonce,
        sizeof pair.bob.internal_local_nonce);
    AC_TEST_ASSERT_EQ_INT(pair.alice.internal_local_secret_cleared, 1U);
    AC_TEST_ASSERT_ZERO(
        &pair.alice.internal_local_nonce,
        sizeof pair.alice.internal_local_nonce);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_SCISSORS, 43U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_both_committed(
            &pair,
            alice_commit,
            &alice_commit_length,
            bob_commit,
            &bob_commit_length),
        0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_reveal(
            &pair.alice,
            alice_reveal,
            sizeof alice_reveal,
            &alice_reveal_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(ac_rps_mark_timeout(&pair.alice), AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_TIMED_OUT), 0);
    return 0;
}

static int test_selective_abort_asymmetry(void)
{
    const ac_rps_move bob_private_move = AC_RPS_MOVE_SCISSORS;
    adversarial_pair pair;
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t alice_reveal[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_abort[AC_RPS_MESSAGE_MAX_BYTES];
    size_t alice_commit_length = 0U;
    size_t bob_commit_length = 0U;
    size_t alice_reveal_length = 0U;
    size_t bob_abort_length = 0U;
    ac_rps_move bob_observed_alice_move = AC_RPS_MOVE_PAPER;
    ac_rps_move alice_observed_bob_move = AC_RPS_MOVE_ROCK;
    ac_rps_outcome api_outcome = AC_RPS_OUTCOME_TIE;
    ac_rps_outcome bob_derived_outcome;
    uint8_t alice_transcript[AC_RPS_TRANSCRIPT_MAX_BYTES];
    uint8_t bob_transcript[AC_RPS_TRANSCRIPT_MAX_BYTES];
    size_t alice_transcript_length = 0U;
    size_t bob_transcript_length = 0U;
    unsigned difference;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, bob_private_move, 46U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_both_committed(
            &pair,
            alice_commit,
            &alice_commit_length,
            bob_commit,
            &bob_commit_length),
        0);

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
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_peer_move_if_revealed(
            &pair.bob,
            &bob_observed_alice_move),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(bob_observed_alice_move, AC_RPS_MOVE_ROCK);

    difference =
        ((unsigned)bob_observed_alice_move + 3U -
         (unsigned)bob_private_move) % 3U;
    bob_derived_outcome = difference == 0U
                              ? AC_RPS_OUTCOME_TIE
                              : (difference == 1U
                                     ? AC_RPS_OUTCOME_ALICE_WINS
                                     : AC_RPS_OUTCOME_BOB_WINS);
    AC_TEST_ASSERT_EQ_INT(bob_derived_outcome, AC_RPS_OUTCOME_ALICE_WINS);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_outcome(&pair.bob, &api_outcome),
        AC_ERR_STATE);

    AC_TEST_ASSERT_STATUS(
        ac_rps_get_peer_move_if_revealed(
            &pair.alice,
            &alice_observed_bob_move),
        AC_ERR_STATE);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_outcome(&pair.alice, &api_outcome),
        AC_ERR_STATE);

    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &pair.bob,
            AC_RPS_ABORT_APPLICATION_REQUEST,
            bob_abort,
            sizeof bob_abort,
            &bob_abort_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.alice, bob_abort, bob_abort_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.bob, AC_RPS_STATE_ABORTED_LOCAL), 0);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_ABORTED_PEER), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_outcome(&pair.bob, &api_outcome),
        AC_ERR_STATE);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_outcome(&pair.alice, &api_outcome),
        AC_ERR_STATE);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_peer_move_if_revealed(
            &pair.alice,
            &alice_observed_bob_move),
        AC_ERR_STATE);
    AC_TEST_ASSERT_EQ_INT(
        export_transcript(
            &pair.alice,
            alice_transcript,
            &alice_transcript_length),
        0);
    AC_TEST_ASSERT_EQ_INT(
        export_transcript(
            &pair.bob,
            bob_transcript,
            &bob_transcript_length),
        0);
    AC_TEST_ASSERT_EQ_INT(alice_transcript_length, bob_transcript_length);
    AC_TEST_ASSERT_MEMORY(
        alice_transcript,
        bob_transcript,
        alice_transcript_length);
    return 0;
}

static int test_dropped_delivery_causes_transcript_disagreement(void)
{
    adversarial_pair pair;
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t alice_reveal[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t alice_transcript[AC_RPS_TRANSCRIPT_MAX_BYTES];
    uint8_t bob_transcript[AC_RPS_TRANSCRIPT_MAX_BYTES];
    size_t alice_commit_length = 0U;
    size_t bob_commit_length = 0U;
    size_t alice_reveal_length = 0U;
    size_t alice_transcript_length = 0U;
    size_t bob_transcript_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_PAPER, AC_RPS_MOVE_ROCK, 50U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_both_committed(
            &pair,
            alice_commit,
            &alice_commit_length,
            bob_commit,
            &bob_commit_length),
        0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_reveal(
            &pair.alice,
            alice_reveal,
            sizeof alice_reveal,
            &alice_reveal_length),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        export_transcript(
            &pair.alice,
            alice_transcript,
            &alice_transcript_length),
        0);
    AC_TEST_ASSERT_EQ_INT(
        export_transcript(
            &pair.bob,
            bob_transcript,
            &bob_transcript_length),
        0);
    AC_TEST_ASSERT(alice_transcript_length != bob_transcript_length);
    AC_TEST_ASSERT_STATUS(ac_rps_mark_timeout(&pair.alice), AC_OK);
    AC_TEST_ASSERT_STATUS(ac_rps_mark_timeout(&pair.bob), AC_OK);
    return 0;
}

static int test_terminal_states_reject_further_actions(void)
{
    adversarial_pair pair;
    uint8_t alice_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_commit[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t alice_reveal[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t bob_reveal[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t message[AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t peer_message[AC_RPS_MESSAGE_MAX_BYTES];
    size_t alice_commit_length = 0U;
    size_t bob_commit_length = 0U;
    size_t alice_reveal_length = 0U;
    size_t bob_reveal_length = 0U;
    size_t message_length = 0U;
    size_t peer_message_length = 0U;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 54U), 0);
    AC_TEST_ASSERT_STATUS(ac_rps_mark_timeout(&pair.alice), AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair.alice,
            message,
            sizeof message,
            &message_length),
        AC_ERR_TERMINAL);
    AC_TEST_ASSERT_STATUS(ac_rps_mark_timeout(&pair.alice), AC_ERR_TERMINAL);

    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &pair.bob,
            AC_RPS_ABORT_UNSPECIFIED,
            peer_message,
            sizeof peer_message,
            &peer_message_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.alice, peer_message, peer_message_length),
        AC_ERR_TERMINAL);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 55U), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &pair.alice,
            AC_RPS_ABORT_UNSPECIFIED,
            message,
            sizeof message,
            &message_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair.alice,
            message,
            sizeof message,
            &message_length),
        AC_ERR_TERMINAL);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 56U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_both_committed(
            &pair,
            alice_commit,
            &alice_commit_length,
            bob_commit,
            &bob_commit_length),
        0);
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
        assert_state(&pair.alice, AC_RPS_STATE_COMPLETE), 0);
    AC_TEST_ASSERT_STATUS(ac_rps_mark_timeout(&pair.alice), AC_ERR_TERMINAL);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &pair.alice,
            AC_RPS_ABORT_UNSPECIFIED,
            message,
            sizeof message,
            &message_length),
        AC_ERR_TERMINAL);

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 57U), 0);
    AC_TEST_ASSERT_EQ_INT(
        make_commit_messages(
            &pair,
            alice_commit,
            &alice_commit_length,
            bob_commit,
            &bob_commit_length),
        0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, alice_commit, alice_commit_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_receive(&pair.bob, alice_commit, alice_commit_length),
        AC_ERR_DUPLICATE);
    AC_TEST_ASSERT_EQ_INT(assert_failed(&pair.bob, AC_ERR_DUPLICATE), 0);
    AC_TEST_ASSERT_STATUS(ac_rps_mark_timeout(&pair.bob), AC_ERR_TERMINAL);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &pair.bob,
            AC_RPS_ABORT_UNSPECIFIED,
            message,
            sizeof message,
            &message_length),
        AC_ERR_TERMINAL);
    return 0;
}

static int test_local_misuse_does_not_accuse_peer(void)
{
    adversarial_pair pair;
    uint8_t message[AC_RPS_MESSAGE_MAX_BYTES];
    size_t message_length = 0U;
    ac_status failure = AC_OK;

    AC_TEST_ASSERT_EQ_INT(
        init_pair(&pair, AC_RPS_MOVE_ROCK, AC_RPS_MOVE_PAPER, 58U), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_abort(
            &pair.alice,
            (ac_rps_abort_reason)0,
            message,
            sizeof message,
            &message_length),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_READY), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_reveal(
            &pair.alice,
            message,
            sizeof message,
            &message_length),
        AC_ERR_STATE);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_READY), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_failure(&pair.alice, &failure),
        AC_ERR_STATE);

    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair.alice,
            message,
            sizeof message,
            &message_length),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_rps_make_commit(
            &pair.alice,
            message,
            sizeof message,
            &message_length),
        AC_ERR_STATE);
    AC_TEST_ASSERT_EQ_INT(
        assert_state(&pair.alice, AC_RPS_STATE_COMMIT_SENT), 0);
    AC_TEST_ASSERT_STATUS(
        ac_rps_get_failure(&pair.alice, &failure),
        AC_ERR_STATE);
    return 0;
}

int main(void)
{
    ac_test_suite suite = {0U, 0U};

    AC_TEST_RUN(suite, test_every_message_truncation_prefix);
    AC_TEST_RUN(suite, test_length_and_framing_rejections);
    AC_TEST_RUN(suite, test_header_and_context_mutations);
    AC_TEST_RUN(suite, test_invalid_move_and_abort_reason);
    AC_TEST_RUN(suite, test_duplicate_and_conflicting_commitments);
    AC_TEST_RUN(suite, test_replay_and_reflection_are_context_errors);
    AC_TEST_RUN(suite, test_premature_reveal_is_protocol_failure);
    AC_TEST_RUN(suite, test_invalid_openings_and_unopenable_commitment);
    AC_TEST_RUN(suite, test_duplicate_reveal_and_abort);
    AC_TEST_RUN(suite, test_abort_after_commit_and_timeout_missing_reveal);
    AC_TEST_RUN(suite, test_selective_abort_asymmetry);
    AC_TEST_RUN(suite, test_dropped_delivery_causes_transcript_disagreement);
    AC_TEST_RUN(suite, test_terminal_states_reject_further_actions);
    AC_TEST_RUN(suite, test_local_misuse_does_not_accuse_peer);
    return ac_test_finish(&suite);
}
