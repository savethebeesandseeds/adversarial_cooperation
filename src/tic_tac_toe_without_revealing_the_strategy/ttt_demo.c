#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "ac/bool_circuit.h"
#include "ac/ttt.h"
#include "ac/ttt_core_circuit.h"

static ac_bool_gate core_gates[AC_TTT_CORE_MAX_GATE_COUNT];
static ac_ttt_core_builder_scratch core_builder_scratch;
static uint8_t core_wire_scratch[AC_TTT_CORE_MAX_WIRE_COUNT];

static const char *role_name(ac_ttt_role role)
{
    return role == AC_TTT_ROLE_X ? "X" : "O";
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

static void replace_with_first_empty_policy(ac_ttt_policy *policy)
{
    uint8_t board[AC_TTT_BOARD_CELLS];
    uint32_t index;

    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        size_t cell;

        if (policy->move_by_board[index] == AC_TTT_NO_MOVE) {
            continue;
        }
        decode_board(index, board);
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            if (board[cell] == 0U) {
                policy->move_by_board[index] = (uint8_t)cell;
                break;
            }
        }
    }
}

static void print_report(ac_ttt_role role, const ac_ttt_report *report)
{
    size_t ply;

    (void)printf(
        "%s policy: reachable=%u, required=%u, explored=%u, terminals=%u\n",
        role_name(role),
        (unsigned)report->reachable_board_count,
        (unsigned)report->required_policy_state_count,
        (unsigned)report->explored_node_count,
        (unsigned)report->explored_terminal_node_count);
    if (report->verdict == AC_TTT_VERDICT_NON_LOSING) {
        (void)printf("  verdict: non-losing under the fixed public model\n");
        return;
    }
    if (report->verdict == AC_TTT_VERDICT_LOSING) {
        (void)printf("  verdict: losing; replayable cells:");
        for (ply = 0U; ply < report->counterexample_length; ++ply) {
            (void)printf(" %u", (unsigned)report->counterexample_moves[ply]);
        }
        (void)printf("\n");
        return;
    }
    (void)printf(
        "  verdict: invalid policy at board %u (reason=%u, move=%u)\n",
        (unsigned)report->invalid_board_index,
        (unsigned)report->policy_reason,
        (unsigned)report->invalid_move);
}

static void print_dag_report(const ac_ttt_dag_report *report)
{
    (void)printf(
        "  fixed DAG: states=%u, terminals=%u, edges=%u, "
        "scans=%u/%u/%u\n",
        (unsigned)report->evaluated_board_count,
        (unsigned)report->reachable_terminal_board_count,
        (unsigned)report->edge_count,
        (unsigned)report->graph_scan_entry_count,
        (unsigned)report->policy_scan_entry_count,
        (unsigned)report->value_scan_entry_count);
    (void)printf(
        "  C folds: selectors=%u, opponent-ands=%u; "
        "policy=%u bytes, explicit scratch=%u bytes\n",
        (unsigned)report->selector_equality_count,
        (unsigned)report->opponent_and_count,
        (unsigned)report->serialized_policy_byte_count,
        (unsigned)report->explicit_scratch_byte_count);
}

static void print_core_report(const ac_ttt_core_circuit_report *report)
{
    (void)printf(
        "  Core-%s circuit: AND=%u, XOR=%u, gates=%u, wires=%u, depth=%u\n",
        role_name(report->specialization_role),
        (unsigned)report->and_gate_count,
        (unsigned)report->xor_gate_count,
        (unsigned)report->gate_count,
        (unsigned)report->wire_count,
        (unsigned)report->output_depth);
    (void)printf(
        "  circuit bytes: gates=%zu, evaluator scratch=%zu, "
        "builder scratch=%zu, serialization=%zu\n",
        report->gate_storage_bytes,
        report->evaluator_scratch_bytes,
        report->builder_scratch_bytes,
        report->serialized_bytes);
}

