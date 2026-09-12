#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ac/ttt.h"
#include "test_support.h"

static const uint8_t winning_lines[8][3] = {
    {0U, 1U, 2U},
    {3U, 4U, 5U},
    {6U, 7U, 8U},
    {0U, 3U, 6U},
    {1U, 4U, 7U},
    {2U, 5U, 8U},
    {0U, 4U, 8U},
    {2U, 4U, 6U}
};

static uint32_t board_index(
    const uint8_t board[AC_TTT_BOARD_CELLS])
{
    uint32_t index = 0U;
    uint32_t factor = 1U;
    size_t cell;

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        index += (uint32_t)board[cell] * factor;
        factor *= 3U;
    }
    return index;
}

static ac_ttt_role board_winner(
    const uint8_t board[AC_TTT_BOARD_CELLS])
{
    size_t line;

    for (line = 0U; line < 8U; ++line) {
        uint8_t first = board[winning_lines[line][0]];

        if (first != 0U &&
            board[winning_lines[line][1]] == first &&
            board[winning_lines[line][2]] == first) {
            return (ac_ttt_role)first;
        }
    }
    return (ac_ttt_role)0;
}

static int board_is_full(
    const uint8_t board[AC_TTT_BOARD_CELLS])
{
    size_t cell;

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        if (board[cell] == 0U) {
            return 0;
        }
    }
    return 1;
}

static unsigned board_ply(
    const uint8_t board[AC_TTT_BOARD_CELLS])
{
    unsigned occupied = 0U;
    size_t cell;

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        if (board[cell] != 0U) {
            ++occupied;
        }
    }
    return occupied;
}

static ac_ttt_role board_turn(
    const uint8_t board[AC_TTT_BOARD_CELLS])
{
    unsigned x_count = 0U;
    unsigned o_count = 0U;
    size_t cell;

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        if (board[cell] == (uint8_t)AC_TTT_ROLE_X) {
            ++x_count;
        } else if (board[cell] == (uint8_t)AC_TTT_ROLE_O) {
            ++o_count;
        }
    }
    return x_count == o_count ? AC_TTT_ROLE_X : AC_TTT_ROLE_O;
}

static void decode_board(
    uint32_t index,
    uint8_t board[AC_TTT_BOARD_CELLS])
{
    size_t cell;

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        board[cell] = (uint8_t)(index % 3U);
        index /= 3U;
    }
}

static ac_ttt_role other_role(ac_ttt_role role)
{
    return role == AC_TTT_ROLE_X ? AC_TTT_ROLE_O : AC_TTT_ROLE_X;
}

static void oracle_mark_reachable(
    uint8_t board[AC_TTT_BOARD_CELLS],
    ac_ttt_role turn,
    uint8_t reachable[AC_TTT_STATE_COUNT],
    uint8_t terminal[AC_TTT_STATE_COUNT])
{
    uint32_t index = board_index(board);
    size_t cell;

    if (reachable[index] != 0U) {
        return;
    }
    reachable[index] = 1U;
    if (board_winner(board) != (ac_ttt_role)0 || board_is_full(board)) {
        terminal[index] = 1U;
        return;
    }

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        if (board[cell] == 0U) {
            board[cell] = (uint8_t)turn;
            oracle_mark_reachable(
                board, other_role(turn), reachable, terminal);
            board[cell] = 0U;
        }
    }
}

typedef struct {
    uint8_t reachable[AC_TTT_STATE_COUNT];
    uint32_t state_by_ply[AC_TTT_MAX_PLIES + 1U];
    uint32_t nonterminal_by_ply[AC_TTT_MAX_PLIES + 1U];
    uint32_t terminal_by_ply[AC_TTT_MAX_PLIES + 1U];
    uint32_t edge_by_ply[AC_TTT_MAX_PLIES + 1U];
    uint32_t x_win_by_ply[AC_TTT_MAX_PLIES + 1U];
    uint32_t o_win_by_ply[AC_TTT_MAX_PLIES + 1U];
    uint32_t draw_by_ply[AC_TTT_MAX_PLIES + 1U];
    uint32_t x_edge_count;
    uint32_t o_edge_count;
} oracle_graph_census;

static void oracle_census_visit(
    uint8_t board[AC_TTT_BOARD_CELLS],
    ac_ttt_role turn,
    oracle_graph_census *census)
{
    uint32_t index = board_index(board);
    unsigned ply;
    ac_ttt_role winner;
    size_t cell;

    if (census->reachable[index] != 0U) {
        return;
    }
    census->reachable[index] = 1U;
    ply = board_ply(board);
    ++census->state_by_ply[ply];
    winner = board_winner(board);
    if (winner != (ac_ttt_role)0 || board_is_full(board)) {
        ++census->terminal_by_ply[ply];
        if (winner == AC_TTT_ROLE_X) {
            ++census->x_win_by_ply[ply];
        } else if (winner == AC_TTT_ROLE_O) {
            ++census->o_win_by_ply[ply];
        } else {
            ++census->draw_by_ply[ply];
        }
        return;
    }

    ++census->nonterminal_by_ply[ply];
    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        if (board[cell] == 0U) {
            ++census->edge_by_ply[ply];
            if (turn == AC_TTT_ROLE_X) {
                ++census->x_edge_count;
            } else {
                ++census->o_edge_count;
            }
            board[cell] = (uint8_t)turn;
            oracle_census_visit(
                board, other_role(turn), census);
            board[cell] = 0U;
        }
    }
}

