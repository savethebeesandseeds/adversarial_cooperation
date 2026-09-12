#include "ac/ttt.h"

#include <stddef.h>
#include <string.h>

enum {
    AC_TTT_KIND_UNREACHABLE = 0,
    AC_TTT_KIND_TERMINAL = 1,
    AC_TTT_KIND_X_TO_MOVE = 2,
    AC_TTT_KIND_O_TO_MOVE = 3
};

typedef struct {
    uint8_t kind_by_board[AC_TTT_STATE_COUNT];
    uint32_t reachable_count;
    uint32_t x_to_move_count;
    uint32_t o_to_move_count;
    uint32_t terminal_count;
} ac_ttt_classification;

static const uint8_t ac_ttt_winning_lines[8][3] = {
    {0U, 1U, 2U},
    {3U, 4U, 5U},
    {6U, 7U, 8U},
    {0U, 3U, 6U},
    {1U, 4U, 7U},
    {2U, 5U, 8U},
    {0U, 4U, 8U},
    {2U, 4U, 6U}
};

static const uint32_t ac_ttt_powers_of_three[AC_TTT_BOARD_CELLS] = {
    1U, 3U, 9U, 27U, 81U, 243U, 729U, 2187U, 6561U
};

static int ac_ttt_role_is_valid(ac_ttt_role role)
{
    return role == AC_TTT_ROLE_X || role == AC_TTT_ROLE_O;
}

static ac_ttt_role ac_ttt_other_role(ac_ttt_role role)
{
    return role == AC_TTT_ROLE_X ? AC_TTT_ROLE_O : AC_TTT_ROLE_X;
}

static uint32_t ac_ttt_board_index(
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

static void ac_ttt_decode_board(
    uint32_t index,
    uint8_t board[AC_TTT_BOARD_CELLS])
{
    size_t cell;

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        board[cell] = (uint8_t)(index % 3U);
        index /= 3U;
    }
}

static ac_ttt_role ac_ttt_board_winner(
    const uint8_t board[AC_TTT_BOARD_CELLS])
{
    size_t line;

    for (line = 0U; line < 8U; ++line) {
        uint8_t first = board[ac_ttt_winning_lines[line][0]];

        if (first != 0U &&
            board[ac_ttt_winning_lines[line][1]] == first &&
            board[ac_ttt_winning_lines[line][2]] == first) {
            return (ac_ttt_role)first;
        }
    }
    return (ac_ttt_role)0;
}

static int ac_ttt_board_is_full(
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

static ac_ttt_role ac_ttt_board_turn(
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

static void ac_ttt_classify_visit(
    uint8_t board[AC_TTT_BOARD_CELLS],
    ac_ttt_role turn,
    ac_ttt_classification *classification)
{
    uint32_t index = ac_ttt_board_index(board);
    size_t cell;

    if (classification->kind_by_board[index] != AC_TTT_KIND_UNREACHABLE) {
        return;
    }

    ++classification->reachable_count;
    if (ac_ttt_board_winner(board) != (ac_ttt_role)0 ||
        ac_ttt_board_is_full(board)) {
        classification->kind_by_board[index] = AC_TTT_KIND_TERMINAL;
        ++classification->terminal_count;
        return;
    }

    if (turn == AC_TTT_ROLE_X) {
        classification->kind_by_board[index] = AC_TTT_KIND_X_TO_MOVE;
        ++classification->x_to_move_count;
    } else {
        classification->kind_by_board[index] = AC_TTT_KIND_O_TO_MOVE;
        ++classification->o_to_move_count;
    }

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        if (board[cell] == 0U) {
            board[cell] = (uint8_t)turn;
            ac_ttt_classify_visit(
                board, ac_ttt_other_role(turn), classification);
            board[cell] = 0U;
        }
    }
}

static void ac_ttt_build_classification(
    ac_ttt_classification *classification)
{
    uint8_t board[AC_TTT_BOARD_CELLS] = {0U};

    (void)memset(classification, 0, sizeof(*classification));
    ac_ttt_classify_visit(board, AC_TTT_ROLE_X, classification);
}

static int ac_ttt_minimax_score(
    uint8_t board[AC_TTT_BOARD_CELLS],
    ac_ttt_role claimant,
    int8_t score_by_board[AC_TTT_STATE_COUNT],
    uint8_t score_known[AC_TTT_STATE_COUNT])
{
    uint32_t index = ac_ttt_board_index(board);
    ac_ttt_role winner;
    ac_ttt_role turn;
    int best;
    size_t cell;

    if (score_known[index] != 0U) {
        return score_by_board[index];
    }

    winner = ac_ttt_board_winner(board);
    if (winner != (ac_ttt_role)0) {
        best = winner == claimant ? 1 : -1;
    } else if (ac_ttt_board_is_full(board)) {
        best = 0;
    } else {
        turn = ac_ttt_board_turn(board);
        best = turn == claimant ? -2 : 2;
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            int child_score;

            if (board[cell] != 0U) {
                continue;
            }
            board[cell] = (uint8_t)turn;
            child_score = ac_ttt_minimax_score(
                board, claimant, score_by_board, score_known);
            board[cell] = 0U;
            if ((turn == claimant && child_score > best) ||
                (turn != claimant && child_score < best)) {
                best = child_score;
            }
        }
    }

    score_by_board[index] = (int8_t)best;
    score_known[index] = 1U;
    return best;
}

