#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ac/bool_circuit.h"
#include "ac/commitment.h"
#include "ac/ttt.h"
#include "ac/ttt_bind.h"
#include "ac/ttt_core_circuit.h"
#include "test_support.h"

static const uint32_t vector_statement_round = UINT32_C(0x01020304);
static const uint32_t reference_vector_statement_round =
    UINT32_C(0x11223344);

static const uint8_t vector_digest[AC_COMMITMENT_DIGEST_BYTES] = {
    0xb5U, 0x31U, 0x30U, 0xb6U, 0xc3U, 0xc2U, 0xebU, 0xf8U,
    0xcaU, 0x9fU, 0xfbU, 0x05U, 0x35U, 0x8aU, 0x61U, 0xdeU,
    0x82U, 0x0eU, 0xe4U, 0x65U, 0x58U, 0x7dU, 0xafU, 0xacU,
    0x77U, 0x20U, 0x06U, 0x0eU, 0x06U, 0xb5U, 0x1aU, 0x00U
};

static const uint8_t reference_vector_digest[AC_COMMITMENT_DIGEST_BYTES] = {
    0x45U, 0x38U, 0xc8U, 0x75U, 0xf4U, 0xe9U, 0x19U, 0x66U,
    0x50U, 0x30U, 0xe7U, 0x95U, 0x0bU, 0x0eU, 0xa9U, 0x2cU,
    0x2dU, 0xf4U, 0xf1U, 0x94U, 0xc1U, 0x0fU, 0x2eU, 0x02U,
    0x1fU, 0xd7U, 0x03U, 0x63U, 0xe7U, 0x22U, 0xf3U, 0x78U
};

static ac_bool_gate reference_vector_core_gates[
    AC_TTT_CORE_X_GATE_COUNT];
static ac_ttt_core_builder_scratch reference_vector_core_builder;
static uint8_t reference_vector_core_wires[AC_TTT_CORE_X_WIRE_COUNT];

static void fill_sequence(uint8_t *output, size_t length, uint8_t first)
{
    size_t index;

    for (index = 0U; index < length; ++index) {
        output[index] = (uint8_t)(first + (uint8_t)index);
    }
}

static void make_vector_session(
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES])
{
    fill_sequence(session_id, AC_COMMITMENT_SESSION_ID_BYTES, 0x40U);
}

static void make_vector_nonce(ac_commitment_nonce *nonce)
{
    fill_sequence(nonce->bytes, sizeof nonce->bytes, 0xa0U);
}

static void make_reference_vector_session(
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES])
{
    fill_sequence(session_id, AC_COMMITMENT_SESSION_ID_BYTES, 0x10U);
}

static void make_reference_vector_nonce(ac_commitment_nonce *nonce)
{
    fill_sequence(nonce->bytes, sizeof nonce->bytes, 0x80U);
}

static void make_vector_policy(ac_ttt_policy *policy)
{
    size_t index;

    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        policy->move_by_board[index] =
            (uint8_t)((17U * index + 3U) & 0xffU);
    }
}