static void oracle_history_visit(
    uint8_t board[AC_TTT_BOARD_CELLS],
    ac_ttt_role turn,
    uint64_t *node_count,
    uint64_t *terminal_game_count)
{
    size_t cell;

    ++*node_count;
    if (board_winner(board) != (ac_ttt_role)0 || board_is_full(board)) {
        ++*terminal_game_count;
        return;
    }

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        if (board[cell] == 0U) {
            board[cell] = (uint8_t)turn;
            oracle_history_visit(
                board,
                other_role(turn),
                node_count,
                terminal_game_count);
            board[cell] = 0U;
        }
    }
}

/*
 * Independent test oracle: classify legal states by exhaustive history
 * traversal, then solve their values bottom-up by ply. It deliberately does
 * not call the production verifier or reuse its recursive minimax routine.
 */
static void oracle_build_expected_policy(
    ac_ttt_role claimant,
    ac_ttt_policy *expected)
{
    uint8_t reachable[AC_TTT_STATE_COUNT];
    uint8_t terminal[AC_TTT_STATE_COUNT];
    int8_t value_by_board[AC_TTT_STATE_COUNT];
    uint8_t board[AC_TTT_BOARD_CELLS] = {0U};
    int ply;
    uint32_t index;

    (void)memset(expected->move_by_board, AC_TTT_NO_MOVE,
                 sizeof expected->move_by_board);
    (void)memset(reachable, 0, sizeof reachable);
    (void)memset(terminal, 0, sizeof terminal);
    (void)memset(value_by_board, 0, sizeof value_by_board);
    oracle_mark_reachable(
        board, AC_TTT_ROLE_X, reachable, terminal);

    for (ply = (int)AC_TTT_MAX_PLIES; ply >= 0; --ply) {
        for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
            ac_ttt_role winner;
            ac_ttt_role turn;
            int best;
            size_t cell;

            if (reachable[index] == 0U) {
                continue;
            }
            decode_board(index, board);
            if (board_ply(board) != (unsigned)ply) {
                continue;
            }
            winner = board_winner(board);
            if (winner != (ac_ttt_role)0) {
                value_by_board[index] =
                    (int8_t)(winner == claimant ? 1 : -1);
                continue;
            }
            if (terminal[index] != 0U) {
                value_by_board[index] = 0;
                continue;
            }

            turn = board_turn(board);
            best = turn == claimant ? -2 : 2;
            for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
                int child_value;

                if (board[cell] != 0U) {
                    continue;
                }
                board[cell] = (uint8_t)turn;
                child_value = value_by_board[board_index(board)];
                board[cell] = 0U;
                if ((turn == claimant && child_value > best) ||
                    (turn != claimant && child_value < best)) {
                    best = child_value;
                }
            }
            value_by_board[index] = (int8_t)best;
        }
    }

    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        int best = -2;
        uint8_t best_move = AC_TTT_NO_MOVE;
        size_t cell;

        if (reachable[index] == 0U || terminal[index] != 0U) {
            continue;
        }
        decode_board(index, board);
        if (board_turn(board) != claimant) {
            continue;
        }
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            int child_value;

            if (board[cell] != 0U) {
                continue;
            }
            board[cell] = (uint8_t)claimant;
            child_value = value_by_board[board_index(board)];
            board[cell] = 0U;
            if (child_value > best) {
                best = child_value;
                best_move = (uint8_t)cell;
            }
        }
        expected->move_by_board[index] = best_move;
    }
}

static size_t count_policy_moves(const ac_ttt_policy *policy)
{
    size_t count = 0U;
    size_t index;

    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        if (policy->move_by_board[index] != AC_TTT_NO_MOVE) {
            ++count;
        }
    }
    return count;
}

static int assert_common_counts(const ac_ttt_report *report)
{
    AC_TEST_ASSERT_EQ_INT(report->reachable_board_count, 5478U);
    AC_TEST_ASSERT_EQ_INT(report->x_to_move_board_count, 2423U);
    AC_TEST_ASSERT_EQ_INT(report->o_to_move_board_count, 2097U);
    AC_TEST_ASSERT_EQ_INT(report->reachable_terminal_board_count, 958U);
    return 0;
}

