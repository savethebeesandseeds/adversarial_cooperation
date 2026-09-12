#ifndef AC_TTT_H
#define AC_TTT_H

#include <stdint.h>

#include "ac/status.h"

#define AC_TTT_GAME_VERSION 1U
#define AC_TTT_BOARD_CELLS 9U
#define AC_TTT_STATE_COUNT 19683U
#define AC_TTT_MAX_PLIES 9U
#define AC_TTT_NO_MOVE UINT8_C(0xff)

/*
 * Public finite-game model. The policy supplied to this API is fully
 * disclosed. These types and functions provide no strategy privacy, proof of
 * knowledge, authorship, commitment, authentication, or future compliance.
 */

typedef enum {
    AC_TTT_ROLE_X = 1,
    AC_TTT_ROLE_O = 2
} ac_ttt_role;

typedef enum {
    AC_TTT_VERDICT_NOT_COMPUTED = 0,
    AC_TTT_VERDICT_NON_LOSING,
    AC_TTT_VERDICT_INVALID_POLICY,
    AC_TTT_VERDICT_LOSING
} ac_ttt_verdict;

typedef enum {
    AC_TTT_POLICY_REASON_NONE = 0,
    AC_TTT_POLICY_REASON_MISSING_MOVE,
    AC_TTT_POLICY_REASON_OUT_OF_RANGE_MOVE,
    AC_TTT_POLICY_REASON_OCCUPIED_CELL,
    AC_TTT_POLICY_REASON_UNEXPECTED_ENTRY
} ac_ttt_policy_reason;

typedef struct {
    uint8_t move_by_board[AC_TTT_STATE_COUNT];
} ac_ttt_policy;

typedef struct {
    ac_ttt_verdict verdict;
    ac_ttt_policy_reason policy_reason;
    uint32_t invalid_board_index;
    uint8_t invalid_move;
    uint32_t reachable_board_count;
    uint32_t x_to_move_board_count;
    uint32_t o_to_move_board_count;
    uint32_t reachable_terminal_board_count;
    uint32_t required_policy_state_count;
    uint32_t explored_node_count;
    uint32_t explored_terminal_node_count;
    uint8_t counterexample_length;
    uint8_t counterexample_moves[AC_TTT_MAX_PLIES];
} ac_ttt_report;

/*
 * Report for the fixed-DAG relation evaluator. Its counts describe complete
 * public scans and Boolean fold operations in the C evaluator; they are not
 * proof-system gates, proof sizes, timing guarantees, or DFS diagnostics.
 */
typedef struct {
    ac_ttt_verdict verdict;
    ac_ttt_policy_reason policy_reason;
    uint32_t invalid_board_index;
    uint8_t invalid_move;
    uint32_t reachable_board_count;
    uint32_t x_to_move_board_count;
    uint32_t o_to_move_board_count;
    uint32_t reachable_terminal_board_count;
    uint32_t required_policy_state_count;
    uint32_t x_win_terminal_board_count;
    uint32_t o_win_terminal_board_count;
    uint32_t draw_terminal_board_count;
    uint32_t x_edge_count;
    uint32_t o_edge_count;
    uint32_t edge_count;
    uint32_t graph_scan_entry_count;
    uint32_t policy_scan_entry_count;
    uint32_t value_scan_entry_count;
    uint32_t evaluated_board_count;
    uint32_t selector_equality_count;
    uint32_t selector_and_count;
    uint32_t selector_or_count;
    uint32_t opponent_and_count;
    uint32_t serialized_policy_byte_count;
    uint32_t classification_table_byte_count;
    uint32_t value_table_byte_count;
    uint32_t board_scratch_byte_count;
    uint32_t explicit_scratch_byte_count;
} ac_ttt_dag_report;

/* Initializes every policy-table byte to AC_TTT_NO_MOVE. NULL is a no-op. */
void ac_ttt_policy_init(ac_ttt_policy *policy);

/*
 * Builds a deterministic, public minimax reference policy with ascending-cell
 * tie breaking. This is a teaching fixture, not a secret generator or a
 * cryptographic mechanism. A non-NULL output is initialized even when role is
 * invalid.
 */
ac_status ac_ttt_build_reference_policy(
    ac_ttt_role role,
    ac_ttt_policy *policy);

/*
 * Exhaustively checks a fully disclosed canonical policy from the empty board.
 * API execution status is returned separately from the mathematical verdict.
 * A non-NULL report is cleared before argument validation. Policy and report
 * storage must not overlap.
 */
ac_status ac_ttt_verify_policy(
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    ac_ttt_report *report);

/*
 * Evaluates the same disclosed-policy verdict using a complete fixed public
 * game-DAG schedule. It has no recursion, early verdict exit, policy-selected
 * successor address, counterexample trace, privacy, or cryptographic claim.
 * Use ac_ttt_verify_policy() when a replayable losing line is required. API
 * execution status is separate from the mathematical verdict. A non-NULL
 * report is cleared before argument validation. Policy and report storage
 * must not overlap.
 */
ac_status ac_ttt_evaluate_policy_dag(
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    ac_ttt_dag_report *report);

#endif