static int test_context_and_packed_encodings(void)
{
    static const uint8_t expected_protocol_id[
        AC_COMMITMENT_PROTOCOL_ID_BYTES] = {
        'A', 'C', '-', 'T', 'T', 'T', '-', 'P',
        'O', 'L', 'I', 'C', 'Y', '-', 'V', '1'
    };
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES];
    uint8_t payload[AC_TTT_BIND_PAYLOAD_BYTES];
    uint8_t public_input[AC_TTT_BIND_PUBLIC_INPUT_BYTES];
    uint8_t witness[AC_TTT_BIND_WITNESS_INPUT_BYTES];
    ac_ttt_policy policy;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;
    ac_commitment_context context;

    make_vector_session(session_id);
    make_vector_policy(&policy);
    make_vector_nonce(&nonce);
    (void)memcpy(digest.bytes, vector_digest, sizeof digest.bytes);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_context_init(
            session_id, vector_statement_round, &context),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(
        context.protocol_id,
        expected_protocol_id,
        sizeof expected_protocol_id);
    AC_TEST_ASSERT_EQ_INT(
        context.protocol_version, AC_TTT_BIND_PROFILE_VERSION);
    AC_TEST_ASSERT_MEMORY(
        context.session_id, session_id, sizeof context.session_id);
    AC_TEST_ASSERT_EQ_INT(context.round, vector_statement_round);
    AC_TEST_ASSERT_EQ_INT(
        context.committer_role, AC_TTT_BIND_PROTOCOL_ROLE_PROVER);
    AC_TEST_ASSERT_EQ_INT(
        context.recipient_role, AC_TTT_BIND_PROTOCOL_ROLE_VERIFIER);
    AC_TEST_ASSERT_EQ_INT(
        context.payload_type, AC_TTT_BIND_PAYLOAD_TYPE_POLICY);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_encode_payload(
            AC_TTT_ROLE_X, &policy, payload),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(payload[0], AC_TTT_GAME_VERSION);
    AC_TEST_ASSERT_EQ_INT(payload[1], AC_TTT_ROLE_X);
    AC_TEST_ASSERT_EQ_INT(payload[2], 0U);
    AC_TEST_ASSERT_EQ_INT(payload[3], 0U);
    AC_TEST_ASSERT_MEMORY(
        payload + AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES,
        policy.move_by_board,
        sizeof policy.move_by_board);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_encode_public_input(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &digest,
            public_input),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(public_input, payload, 4U);
    AC_TEST_ASSERT_MEMORY(
        public_input + AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES,
        session_id,
        sizeof session_id);
    AC_TEST_ASSERT_MEMORY(
        public_input +
            AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES +
            AC_COMMITMENT_SESSION_ID_BYTES,
        "\x01\x02\x03\x04",
        4U);
    AC_TEST_ASSERT_MEMORY(
        public_input +
            AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES +
            AC_COMMITMENT_SESSION_ID_BYTES +
            4U,
        digest.bytes,
        sizeof digest.bytes);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_encode_witness(&policy, &nonce, witness),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(
        witness, policy.move_by_board, sizeof policy.move_by_board);
    AC_TEST_ASSERT_MEMORY(
        witness + AC_TTT_STATE_COUNT,
        nonce.bytes,
        sizeof nonce.bytes);
    AC_TEST_ASSERT_EQ_INT(
        AC_TTT_BIND_COMMITMENT_MESSAGE_BYTES, 19809U);
    AC_TEST_ASSERT_EQ_INT(AC_TTT_BIND_BLAKE2B_BLOCK_COUNT, 155U);
    return 0;
}

static int test_published_profile_vector(void)
{
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES];
    ac_ttt_policy policy;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;

    make_vector_session(session_id);
    make_vector_policy(&policy);
    make_vector_nonce(&nonce);
    (void)memset(&digest, 0, sizeof digest);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_compute_with_nonce(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(
        digest.bytes, vector_digest, sizeof vector_digest);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_OK);
    return 0;
}

static int test_positive_reference_policy_cross_layer_vector(void)
{
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES];
    uint8_t core_public[AC_TTT_CORE_PUBLIC_INPUT_BYTES];
    uint8_t bind_public[AC_TTT_BIND_PUBLIC_INPUT_BYTES];
    uint8_t bind_witness[AC_TTT_BIND_WITNESS_INPUT_BYTES];
    ac_ttt_policy policy;
    ac_ttt_report recursive_report;
    ac_ttt_dag_report dag_report;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;
    ac_bool_circuit core_circuit;
    ac_ttt_core_circuit_report core_report;
    uint8_t core_acceptance = UINT8_C(0xa5);

    make_reference_vector_session(session_id);
    make_reference_vector_nonce(&nonce);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(
            AC_TTT_ROLE_X, &policy, &recursive_report),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        recursive_report.verdict, AC_TTT_VERDICT_NON_LOSING);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_evaluate_policy_dag(
            AC_TTT_ROLE_X, &policy, &dag_report),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        dag_report.verdict, AC_TTT_VERDICT_NON_LOSING);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_compute_with_nonce(
            AC_TTT_ROLE_X,
            session_id,
            reference_vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(
        digest.bytes,
        reference_vector_digest,
        sizeof reference_vector_digest);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            session_id,
            reference_vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_OK);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_core_encode_public_input(
            AC_TTT_ROLE_X, core_public),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_encode_public_input(
            AC_TTT_ROLE_X,
            session_id,
            reference_vector_statement_round,
            &digest,
            bind_public),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_encode_witness(&policy, &nonce, bind_witness),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(
        bind_public, core_public, AC_TTT_CORE_PUBLIC_INPUT_BYTES);
    AC_TEST_ASSERT_MEMORY(
        bind_witness,
        policy.move_by_board,
        AC_TTT_CORE_WITNESS_INPUT_BYTES);
    AC_TEST_ASSERT_MEMORY(
        bind_witness + AC_TTT_CORE_WITNESS_INPUT_BYTES,
        nonce.bytes,
        sizeof nonce.bytes);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_core_circuit_build(
            AC_TTT_ROLE_X,
            reference_vector_core_gates,
            AC_TTT_CORE_X_GATE_COUNT,
            &reference_vector_core_builder,
            &core_circuit,
            &core_report),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &core_circuit,
            bind_public,
            AC_TTT_CORE_PUBLIC_INPUT_BYTES,
            bind_witness,
            AC_TTT_CORE_WITNESS_INPUT_BYTES,
            reference_vector_core_wires,
            sizeof reference_vector_core_wires,
            &core_acceptance),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(core_acceptance, 1U);
    AC_TEST_ASSERT_EQ_INT(
        core_report.gate_count, AC_TTT_CORE_X_GATE_COUNT);
    return 0;
}