static int assert_dag_common_counts(
    ac_ttt_role role,
    const ac_ttt_dag_report *report)
{
    uint32_t claimant_edge_count = role == AC_TTT_ROLE_X ? 8631U : 7536U;
    uint32_t opponent_edge_count = role == AC_TTT_ROLE_X ? 7536U : 8631U;

    AC_TEST_ASSERT_EQ_INT(report->reachable_board_count, 5478U);
    AC_TEST_ASSERT_EQ_INT(report->x_to_move_board_count, 2423U);
    AC_TEST_ASSERT_EQ_INT(report->o_to_move_board_count, 2097U);
    AC_TEST_ASSERT_EQ_INT(report->reachable_terminal_board_count, 958U);
    AC_TEST_ASSERT_EQ_INT(
        report->required_policy_state_count,
        role == AC_TTT_ROLE_X ? 2423U : 2097U);
    AC_TEST_ASSERT_EQ_INT(report->x_win_terminal_board_count, 626U);
    AC_TEST_ASSERT_EQ_INT(report->o_win_terminal_board_count, 316U);
    AC_TEST_ASSERT_EQ_INT(report->draw_terminal_board_count, 16U);
    AC_TEST_ASSERT_EQ_INT(report->x_edge_count, 8631U);
    AC_TEST_ASSERT_EQ_INT(report->o_edge_count, 7536U);
    AC_TEST_ASSERT_EQ_INT(report->edge_count, 16167U);
    AC_TEST_ASSERT_EQ_INT(report->graph_scan_entry_count, 19683U);
    AC_TEST_ASSERT_EQ_INT(report->policy_scan_entry_count, 19683U);
    AC_TEST_ASSERT_EQ_INT(report->value_scan_entry_count, 19683U);
    AC_TEST_ASSERT_EQ_INT(report->evaluated_board_count, 5478U);
    AC_TEST_ASSERT_EQ_INT(
        report->selector_equality_count, claimant_edge_count);
    AC_TEST_ASSERT_EQ_INT(report->selector_and_count, claimant_edge_count);
    AC_TEST_ASSERT_EQ_INT(report->selector_or_count, claimant_edge_count);
    AC_TEST_ASSERT_EQ_INT(report->opponent_and_count, opponent_edge_count);
    AC_TEST_ASSERT_EQ_INT(report->serialized_policy_byte_count, 19683U);
    AC_TEST_ASSERT_EQ_INT(report->classification_table_byte_count, 19683U);
    AC_TEST_ASSERT_EQ_INT(report->value_table_byte_count, 19683U);
    AC_TEST_ASSERT_EQ_INT(report->board_scratch_byte_count, 9U);
    AC_TEST_ASSERT_EQ_INT(report->explicit_scratch_byte_count, 39375U);
    return 0;
}

static int assert_dag_matches_recursive(
    ac_ttt_role role,
    const ac_ttt_policy *policy)
{
    ac_ttt_report recursive_report;
    ac_ttt_dag_report dag_report;

    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(role, policy, &recursive_report), AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_evaluate_policy_dag(role, policy, &dag_report), AC_OK);
    AC_TEST_ASSERT_EQ_INT(dag_report.verdict, recursive_report.verdict);
    AC_TEST_ASSERT_EQ_INT(assert_dag_common_counts(role, &dag_report), 0);
    if (recursive_report.verdict == AC_TTT_VERDICT_INVALID_POLICY) {
        AC_TEST_ASSERT_EQ_INT(
            dag_report.policy_reason, recursive_report.policy_reason);
        AC_TEST_ASSERT_EQ_INT(
            dag_report.invalid_board_index,
            recursive_report.invalid_board_index);
        AC_TEST_ASSERT_EQ_INT(
            dag_report.invalid_move, recursive_report.invalid_move);
    } else {
        AC_TEST_ASSERT_EQ_INT(
            dag_report.policy_reason, AC_TTT_POLICY_REASON_NONE);
        AC_TEST_ASSERT_EQ_INT(
            dag_report.invalid_board_index, AC_TTT_STATE_COUNT);
        AC_TEST_ASSERT_EQ_INT(dag_report.invalid_move, AC_TTT_NO_MOVE);
    }
    return 0;
}

static int assert_replay_is_opponent_win(
    ac_ttt_role claimant,
    const ac_ttt_policy *policy,
    const ac_ttt_report *report)
{
    uint8_t board[AC_TTT_BOARD_CELLS] = {0U};
    ac_ttt_role turn = AC_TTT_ROLE_X;
    size_t ply;

    AC_TEST_ASSERT(report->counterexample_length > 0U);
    AC_TEST_ASSERT(report->counterexample_length <= AC_TTT_MAX_PLIES);
    for (ply = 0U; ply < report->counterexample_length; ++ply) {
        uint8_t move = report->counterexample_moves[ply];
        ac_ttt_role winner;

        AC_TEST_ASSERT_EQ_INT(board_winner(board), 0U);
        AC_TEST_ASSERT(move < AC_TTT_BOARD_CELLS);
        AC_TEST_ASSERT_EQ_INT(board[move], 0U);
        if (turn == claimant) {
            AC_TEST_ASSERT_EQ_INT(
                policy->move_by_board[board_index(board)], move);
        }
        board[move] = (uint8_t)turn;
        winner = board_winner(board);
        if (ply + 1U < report->counterexample_length) {
            AC_TEST_ASSERT_EQ_INT(winner, 0U);
        } else {
            AC_TEST_ASSERT_EQ_INT(winner, other_role(claimant));
        }
        turn = other_role(turn);
    }
    return 0;
}

static int test_reference_policy_matches_independent_retrograde_oracle(void)
{
    const ac_ttt_role roles[2] = {AC_TTT_ROLE_X, AC_TTT_ROLE_O};
    size_t role_index;

    for (role_index = 0U; role_index < 2U; ++role_index) {
        ac_ttt_policy actual;
        ac_ttt_policy expected;

        AC_TEST_ASSERT_STATUS(
            ac_ttt_build_reference_policy(roles[role_index], &actual), AC_OK);
        oracle_build_expected_policy(roles[role_index], &expected);
        AC_TEST_ASSERT_MEMORY(&actual, &expected, sizeof actual);
    }
    return 0;
}

