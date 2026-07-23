#ifndef AC_TTT_CORE_CIRCUIT_H
#define AC_TTT_CORE_CIRCUIT_H

#include <stddef.h>
#include <stdint.h>

#include "ac/bool_circuit.h"
#include "ac/status.h"
#include "ac/ttt.h"

#define AC_TTT_CORE_PUBLIC_INPUT_BYTES 4U
#define AC_TTT_CORE_PUBLIC_INPUT_BITS 32U
#define AC_TTT_CORE_WITNESS_INPUT_BYTES AC_TTT_STATE_COUNT
#define AC_TTT_CORE_WITNESS_INPUT_BITS 157464U

#define AC_TTT_CORE_X_AND_GATE_COUNT 215022U
#define AC_TTT_CORE_X_XOR_GATE_COUNT 31350U
#define AC_TTT_CORE_X_GATE_COUNT 246372U
#define AC_TTT_CORE_X_WIRE_COUNT 403870U
#define AC_TTT_CORE_X_OUTPUT_DEPTH 31U
#define AC_TTT_CORE_X_SERIALIZED_BYTES 2217380U

#define AC_TTT_CORE_O_AND_GATE_COUNT 209313U
#define AC_TTT_CORE_O_XOR_GATE_COUNT 27344U
#define AC_TTT_CORE_O_GATE_COUNT 236657U
#define AC_TTT_CORE_O_WIRE_COUNT 394155U
#define AC_TTT_CORE_O_OUTPUT_DEPTH 30U
#define AC_TTT_CORE_O_SERIALIZED_BYTES 2129945U

#define AC_TTT_CORE_MAX_GATE_COUNT AC_TTT_CORE_X_GATE_COUNT
#define AC_TTT_CORE_MAX_WIRE_COUNT AC_TTT_CORE_X_WIRE_COUNT
#define AC_TTT_CORE_MAX_SERIALIZED_BYTES AC_TTT_CORE_X_SERIALIZED_BYTES

/*
 * Caller-owned construction memory. It contains public game classification
 * and wire/depth bookkeeping only. The policy is an input to the resulting
 * circuit and is never supplied to the builder.
 */
typedef struct {
    uint32_t first_selector_wire_by_board[AC_TTT_STATE_COUNT];
    uint32_t signal_wire_by_board[AC_TTT_STATE_COUNT];
    uint32_t signal_depth_by_board[AC_TTT_STATE_COUNT];
    uint8_t kind_by_board[AC_TTT_STATE_COUNT];
    uint8_t board[AC_TTT_BOARD_CELLS];
} ac_ttt_core_builder_scratch;

typedef struct {
    ac_ttt_role specialization_role;
    uint32_t public_input_bits;
    uint32_t witness_input_bits;
    uint32_t required_policy_state_count;
    uint32_t selector_equality_count;
    uint32_t and_gate_count;
    uint32_t xor_gate_count;
    uint32_t gate_count;
    uint32_t wire_count;
    uint32_t output_depth;
    size_t gate_storage_bytes;
    size_t evaluator_scratch_bytes;
    size_t builder_scratch_bytes;
    size_t serialized_bytes;
} ac_ttt_core_circuit_report;

/*
 * Encodes [version, role, initial-board-index-u16be]. Version 1 has only the
 * empty initial board. A non-NULL output is cleared before role validation.
 */
ac_status ac_ttt_core_encode_public_input(
    ac_ttt_role role,
    uint8_t output[AC_TTT_CORE_PUBLIC_INPUT_BYTES]);

/* Returns the exact caller-owned storage requirements for one specialization. */
ac_status ac_ttt_core_circuit_requirements(
    ac_ttt_role role,
    ac_ttt_core_circuit_report *report);

/*
 * Builds a deterministic XOR/AND circuit in caller-owned gate storage.
 * `gates`, `scratch`, `circuit`, and `report` must be distinct. The witness
 * wire range is typed but plaintext: this function provides no commitment,
 * proof, zero knowledge, or privacy mechanism.
 */
ac_status ac_ttt_core_circuit_build(
    ac_ttt_role role,
    ac_bool_gate *gates,
    size_t gate_capacity,
    ac_ttt_core_builder_scratch *scratch,
    ac_bool_circuit *circuit,
    ac_ttt_core_circuit_report *report);

#endif