void ac_ttt_policy_init(ac_ttt_policy *policy)
{
    if (policy != NULL) {
        (void)memset(
            policy->move_by_board,
            AC_TTT_NO_MOVE,
            sizeof(policy->move_by_board));
    }
}

ac_status ac_ttt_build_reference_policy(
    ac_ttt_role role,
    ac_ttt_policy *policy)
{
    ac_ttt_classification classification;
    int8_t score_by_board[AC_TTT_STATE_COUNT];
    uint8_t score_known[AC_TTT_STATE_COUNT];
    uint8_t board[AC_TTT_BOARD_CELLS] = {0U};
    uint8_t required_kind;
    uint32_t index;

    ac_ttt_policy_init(policy);
    if (policy == NULL || !ac_ttt_role_is_valid(role)) {
        return AC_ERR_ARGUMENT;
    }

    ac_ttt_build_classification(&classification);
    (void)memset(score_by_board, 0, sizeof(score_by_board));
    (void)memset(score_known, 0, sizeof(score_known));
    (void)ac_ttt_minimax_score(
        board, role, score_by_board, score_known);

    required_kind = role == AC_TTT_ROLE_X
                        ? AC_TTT_KIND_X_TO_MOVE
                        : AC_TTT_KIND_O_TO_MOVE;
    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        int best_score = -2;
        uint8_t best_move = AC_TTT_NO_MOVE;
        size_t cell;

        if (classification.kind_by_board[index] != required_kind) {
            continue;
        }
        ac_ttt_decode_board(index, board);
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            int child_score;

            if (board[cell] != 0U) {
                continue;
            }
            board[cell] = (uint8_t)role;
            child_score = ac_ttt_minimax_score(
                board, role, score_by_board, score_known);
            board[cell] = 0U;
            if (child_score > best_score) {
                best_score = child_score;
                best_move = (uint8_t)cell;
            }
        }
        policy->move_by_board[index] = best_move;
    }
    return AC_OK;
}

static void ac_ttt_set_invalid_policy(
    ac_ttt_report *report,
    ac_ttt_policy_reason reason,
    uint32_t board_index,
    uint8_t move)
{
    report->verdict = AC_TTT_VERDICT_INVALID_POLICY;
    report->policy_reason = reason;
    report->invalid_board_index = board_index;
    report->invalid_move = move;
}

static int ac_ttt_verify_visit(
    uint8_t board[AC_TTT_BOARD_CELLS],
    ac_ttt_role turn,
    ac_ttt_role claimant,
    const ac_ttt_policy *policy,
    ac_ttt_report *report,
    uint8_t path[AC_TTT_MAX_PLIES],
    uint8_t depth)
{
    ac_ttt_role winner;
    size_t cell;

    ++report->explored_node_count;
    winner = ac_ttt_board_winner(board);
    if (winner != (ac_ttt_role)0 || ac_ttt_board_is_full(board)) {
        ++report->explored_terminal_node_count;
        if (winner == ac_ttt_other_role(claimant)) {
            report->counterexample_length = depth;
            if (depth != 0U) {
                (void)memcpy(
                    report->counterexample_moves, path, (size_t)depth);
            }
            return 0;
        }
        return 1;
    }

    if (turn == claimant) {
        uint32_t index = ac_ttt_board_index(board);
        uint8_t move = policy->move_by_board[index];
        int non_losing;

        path[depth] = move;
        board[move] = (uint8_t)turn;
        non_losing = ac_ttt_verify_visit(
            board,
            ac_ttt_other_role(turn),
            claimant,
            policy,
            report,
            path,
            (uint8_t)(depth + 1U));
        board[move] = 0U;
        return non_losing;
    }

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        int non_losing;

        if (board[cell] != 0U) {
            continue;
        }
        path[depth] = (uint8_t)cell;
        board[cell] = (uint8_t)turn;
        non_losing = ac_ttt_verify_visit(
            board,
            ac_ttt_other_role(turn),
            claimant,
            policy,
            report,
            path,
            (uint8_t)(depth + 1U));
        board[cell] = 0U;
        if (!non_losing) {
            return 0;
        }
    }
    return 1;
}