static int test_profile_mutations_are_rejected(void)
{
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES];
    uint8_t changed_session[AC_COMMITMENT_SESSION_ID_BYTES];
    uint32_t changed_round;
    ac_ttt_policy policy;
    ac_commitment_nonce nonce;
    ac_commitment_nonce changed_nonce;
    ac_commitment_digest digest;
    ac_commitment_digest changed_digest;
    uint8_t saved_policy_byte;

    make_vector_session(session_id);
    make_vector_policy(&policy);
    make_vector_nonce(&nonce);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_compute_with_nonce(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_OK);

    saved_policy_byte = policy.move_by_board[1234U];
    policy.move_by_board[1234U] ^= 1U;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_ERR_INVALID_OPENING);
    policy.move_by_board[1234U] = saved_policy_byte;

    changed_nonce = nonce;
    changed_nonce.bytes[17] ^= 1U;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            &changed_nonce,
            &digest),
        AC_ERR_INVALID_OPENING);

    (void)memcpy(changed_session, session_id, sizeof changed_session);
    changed_session[31] ^= 1U;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            changed_session,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_ERR_INVALID_OPENING);

    changed_round = vector_statement_round + 1U;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            session_id,
            changed_round,
            &policy,
            &nonce,
            &digest),
        AC_ERR_INVALID_OPENING);

    changed_digest = digest;
    changed_digest.bytes[9] ^= 1U;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            &nonce,
            &changed_digest),
        AC_ERR_INVALID_OPENING);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_O,
            session_id,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_ERR_INVALID_OPENING);
    return 0;
}

static int test_randomized_create_and_nonce_clear(void)
{
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES];
    ac_ttt_policy policy;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;

    make_vector_session(session_id);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_create(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            &digest,
            &nonce),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_OK);
    ac_commitment_nonce_clear(&nonce);
    AC_TEST_ASSERT_ZERO(nonce.bytes, sizeof nonce.bytes);
    return 0;
}

static int test_argument_failures_and_output_clearing(void)
{
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES];
    uint8_t zero_session[AC_COMMITMENT_SESSION_ID_BYTES] = {0U};
    uint8_t payload[AC_TTT_BIND_PAYLOAD_BYTES];
    uint8_t public_input[AC_TTT_BIND_PUBLIC_INPUT_BYTES];
    uint8_t witness[AC_TTT_BIND_WITNESS_INPUT_BYTES];
    ac_ttt_policy policy;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;
    ac_commitment_context context;
    union {
        ac_ttt_policy policy;
        uint8_t payload[AC_TTT_BIND_PAYLOAD_BYTES];
    } overlapping_storage;
    uint8_t first_policy_byte;

    make_vector_session(session_id);
    make_vector_policy(&policy);
    make_vector_nonce(&nonce);
    (void)memset(&digest, 0xa5, sizeof digest);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_compute_with_nonce(
            (ac_ttt_role)0,
            session_id,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(digest.bytes, sizeof digest.bytes);

    (void)memset(&digest, 0xa5, sizeof digest);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_compute_with_nonce(
            AC_TTT_ROLE_X,
            zero_session,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_ERR_CONTEXT);
    AC_TEST_ASSERT_ZERO(digest.bytes, sizeof digest.bytes);

    (void)memset(&digest, 0xa5, sizeof digest);
    (void)memset(&nonce, 0xa5, sizeof nonce);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_create(
            AC_TTT_ROLE_X,
            zero_session,
            vector_statement_round,
            &policy,
            &digest,
            &nonce),
        AC_ERR_CONTEXT);
    AC_TEST_ASSERT_ZERO(digest.bytes, sizeof digest.bytes);
    AC_TEST_ASSERT_ZERO(nonce.bytes, sizeof nonce.bytes);

    (void)memset(&context, 0xa5, sizeof context);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_context_init(
            zero_session, vector_statement_round, &context),
        AC_ERR_CONTEXT);
    AC_TEST_ASSERT_ZERO(&context, sizeof context);

    (void)memset(payload, 0xa5, sizeof payload);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_encode_payload(
            (ac_ttt_role)0, &policy, payload),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(payload, sizeof payload);

    (void)memset(public_input, 0xa5, sizeof public_input);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_encode_public_input(
            AC_TTT_ROLE_X,
            zero_session,
            vector_statement_round,
            &digest,
            public_input),
        AC_ERR_CONTEXT);
    AC_TEST_ASSERT_ZERO(public_input, sizeof public_input);

    (void)memset(witness, 0xa5, sizeof witness);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_encode_witness(NULL, &nonce, witness),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(witness, sizeof witness);

    overlapping_storage.policy = policy;
    first_policy_byte = overlapping_storage.policy.move_by_board[0];
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_encode_payload(
            AC_TTT_ROLE_X,
            &overlapping_storage.policy,
            overlapping_storage.payload),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(
        overlapping_storage.policy.move_by_board[0],
        first_policy_byte);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_context_init(
            session_id, vector_statement_round, NULL),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            NULL,
            &digest),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_circuit_measure(NULL),
        AC_ERR_ARGUMENT);
    return 0;
}