static int test_reference_policies_and_state_counts(void)
{
    ac_ttt_policy x_policy;
    ac_ttt_policy o_policy;
    ac_ttt_report x_report;
    ac_ttt_report o_report;

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &x_policy), AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_O, &o_policy), AC_OK);
    AC_TEST_ASSERT_EQ_INT(x_policy.move_by_board[0], 0U);
    AC_TEST_ASSERT_EQ_INT(o_policy.move_by_board[1], 4U);
    AC_TEST_ASSERT_EQ_INT(count_policy_moves(&x_policy), 2423U);
    AC_TEST_ASSERT_EQ_INT(count_policy_moves(&o_policy), 2097U);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_X, &x_policy, &x_report), AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_O, &o_policy, &o_report), AC_OK);
    AC_TEST_ASSERT_EQ_INT(x_report.verdict, AC_TTT_VERDICT_NON_LOSING);
    AC_TEST_ASSERT_EQ_INT(o_report.verdict, AC_TTT_VERDICT_NON_LOSING);
    AC_TEST_ASSERT_EQ_INT(x_report.policy_reason, AC_TTT_POLICY_REASON_NONE);
    AC_TEST_ASSERT_EQ_INT(o_report.policy_reason, AC_TTT_POLICY_REASON_NONE);
    AC_TEST_ASSERT_EQ_INT(assert_common_counts(&x_report), 0);
    AC_TEST_ASSERT_EQ_INT(assert_common_counts(&o_report), 0);
    AC_TEST_ASSERT_EQ_INT(x_report.required_policy_state_count, 2423U);
    AC_TEST_ASSERT_EQ_INT(o_report.required_policy_state_count, 2097U);
    AC_TEST_ASSERT_EQ_INT(x_report.explored_node_count, 266U);
    AC_TEST_ASSERT_EQ_INT(x_report.explored_terminal_node_count, 101U);
    AC_TEST_ASSERT_EQ_INT(o_report.explored_node_count, 2056U);
    AC_TEST_ASSERT_EQ_INT(o_report.explored_terminal_node_count, 681U);
    AC_TEST_ASSERT_EQ_INT(x_report.counterexample_length, 0U);
    AC_TEST_ASSERT_EQ_INT(o_report.counterexample_length, 0U);
    return 0;
}

static int test_reference_and_report_determinism(void)
{
    ac_ttt_policy first;
    ac_ttt_policy second;
    ac_ttt_report first_report;
    ac_ttt_report second_report;

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &first), AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &second), AC_OK);
    AC_TEST_ASSERT_MEMORY(&first, &second, sizeof first);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_X, &first, &first_report), AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_X, &first, &second_report), AC_OK);
    AC_TEST_ASSERT_MEMORY(
        &first_report, &second_report, sizeof first_report);
    return 0;
}

static int test_canonical_losing_policy_and_replay(void)
{
    static const uint8_t expected_trace[8] = {
        0U, 2U, 1U, 5U, 3U, 6U, 4U, 8U
    };
    ac_ttt_policy policy;
    ac_ttt_report report;

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    AC_TEST_ASSERT_EQ_INT(policy.move_by_board[19], 3U);
    policy.move_by_board[19] = 1U;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_X, &policy, &report), AC_OK);
    AC_TEST_ASSERT_EQ_INT(report.verdict, AC_TTT_VERDICT_LOSING);
    AC_TEST_ASSERT_EQ_INT(report.policy_reason, AC_TTT_POLICY_REASON_NONE);
    AC_TEST_ASSERT_EQ_INT(report.counterexample_length, sizeof expected_trace);
    AC_TEST_ASSERT_MEMORY(
        report.counterexample_moves, expected_trace, sizeof expected_trace);
    AC_TEST_ASSERT_EQ_INT(
        assert_replay_is_opponent_win(AC_TTT_ROLE_X, &policy, &report), 0);
    return 0;
}

static int test_late_opponent_branch_is_exhaustive(void)
{
    static const uint8_t expected_trace[9] = {
        8U, 7U, 2U, 0U, 4U, 1U, 3U, 5U, 6U
    };
    ac_ttt_policy policy;
    ac_ttt_report report;

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_O, &policy), AC_OK);
    AC_TEST_ASSERT_EQ_INT(policy.move_by_board[6561], 4U);
    policy.move_by_board[6561] = 7U;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_O, &policy, &report), AC_OK);
    AC_TEST_ASSERT_EQ_INT(report.verdict, AC_TTT_VERDICT_LOSING);
    AC_TEST_ASSERT_EQ_INT(report.counterexample_length, sizeof expected_trace);
    AC_TEST_ASSERT_MEMORY(
        report.counterexample_moves, expected_trace, sizeof expected_trace);
    AC_TEST_ASSERT_EQ_INT(report.explored_node_count, 1924U);
    AC_TEST_ASSERT_EQ_INT(report.explored_terminal_node_count, 631U);
    AC_TEST_ASSERT_EQ_INT(
        assert_replay_is_opponent_win(AC_TTT_ROLE_O, &policy, &report), 0);
    return 0;
}