ac_status ac_ttt_verify_policy(
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    ac_ttt_report *report)
{
    ac_ttt_classification classification;
    uint8_t board[AC_TTT_BOARD_CELLS] = {0U};
    uint8_t path[AC_TTT_MAX_PLIES] = {0U};
    uint8_t required_kind;
    uint32_t index;
    int non_losing;

    if (report != NULL) {
        (void)memset(report, 0, sizeof(*report));
    }
    if (report == NULL || policy == NULL || !ac_ttt_role_is_valid(role)) {
        return AC_ERR_ARGUMENT;
    }

    ac_ttt_build_classification(&classification);
    report->invalid_board_index = AC_TTT_STATE_COUNT;
    report->invalid_move = AC_TTT_NO_MOVE;
    report->reachable_board_count = classification.reachable_count;
    report->x_to_move_board_count = classification.x_to_move_count;
    report->o_to_move_board_count = classification.o_to_move_count;
    report->reachable_terminal_board_count = classification.terminal_count;
    report->required_policy_state_count = role == AC_TTT_ROLE_X
                                              ? classification.x_to_move_count
                                              : classification.o_to_move_count;

    required_kind = role == AC_TTT_ROLE_X
                        ? AC_TTT_KIND_X_TO_MOVE
                        : AC_TTT_KIND_O_TO_MOVE;
    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        uint8_t move = policy->move_by_board[index];

        if (classification.kind_by_board[index] == required_kind) {
            if (move == AC_TTT_NO_MOVE) {
                ac_ttt_set_invalid_policy(
                    report,
                    AC_TTT_POLICY_REASON_MISSING_MOVE,
                    index,
                    move);
                return AC_OK;
            }
            if (move >= AC_TTT_BOARD_CELLS) {
                ac_ttt_set_invalid_policy(
                    report,
                    AC_TTT_POLICY_REASON_OUT_OF_RANGE_MOVE,
                    index,
                    move);
                return AC_OK;
            }
            ac_ttt_decode_board(index, board);
            if (board[move] != 0U) {
                ac_ttt_set_invalid_policy(
                    report,
                    AC_TTT_POLICY_REASON_OCCUPIED_CELL,
                    index,
                    move);
                return AC_OK;
            }
        } else if (move != AC_TTT_NO_MOVE) {
            ac_ttt_set_invalid_policy(
                report,
                AC_TTT_POLICY_REASON_UNEXPECTED_ENTRY,
                index,
                move);
            return AC_OK;
        }
    }

    (void)memset(board, 0, sizeof(board));
    non_losing = ac_ttt_verify_visit(
        board,
        AC_TTT_ROLE_X,
        role,
        policy,
        report,
        path,
        0U);
    report->verdict = non_losing
                          ? AC_TTT_VERDICT_NON_LOSING
                          : AC_TTT_VERDICT_LOSING;
    return AC_OK;
}