static int test_bind_does_not_check_policy_semantics(void)
{
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES];
    ac_ttt_policy policy;
    ac_ttt_report semantic_report;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;

    make_vector_session(session_id);
    make_vector_policy(&policy);
    make_vector_nonce(&nonce);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_compute_with_nonce(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            session_id,
            vector_statement_round,
            &policy,
            &nonce,
            &digest),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(
            AC_TTT_ROLE_X, &policy, &semantic_report),
        AC_OK);
    AC_TEST_ASSERT(
        semantic_report.verdict != AC_TTT_VERDICT_NON_LOSING);
    return 0;
}

static int test_minimum_and_full_adder_identity(void)
{
    uint8_t left;
    uint8_t right;
    uint8_t carry;

    for (left = 0U; left <= 1U; ++left) {
        for (right = 0U; right <= 1U; ++right) {
            for (carry = 0U; carry <= 1U; ++carry) {
                uint8_t sum;
                uint8_t carry_out;
                uint8_t expected;

                sum = (uint8_t)((left ^ carry) ^ right);
                carry_out = (uint8_t)(
                    ((left ^ carry) & (right ^ carry)) ^ carry);
                expected = (uint8_t)(left + right + carry);
                AC_TEST_ASSERT_EQ_INT(sum, expected & 1U);
                AC_TEST_ASSERT_EQ_INT(carry_out, expected >> 1);
            }
        }
    }
    return 0;
}

static int test_bind_symbolic_measurement(void)
{
    const uint64_t compression_and =
        UINT64_C(96) * UINT64_C(6) * UINT64_C(63);
    const uint64_t compression_xor =
        UINT64_C(96) *
            (UINT64_C(6) * UINT64_C(251) +
             UINT64_C(4) * UINT64_C(64)) +
        UINT64_C(1024);
    const uint64_t expected_and =
        UINT64_C(155) * compression_and +
        UINT64_C(255) +
        UINT64_C(255) +
        UINT64_C(1);
    const uint64_t expected_xor =
        UINT64_C(155) * compression_xor +
        UINT64_C(512) +
        UINT64_C(257);
    ac_ttt_bind_circuit_report report;

    AC_TEST_ASSERT_EQ_INT(compression_and, 36288U);
    AC_TEST_ASSERT_EQ_INT(compression_xor, 170176U);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_bind_circuit_measure(&report),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(report.commitment_message_bytes, 19809U);
    AC_TEST_ASSERT_EQ_INT(report.blake2b_block_count, 155U);
    AC_TEST_ASSERT_EQ_INT(report.public_input_bits, 576U);
    AC_TEST_ASSERT_EQ_INT(report.witness_input_bits, 157720U);
    AC_TEST_ASSERT_EQ_INT(report.modeled_and_gate_count, expected_and);
    AC_TEST_ASSERT_EQ_INT(report.modeled_xor_gate_count, expected_xor);
    AC_TEST_ASSERT_EQ_INT(report.modeled_gate_count, 32003200U);
    AC_TEST_ASSERT_EQ_INT(report.modeled_wire_count, 32161498U);
    AC_TEST_ASSERT_EQ_INT(report.modeled_output_depth, 1558999U);
    AC_TEST_ASSERT_EQ_INT(
        report.gate_storage_bytes_if_emitted, 384038400U);
    AC_TEST_ASSERT_EQ_INT(
        report.evaluator_scratch_bytes_if_emitted, 32161498U);
    AC_TEST_ASSERT_EQ_INT(
        report.serialized_bytes_if_emitted, 288028832U);
    return 0;
}