static int test_required_entry_failures(void)
{
    ac_ttt_policy policy;
    ac_ttt_report report;

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    policy.move_by_board[0] = AC_TTT_NO_MOVE;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_X, &policy, &report), AC_OK);
    AC_TEST_ASSERT_EQ_INT(report.verdict, AC_TTT_VERDICT_INVALID_POLICY);
    AC_TEST_ASSERT_EQ_INT(
        report.policy_reason, AC_TTT_POLICY_REASON_MISSING_MOVE);
    AC_TEST_ASSERT_EQ_INT(report.invalid_board_index, 0U);
    AC_TEST_ASSERT_EQ_INT(report.invalid_move, AC_TTT_NO_MOVE);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    policy.move_by_board[0] = AC_TTT_BOARD_CELLS;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_X, &policy, &report), AC_OK);
    AC_TEST_ASSERT_EQ_INT(report.verdict, AC_TTT_VERDICT_INVALID_POLICY);
    AC_TEST_ASSERT_EQ_INT(
        report.policy_reason, AC_TTT_POLICY_REASON_OUT_OF_RANGE_MOVE);
    AC_TEST_ASSERT_EQ_INT(report.invalid_board_index, 0U);
    AC_TEST_ASSERT_EQ_INT(report.invalid_move, AC_TTT_BOARD_CELLS);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    AC_TEST_ASSERT_EQ_INT(policy.move_by_board[19], 3U);
    policy.move_by_board[19] = 0U;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_X, &policy, &report), AC_OK);
    AC_TEST_ASSERT_EQ_INT(report.verdict, AC_TTT_VERDICT_INVALID_POLICY);
    AC_TEST_ASSERT_EQ_INT(
        report.policy_reason, AC_TTT_POLICY_REASON_OCCUPIED_CELL);
    AC_TEST_ASSERT_EQ_INT(report.invalid_board_index, 19U);
    AC_TEST_ASSERT_EQ_INT(report.invalid_move, 0U);
    return 0;
}

static int expect_unexpected_entry(
    const uint8_t board[AC_TTT_BOARD_CELLS],
    uint8_t move)
{
    ac_ttt_policy policy;
    ac_ttt_report report;
    uint32_t index = board_index(board);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    AC_TEST_ASSERT_EQ_INT(policy.move_by_board[index], AC_TTT_NO_MOVE);
    policy.move_by_board[index] = move;
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_X, &policy, &report), AC_OK);
    AC_TEST_ASSERT_EQ_INT(report.verdict, AC_TTT_VERDICT_INVALID_POLICY);
    AC_TEST_ASSERT_EQ_INT(
        report.policy_reason, AC_TTT_POLICY_REASON_UNEXPECTED_ENTRY);
    AC_TEST_ASSERT_EQ_INT(report.invalid_board_index, index);
    AC_TEST_ASSERT_EQ_INT(report.invalid_move, move);
    return 0;
}

static int test_unexpected_opponent_terminal_and_invalid_entries(void)
{
    const uint8_t opponent_turn[AC_TTT_BOARD_CELLS] = {
        1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U
    };
    const uint8_t terminal[AC_TTT_BOARD_CELLS] = {
        1U, 1U, 1U, 2U, 2U, 0U, 0U, 0U, 0U
    };
    const uint8_t wrong_count[AC_TTT_BOARD_CELLS] = {
        2U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U
    };
    const uint8_t simultaneous_winners[AC_TTT_BOARD_CELLS] = {
        1U, 1U, 1U, 2U, 2U, 2U, 0U, 0U, 0U
    };
    const uint8_t post_win[AC_TTT_BOARD_CELLS] = {
        1U, 1U, 1U, 2U, 2U, 0U, 2U, 0U, 0U
    };

    AC_TEST_ASSERT_EQ_INT(expect_unexpected_entry(opponent_turn, 4U), 0);
    AC_TEST_ASSERT_EQ_INT(expect_unexpected_entry(terminal, 5U), 0);
    AC_TEST_ASSERT_EQ_INT(expect_unexpected_entry(wrong_count, 1U), 0);
    AC_TEST_ASSERT_EQ_INT(
        expect_unexpected_entry(simultaneous_winners, 6U), 0);
    AC_TEST_ASSERT_EQ_INT(expect_unexpected_entry(post_win, 7U), 0);
    return 0;
}