static int evaluate_core(
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    int expected_acceptance,
    int print_counts)
{
    ac_bool_circuit circuit;
    ac_ttt_core_circuit_report circuit_report;
    uint8_t public_input[AC_TTT_CORE_PUBLIC_INPUT_BYTES];
    uint8_t acceptance = 0U;

    if (ac_ttt_core_circuit_build(
            role,
            core_gates,
            AC_TTT_CORE_MAX_GATE_COUNT,
            &core_builder_scratch,
            &circuit,
            &circuit_report) != AC_OK ||
        ac_ttt_core_encode_public_input(role, public_input) != AC_OK ||
        ac_bool_circuit_evaluate(
            &circuit,
            public_input,
            sizeof public_input,
            policy->move_by_board,
            sizeof policy->move_by_board,
            core_wire_scratch,
            sizeof core_wire_scratch,
            &acceptance) != AC_OK) {
        (void)fprintf(stderr, "Core circuit execution failed\n");
        return 1;
    }
    if ((int)acceptance != expected_acceptance) {
        (void)fprintf(stderr, "Core circuit and C evaluators disagree\n");
        return 1;
    }
    if (print_counts != 0) {
        print_core_report(&circuit_report);
    } else {
        (void)printf("  Core-%s circuit acceptance: %u\n",
            role_name(role), (unsigned)acceptance);
    }
    return 0;
}

static int demonstrate_reference(ac_ttt_role role)
{
    ac_ttt_policy policy;
    ac_ttt_report report;
    ac_ttt_dag_report dag_report;

    if (ac_ttt_build_reference_policy(role, &policy) != AC_OK ||
        ac_ttt_verify_policy(role, &policy, &report) != AC_OK ||
        ac_ttt_evaluate_policy_dag(role, &policy, &dag_report) != AC_OK) {
        (void)fprintf(stderr, "reference-policy execution failed\n");
        return 1;
    }
    if (report.verdict != dag_report.verdict) {
        (void)fprintf(stderr, "recursive and DAG verdicts disagree\n");
        return 1;
    }
    print_report(role, &report);
    print_dag_report(&dag_report);
    if (evaluate_core(
            role,
            &policy,
            report.verdict == AC_TTT_VERDICT_NON_LOSING,
            1) != 0) {
        return 1;
    }
    return report.verdict == AC_TTT_VERDICT_NON_LOSING ? 0 : 1;
}

static int demonstrate_counterexample(void)
{
    ac_ttt_policy policy;
    ac_ttt_report report;
    ac_ttt_dag_report dag_report;

    if (ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy) != AC_OK) {
        (void)fprintf(stderr, "could not build public X fixture\n");
        return 1;
    }
    replace_with_first_empty_policy(&policy);
    if (ac_ttt_verify_policy(AC_TTT_ROLE_X, &policy, &report) != AC_OK ||
        ac_ttt_evaluate_policy_dag(
            AC_TTT_ROLE_X, &policy, &dag_report) != AC_OK) {
        (void)fprintf(stderr, "counterexample execution failed\n");
        return 1;
    }
    if (report.verdict != dag_report.verdict) {
        (void)fprintf(stderr, "recursive and DAG verdicts disagree\n");
        return 1;
    }
    (void)printf("\nA canonical but naive public policy:\n");
    print_report(AC_TTT_ROLE_X, &report);
    print_dag_report(&dag_report);
    if (evaluate_core(
            AC_TTT_ROLE_X,
            &policy,
            report.verdict == AC_TTT_VERDICT_NON_LOSING,
            0) != 0) {
        return 1;
    }
    return report.verdict == AC_TTT_VERDICT_LOSING ? 0 : 1;
}

int main(void)
{
    (void)printf(
        "EDUCATIONAL PUBLIC BASELINE -- NOT STRATEGY PRIVACY\n"
        "All three computation paths receive the entire policy table. The\n"
        "circuit's witness wires are plaintext, and circuit serialization is\n"
        "not a commitment or proof. This provides neither zero knowledge,\n"
        "proof of knowledge, anti-cloning, nor a promise that any person will\n"
        "follow the policy later.\n\n");

    if (demonstrate_reference(AC_TTT_ROLE_X) != 0 ||
        demonstrate_reference(AC_TTT_ROLE_O) != 0 ||
        demonstrate_counterexample() != 0) {
        return 1;
    }
    return 0;
}