static int test_combined_relation_measurement(void)
{
    ac_ttt_relation_circuit_report x_report;
    ac_ttt_relation_circuit_report o_report;

    AC_TEST_ASSERT_STATUS(
        ac_ttt_relation_circuit_measure(AC_TTT_ROLE_X, &x_report),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(x_report.role, AC_TTT_ROLE_X);
    AC_TEST_ASSERT_EQ_INT(x_report.public_input_bits, 576U);
    AC_TEST_ASSERT_EQ_INT(x_report.witness_input_bits, 157720U);
    AC_TEST_ASSERT_EQ_INT(x_report.modeled_and_gate_count, 5840174U);
    AC_TEST_ASSERT_EQ_INT(x_report.modeled_xor_gate_count, 26409399U);
    AC_TEST_ASSERT_EQ_INT(x_report.modeled_gate_count, 32249573U);
    AC_TEST_ASSERT_EQ_INT(x_report.modeled_wire_count, 32407871U);
    AC_TEST_ASSERT_EQ_INT(x_report.modeled_output_depth, 1559000U);
    AC_TEST_ASSERT_EQ_INT(
        x_report.gate_storage_bytes_if_emitted, 386994876U);
    AC_TEST_ASSERT_EQ_INT(
        x_report.evaluator_scratch_bytes_if_emitted, 32407871U);
    AC_TEST_ASSERT_EQ_INT(
        x_report.serialized_bytes_if_emitted, 290246189U);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_relation_circuit_measure(AC_TTT_ROLE_O, &o_report),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(o_report.role, AC_TTT_ROLE_O);
    AC_TEST_ASSERT_EQ_INT(o_report.public_input_bits, 576U);
    AC_TEST_ASSERT_EQ_INT(o_report.witness_input_bits, 157720U);
    AC_TEST_ASSERT_EQ_INT(o_report.modeled_and_gate_count, 5834465U);
    AC_TEST_ASSERT_EQ_INT(o_report.modeled_xor_gate_count, 26405393U);
    AC_TEST_ASSERT_EQ_INT(o_report.modeled_gate_count, 32239858U);
    AC_TEST_ASSERT_EQ_INT(o_report.modeled_wire_count, 32398156U);
    AC_TEST_ASSERT_EQ_INT(o_report.modeled_output_depth, 1559000U);
    AC_TEST_ASSERT_EQ_INT(
        o_report.gate_storage_bytes_if_emitted, 386878296U);
    AC_TEST_ASSERT_EQ_INT(
        o_report.evaluator_scratch_bytes_if_emitted, 32398156U);
    AC_TEST_ASSERT_EQ_INT(
        o_report.serialized_bytes_if_emitted, 290158754U);

    (void)memset(&x_report, 0xa5, sizeof x_report);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_relation_circuit_measure((ac_ttt_role)0, &x_report),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(&x_report, sizeof x_report);
    return 0;
}

int main(void)
{
    ac_test_suite suite = {0U, 0U};

    AC_TEST_RUN(suite, test_context_and_packed_encodings);
    AC_TEST_RUN(suite, test_published_profile_vector);
    AC_TEST_RUN(suite, test_positive_reference_policy_cross_layer_vector);
    AC_TEST_RUN(suite, test_profile_mutations_are_rejected);
    AC_TEST_RUN(suite, test_randomized_create_and_nonce_clear);
    AC_TEST_RUN(suite, test_argument_failures_and_output_clearing);
    AC_TEST_RUN(suite, test_bind_does_not_check_policy_semantics);
    AC_TEST_RUN(suite, test_minimum_and_full_adder_identity);
    AC_TEST_RUN(suite, test_bind_symbolic_measurement);
    AC_TEST_RUN(suite, test_combined_relation_measurement);
    return ac_test_finish(&suite);
}