static int test_independent_graph_and_history_census(void)
{
    static const uint32_t expected_states[AC_TTT_MAX_PLIES + 1U] = {
        1U, 9U, 72U, 252U, 756U, 1260U, 1520U, 1140U, 390U, 78U
    };
    static const uint32_t expected_nonterminal[AC_TTT_MAX_PLIES + 1U] = {
        1U, 9U, 72U, 252U, 756U, 1140U, 1372U, 696U, 222U, 0U
    };
    static const uint32_t expected_terminal[AC_TTT_MAX_PLIES + 1U] = {
        0U, 0U, 0U, 0U, 0U, 120U, 148U, 444U, 168U, 78U
    };
    static const uint32_t expected_edges[AC_TTT_MAX_PLIES + 1U] = {
        9U, 72U, 504U, 1512U, 3780U,
        4560U, 4116U, 1392U, 222U, 0U
    };
    static const uint32_t expected_x_wins[AC_TTT_MAX_PLIES + 1U] = {
        0U, 0U, 0U, 0U, 0U, 120U, 0U, 444U, 0U, 62U
    };
    static const uint32_t expected_o_wins[AC_TTT_MAX_PLIES + 1U] = {
        0U, 0U, 0U, 0U, 0U, 0U, 148U, 0U, 168U, 0U
    };
    static const uint32_t expected_draws[AC_TTT_MAX_PLIES + 1U] = {
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 16U
    };
    oracle_graph_census census;
    uint8_t board[AC_TTT_BOARD_CELLS] = {0U};
    uint64_t history_nodes = 0U;
    uint64_t terminal_games = 0U;
    uint32_t state_total = 0U;
    uint32_t nonterminal_total = 0U;
    uint32_t terminal_total = 0U;
    uint32_t edge_total = 0U;
    uint32_t x_win_total = 0U;
    uint32_t o_win_total = 0U;
    uint32_t draw_total = 0U;
    size_t ply;

    (void)memset(&census, 0, sizeof census);
    oracle_census_visit(board, AC_TTT_ROLE_X, &census);
    for (ply = 0U; ply <= AC_TTT_MAX_PLIES; ++ply) {
        AC_TEST_ASSERT_EQ_INT(census.state_by_ply[ply], expected_states[ply]);
        AC_TEST_ASSERT_EQ_INT(
            census.nonterminal_by_ply[ply], expected_nonterminal[ply]);
        AC_TEST_ASSERT_EQ_INT(
            census.terminal_by_ply[ply], expected_terminal[ply]);
        AC_TEST_ASSERT_EQ_INT(census.edge_by_ply[ply], expected_edges[ply]);
        AC_TEST_ASSERT_EQ_INT(census.x_win_by_ply[ply], expected_x_wins[ply]);
        AC_TEST_ASSERT_EQ_INT(census.o_win_by_ply[ply], expected_o_wins[ply]);
        AC_TEST_ASSERT_EQ_INT(census.draw_by_ply[ply], expected_draws[ply]);
        state_total += census.state_by_ply[ply];
        nonterminal_total += census.nonterminal_by_ply[ply];
        terminal_total += census.terminal_by_ply[ply];
        edge_total += census.edge_by_ply[ply];
        x_win_total += census.x_win_by_ply[ply];
        o_win_total += census.o_win_by_ply[ply];
        draw_total += census.draw_by_ply[ply];
    }
    AC_TEST_ASSERT_EQ_INT(state_total, 5478U);
    AC_TEST_ASSERT_EQ_INT(nonterminal_total, 4520U);
    AC_TEST_ASSERT_EQ_INT(terminal_total, 958U);
    AC_TEST_ASSERT_EQ_INT(edge_total, 16167U);
    AC_TEST_ASSERT_EQ_INT(census.x_edge_count, 8631U);
    AC_TEST_ASSERT_EQ_INT(census.o_edge_count, 7536U);
    AC_TEST_ASSERT_EQ_INT(x_win_total, 626U);
    AC_TEST_ASSERT_EQ_INT(o_win_total, 316U);
    AC_TEST_ASSERT_EQ_INT(draw_total, 16U);

    (void)memset(board, 0, sizeof board);
    oracle_history_visit(
        board, AC_TTT_ROLE_X, &history_nodes, &terminal_games);
    AC_TEST_ASSERT_EQ_INT(history_nodes, UINT64_C(549946));
    AC_TEST_ASSERT_EQ_INT(terminal_games, UINT64_C(255168));
    return 0;
}

static int test_dag_reference_losses_and_determinism(void)
{
    const ac_ttt_role roles[2] = {AC_TTT_ROLE_X, AC_TTT_ROLE_O};
    size_t role_index;

    for (role_index = 0U; role_index < 2U; ++role_index) {
        ac_ttt_policy policy;
        ac_ttt_dag_report first;
        ac_ttt_dag_report second;

        AC_TEST_ASSERT_STATUS(
            ac_ttt_build_reference_policy(roles[role_index], &policy), AC_OK);
        AC_TEST_ASSERT_EQ_INT(
            assert_dag_matches_recursive(roles[role_index], &policy), 0);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_evaluate_policy_dag(
                roles[role_index], &policy, &first),
            AC_OK);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_evaluate_policy_dag(
                roles[role_index], &policy, &second),
            AC_OK);
        AC_TEST_ASSERT_MEMORY(&first, &second, sizeof first);
        AC_TEST_ASSERT_EQ_INT(first.verdict, AC_TTT_VERDICT_NON_LOSING);
    }

    {
        ac_ttt_policy policy;
        ac_ttt_dag_report report;

        AC_TEST_ASSERT_STATUS(
            ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
        policy.move_by_board[19] = 1U;
        AC_TEST_ASSERT_EQ_INT(
            assert_dag_matches_recursive(AC_TTT_ROLE_X, &policy), 0);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_evaluate_policy_dag(
                AC_TTT_ROLE_X, &policy, &report),
            AC_OK);
        AC_TEST_ASSERT_EQ_INT(report.verdict, AC_TTT_VERDICT_LOSING);

        AC_TEST_ASSERT_STATUS(
            ac_ttt_build_reference_policy(AC_TTT_ROLE_O, &policy), AC_OK);
        policy.move_by_board[6561] = 7U;
        AC_TEST_ASSERT_EQ_INT(
            assert_dag_matches_recursive(AC_TTT_ROLE_O, &policy), 0);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_evaluate_policy_dag(
                AC_TTT_ROLE_O, &policy, &report),
            AC_OK);
        AC_TEST_ASSERT_EQ_INT(report.verdict, AC_TTT_VERDICT_LOSING);
    }
    return 0;
}