static ac_status ac_ttt_dag_build_graph(
    uint8_t kind_by_board[AC_TTT_STATE_COUNT],
    uint8_t board[AC_TTT_BOARD_CELLS],
    ac_ttt_dag_report *report)
{
    uint32_t index;

    (void)memset(kind_by_board, 0, AC_TTT_STATE_COUNT);
    kind_by_board[0] = AC_TTT_KIND_X_TO_MOVE;

    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        ac_ttt_role winner;
        ac_ttt_role turn;
        size_t cell;

        ++report->graph_scan_entry_count;
        if (kind_by_board[index] == AC_TTT_KIND_UNREACHABLE) {
            continue;
        }

        ac_ttt_decode_board(index, board);
        ++report->reachable_board_count;
        winner = ac_ttt_board_winner(board);
        if (winner != (ac_ttt_role)0 || ac_ttt_board_is_full(board)) {
            kind_by_board[index] = AC_TTT_KIND_TERMINAL;
            ++report->reachable_terminal_board_count;
            if (winner == AC_TTT_ROLE_X) {
                ++report->x_win_terminal_board_count;
            } else if (winner == AC_TTT_ROLE_O) {
                ++report->o_win_terminal_board_count;
            } else {
                ++report->draw_terminal_board_count;
            }
            continue;
        }

        turn = ac_ttt_board_turn(board);
        if (turn == AC_TTT_ROLE_X) {
            kind_by_board[index] = AC_TTT_KIND_X_TO_MOVE;
            ++report->x_to_move_board_count;
        } else {
            kind_by_board[index] = AC_TTT_KIND_O_TO_MOVE;
            ++report->o_to_move_board_count;
        }

        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            uint32_t child_index;

            if (board[cell] != 0U) {
                continue;
            }
            child_index = index +
                          (uint32_t)turn * ac_ttt_powers_of_three[cell];
            if (child_index >= AC_TTT_STATE_COUNT) {
                return AC_ERR_STATE;
            }
            ++report->edge_count;
            if (turn == AC_TTT_ROLE_X) {
                ++report->x_edge_count;
            } else {
                ++report->o_edge_count;
            }
            if (kind_by_board[child_index] == AC_TTT_KIND_UNREACHABLE) {
                kind_by_board[child_index] =
                    ac_ttt_other_role(turn) == AC_TTT_ROLE_X
                        ? AC_TTT_KIND_X_TO_MOVE
                        : AC_TTT_KIND_O_TO_MOVE;
            }
        }
    }
    return AC_OK;
}

static void ac_ttt_dag_record_first_policy_error(
    ac_ttt_dag_report *report,
    int *policy_is_valid,
    ac_ttt_policy_reason reason,
    uint32_t board_index,
    uint8_t move)
{
    if (*policy_is_valid != 0) {
        *policy_is_valid = 0;
        report->policy_reason = reason;
        report->invalid_board_index = board_index;
        report->invalid_move = move;
    }
}

static int ac_ttt_dag_validate_policy(
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    const uint8_t kind_by_board[AC_TTT_STATE_COUNT],
    uint8_t board[AC_TTT_BOARD_CELLS],
    ac_ttt_dag_report *report)
{
    uint8_t required_kind = role == AC_TTT_ROLE_X
                                ? AC_TTT_KIND_X_TO_MOVE
                                : AC_TTT_KIND_O_TO_MOVE;
    int policy_is_valid = 1;
    uint32_t index;

    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        ac_ttt_policy_reason reason = AC_TTT_POLICY_REASON_NONE;
        uint8_t move = policy->move_by_board[index];

        ++report->policy_scan_entry_count;
        if (kind_by_board[index] == required_kind) {
            uint8_t in_range = 0U;
            uint8_t targets_empty = 0U;
            size_t cell;

            ac_ttt_decode_board(index, board);
            for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
                uint8_t selected =
                    (uint8_t)(move == (uint8_t)cell);
                uint8_t empty = (uint8_t)(board[cell] == 0U);

                in_range = (uint8_t)(in_range | selected);
                targets_empty =
                    (uint8_t)(targets_empty | (uint8_t)(selected & empty));
            }

            if (move == AC_TTT_NO_MOVE) {
                reason = AC_TTT_POLICY_REASON_MISSING_MOVE;
            } else if (in_range == 0U) {
                reason = AC_TTT_POLICY_REASON_OUT_OF_RANGE_MOVE;
            } else if (targets_empty == 0U) {
                reason = AC_TTT_POLICY_REASON_OCCUPIED_CELL;
            }
        } else if (move != AC_TTT_NO_MOVE) {
            reason = AC_TTT_POLICY_REASON_UNEXPECTED_ENTRY;
        }

        if (reason != AC_TTT_POLICY_REASON_NONE) {
            ac_ttt_dag_record_first_policy_error(
                report, &policy_is_valid, reason, index, move);
        }
    }
    return policy_is_valid;
}

