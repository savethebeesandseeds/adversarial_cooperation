#include "ac/status.h"
#include "ac/ttt.h"

#include <stdint.h>

#include "../../tests/test_support.h"

/* Public scalar ABI implemented by web/wasm/ttt_web.c. */
ac_status ac_ttt_web_run(uint32_t scenario);
uint32_t ac_ttt_web_get_abi_version(void);
uint32_t ac_ttt_web_get_scenario(void);
uint32_t ac_ttt_web_get_status(void);
uint32_t ac_ttt_web_get_role(void);
uint32_t ac_ttt_web_get_verdict(void);
uint32_t ac_ttt_web_get_policy_reason(void);
uint32_t ac_ttt_web_get_invalid_board_index(void);
uint32_t ac_ttt_web_get_invalid_move(void);
uint32_t ac_ttt_web_get_reachable_board_count(void);
uint32_t ac_ttt_web_get_x_to_move_board_count(void);
uint32_t ac_ttt_web_get_o_to_move_board_count(void);
uint32_t ac_ttt_web_get_reachable_terminal_board_count(void);
uint32_t ac_ttt_web_get_required_policy_state_count(void);
uint32_t ac_ttt_web_get_explored_node_count(void);
uint32_t ac_ttt_web_get_explored_terminal_node_count(void);
uint32_t ac_ttt_web_get_counterexample_length(void);
uint32_t ac_ttt_web_get_counterexample_move(uint32_t index);

enum {
    TEST_ABI_VERSION = 1U,
    TEST_SCENARIO_REFERENCE_X = 1U,
    TEST_SCENARIO_REFERENCE_O = 2U,
    TEST_SCENARIO_NAIVE_X = 3U
};

static int assert_common_census(void)
{
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_reachable_board_count(), 5478U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_x_to_move_board_count(), 2423U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_o_to_move_board_count(), 2097U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_reachable_terminal_board_count(), 958U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_policy_reason(), AC_TTT_POLICY_REASON_NONE);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_invalid_board_index(), AC_TTT_STATE_COUNT);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_invalid_move(), AC_TTT_NO_MOVE);
    return 0;
}

static int test_initial_getter_boundary(void)
{
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_abi_version(), TEST_ABI_VERSION);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_scenario(), 0U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_role(), 0U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_verdict(), AC_TTT_VERDICT_NOT_COMPUTED);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_counterexample_length(), 0U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_counterexample_move(0U), AC_TTT_NO_MOVE);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_counterexample_move(UINT32_MAX), AC_TTT_NO_MOVE);
    return 0;
}

static int test_reference_x_record(void)
{
    AC_TEST_ASSERT_STATUS(
        ac_ttt_web_run(TEST_SCENARIO_REFERENCE_X), AC_OK);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_status(), AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_scenario(), TEST_SCENARIO_REFERENCE_X);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_role(), AC_TTT_ROLE_X);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_verdict(), AC_TTT_VERDICT_NON_LOSING);
    AC_TEST_ASSERT_EQ_INT(assert_common_census(), 0);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_required_policy_state_count(), 2423U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_explored_node_count(), 266U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_explored_terminal_node_count(), 101U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_counterexample_length(), 0U);
    return 0;
}

static int test_reference_o_record(void)
{
    AC_TEST_ASSERT_STATUS(
        ac_ttt_web_run(TEST_SCENARIO_REFERENCE_O), AC_OK);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_status(), AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_scenario(), TEST_SCENARIO_REFERENCE_O);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_role(), AC_TTT_ROLE_O);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_verdict(), AC_TTT_VERDICT_NON_LOSING);
    AC_TEST_ASSERT_EQ_INT(assert_common_census(), 0);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_required_policy_state_count(), 2097U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_explored_node_count(), 2056U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_explored_terminal_node_count(), 681U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_counterexample_length(), 0U);
    return 0;
}

static int test_naive_x_losing_record(void)
{
    static const uint8_t expected_counterexample[8] = {
        0U, 1U, 2U, 4U, 3U, 6U, 5U, 7U
    };
    uint32_t index;

    AC_TEST_ASSERT_STATUS(
        ac_ttt_web_run(TEST_SCENARIO_NAIVE_X), AC_OK);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_status(), AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_scenario(), TEST_SCENARIO_NAIVE_X);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_role(), AC_TTT_ROLE_X);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_verdict(), AC_TTT_VERDICT_LOSING);
    AC_TEST_ASSERT_EQ_INT(assert_common_census(), 0);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_required_policy_state_count(), 2423U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_explored_node_count(), 33U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_explored_terminal_node_count(), 9U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_counterexample_length(),
        sizeof expected_counterexample);
    for (index = 0U; index < sizeof expected_counterexample; ++index) {
        AC_TEST_ASSERT_EQ_INT(
            ac_ttt_web_get_counterexample_move(index),
            expected_counterexample[index]);
    }
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_counterexample_move(sizeof expected_counterexample),
        AC_TTT_NO_MOVE);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_counterexample_move(UINT32_MAX), AC_TTT_NO_MOVE);
    return 0;
}

static int test_each_run_replaces_the_record(void)
{
    AC_TEST_ASSERT_STATUS(
        ac_ttt_web_run(TEST_SCENARIO_NAIVE_X), AC_OK);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_counterexample_length(), 8U);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_web_run(TEST_SCENARIO_REFERENCE_O), AC_OK);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_scenario(), TEST_SCENARIO_REFERENCE_O);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_verdict(), AC_TTT_VERDICT_NON_LOSING);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_counterexample_length(), 0U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_counterexample_move(0U), AC_TTT_NO_MOVE);
    return 0;
}

static int test_invalid_scenario_clears_the_record(void)
{
    const uint32_t invalid_scenario = UINT32_C(0x7fffffff);

    AC_TEST_ASSERT_STATUS(
        ac_ttt_web_run(TEST_SCENARIO_NAIVE_X), AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_ttt_web_run(invalid_scenario), AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_scenario(), invalid_scenario);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_status(), AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_role(), 0U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_verdict(), AC_TTT_VERDICT_NOT_COMPUTED);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_policy_reason(), 0U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_reachable_board_count(), 0U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_explored_node_count(), 0U);
    AC_TEST_ASSERT_EQ_INT(ac_ttt_web_get_counterexample_length(), 0U);
    AC_TEST_ASSERT_EQ_INT(
        ac_ttt_web_get_counterexample_move(0U), AC_TTT_NO_MOVE);
    return 0;
}

int main(void)
{
    ac_test_suite suite = {0U, 0U};

    AC_TEST_RUN(suite, test_initial_getter_boundary);
    AC_TEST_RUN(suite, test_reference_x_record);
    AC_TEST_RUN(suite, test_reference_o_record);
    AC_TEST_RUN(suite, test_naive_x_losing_record);
    AC_TEST_RUN(suite, test_each_run_replaces_the_record);
    AC_TEST_RUN(suite, test_invalid_scenario_clears_the_record);
    return ac_test_finish(&suite);
}