static int test_dag_malformed_and_byte_domain_differential(void)
{
    const uint8_t opponent_turn[AC_TTT_BOARD_CELLS] = {
        1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U
    };
    const uint8_t terminal[AC_TTT_BOARD_CELLS] = {
        1U, 1U, 1U, 2U, 2U, 0U, 0U, 0U, 0U
    };
    const uint8_t wrong_count[AC_TTT_BOARD_CELLS] = {
        2U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U
    };
    const uint8_t simultaneous_winners[AC_TTT_BOARD_CELLS] = {
        1U, 1U, 1U, 2U, 2U, 2U, 0U, 0U, 0U
    };
    const uint8_t post_win[AC_TTT_BOARD_CELLS] = {
        1U, 1U, 1U, 2U, 2U, 0U, 2U, 0U, 0U
    };
    const uint8_t *unexpected_boards[5] = {
        opponent_turn, terminal, wrong_count, simultaneous_winners, post_win
    };
    ac_ttt_policy policy;
    uint32_t byte_value;
    size_t board_case;

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    for (byte_value = 0U; byte_value <= UINT8_MAX; ++byte_value) {
        policy.move_by_board[0] = (uint8_t)byte_value;
        AC_TEST_ASSERT_EQ_INT(
            assert_dag_matches_recursive(AC_TTT_ROLE_X, &policy), 0);
    }

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    policy.move_by_board[19] = 0U;
    AC_TEST_ASSERT_EQ_INT(
        assert_dag_matches_recursive(AC_TTT_ROLE_X, &policy), 0);

    for (board_case = 0U; board_case < 5U; ++board_case) {
        uint32_t index = board_index(unexpected_boards[board_case]);

        AC_TEST_ASSERT_STATUS(
            ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
        policy.move_by_board[index] = 0U;
        AC_TEST_ASSERT_EQ_INT(
            assert_dag_matches_recursive(AC_TTT_ROLE_X, &policy), 0);
    }

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    AC_TEST_ASSERT_EQ_INT(policy.move_by_board[1], AC_TTT_NO_MOVE);
    policy.move_by_board[1] = 4U;
    AC_TEST_ASSERT_EQ_INT(
        assert_dag_matches_recursive(AC_TTT_ROLE_X, &policy), 0);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        policy.move_by_board[AC_TTT_STATE_COUNT - 1U], AC_TTT_NO_MOVE);
    policy.move_by_board[AC_TTT_STATE_COUNT - 1U] = 0U;
    AC_TEST_ASSERT_EQ_INT(
        assert_dag_matches_recursive(AC_TTT_ROLE_X, &policy), 0);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    policy.move_by_board[0] = AC_TTT_NO_MOVE;
    policy.move_by_board[1] = 4U;
    AC_TEST_ASSERT_EQ_INT(
        assert_dag_matches_recursive(AC_TTT_ROLE_X, &policy), 0);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy), AC_OK);
    policy.move_by_board[1] = 4U;
    policy.move_by_board[19] = AC_TTT_NO_MOVE;
    AC_TEST_ASSERT_EQ_INT(
        assert_dag_matches_recursive(AC_TTT_ROLE_X, &policy), 0);
    return 0;
}

static int test_dag_all_reference_single_entry_legal_mutations(void)
{
    const ac_ttt_role roles[2] = {AC_TTT_ROLE_X, AC_TTT_ROLE_O};
    const uint32_t expected_mutations[2] = {6208U, 5439U};
    size_t role_index;

    for (role_index = 0U; role_index < 2U; ++role_index) {
        ac_ttt_policy policy;
        uint8_t board[AC_TTT_BOARD_CELLS];
        uint32_t mutation_count = 0U;
        uint32_t index;

        AC_TEST_ASSERT_STATUS(
            ac_ttt_build_reference_policy(roles[role_index], &policy), AC_OK);
        for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
            uint8_t original_move = policy.move_by_board[index];
            size_t cell;

            if (original_move == AC_TTT_NO_MOVE) {
                continue;
            }
            decode_board(index, board);
            for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
                if (board[cell] != 0U || (uint8_t)cell == original_move) {
                    continue;
                }
                policy.move_by_board[index] = (uint8_t)cell;
                AC_TEST_ASSERT_EQ_INT(
                    assert_dag_matches_recursive(
                        roles[role_index], &policy),
                    0);
                policy.move_by_board[index] = original_move;
                ++mutation_count;
            }
        }
        AC_TEST_ASSERT_EQ_INT(
            mutation_count, expected_mutations[role_index]);
    }
    return 0;
}

static uint32_t deterministic_corpus_mix(uint32_t value)
{
    value ^= value >> 16;
    value *= UINT32_C(0x7feb352d);
    value ^= value >> 15;
    value *= UINT32_C(0x846ca68b);
    value ^= value >> 16;
    return value;
}

