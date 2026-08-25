#include "ac/ttt.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define AC_TTT_WEB_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define AC_TTT_WEB_EXPORT
#endif

/*
 * Thin browser ABI for the existing disclosed-policy Tic-Tac-Toe checker.
 *
 * JavaScript calls ac_ttt_web_run() with one of the scenario identifiers
 * below, then reads the immutable result through the scalar getters.  The
 * Emscripten module exposes each function as Module._ac_ttt_web_... .  No
 * policy checking or game-tree logic is implemented here; that remains in
 * src/protocols/ttt.c.  This bridge is an educational execution adapter, not
 * a privacy, zero-knowledge, authorship, or cryptographic mechanism.
 */

enum {
    AC_TTT_WEB_ABI_VERSION = 1U,
    AC_TTT_WEB_SCENARIO_REFERENCE_X = 1U,
    AC_TTT_WEB_SCENARIO_REFERENCE_O = 2U,
    AC_TTT_WEB_SCENARIO_NAIVE_X = 3U
};

typedef struct {
    uint32_t scenario;
    ac_status status;
    ac_ttt_role role;
    ac_ttt_report report;
} ac_ttt_web_result;

static ac_ttt_web_result ac_ttt_web_last_result;

static uint8_t ac_ttt_web_first_empty_cell(uint32_t board_index)
{
    uint32_t remaining = board_index;
    uint8_t cell;

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        if (remaining % 3U == 0U) {
            return cell;
        }
        remaining /= 3U;
    }
    return AC_TTT_NO_MOVE;
}

/*
 * Construct the deterministic teaching fixture called "naive X": at every
 * canonical X-turn policy entry, choose the lowest-numbered empty cell.  The
 * public reference builder identifies exactly which entries are required;
 * the authoritative verifier still decides whether the resulting policy is
 * valid and non-losing.
 */
static ac_status ac_ttt_web_build_naive_x(ac_ttt_policy *policy)
{
    uint32_t board_index;
    ac_status status;

    status = ac_ttt_build_reference_policy(AC_TTT_ROLE_X, policy);
    if (status != AC_OK) {
        return status;
    }

    for (board_index = 0U;
         board_index < AC_TTT_STATE_COUNT;
         ++board_index) {
        uint8_t move;

        if (policy->move_by_board[board_index] == AC_TTT_NO_MOVE) {
            continue;
        }
        move = ac_ttt_web_first_empty_cell(board_index);
        if (move == AC_TTT_NO_MOVE) {
            return AC_ERR_STATE;
        }
        policy->move_by_board[board_index] = move;
    }
    return AC_OK;
}

AC_TTT_WEB_EXPORT ac_status ac_ttt_web_run(uint32_t scenario)
{
    ac_ttt_policy policy;
    ac_status status;

    (void)memset(&ac_ttt_web_last_result, 0, sizeof ac_ttt_web_last_result);
    ac_ttt_web_last_result.scenario = scenario;

    switch (scenario) {
    case AC_TTT_WEB_SCENARIO_REFERENCE_X:
        ac_ttt_web_last_result.role = AC_TTT_ROLE_X;
        status = ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy);
        break;
    case AC_TTT_WEB_SCENARIO_REFERENCE_O:
        ac_ttt_web_last_result.role = AC_TTT_ROLE_O;
        status = ac_ttt_build_reference_policy(AC_TTT_ROLE_O, &policy);
        break;
    case AC_TTT_WEB_SCENARIO_NAIVE_X:
        ac_ttt_web_last_result.role = AC_TTT_ROLE_X;
        status = ac_ttt_web_build_naive_x(&policy);
        break;
    default:
        ac_ttt_web_last_result.status = AC_ERR_ARGUMENT;
        return AC_ERR_ARGUMENT;
    }

    if (status == AC_OK) {
        status = ac_ttt_verify_policy(
            ac_ttt_web_last_result.role,
            &policy,
            &ac_ttt_web_last_result.report);
    }
    ac_ttt_web_last_result.status = status;
    return status;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_abi_version(void)
{
    return AC_TTT_WEB_ABI_VERSION;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_scenario(void)
{
    return ac_ttt_web_last_result.scenario;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_status(void)
{
    return (uint32_t)ac_ttt_web_last_result.status;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_role(void)
{
    return (uint32_t)ac_ttt_web_last_result.role;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_verdict(void)
{
    return (uint32_t)ac_ttt_web_last_result.report.verdict;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_policy_reason(void)
{
    return (uint32_t)ac_ttt_web_last_result.report.policy_reason;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_invalid_board_index(void)
{
    return ac_ttt_web_last_result.report.invalid_board_index;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_invalid_move(void)
{
    return (uint32_t)ac_ttt_web_last_result.report.invalid_move;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_reachable_board_count(void)
{
    return ac_ttt_web_last_result.report.reachable_board_count;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_x_to_move_board_count(void)
{
    return ac_ttt_web_last_result.report.x_to_move_board_count;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_o_to_move_board_count(void)
{
    return ac_ttt_web_last_result.report.o_to_move_board_count;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_reachable_terminal_board_count(void)
{
    return ac_ttt_web_last_result.report.reachable_terminal_board_count;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_required_policy_state_count(void)
{
    return ac_ttt_web_last_result.report.required_policy_state_count;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_explored_node_count(void)
{
    return ac_ttt_web_last_result.report.explored_node_count;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_explored_terminal_node_count(void)
{
    return ac_ttt_web_last_result.report.explored_terminal_node_count;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_counterexample_length(void)
{
    return (uint32_t)ac_ttt_web_last_result.report.counterexample_length;
}

AC_TTT_WEB_EXPORT uint32_t ac_ttt_web_get_counterexample_move(uint32_t index)
{
    if (index >= ac_ttt_web_last_result.report.counterexample_length) {
        return (uint32_t)AC_TTT_NO_MOVE;
    }
    return (uint32_t)ac_ttt_web_last_result.report.counterexample_moves[index];
}