static ac_status ac_ttt_dag_evaluate_values(
    ac_ttt_role claimant,
    const ac_ttt_policy *policy,
    const uint8_t kind_by_board[AC_TTT_STATE_COUNT],
    uint8_t value_by_board[AC_TTT_STATE_COUNT],
    uint8_t board[AC_TTT_BOARD_CELLS],
    ac_ttt_dag_report *report)
{
    ac_ttt_role opponent = ac_ttt_other_role(claimant);
    uint32_t index = AC_TTT_STATE_COUNT;

    (void)memset(value_by_board, 0, AC_TTT_STATE_COUNT);
    while (index > 0U) {
        ac_ttt_role turn;
        uint8_t value;
        size_t cell;

        --index;
        ++report->value_scan_entry_count;
        if (kind_by_board[index] == AC_TTT_KIND_UNREACHABLE) {
            continue;
        }

        ++report->evaluated_board_count;
        ac_ttt_decode_board(index, board);
        if (kind_by_board[index] == AC_TTT_KIND_TERMINAL) {
            value_by_board[index] =
                (uint8_t)(ac_ttt_board_winner(board) != opponent);
            continue;
        }

        turn = kind_by_board[index] == AC_TTT_KIND_X_TO_MOVE
                   ? AC_TTT_ROLE_X
                   : AC_TTT_ROLE_O;
        value = turn == claimant ? 0U : 1U;
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            uint32_t child_index;

            if (board[cell] != 0U) {
                continue;
            }
            child_index = index +
                          (uint32_t)turn * ac_ttt_powers_of_three[cell];
            if (child_index >= AC_TTT_STATE_COUNT ||
                kind_by_board[child_index] == AC_TTT_KIND_UNREACHABLE) {
                return AC_ERR_STATE;
            }

            if (turn == claimant) {
                uint8_t selected = (uint8_t)(
                    policy->move_by_board[index] == (uint8_t)cell);
                uint8_t term =
                    (uint8_t)(selected & value_by_board[child_index]);

                value = (uint8_t)(value | term);
                ++report->selector_equality_count;
                ++report->selector_and_count;
                ++report->selector_or_count;
            } else {
                value = (uint8_t)(
                    value & value_by_board[child_index]);
                ++report->opponent_and_count;
            }
        }
        value_by_board[index] = value;
    }
    return AC_OK;
}

ac_status ac_ttt_evaluate_policy_dag(
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    ac_ttt_dag_report *report)
{
    uint8_t kind_by_board[AC_TTT_STATE_COUNT];
    uint8_t value_by_board[AC_TTT_STATE_COUNT];
    uint8_t board[AC_TTT_BOARD_CELLS];
    ac_status status;
    int policy_is_valid;

    if (report != NULL) {
        (void)memset(report, 0, sizeof(*report));
    }
    if (report == NULL || policy == NULL || !ac_ttt_role_is_valid(role)) {
        return AC_ERR_ARGUMENT;
    }

    report->invalid_board_index = AC_TTT_STATE_COUNT;
    report->invalid_move = AC_TTT_NO_MOVE;
    report->serialized_policy_byte_count =
        (uint32_t)sizeof(policy->move_by_board);
    report->classification_table_byte_count =
        (uint32_t)sizeof(kind_by_board);
    report->value_table_byte_count = (uint32_t)sizeof(value_by_board);
    report->board_scratch_byte_count = (uint32_t)sizeof(board);
    report->explicit_scratch_byte_count =
        report->classification_table_byte_count +
        report->value_table_byte_count +
        report->board_scratch_byte_count;

    status = ac_ttt_dag_build_graph(kind_by_board, board, report);
    if (status != AC_OK) {
        return status;
    }
    report->required_policy_state_count = role == AC_TTT_ROLE_X
                                              ? report->x_to_move_board_count
                                              : report->o_to_move_board_count;

    policy_is_valid = ac_ttt_dag_validate_policy(
        role, policy, kind_by_board, board, report);
    status = ac_ttt_dag_evaluate_values(
        role, policy, kind_by_board, value_by_board, board, report);
    if (status != AC_OK) {
        return status;
    }

    if (policy_is_valid == 0) {
        report->verdict = AC_TTT_VERDICT_INVALID_POLICY;
    } else {
        report->verdict = value_by_board[0] != 0U
                              ? AC_TTT_VERDICT_NON_LOSING
                              : AC_TTT_VERDICT_LOSING;
    }
    return AC_OK;
}