static size_t build_deterministic_corpus_policy(
    ac_ttt_role role,
    const ac_ttt_policy *reference,
    uint32_t seed,
    ac_ttt_policy *policy)
{
    uint8_t board[AC_TTT_BOARD_CELLS];
    size_t changed_count = 0U;
    uint32_t index;

    ac_ttt_policy_init(policy);
    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        uint8_t reference_move = reference->move_by_board[index];
        uint8_t empty_count = 0U;
        uint8_t selected_ordinal;
        uint8_t seen_empty = 0U;
        size_t cell;

        if (reference_move == AC_TTT_NO_MOVE) {
            continue;
        }
        decode_board(index, board);
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            if (board[cell] == 0U) {
                ++empty_count;
            }
        }
        selected_ordinal = (uint8_t)(
            deterministic_corpus_mix(
                seed ^ (index * UINT32_C(0x9e3779b9)) ^
                ((uint32_t)role * UINT32_C(0x85ebca6b))) %
            empty_count);
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            if (board[cell] != 0U) {
                continue;
            }
            if (seen_empty == selected_ordinal) {
                policy->move_by_board[index] = (uint8_t)cell;
                if ((uint8_t)cell != reference_move) {
                    ++changed_count;
                }
                break;
            }
            ++seen_empty;
        }
    }
    return changed_count;
}

static int test_dag_deterministic_multi_entry_corpus(void)
{
    const ac_ttt_role roles[2] = {AC_TTT_ROLE_X, AC_TTT_ROLE_O};
    size_t role_index;

    for (role_index = 0U; role_index < 2U; ++role_index) {
        ac_ttt_policy reference;
        ac_ttt_policy first;
        ac_ttt_policy second;
        uint32_t seed;

        AC_TEST_ASSERT_STATUS(
            ac_ttt_build_reference_policy(
                roles[role_index], &reference),
            AC_OK);
        AC_TEST_ASSERT(
            build_deterministic_corpus_policy(
                roles[role_index], &reference, 0U, &first) > 0U);
        AC_TEST_ASSERT(
            build_deterministic_corpus_policy(
                roles[role_index], &reference, 0U, &second) > 0U);
        AC_TEST_ASSERT_MEMORY(&first, &second, sizeof first);

        for (seed = 0U; seed < 256U; ++seed) {
            AC_TEST_ASSERT(
                build_deterministic_corpus_policy(
                    roles[role_index], &reference, seed, &first) > 0U);
            AC_TEST_ASSERT_EQ_INT(
                assert_dag_matches_recursive(
                    roles[role_index], &first),
                0);
        }
    }
    return 0;
}

static int test_initialization_and_argument_errors(void)
{
    ac_ttt_policy policy;
    ac_ttt_report report;
    ac_ttt_dag_report dag_report;
    size_t index;

    (void)memset(&policy, 0, sizeof policy);
    ac_ttt_policy_init(&policy);
    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        AC_TEST_ASSERT_EQ_INT(policy.move_by_board[index], AC_TTT_NO_MOVE);
    }
    ac_ttt_policy_init(NULL);

    (void)memset(&policy, 0, sizeof policy);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy((ac_ttt_role)0, &policy),
        AC_ERR_ARGUMENT);
    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        AC_TEST_ASSERT_EQ_INT(policy.move_by_board[index], AC_TTT_NO_MOVE);
    }
    AC_TEST_ASSERT_STATUS(
        ac_ttt_build_reference_policy(AC_TTT_ROLE_X, NULL), AC_ERR_ARGUMENT);

    (void)memset(&report, 0xa5, sizeof report);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy((ac_ttt_role)0, &policy, &report),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(&report, sizeof report);

    (void)memset(&report, 0xa5, sizeof report);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_X, NULL, &report),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(&report, sizeof report);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_verify_policy(AC_TTT_ROLE_X, &policy, NULL),
        AC_ERR_ARGUMENT);

    (void)memset(&dag_report, 0xa5, sizeof dag_report);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_evaluate_policy_dag(
            (ac_ttt_role)0, &policy, &dag_report),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(&dag_report, sizeof dag_report);

    (void)memset(&dag_report, 0xa5, sizeof dag_report);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_evaluate_policy_dag(
            AC_TTT_ROLE_X, NULL, &dag_report),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(&dag_report, sizeof dag_report);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_evaluate_policy_dag(AC_TTT_ROLE_X, &policy, NULL),
        AC_ERR_ARGUMENT);
    return 0;
}

int main(void)
{
    ac_test_suite suite = {0U, 0U};

    AC_TEST_RUN(
        suite, test_reference_policy_matches_independent_retrograde_oracle);
    AC_TEST_RUN(suite, test_reference_policies_and_state_counts);
    AC_TEST_RUN(suite, test_reference_and_report_determinism);
    AC_TEST_RUN(suite, test_canonical_losing_policy_and_replay);
    AC_TEST_RUN(suite, test_late_opponent_branch_is_exhaustive);
    AC_TEST_RUN(suite, test_required_entry_failures);
    AC_TEST_RUN(suite, test_unexpected_opponent_terminal_and_invalid_entries);
    AC_TEST_RUN(suite, test_independent_graph_and_history_census);
    AC_TEST_RUN(suite, test_dag_reference_losses_and_determinism);
    AC_TEST_RUN(suite, test_dag_malformed_and_byte_domain_differential);
    AC_TEST_RUN(
        suite, test_dag_all_reference_single_entry_legal_mutations);
    AC_TEST_RUN(suite, test_dag_deterministic_multi_entry_corpus);
    AC_TEST_RUN(suite, test_initialization_and_argument_errors);
    return ac_test_finish(&suite);
}
