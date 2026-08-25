#ifndef AC_TTT_BIND_H
#define AC_TTT_BIND_H

#include <stddef.h>
#include <stdint.h>

#include "ac/commitment.h"
#include "ac/status.h"
#include "ac/ttt.h"

#define AC_TTT_BIND_PROFILE_VERSION 1U
#define AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES 4U
#define AC_TTT_BIND_PAYLOAD_TYPE_POLICY 1U
#define AC_TTT_BIND_PROTOCOL_ROLE_PROVER 1U
#define AC_TTT_BIND_PROTOCOL_ROLE_VERIFIER 2U
#define AC_TTT_BIND_PAYLOAD_BYTES \
    (AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES + AC_TTT_STATE_COUNT)
#define AC_TTT_BIND_PUBLIC_INPUT_BYTES \
    (AC_TTT_BIND_CORE_PUBLIC_INPUT_BYTES + \
     AC_COMMITMENT_SESSION_ID_BYTES + \
     4U + \
     AC_COMMITMENT_DIGEST_BYTES)
#define AC_TTT_BIND_WITNESS_INPUT_BYTES \
    (AC_TTT_STATE_COUNT + AC_COMMITMENT_NONCE_BYTES)
#define AC_TTT_BIND_PUBLIC_INPUT_BITS 576U
#define AC_TTT_BIND_WITNESS_INPUT_BITS 157720U
#define AC_TTT_BIND_COMMITMENT_MESSAGE_BYTES 19809U
#define AC_TTT_BIND_BLAKE2B_BLOCK_BYTES 128U
#define AC_TTT_BIND_BLAKE2B_BLOCK_COUNT 155U

/*
 * Educational profile for committing to the complete Canonical Policy
 * Version 1 table. This composes the repository's existing nonce-and-BLAKE2b
 * commitment. It is not a zero-knowledge proof, an extractable commitment,
 * an authorship claim, a strategy oracle, or a production protocol.
 */

typedef struct {
    uint32_t commitment_message_bytes;
    uint32_t blake2b_block_count;
    uint32_t public_input_bits;
    uint32_t witness_input_bits;
    uint32_t modeled_and_gate_count;
    uint32_t modeled_xor_gate_count;
    uint32_t modeled_gate_count;
    uint32_t modeled_wire_count;
    uint32_t modeled_output_depth;
    size_t gate_storage_bytes_if_emitted;
    size_t evaluator_scratch_bytes_if_emitted;
    size_t serialized_bytes_if_emitted;
} ac_ttt_bind_circuit_report;

typedef struct {
    ac_ttt_role role;
    uint32_t public_input_bits;
    uint32_t witness_input_bits;
    uint32_t modeled_and_gate_count;
    uint32_t modeled_xor_gate_count;
    uint32_t modeled_gate_count;
    uint32_t modeled_wire_count;
    uint32_t modeled_output_depth;
    size_t gate_storage_bytes_if_emitted;
    size_t evaluator_scratch_bytes_if_emitted;
    size_t serialized_bytes_if_emitted;
} ac_ttt_relation_circuit_report;

/*
 * Initializes the fixed TTT policy-commitment context. Version, protocol
 * identifier, prover/verifier roles, and payload type are profile constants.
 * The session identifier must be nonzero. The statement round is public
 * context; uniqueness and replay policy remain application obligations.
 */
ac_status ac_ttt_bind_context_init(
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    ac_commitment_context *context);

/*
 * Encodes [version, role, empty-board-u16be, canonical-policy-bytes].
 * The output and policy storage must be distinct.
 */
ac_status ac_ttt_bind_encode_payload(
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    uint8_t output[AC_TTT_BIND_PAYLOAD_BYTES]);

/*
 * Encodes the 72-byte public instance:
 * [Core public input][session identifier][statement round u32be]
 * [commitment digest].
 */
ac_status ac_ttt_bind_encode_public_input(
    ac_ttt_role role,
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    const ac_commitment_digest *digest,
    uint8_t output[AC_TTT_BIND_PUBLIC_INPUT_BYTES]);

/*
 * Encodes the plaintext witness layout used by the cost model:
 * [policy bytes][commitment nonce]. This function provides no secrecy.
 */
ac_status ac_ttt_bind_encode_witness(
    const ac_ttt_policy *policy,
    const ac_commitment_nonce *nonce,
    uint8_t output[AC_TTT_BIND_WITNESS_INPUT_BYTES]);

ac_status ac_ttt_bind_commitment_create(
    ac_ttt_role role,
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    const ac_ttt_policy *policy,
    ac_commitment_digest *digest,
    ac_commitment_nonce *nonce);

ac_status ac_ttt_bind_commitment_compute_with_nonce(
    ac_ttt_role role,
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    const ac_ttt_policy *policy,
    const ac_commitment_nonce *nonce,
    ac_commitment_digest *digest);

ac_status ac_ttt_bind_commitment_verify(
    ac_ttt_role role,
    const uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES],
    uint32_t statement_round,
    const ac_ttt_policy *policy,
    const ac_commitment_nonce *nonce,
    const ac_commitment_digest *digest);

/*
 * Symbolically walks one frozen XOR/AND representation and reports its exact
 * gate counts and depth. It does not emit, evaluate, or serialize that
 * circuit and therefore establishes no commitment or proof property.
 */
ac_status ac_ttt_bind_circuit_measure(
    ac_ttt_bind_circuit_report *report);

/*
 * Adds one final AND to the selected role-specific Core count and the Bind
 * count. This is a cost composition only, not an emitted relation circuit.
 */
ac_status ac_ttt_relation_circuit_measure(
    ac_ttt_role role,
    ac_ttt_relation_circuit_report *report);

#endif
