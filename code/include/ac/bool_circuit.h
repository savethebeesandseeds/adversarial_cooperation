#ifndef AC_BOOL_CIRCUIT_H
#define AC_BOOL_CIRCUIT_H

#include <stddef.h>
#include <stdint.h>

#include "ac/status.h"

#define AC_BOOL_CIRCUIT_FORMAT_VERSION UINT16_C(1)
#define AC_BOOL_CIRCUIT_BASIS_XOR_AND UINT16_C(1)
#define AC_BOOL_CIRCUIT_HEADER_BYTES 32U
#define AC_BOOL_CIRCUIT_GATE_BYTES 9U

typedef enum {
    AC_BOOL_GATE_XOR = 1,
    AC_BOOL_GATE_AND = 2
} ac_bool_gate_opcode;

/*
 * The reserved bytes make accidental ABI changes visible. They must be zero.
 * Raw instances of this type are never a portable serialization format.
 */
typedef struct {
    uint32_t left;
    uint32_t right;
    uint8_t opcode;
    uint8_t reserved[3];
} ac_bool_gate;

_Static_assert(
    sizeof(ac_bool_gate) == 12U,
    "ac_bool_gate must occupy exactly 12 bytes on the supported C11 ABI");

/*
 * Borrowed, immutable circuit view. Wire 0 is false, wire 1 is true, public
 * input bits follow, then witness-typed input bits, then gate outputs.
 * "Witness" names a wire range only; this API provides no secrecy or proof.
 */
typedef struct {
    uint32_t public_input_bits;
    uint32_t witness_input_bits;
    uint32_t gate_count;
    uint32_t wire_count;
    uint32_t output_wire;
    const ac_bool_gate *gates;
} ac_bool_circuit;

typedef struct {
    uint32_t xor_gate_count;
    uint32_t and_gate_count;
    uint32_t maximum_depth;
    uint32_t output_depth;
} ac_bool_circuit_analysis;

/*
 * Initializes a borrowed circuit view and derives its exact wire count.
 * The output view and any nonempty gate storage must be distinct. A distinct,
 * non-NULL output view is cleared before structural validation.
 */
ac_status ac_bool_circuit_init(
    ac_bool_circuit *circuit,
    uint32_t public_input_bits,
    uint32_t witness_input_bits,
    uint32_t gate_count,
    uint32_t output_wire,
    const ac_bool_gate *gates);

/* Checks count arithmetic, topology, opcodes, reserved bytes, and output. */
ac_status ac_bool_circuit_validate(const ac_bool_circuit *circuit);

/*
 * Computes one depth per wire in caller storage and returns gate counts and
 * circuit depths. The depth buffer, report, circuit view, and gate storage
 * must be pairwise distinct. Any overlap is rejected before output mutation.
 * Once all regions are known to be distinct, the report is cleared before
 * structural validation. Depth capacity is measured in uint32_t entries.
 */
ac_status ac_bool_circuit_analyze(
    const ac_bool_circuit *circuit,
    uint32_t *depth_scratch,
    size_t depth_capacity,
    ac_bool_circuit_analysis *analysis);

/*
 * Evaluates packed inputs, least-significant bit first within every byte.
 * Exact byte lengths are required and unused high bits must be zero. The
 * public input, witness input, one-byte-per-wire scratch, acceptance output,
 * circuit view, and gate storage must be distinct. Any overlap is rejected
 * before output mutation. Once all regions are known to be distinct, the
 * acceptance byte is cleared before structural validation. Used scratch is
 * wiped via a best-effort volatile loop before a normal return; this is not a
 * guarantee about registers, compiler copies, the operating system, or input
 * buffers.
 */
ac_status ac_bool_circuit_evaluate(
    const ac_bool_circuit *circuit,
    const uint8_t *public_input,
    size_t public_input_bytes,
    const uint8_t *witness_input,
    size_t witness_input_bytes,
    uint8_t *wire_scratch,
    size_t wire_scratch_capacity,
    uint8_t *acceptance_out);

/*
 * Returns 32 + 9 * gate_count after validating the circuit. The size output
 * must not overlap the circuit view or borrowed gates.
 */
ac_status ac_bool_circuit_serialized_size(
    const ac_bool_circuit *circuit,
    size_t *serialized_size_out);

/*
 * Writes the canonical big-endian ACBCIR01 representation. The output,
 * written-count output, circuit view, and borrowed gates must be pairwise
 * distinct. Any overlap is rejected before output mutation. Once all regions
 * are known to be distinct, the written-count output is cleared before
 * structural validation.
 */
ac_status ac_bool_circuit_serialize(
    const ac_bool_circuit *circuit,
    uint8_t *output,
    size_t output_capacity,
    size_t *written_out);

#endif
