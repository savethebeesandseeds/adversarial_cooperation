#include "ac/ttt_bind.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include <sodium.h>

#include "ac/bool_circuit.h"
#include "ac/ttt_core_circuit.h"

#define AC_TTT_BIND_EXPECTED_AND_GATES UINT32_C(5625151)
#define AC_TTT_BIND_EXPECTED_XOR_GATES UINT32_C(26378049)
#define AC_TTT_BIND_EXPECTED_GATE_COUNT UINT32_C(32003200)
#define AC_TTT_BIND_EXPECTED_WIRE_COUNT UINT32_C(32161498)
#define AC_TTT_BIND_EXPECTED_OUTPUT_DEPTH UINT32_C(1558999)

static const uint8_t ac_ttt_bind_protocol_id[
    AC_COMMITMENT_PROTOCOL_ID_BYTES] = {
    'A', 'C', '-', 'T', 'T', 'T', '-', 'P',
    'O', 'L', 'I', 'C', 'Y', '-', 'V', '1'
};

typedef struct {
    uint32_t depth[64];
} ac_ttt_bind_symbolic_word;

typedef struct {
    uint64_t and_gate_count;
    uint64_t xor_gate_count;
} ac_ttt_bind_symbolic_builder;

_Static_assert(CHAR_BIT == 8, "the TTT Bind profile requires eight-bit bytes");
_Static_assert(
    AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES == AC_TTT_CORE_PUBLIC_INPUT_BYTES,
    "Bind and Core must share the same four public bytes");
_Static_assert(
    AC_TTT_BIND_PUBLIC_INPUT_BITS == AC_TTT_BIND_PUBLIC_INPUT_BYTES * 8U,
    "Bind public bit count must match its packed bytes");
_Static_assert(
    AC_TTT_BIND_WITNESS_INPUT_BITS == AC_TTT_BIND_WITNESS_INPUT_BYTES * 8U,
    "Bind witness bit count must match its packed bytes");
_Static_assert(
    AC_TTT_BIND_COMMITMENT_MESSAGE_BYTES ==
        122U + AC_TTT_BIND_PAYLOAD_BYTES,
    "Bind message length must match commitment-v1 framing");

static int ac_ttt_bind_role_is_valid(ac_ttt_role role)
{
    return role == AC_TTT_ROLE_X || role == AC_TTT_ROLE_O;
}

static int ac_ttt_bind_bytes_are_all_zero(
    const uint8_t *bytes,
    size_t length)
{
    uint8_t aggregate = 0U;
    size_t index;

    for (index = 0U; index < length; ++index) {
        aggregate |= bytes[index];
    }
    return aggregate == 0U;
}

static int ac_ttt_bind_regions_overlap(
    const void *first,
    size_t first_length,
    const void *second,
    size_t second_length)
{
    uintptr_t first_address;
    uintptr_t second_address;

    if (first == NULL || second == NULL ||
        first_length == 0U || second_length == 0U) {
        return 0;
    }
    first_address = (uintptr_t)first;
    second_address = (uintptr_t)second;
    if (first_address <= second_address) {
        return second_address - first_address < first_length;
    }
    return first_address - second_address < second_length;
}

static void ac_ttt_bind_write_u32be(uint8_t output[4], uint32_t value)
{
    output[0] = (uint8_t)(value >> 24);
    output[1] = (uint8_t)(value >> 16);
    output[2] = (uint8_t)(value >> 8);
    output[3] = (uint8_t)value;
}

static ac_status ac_ttt_bind_validate_profile_inputs(
    ac_ttt_role role,
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    const ac_ttt_policy *policy)
{
    if (!ac_ttt_bind_role_is_valid(role) ||
        session_id == NULL ||
        policy == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (ac_ttt_bind_bytes_are_all_zero(
            session_id, AC_COMMITMENT_SESSION_ID_BYTES)) {
        return AC_ERR_CONTEXT;
    }
    return AC_OK;
}

ac_status ac_ttt_bind_context_init(
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    ac_commitment_context *context)
{
    if (context == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (session_id != NULL &&
        ac_ttt_bind_regions_overlap(
            context,
            sizeof *context,
            session_id,
            AC_COMMITMENT_SESSION_ID_BYTES)) {
        return AC_ERR_ARGUMENT;
    }
    (void)memset(context, 0, sizeof *context);
    if (session_id == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (ac_ttt_bind_bytes_are_all_zero(
            session_id, AC_COMMITMENT_SESSION_ID_BYTES)) {
        return AC_ERR_CONTEXT;
    }

    (void)memcpy(
        context->protocol_id,
        ac_ttt_bind_protocol_id,
        sizeof context->protocol_id);
    context->protocol_version = AC_TTT_BIND_PROFILE_VERSION;
    (void)memcpy(
        context->session_id,
        session_id,
        sizeof context->session_id);
    context->round = statement_round;
    context->committer_role = AC_TTT_BIND_PROTOCOL_ROLE_PROVER;
    context->recipient_role = AC_TTT_BIND_PROTOCOL_ROLE_VERIFIER;
    context->payload_type = AC_TTT_BIND_PAYLOAD_TYPE_POLICY;
    return AC_OK;
}

ac_status ac_ttt_bind_encode_payload(
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    uint8_t output[AC_TTT_BIND_PAYLOAD_BYTES])
{
    if (output == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (policy != NULL &&
        ac_ttt_bind_regions_overlap(
            output,
            AC_TTT_BIND_PAYLOAD_BYTES,
            policy,
            sizeof *policy)) {
        return AC_ERR_ARGUMENT;
    }
    (void)memset(output, 0, AC_TTT_BIND_PAYLOAD_BYTES);
    if (!ac_ttt_bind_role_is_valid(role) || policy == NULL) {
        return AC_ERR_ARGUMENT;
    }

    output[0] = AC_TTT_GAME_VERSION;
    output[1] = (uint8_t)role;
    output[2] = 0U;
    output[3] = 0U;
    (void)memcpy(
        output + AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES,
        policy->move_by_board,
        sizeof policy->move_by_board);
    return AC_OK;
}

ac_status ac_ttt_bind_encode_public_input(
    ac_ttt_role role,
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    const ac_commitment_digest *digest,
    uint8_t output[AC_TTT_BIND_PUBLIC_INPUT_BYTES])
{
    if (output == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if ((session_id != NULL &&
         ac_ttt_bind_regions_overlap(
             output,
             AC_TTT_BIND_PUBLIC_INPUT_BYTES,
             session_id,
             AC_COMMITMENT_SESSION_ID_BYTES)) ||
        (digest != NULL &&
         ac_ttt_bind_regions_overlap(
             output,
             AC_TTT_BIND_PUBLIC_INPUT_BYTES,
             digest,
             sizeof *digest))) {
        return AC_ERR_ARGUMENT;
    }
    (void)memset(output, 0, AC_TTT_BIND_PUBLIC_INPUT_BYTES);
    if (!ac_ttt_bind_role_is_valid(role) ||
        session_id == NULL ||
        digest == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (ac_ttt_bind_bytes_are_all_zero(
            session_id, AC_COMMITMENT_SESSION_ID_BYTES)) {
        return AC_ERR_CONTEXT;
    }

    output[0] = AC_TTT_GAME_VERSION;
    output[1] = (uint8_t)role;
    output[2] = 0U;
    output[3] = 0U;
    (void)memcpy(
        output + AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES,
        session_id,
        AC_COMMITMENT_SESSION_ID_BYTES);
    ac_ttt_bind_write_u32be(
        output +
            AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES +
            AC_COMMITMENT_SESSION_ID_BYTES,
        statement_round);
    (void)memcpy(
        output +
            AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES +
            AC_COMMITMENT_SESSION_ID_BYTES +
            4U,
        digest->bytes,
        sizeof digest->bytes);
    return AC_OK;
}

ac_status ac_ttt_bind_encode_witness(
    const ac_ttt_policy *policy,
    const ac_commitment_nonce *nonce,
    uint8_t output[AC_TTT_BIND_WITNESS_INPUT_BYTES])
{
    if (output == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if ((policy != NULL &&
         ac_ttt_bind_regions_overlap(
             output,
             AC_TTT_BIND_WITNESS_INPUT_BYTES,
             policy,
             sizeof *policy)) ||
        (nonce != NULL &&
         ac_ttt_bind_regions_overlap(
             output,
             AC_TTT_BIND_WITNESS_INPUT_BYTES,
             nonce,
             sizeof *nonce))) {
        return AC_ERR_ARGUMENT;
    }
    (void)memset(output, 0, AC_TTT_BIND_WITNESS_INPUT_BYTES);
    if (policy == NULL || nonce == NULL) {
        return AC_ERR_ARGUMENT;
    }

    (void)memcpy(
        output,
        policy->move_by_board,
        sizeof policy->move_by_board);
    (void)memcpy(
        output + AC_TTT_STATE_COUNT,
        nonce->bytes,
        sizeof nonce->bytes);
    return AC_OK;
}

static ac_status ac_ttt_bind_build_context_and_payload(
    ac_ttt_role role,
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    const ac_ttt_policy *policy,
    ac_commitment_context *context,
    uint8_t payload[AC_TTT_BIND_PAYLOAD_BYTES])
{
    ac_status status;

    status = ac_ttt_bind_validate_profile_inputs(role, session_id, policy);
    if (status != AC_OK) {
        return status;
    }
    status = ac_ttt_bind_context_init(
        session_id, statement_round, context);
    if (status != AC_OK) {
        return status;
    }
    return ac_ttt_bind_encode_payload(role, policy, payload);
}

ac_status ac_ttt_bind_commitment_compute_with_nonce(
    ac_ttt_role role,
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    const ac_ttt_policy *policy,
    const ac_commitment_nonce *nonce,
    ac_commitment_digest *digest)
{
    ac_commitment_context context;
    uint8_t payload[AC_TTT_BIND_PAYLOAD_BYTES];
    ac_status status;

    if (digest == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if ((session_id != NULL &&
         ac_ttt_bind_regions_overlap(
             digest,
             sizeof *digest,
             session_id,
             AC_COMMITMENT_SESSION_ID_BYTES)) ||
        (policy != NULL &&
         ac_ttt_bind_regions_overlap(
             digest,
             sizeof *digest,
             policy,
             sizeof *policy)) ||
        (nonce != NULL &&
         ac_ttt_bind_regions_overlap(
             digest,
             sizeof *digest,
             nonce,
             sizeof *nonce))) {
        return AC_ERR_ARGUMENT;
    }
    (void)memset(digest, 0, sizeof *digest);
    if (nonce == NULL) {
        return AC_ERR_ARGUMENT;
    }

    (void)memset(&context, 0, sizeof context);
    (void)memset(payload, 0, sizeof payload);
    status = ac_ttt_bind_build_context_and_payload(
        role,
        session_id,
        statement_round,
        policy,
        &context,
        payload);
    if (status == AC_OK) {
        status = ac_commitment_compute_with_nonce(
            &context,
            payload,
            sizeof payload,
            nonce,
            digest);
    }
    sodium_memzero(payload, sizeof payload);
    sodium_memzero(&context, sizeof context);
    return status;
}

ac_status ac_ttt_bind_commitment_create(
    ac_ttt_role role,
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    const ac_ttt_policy *policy,
    ac_commitment_digest *digest,
    ac_commitment_nonce *nonce)
{
    ac_commitment_context context;
    uint8_t payload[AC_TTT_BIND_PAYLOAD_BYTES];
    ac_status status;

    if (digest == NULL || nonce == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (ac_ttt_bind_regions_overlap(
            digest, sizeof *digest, nonce, sizeof *nonce) ||
        (session_id != NULL &&
         (ac_ttt_bind_regions_overlap(
              digest,
              sizeof *digest,
              session_id,
              AC_COMMITMENT_SESSION_ID_BYTES) ||
          ac_ttt_bind_regions_overlap(
              nonce,
              sizeof *nonce,
              session_id,
              AC_COMMITMENT_SESSION_ID_BYTES))) ||
        (policy != NULL &&
         (ac_ttt_bind_regions_overlap(
              digest,
              sizeof *digest,
              policy,
              sizeof *policy) ||
          ac_ttt_bind_regions_overlap(
              nonce,
              sizeof *nonce,
              policy,
              sizeof *policy)))) {
        return AC_ERR_ARGUMENT;
    }
    (void)memset(digest, 0, sizeof *digest);
    (void)memset(nonce, 0, sizeof *nonce);

    (void)memset(&context, 0, sizeof context);
    (void)memset(payload, 0, sizeof payload);
    status = ac_ttt_bind_build_context_and_payload(
        role,
        session_id,
        statement_round,
        policy,
        &context,
        payload);
    if (status == AC_OK) {
        status = ac_commitment_create(
            &context,
            payload,
            sizeof payload,
            digest,
            nonce);
    }
    sodium_memzero(payload, sizeof payload);
    sodium_memzero(&context, sizeof context);
    return status;
}

ac_status ac_ttt_bind_commitment_verify(
    ac_ttt_role role,
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    const ac_ttt_policy *policy,
    const ac_commitment_nonce *nonce,
    const ac_commitment_digest *digest)
{
    ac_commitment_context context;
    uint8_t payload[AC_TTT_BIND_PAYLOAD_BYTES];
    ac_status status;

    if (nonce == NULL || digest == NULL) {
        return AC_ERR_ARGUMENT;
    }
    (void)memset(&context, 0, sizeof context);
    (void)memset(payload, 0, sizeof payload);
    status = ac_ttt_bind_build_context_and_payload(
        role,
        session_id,
        statement_round,
        policy,
        &context,
        payload);
    if (status == AC_OK) {
        status = ac_commitment_verify(
            &context,
            payload,
            sizeof payload,
            nonce,
            digest);
    }
    sodium_memzero(payload, sizeof payload);
    sodium_memzero(&context, sizeof context);
    return status;
}

static uint32_t ac_ttt_bind_symbolic_gate(
    ac_ttt_bind_symbolic_builder *builder,
    uint32_t left_depth,
    uint32_t right_depth,
    ac_bool_gate_opcode opcode)
{
    if (opcode == AC_BOOL_GATE_AND) {
        builder->and_gate_count += 1U;
    } else {
        builder->xor_gate_count += 1U;
    }
    return (left_depth > right_depth ? left_depth : right_depth) + 1U;
}

static uint32_t ac_ttt_bind_symbolic_gate_depth(
    uint32_t left_depth,
    uint32_t right_depth)
{
    return (left_depth > right_depth ? left_depth : right_depth) + 1U;
}

static ac_ttt_bind_symbolic_word ac_ttt_bind_symbolic_xor_word(
    ac_ttt_bind_symbolic_builder *builder,
    const ac_ttt_bind_symbolic_word *left,
    const ac_ttt_bind_symbolic_word *right)
{
    ac_ttt_bind_symbolic_word output;
    size_t bit;

    builder->xor_gate_count += 64U;
    for (bit = 0U; bit < 64U; ++bit) {
        output.depth[bit] = ac_ttt_bind_symbolic_gate_depth(
            left->depth[bit],
            right->depth[bit]);
    }
    return output;
}

static ac_ttt_bind_symbolic_word ac_ttt_bind_symbolic_rotate_right(
    const ac_ttt_bind_symbolic_word *input,
    size_t distance)
{
    ac_ttt_bind_symbolic_word output;
    size_t bit;

    for (bit = 0U; bit < 64U; ++bit) {
        output.depth[bit] = input->depth[(bit + distance) % 64U];
    }
    return output;
}

static ac_ttt_bind_symbolic_word ac_ttt_bind_symbolic_add(
    ac_ttt_bind_symbolic_builder *builder,
    const ac_ttt_bind_symbolic_word *left,
    const ac_ttt_bind_symbolic_word *right)
{
    ac_ttt_bind_symbolic_word output;
    uint32_t carry_depth;
    size_t bit;

    /*
     * Count the frozen 64-bit shape in bulk, then walk only its depths. Bit
     * zero is the one-AND/one-XOR half-adder implied by carry-in zero; bits
     * 1..62 use the full relation; bit 63 emits only its sum because the
     * final carry is discarded. This is equivalent to recording every
     * individual gate but avoids making the symbolic counter itself scale
     * with all 32 million modeled gates.
     */
    builder->and_gate_count += 63U;
    builder->xor_gate_count += 251U;
    output.depth[0] = ac_ttt_bind_symbolic_gate_depth(
        left->depth[0],
        right->depth[0]);
    carry_depth = ac_ttt_bind_symbolic_gate_depth(
        left->depth[0],
        right->depth[0]);

    for (bit = 1U; bit < 63U; ++bit) {
        uint32_t left_xor_carry;
        uint32_t right_xor_carry;
        uint32_t carry_product;

        left_xor_carry = ac_ttt_bind_symbolic_gate_depth(
            left->depth[bit],
            carry_depth);
        right_xor_carry = ac_ttt_bind_symbolic_gate_depth(
            right->depth[bit],
            carry_depth);
        output.depth[bit] = ac_ttt_bind_symbolic_gate_depth(
            left_xor_carry,
            right->depth[bit]);
        carry_product = ac_ttt_bind_symbolic_gate_depth(
            left_xor_carry,
            right_xor_carry);
        carry_depth = ac_ttt_bind_symbolic_gate_depth(
            carry_product,
            carry_depth);
    }

    carry_depth = ac_ttt_bind_symbolic_gate_depth(
        left->depth[63],
        carry_depth);
    output.depth[63] = ac_ttt_bind_symbolic_gate_depth(
        carry_depth,
        right->depth[63]);
    return output;
}

static ac_ttt_bind_symbolic_word ac_ttt_bind_symbolic_add_three(
    ac_ttt_bind_symbolic_builder *builder,
    const ac_ttt_bind_symbolic_word *first,
    const ac_ttt_bind_symbolic_word *second,
    const ac_ttt_bind_symbolic_word *third)
{
    ac_ttt_bind_symbolic_word partial;

    partial = ac_ttt_bind_symbolic_add(builder, first, second);
    return ac_ttt_bind_symbolic_add(builder, &partial, third);
}

static void ac_ttt_bind_symbolic_g(
    ac_ttt_bind_symbolic_builder *builder,
    ac_ttt_bind_symbolic_word words[16],
    size_t a,
    size_t b,
    size_t c,
    size_t d,
    const ac_ttt_bind_symbolic_word *x,
    const ac_ttt_bind_symbolic_word *y)
{
    ac_ttt_bind_symbolic_word mixed;

    words[a] = ac_ttt_bind_symbolic_add_three(
        builder, &words[a], &words[b], x);
    mixed = ac_ttt_bind_symbolic_xor_word(
        builder, &words[d], &words[a]);
    words[d] = ac_ttt_bind_symbolic_rotate_right(&mixed, 32U);
    words[c] = ac_ttt_bind_symbolic_add(
        builder, &words[c], &words[d]);
    mixed = ac_ttt_bind_symbolic_xor_word(
        builder, &words[b], &words[c]);
    words[b] = ac_ttt_bind_symbolic_rotate_right(&mixed, 24U);
    words[a] = ac_ttt_bind_symbolic_add_three(
        builder, &words[a], &words[b], y);
    mixed = ac_ttt_bind_symbolic_xor_word(
        builder, &words[d], &words[a]);
    words[d] = ac_ttt_bind_symbolic_rotate_right(&mixed, 16U);
    words[c] = ac_ttt_bind_symbolic_add(
        builder, &words[c], &words[d]);
    mixed = ac_ttt_bind_symbolic_xor_word(
        builder, &words[b], &words[c]);
    words[b] = ac_ttt_bind_symbolic_rotate_right(&mixed, 63U);
}

static void ac_ttt_bind_symbolic_compress(
    ac_ttt_bind_symbolic_builder *builder,
    ac_ttt_bind_symbolic_word state[8])
{
    ac_ttt_bind_symbolic_word words[16];
    ac_ttt_bind_symbolic_word zero_word;
    ac_ttt_bind_symbolic_word mixed;
    size_t index;
    size_t round;

    (void)memset(&zero_word, 0, sizeof zero_word);
    for (index = 0U; index < 8U; ++index) {
        words[index] = state[index];
        words[index + 8U] = zero_word;
    }

    /*
     * Message bits, IV bits, counters, and the last-block flag are all
     * depth-zero inputs or constants. Their exact values affect function
     * values, not this frozen construction's count or depth.
     */
    for (round = 0U; round < 12U; ++round) {
        ac_ttt_bind_symbolic_g(
            builder, words, 0U, 4U, 8U, 12U, &zero_word, &zero_word);
        ac_ttt_bind_symbolic_g(
            builder, words, 1U, 5U, 9U, 13U, &zero_word, &zero_word);
        ac_ttt_bind_symbolic_g(
            builder, words, 2U, 6U, 10U, 14U, &zero_word, &zero_word);
        ac_ttt_bind_symbolic_g(
            builder, words, 3U, 7U, 11U, 15U, &zero_word, &zero_word);
        ac_ttt_bind_symbolic_g(
            builder, words, 0U, 5U, 10U, 15U, &zero_word, &zero_word);
        ac_ttt_bind_symbolic_g(
            builder, words, 1U, 6U, 11U, 12U, &zero_word, &zero_word);
        ac_ttt_bind_symbolic_g(
            builder, words, 2U, 7U, 8U, 13U, &zero_word, &zero_word);
        ac_ttt_bind_symbolic_g(
            builder, words, 3U, 4U, 9U, 14U, &zero_word, &zero_word);
    }

    for (index = 0U; index < 8U; ++index) {
        mixed = ac_ttt_bind_symbolic_xor_word(
            builder, &state[index], &words[index]);
        state[index] = ac_ttt_bind_symbolic_xor_word(
            builder, &mixed, &words[index + 8U]);
    }
}

static uint32_t ac_ttt_bind_symbolic_reduce_and(
    ac_ttt_bind_symbolic_builder *builder,
    uint32_t *depths,
    size_t count)
{
    while (count > 1U) {
        size_t input;
        size_t output = 0U;

        for (input = 0U; input < count; input += 2U) {
            if (input + 1U < count) {
                depths[output] = ac_ttt_bind_symbolic_gate(
                    builder,
                    depths[input],
                    depths[input + 1U],
                    AC_BOOL_GATE_AND);
            } else {
                depths[output] = depths[input];
            }
            output += 1U;
        }
        count = output;
    }
    return depths[0];
}

static ac_status ac_ttt_bind_checked_report(
    uint64_t and_gate_count,
    uint64_t xor_gate_count,
    uint64_t output_depth,
    ac_ttt_bind_circuit_report *report)
{
    uint64_t gate_count;
    uint64_t wire_count;
    uint64_t serialized_bytes;
    uint64_t gate_storage_bytes;

    gate_count = and_gate_count + xor_gate_count;
    wire_count =
        2U +
        AC_TTT_BIND_PUBLIC_INPUT_BITS +
        AC_TTT_BIND_WITNESS_INPUT_BITS +
        gate_count;
    gate_storage_bytes = gate_count * sizeof(ac_bool_gate);
    serialized_bytes =
        AC_BOOL_CIRCUIT_HEADER_BYTES +
        gate_count * AC_BOOL_CIRCUIT_GATE_BYTES;

    if (and_gate_count > UINT32_MAX ||
        xor_gate_count > UINT32_MAX ||
        gate_count > UINT32_MAX ||
        wire_count > UINT32_MAX ||
        output_depth > UINT32_MAX ||
        gate_storage_bytes > SIZE_MAX ||
        wire_count > SIZE_MAX ||
        serialized_bytes > SIZE_MAX) {
        return AC_ERR_LENGTH;
    }
    if (and_gate_count != AC_TTT_BIND_EXPECTED_AND_GATES ||
        xor_gate_count != AC_TTT_BIND_EXPECTED_XOR_GATES ||
        gate_count != AC_TTT_BIND_EXPECTED_GATE_COUNT ||
        wire_count != AC_TTT_BIND_EXPECTED_WIRE_COUNT ||
        output_depth != AC_TTT_BIND_EXPECTED_OUTPUT_DEPTH) {
        return AC_ERR_STATE;
    }

    report->commitment_message_bytes =
        AC_TTT_BIND_COMMITMENT_MESSAGE_BYTES;
    report->blake2b_block_count = AC_TTT_BIND_BLAKE2B_BLOCK_COUNT;
    report->public_input_bits = AC_TTT_BIND_PUBLIC_INPUT_BITS;
    report->witness_input_bits = AC_TTT_BIND_WITNESS_INPUT_BITS;
    report->modeled_and_gate_count = (uint32_t)and_gate_count;
    report->modeled_xor_gate_count = (uint32_t)xor_gate_count;
    report->modeled_gate_count = (uint32_t)gate_count;
    report->modeled_wire_count = (uint32_t)wire_count;
    report->modeled_output_depth = (uint32_t)output_depth;
    report->gate_storage_bytes_if_emitted = (size_t)gate_storage_bytes;
    report->evaluator_scratch_bytes_if_emitted = (size_t)wire_count;
    report->serialized_bytes_if_emitted = (size_t)serialized_bytes;
    return AC_OK;
}

ac_status ac_ttt_bind_circuit_measure(
    ac_ttt_bind_circuit_report *report)
{
    ac_ttt_bind_symbolic_builder builder = {0U, 0U};
    ac_ttt_bind_symbolic_word state[8];
    uint32_t digest_equal_depths[256];
    uint32_t session_zero_depths[256];
    uint32_t digest_equal_depth;
    uint32_t session_zero_depth;
    uint32_t session_nonzero_depth;
    uint32_t output_depth;
    size_t block;
    size_t word;
    size_t bit;

    if (report == NULL) {
        return AC_ERR_ARGUMENT;
    }
    (void)memset(report, 0, sizeof *report);
    (void)memset(state, 0, sizeof state);

    for (block = 0U; block < AC_TTT_BIND_BLAKE2B_BLOCK_COUNT; ++block) {
        ac_ttt_bind_symbolic_compress(&builder, state);
    }

    for (word = 0U; word < 4U; ++word) {
        for (bit = 0U; bit < 64U; ++bit) {
            uint32_t difference_depth;
            size_t output_index = word * 64U + bit;

            difference_depth = ac_ttt_bind_symbolic_gate(
                &builder,
                state[word].depth[bit],
                0U,
                AC_BOOL_GATE_XOR);
            digest_equal_depths[output_index] =
                ac_ttt_bind_symbolic_gate(
                    &builder,
                    difference_depth,
                    0U,
                    AC_BOOL_GATE_XOR);
        }
    }
    digest_equal_depth = ac_ttt_bind_symbolic_reduce_and(
        &builder, digest_equal_depths, 256U);

    for (bit = 0U; bit < 256U; ++bit) {
        session_zero_depths[bit] = ac_ttt_bind_symbolic_gate(
            &builder, 0U, 0U, AC_BOOL_GATE_XOR);
    }
    session_zero_depth = ac_ttt_bind_symbolic_reduce_and(
        &builder, session_zero_depths, 256U);
    session_nonzero_depth = ac_ttt_bind_symbolic_gate(
        &builder,
        session_zero_depth,
        0U,
        AC_BOOL_GATE_XOR);
    output_depth = ac_ttt_bind_symbolic_gate(
        &builder,
        digest_equal_depth,
        session_nonzero_depth,
        AC_BOOL_GATE_AND);

    return ac_ttt_bind_checked_report(
        builder.and_gate_count,
        builder.xor_gate_count,
        output_depth,
        report);
}

ac_status ac_ttt_relation_circuit_measure(
    ac_ttt_role role,
    ac_ttt_relation_circuit_report *report)
{
    ac_ttt_bind_circuit_report bind_report;
    uint64_t and_gate_count;
    uint64_t xor_gate_count;
    uint64_t gate_count;
    uint64_t wire_count;
    uint64_t output_depth;
    uint64_t gate_storage_bytes;
    uint64_t serialized_bytes;
    uint32_t core_and;
    uint32_t core_xor;

    if (report == NULL) {
        return AC_ERR_ARGUMENT;
    }
    (void)memset(report, 0, sizeof *report);
    if (!ac_ttt_bind_role_is_valid(role)) {
        return AC_ERR_ARGUMENT;
    }
    if (ac_ttt_bind_circuit_measure(&bind_report) != AC_OK) {
        return AC_ERR_STATE;
    }

    core_and = role == AC_TTT_ROLE_X
        ? AC_TTT_CORE_X_AND_GATE_COUNT
        : AC_TTT_CORE_O_AND_GATE_COUNT;
    core_xor = role == AC_TTT_ROLE_X
        ? AC_TTT_CORE_X_XOR_GATE_COUNT
        : AC_TTT_CORE_O_XOR_GATE_COUNT;
    and_gate_count =
        (uint64_t)bind_report.modeled_and_gate_count + core_and + 1U;
    xor_gate_count =
        (uint64_t)bind_report.modeled_xor_gate_count + core_xor;
    gate_count = and_gate_count + xor_gate_count;
    wire_count =
        2U +
        AC_TTT_BIND_PUBLIC_INPUT_BITS +
        AC_TTT_BIND_WITNESS_INPUT_BITS +
        gate_count;
    output_depth = (uint64_t)bind_report.modeled_output_depth + 1U;
    gate_storage_bytes = gate_count * sizeof(ac_bool_gate);
    serialized_bytes =
        AC_BOOL_CIRCUIT_HEADER_BYTES +
        gate_count * AC_BOOL_CIRCUIT_GATE_BYTES;

    if (and_gate_count > UINT32_MAX ||
        xor_gate_count > UINT32_MAX ||
        gate_count > UINT32_MAX ||
        wire_count > UINT32_MAX ||
        output_depth > UINT32_MAX ||
        gate_storage_bytes > SIZE_MAX ||
        wire_count > SIZE_MAX ||
        serialized_bytes > SIZE_MAX) {
        return AC_ERR_LENGTH;
    }

    report->role = role;
    report->public_input_bits = AC_TTT_BIND_PUBLIC_INPUT_BITS;
    report->witness_input_bits = AC_TTT_BIND_WITNESS_INPUT_BITS;
    report->modeled_and_gate_count = (uint32_t)and_gate_count;
    report->modeled_xor_gate_count = (uint32_t)xor_gate_count;
    report->modeled_gate_count = (uint32_t)gate_count;
    report->modeled_wire_count = (uint32_t)wire_count;
    report->modeled_output_depth = (uint32_t)output_depth;
    report->gate_storage_bytes_if_emitted = (size_t)gate_storage_bytes;
    report->evaluator_scratch_bytes_if_emitted = (size_t)wire_count;
    report->serialized_bytes_if_emitted = (size_t)serialized_bytes;
    return AC_OK;
}
