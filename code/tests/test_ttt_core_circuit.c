#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ac/bool_circuit.h"
#include "ac/status.h"
#include "ac/ttt.h"
#include "ac/ttt_core_circuit.h"
#include "test_support.h"

#define AC_TTT_CORE_TEST_LANES 64U

typedef struct {
    uint32_t public_bits;
    uint32_t witness_bits;
    uint32_t gate_count;
    uint32_t wire_count;
    uint32_t output_wire;
    uint32_t xor_count;
    uint32_t and_count;
    uint32_t maximum_depth;
    uint32_t output_depth;
} ac_ttt_core_parsed_circuit;

typedef struct {
    const uint8_t *serialized;
    size_t serialized_size;
    ac_ttt_role role;
    size_t count;
    size_t executed;
} ac_ttt_core_batch;

static ac_bool_gate ac_ttt_core_gates_a[AC_TTT_CORE_MAX_GATE_COUNT];
static ac_bool_gate ac_ttt_core_gates_b[AC_TTT_CORE_MAX_GATE_COUNT];
static ac_ttt_core_builder_scratch ac_ttt_core_builder_memory;
static uint8_t ac_ttt_core_serialized_a[AC_TTT_CORE_MAX_SERIALIZED_BYTES];
static uint8_t ac_ttt_core_serialized_b[AC_TTT_CORE_MAX_SERIALIZED_BYTES];
static uint32_t ac_ttt_core_depths[AC_TTT_CORE_MAX_WIRE_COUNT];
static uint8_t ac_ttt_core_scalar_wires[AC_TTT_CORE_MAX_WIRE_COUNT];
static uint64_t ac_ttt_core_batch_wires[AC_TTT_CORE_MAX_WIRE_COUNT];
static ac_ttt_policy ac_ttt_core_batch_policies[AC_TTT_CORE_TEST_LANES];
static uint8_t
    ac_ttt_core_batch_public[AC_TTT_CORE_TEST_LANES]
                            [AC_TTT_CORE_PUBLIC_INPUT_BYTES];
static uint8_t ac_ttt_core_batch_expected[AC_TTT_CORE_TEST_LANES];

static uint32_t ac_ttt_core_read_u32be(const uint8_t input[4])
{
    return ((uint32_t)input[0] << 24) |
           ((uint32_t)input[1] << 16) |
           ((uint32_t)input[2] << 8) |
           (uint32_t)input[3];
}

static uint16_t ac_ttt_core_read_u16be(const uint8_t input[2])
{
    return (uint16_t)(((uint16_t)input[0] << 8) | input[1]);
}

static uint32_t ac_ttt_core_max_u32(uint32_t first, uint32_t second)
{
    return first > second ? first : second;
}

static ac_ttt_role ac_ttt_core_test_other_role(ac_ttt_role role)
{
    return role == AC_TTT_ROLE_X ? AC_TTT_ROLE_O : AC_TTT_ROLE_X;
}

static int ac_ttt_core_parse_and_analyze(
    const uint8_t *serialized,
    size_t serialized_size,
    ac_ttt_core_parsed_circuit *parsed)
{
    static const uint8_t magic[8] = {
        'A', 'C', 'B', 'C', 'I', 'R', '0', '1'
    };
    uint32_t gate_index;
    uint32_t input_wire_count;
    uint32_t maximum_depth = 0U;
    uint64_t encoded_size_u64;
    size_t encoded_size;

    if (serialized == NULL || parsed == NULL ||
        serialized_size < AC_BOOL_CIRCUIT_HEADER_BYTES ||
        memcmp(serialized, magic, sizeof magic) != 0 ||
        ac_ttt_core_read_u16be(serialized + 8U) !=
            AC_BOOL_CIRCUIT_FORMAT_VERSION ||
        ac_ttt_core_read_u16be(serialized + 10U) !=
            AC_BOOL_CIRCUIT_BASIS_XOR_AND) {
        return 1;
    }
    parsed->public_bits = ac_ttt_core_read_u32be(serialized + 12U);
    parsed->witness_bits = ac_ttt_core_read_u32be(serialized + 16U);
    parsed->gate_count = ac_ttt_core_read_u32be(serialized + 20U);
    parsed->wire_count = ac_ttt_core_read_u32be(serialized + 24U);
    parsed->output_wire = ac_ttt_core_read_u32be(serialized + 28U);
    parsed->xor_count = 0U;
    parsed->and_count = 0U;
    parsed->maximum_depth = 0U;
    parsed->output_depth = 0U;

    encoded_size_u64 = (uint64_t)AC_BOOL_CIRCUIT_HEADER_BYTES +
                       (uint64_t)parsed->gate_count *
                           (uint64_t)AC_BOOL_CIRCUIT_GATE_BYTES;
    encoded_size = (size_t)encoded_size_u64;
    if ((uint64_t)encoded_size != encoded_size_u64 ||
        serialized_size != encoded_size ||
        parsed->witness_bits > UINT32_MAX - 2U ||
        parsed->public_bits > UINT32_MAX - parsed->witness_bits - 2U) {
        return 1;
    }
    input_wire_count = 2U + parsed->public_bits + parsed->witness_bits;
    if (parsed->gate_count > UINT32_MAX - input_wire_count ||
        parsed->wire_count != input_wire_count + parsed->gate_count ||
        parsed->wire_count > AC_TTT_CORE_MAX_WIRE_COUNT ||
        parsed->output_wire >= parsed->wire_count) {
        return 1;
    }

    for (gate_index = 0U; gate_index < input_wire_count; ++gate_index) {
        ac_ttt_core_depths[gate_index] = 0U;
    }
    for (gate_index = 0U; gate_index < parsed->gate_count; ++gate_index) {
        const uint8_t *encoded_gate =
            serialized + AC_BOOL_CIRCUIT_HEADER_BYTES +
            (size_t)gate_index * AC_BOOL_CIRCUIT_GATE_BYTES;
        uint8_t opcode = encoded_gate[0];
        uint32_t left = ac_ttt_core_read_u32be(encoded_gate + 1U);
        uint32_t right = ac_ttt_core_read_u32be(encoded_gate + 5U);
        uint32_t output_wire = input_wire_count + gate_index;
        uint32_t depth;

        if (left >= output_wire || right >= output_wire) {
            return 1;
        }
        if (opcode == AC_BOOL_GATE_XOR) {
            ++parsed->xor_count;
        } else if (opcode == AC_BOOL_GATE_AND) {
            ++parsed->and_count;
        } else {
            return 1;
        }
        depth = ac_ttt_core_max_u32(
                    ac_ttt_core_depths[left],
                    ac_ttt_core_depths[right]) +
                1U;
        ac_ttt_core_depths[output_wire] = depth;
        maximum_depth = ac_ttt_core_max_u32(maximum_depth, depth);
    }
    parsed->maximum_depth = maximum_depth;
    parsed->output_depth = ac_ttt_core_depths[parsed->output_wire];
    return 0;
}

static void ac_ttt_core_decode_board(
    uint32_t index,
    uint8_t board[AC_TTT_BOARD_CELLS])
{
    size_t cell;

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        board[cell] = (uint8_t)(index % 3U);
        index /= 3U;
    }
}

static uint32_t ac_ttt_core_mix(uint32_t value)
{
    value ^= value >> 16;
    value *= UINT32_C(0x7feb352d);
    value ^= value >> 15;
    value *= UINT32_C(0x846ca68b);
    value ^= value >> 16;
    return value;
}

static int ac_ttt_core_public_is_valid(
    ac_ttt_role role,
    const uint8_t public_input[AC_TTT_CORE_PUBLIC_INPUT_BYTES])
{
    return public_input[0] == AC_TTT_GAME_VERSION &&
           public_input[1] == (uint8_t)role &&
           public_input[2] == 0U && public_input[3] == 0U;
}

static int ac_ttt_core_baseline_acceptance(
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    const uint8_t public_input[AC_TTT_CORE_PUBLIC_INPUT_BYTES],
    uint8_t *expected)
{
    ac_ttt_report recursive;
    ac_ttt_dag_report dag;
    int recursive_accepts;
    int dag_accepts;

    if (ac_ttt_verify_policy(role, policy, &recursive) != AC_OK ||
        ac_ttt_evaluate_policy_dag(role, policy, &dag) != AC_OK) {
        return 1;
    }
    recursive_accepts = recursive.verdict == AC_TTT_VERDICT_NON_LOSING;
    dag_accepts = dag.verdict == AC_TTT_VERDICT_NON_LOSING;
    if (recursive_accepts != dag_accepts) {
        return 1;
    }
    *expected = (uint8_t)(
        recursive_accepts != 0 &&
        ac_ttt_core_public_is_valid(role, public_input));
    return 0;
}

static int ac_ttt_core_batch_execute(ac_ttt_core_batch *batch)
{
    uint32_t public_bits;
    uint32_t witness_bits;
    uint32_t gate_count;
    uint32_t wire_count;
    uint32_t output_wire;
    uint32_t gate_index;
    uint32_t bit_index;
    uint64_t active_lanes;
    size_t lane;

    if (batch->count == 0U) {
        return 0;
    }
    public_bits = ac_ttt_core_read_u32be(batch->serialized + 12U);
    witness_bits = ac_ttt_core_read_u32be(batch->serialized + 16U);
    gate_count = ac_ttt_core_read_u32be(batch->serialized + 20U);
    wire_count = ac_ttt_core_read_u32be(batch->serialized + 24U);
    output_wire = ac_ttt_core_read_u32be(batch->serialized + 28U);
    if (public_bits != AC_TTT_CORE_PUBLIC_INPUT_BITS ||
        witness_bits != AC_TTT_CORE_WITNESS_INPUT_BITS ||
        wire_count > AC_TTT_CORE_MAX_WIRE_COUNT ||
        batch->serialized_size != AC_BOOL_CIRCUIT_HEADER_BYTES +
                                      (size_t)gate_count *
                                          AC_BOOL_CIRCUIT_GATE_BYTES) {
        return 1;
    }

    active_lanes = batch->count == AC_TTT_CORE_TEST_LANES
                       ? UINT64_MAX
                       : (UINT64_C(1) << batch->count) - UINT64_C(1);
    ac_ttt_core_batch_wires[0] = UINT64_C(0);
    ac_ttt_core_batch_wires[1] = active_lanes;
    for (bit_index = 0U; bit_index < public_bits; ++bit_index) {
        uint64_t packed = UINT64_C(0);

        for (lane = 0U; lane < batch->count; ++lane) {
            if (((ac_ttt_core_batch_public[lane][bit_index / 8U] >>
                  (bit_index % 8U)) &
                 1U) != 0U) {
                packed |= UINT64_C(1) << lane;
            }
        }
        ac_ttt_core_batch_wires[2U + bit_index] = packed;
    }
    for (bit_index = 0U; bit_index < witness_bits; ++bit_index) {
        uint64_t packed = UINT64_C(0);

        for (lane = 0U; lane < batch->count; ++lane) {
            if (((ac_ttt_core_batch_policies[lane]
                      .move_by_board[bit_index / 8U] >>
                  (bit_index % 8U)) &
                 1U) != 0U) {
                packed |= UINT64_C(1) << lane;
            }
        }
        ac_ttt_core_batch_wires[2U + public_bits + bit_index] = packed;
    }

    for (gate_index = 0U; gate_index < gate_count; ++gate_index) {
        const uint8_t *encoded_gate =
            batch->serialized + AC_BOOL_CIRCUIT_HEADER_BYTES +
            (size_t)gate_index * AC_BOOL_CIRCUIT_GATE_BYTES;
        uint8_t opcode = encoded_gate[0];
        uint32_t left = ac_ttt_core_read_u32be(encoded_gate + 1U);
        uint32_t right = ac_ttt_core_read_u32be(encoded_gate + 5U);
        uint32_t result_wire = 2U + public_bits + witness_bits + gate_index;

        if (opcode == AC_BOOL_GATE_XOR) {
            ac_ttt_core_batch_wires[result_wire] =
                ac_ttt_core_batch_wires[left] ^
                ac_ttt_core_batch_wires[right];
        } else if (opcode == AC_BOOL_GATE_AND) {
            ac_ttt_core_batch_wires[result_wire] =
                ac_ttt_core_batch_wires[left] &
                ac_ttt_core_batch_wires[right];
        } else {
            return 1;
        }
    }

    for (lane = 0U; lane < batch->count; ++lane) {
        uint8_t actual = (uint8_t)(
            (ac_ttt_core_batch_wires[output_wire] >> lane) & UINT64_C(1));

        if (actual != ac_ttt_core_batch_expected[lane]) {
            (void)fprintf(
                stderr,
                "batch mismatch role=%u case=%zu actual=%u expected=%u\n",
                (unsigned)batch->role,
                batch->executed + lane,
                (unsigned)actual,
                (unsigned)ac_ttt_core_batch_expected[lane]);
            return 1;
        }
    }
    batch->executed += batch->count;
    batch->count = 0U;
    return 0;
}

static int ac_ttt_core_batch_add(
    ac_ttt_core_batch *batch,
    const ac_ttt_policy *policy,
    const uint8_t public_input[AC_TTT_CORE_PUBLIC_INPUT_BYTES])
{
    uint8_t expected;

    if (batch->count == AC_TTT_CORE_TEST_LANES &&
        ac_ttt_core_batch_execute(batch) != 0) {
        return 1;
    }
    if (ac_ttt_core_baseline_acceptance(
            batch->role, policy, public_input, &expected) != 0) {
        return 1;
    }
    ac_ttt_core_batch_policies[batch->count] = *policy;
    (void)memcpy(
        ac_ttt_core_batch_public[batch->count],
        public_input,
        AC_TTT_CORE_PUBLIC_INPUT_BYTES);
    ac_ttt_core_batch_expected[batch->count] = expected;
    ++batch->count;
    return 0;
}

static int ac_ttt_core_scalar_matches(
    const ac_bool_circuit *circuit,
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    const uint8_t public_input[AC_TTT_CORE_PUBLIC_INPUT_BYTES])
{
    uint8_t expected;
    uint8_t actual = UINT8_C(0xa5);

    if (ac_ttt_core_baseline_acceptance(
            role, policy, public_input, &expected) != 0 ||
        ac_bool_circuit_evaluate(
            circuit,
            public_input,
            AC_TTT_CORE_PUBLIC_INPUT_BYTES,
            policy->move_by_board,
            AC_TTT_CORE_WITNESS_INPUT_BYTES,
            ac_ttt_core_scalar_wires,
            AC_TTT_CORE_MAX_WIRE_COUNT,
            &actual) != AC_OK ||
        actual != expected) {
        return 1;
    }
    return 0;
}

static uint32_t ac_ttt_core_count_legal_alternatives(
    const ac_ttt_policy *reference)
{
    uint8_t board[AC_TTT_BOARD_CELLS];
    uint32_t total = 0U;
    uint32_t index;

    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        uint8_t original = reference->move_by_board[index];
        size_t cell;

        if (original == AC_TTT_NO_MOVE) {
            continue;
        }
        ac_ttt_core_decode_board(index, board);
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            if (board[cell] == 0U && (uint8_t)cell != original) {
                ++total;
            }
        }
    }
    return total;
}

static int ac_ttt_core_make_stratified_mutation(
    const ac_ttt_policy *reference,
    uint32_t seed,
    ac_ttt_policy *policy)
{
    uint8_t board[AC_TTT_BOARD_CELLS];
    uint32_t total = ac_ttt_core_count_legal_alternatives(reference);
    uint32_t bucket_start = (seed * total) / 256U;
    uint32_t bucket_end = ((seed + 1U) * total) / 256U;
    uint32_t width = bucket_end - bucket_start;
    uint32_t target = bucket_start + ac_ttt_core_mix(seed) % width;
    uint32_t seen = 0U;
    uint32_t index;

    *policy = *reference;
    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        uint8_t original = reference->move_by_board[index];
        size_t cell;

        if (original == AC_TTT_NO_MOVE) {
            continue;
        }
        ac_ttt_core_decode_board(index, board);
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            if (board[cell] != 0U || (uint8_t)cell == original) {
                continue;
            }
            if (seen == target) {
                policy->move_by_board[index] = (uint8_t)cell;
                return 0;
            }
            ++seen;
        }
    }
    return 1;
}

static int ac_ttt_core_make_canonical_multi_policy(
    ac_ttt_role role,
    const ac_ttt_policy *reference,
    uint32_t seed,
    ac_ttt_policy *policy)
{
    uint8_t board[AC_TTT_BOARD_CELLS];
    uint32_t index;

    ac_ttt_policy_init(policy);
    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        uint8_t empty_count = 0U;
        uint8_t ordinal;
        uint8_t seen = 0U;
        size_t cell;

        if (reference->move_by_board[index] == AC_TTT_NO_MOVE) {
            continue;
        }
        ac_ttt_core_decode_board(index, board);
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            if (board[cell] == 0U) {
                ++empty_count;
            }
        }
        if (empty_count == 0U) {
            return 1;
        }
        ordinal = (uint8_t)(
            ac_ttt_core_mix(
                seed ^ (index * UINT32_C(0x9e3779b9)) ^
                ((uint32_t)role * UINT32_C(0x85ebca6b))) %
            empty_count);
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            if (board[cell] != 0U) {
                continue;
            }
            if (seen == ordinal) {
                policy->move_by_board[index] = (uint8_t)cell;
                break;
            }
            ++seen;
        }
    }
    return 0;
}

static int ac_ttt_core_make_named_malformed(
    unsigned kind,
    const ac_ttt_policy *reference,
    ac_ttt_policy *policy)
{
    uint8_t board[AC_TTT_BOARD_CELLS];
    uint32_t index;

    *policy = *reference;
    if (kind == 3U) {
        for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
            if (reference->move_by_board[index] == AC_TTT_NO_MOVE) {
                policy->move_by_board[index] = 0U;
                return 0;
            }
        }
        return 1;
    }

    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        size_t cell;

        if (reference->move_by_board[index] == AC_TTT_NO_MOVE) {
            continue;
        }
        if (kind == 0U) {
            policy->move_by_board[index] = AC_TTT_NO_MOVE;
            return 0;
        }
        if (kind == 1U) {
            ac_ttt_core_decode_board(index, board);
            for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
                if (board[cell] != 0U) {
                    policy->move_by_board[index] = (uint8_t)cell;
                    return 0;
                }
            }
        } else if (kind == 2U) {
            policy->move_by_board[index] = AC_TTT_BOARD_CELLS;
            return 0;
        }
    }
    return 1;
}

static int ac_ttt_core_build_known_loss(
    ac_ttt_role role,
    const ac_ttt_policy *reference,
    ac_ttt_policy *losing)
{
    *losing = *reference;
    if (role == AC_TTT_ROLE_X) {
        losing->move_by_board[19U] = 1U;
    } else {
        losing->move_by_board[6561U] = 7U;
    }
    return 0;
}

static int ac_ttt_core_expected_counts(
    ac_ttt_role role,
    uint32_t *and_count,
    uint32_t *xor_count)
{
    uint32_t required;
    uint32_t selector_edges;
    uint32_t other_required;
    uint32_t other_edges;
    uint32_t complements;

    if (role == AC_TTT_ROLE_X) {
        required = 2423U;
        selector_edges = 8631U;
        other_required = 2097U;
        other_edges = 7536U;
        complements = 18904U;
    } else if (role == AC_TTT_ROLE_O) {
        required = 2097U;
        selector_edges = 7536U;
        other_required = 2423U;
        other_edges = 8631U;
        complements = 16436U;
    } else {
        return 1;
    }
    *and_count =
        7U * (selector_edges + AC_TTT_STATE_COUNT - required) +
        selector_edges + (other_edges - other_required) +
        (AC_TTT_STATE_COUNT - 1U) + 31U + 2U;
    *xor_count =
        complements + 2U * (selector_edges - required) + 30U;
    return 0;
}

static int test_requirements_encoder_and_argument_boundaries(void)
{
    const ac_ttt_role roles[2] = {AC_TTT_ROLE_X, AC_TTT_ROLE_O};
    size_t role_index;

    AC_TEST_ASSERT_EQ_INT(sizeof(ac_ttt_core_builder_scratch), 255888U);
    for (role_index = 0U; role_index < 2U; ++role_index) {
        ac_ttt_core_circuit_report report;
        uint8_t public_input[AC_TTT_CORE_PUBLIC_INPUT_BYTES];
        uint32_t expected_and;
        uint32_t expected_xor;

        AC_TEST_ASSERT_STATUS(
            ac_ttt_core_circuit_requirements(roles[role_index], &report),
            AC_OK);
        AC_TEST_ASSERT_EQ_INT(report.specialization_role, roles[role_index]);
        AC_TEST_ASSERT_EQ_INT(
            report.public_input_bits, AC_TTT_CORE_PUBLIC_INPUT_BITS);
        AC_TEST_ASSERT_EQ_INT(
            report.witness_input_bits, AC_TTT_CORE_WITNESS_INPUT_BITS);
        AC_TEST_ASSERT_EQ_INT(
            ac_ttt_core_expected_counts(
                roles[role_index], &expected_and, &expected_xor),
            0);
        AC_TEST_ASSERT_EQ_INT(report.and_gate_count, expected_and);
        AC_TEST_ASSERT_EQ_INT(report.xor_gate_count, expected_xor);
        AC_TEST_ASSERT_EQ_INT(
            report.gate_count, expected_and + expected_xor);
        AC_TEST_ASSERT_EQ_INT(
            report.wire_count,
            2U + AC_TTT_CORE_PUBLIC_INPUT_BITS +
                AC_TTT_CORE_WITNESS_INPUT_BITS + report.gate_count);
        AC_TEST_ASSERT_EQ_INT(
            report.gate_storage_bytes,
            (size_t)report.gate_count * sizeof(ac_bool_gate));
        AC_TEST_ASSERT_EQ_INT(
            report.evaluator_scratch_bytes, report.wire_count);
        AC_TEST_ASSERT_EQ_INT(
            report.builder_scratch_bytes,
            sizeof(ac_ttt_core_builder_scratch));
        AC_TEST_ASSERT_EQ_INT(
            report.serialized_bytes,
            AC_BOOL_CIRCUIT_HEADER_BYTES +
                (size_t)report.gate_count * AC_BOOL_CIRCUIT_GATE_BYTES);

        (void)memset(public_input, 0xa5, sizeof public_input);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_core_encode_public_input(
                roles[role_index], public_input),
            AC_OK);
        AC_TEST_ASSERT_EQ_INT(public_input[0], AC_TTT_GAME_VERSION);
        AC_TEST_ASSERT_EQ_INT(public_input[1], roles[role_index]);
        AC_TEST_ASSERT_EQ_INT(public_input[2], 0U);
        AC_TEST_ASSERT_EQ_INT(public_input[3], 0U);
    }

    {
        ac_ttt_core_circuit_report report;
        ac_bool_circuit circuit;
        uint8_t public_input[AC_TTT_CORE_PUBLIC_INPUT_BYTES];

        (void)memset(&report, 0xa5, sizeof report);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_core_circuit_requirements((ac_ttt_role)0, &report),
            AC_ERR_ARGUMENT);
        AC_TEST_ASSERT_ZERO(&report, sizeof report);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_core_circuit_requirements(AC_TTT_ROLE_X, NULL),
            AC_ERR_ARGUMENT);

        (void)memset(public_input, 0xa5, sizeof public_input);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_core_encode_public_input(
                (ac_ttt_role)0, public_input),
            AC_ERR_ARGUMENT);
        AC_TEST_ASSERT_ZERO(public_input, sizeof public_input);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_core_encode_public_input(AC_TTT_ROLE_X, NULL),
            AC_ERR_ARGUMENT);

        (void)memset(&circuit, 0xa5, sizeof circuit);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_core_circuit_build(
                AC_TTT_ROLE_X,
                ac_ttt_core_gates_a,
                AC_TTT_CORE_X_GATE_COUNT - 1U,
                &ac_ttt_core_builder_memory,
                &circuit,
                &report),
            AC_ERR_CAPACITY);
        AC_TEST_ASSERT_ZERO(&circuit, sizeof circuit);
        AC_TEST_ASSERT_EQ_INT(report.gate_count, AC_TTT_CORE_X_GATE_COUNT);
        AC_TEST_ASSERT_STATUS(
            ac_ttt_core_circuit_build(
                AC_TTT_ROLE_X,
                NULL,
                AC_TTT_CORE_X_GATE_COUNT,
                &ac_ttt_core_builder_memory,
                &circuit,
                &report),
            AC_ERR_ARGUMENT);
    }
    return 0;
}

static int ac_ttt_core_run_bounded_corpus(
    ac_ttt_role role,
    const ac_bool_circuit *circuit,
    const uint8_t *serialized,
    size_t serialized_size)
{
    ac_ttt_policy reference;
    ac_ttt_policy policy;
    ac_ttt_policy losing;
    uint8_t public_input[AC_TTT_CORE_PUBLIC_INPUT_BYTES];
    ac_ttt_core_batch batch;
    uint32_t value;
    unsigned named_kind;
    uint32_t seed;

    if (ac_ttt_build_reference_policy(role, &reference) != AC_OK ||
        ac_ttt_core_encode_public_input(role, public_input) != AC_OK) {
        return 1;
    }
    batch.serialized = serialized;
    batch.serialized_size = serialized_size;
    batch.role = role;
    batch.count = 0U;
    batch.executed = 0U;

    if (ac_ttt_core_batch_add(&batch, &reference, public_input) != 0 ||
        ac_ttt_core_scalar_matches(
            circuit, role, &reference, public_input) != 0) {
        return 1;
    }
    (void)ac_ttt_core_build_known_loss(role, &reference, &losing);
    if (ac_ttt_core_batch_add(&batch, &losing, public_input) != 0 ||
        ac_ttt_core_scalar_matches(
            circuit, role, &losing, public_input) != 0) {
        return 1;
    }

    for (value = 0U; value <= UINT8_MAX; ++value) {
        policy = reference;
        policy.move_by_board[0] = (uint8_t)value;
        if (ac_ttt_core_batch_add(&batch, &policy, public_input) != 0) {
            return 1;
        }
    }
    for (named_kind = 0U; named_kind < 4U; ++named_kind) {
        if (ac_ttt_core_make_named_malformed(
                named_kind, &reference, &policy) != 0 ||
            ac_ttt_core_batch_add(&batch, &policy, public_input) != 0) {
            return 1;
        }
        if (named_kind == 0U &&
            ac_ttt_core_scalar_matches(
                circuit, role, &policy, public_input) != 0) {
            return 1;
        }
    }

    for (seed = 0U; seed < 256U; ++seed) {
        if (ac_ttt_core_make_stratified_mutation(
                &reference, seed, &policy) != 0 ||
            ac_ttt_core_batch_add(&batch, &policy, public_input) != 0) {
            return 1;
        }
    }
    for (seed = 0U; seed < 128U; ++seed) {
        if (ac_ttt_core_make_canonical_multi_policy(
                role, &reference, seed, &policy) != 0 ||
            ac_ttt_core_batch_add(&batch, &policy, public_input) != 0) {
            return 1;
        }
    }

    {
        uint8_t invalid_public[7][AC_TTT_CORE_PUBLIC_INPUT_BYTES];
        size_t case_index;

        for (case_index = 0U; case_index < 7U; ++case_index) {
            (void)memcpy(
                invalid_public[case_index],
                public_input,
                AC_TTT_CORE_PUBLIC_INPUT_BYTES);
        }
        invalid_public[0][0] = 0U;
        invalid_public[1][0] = 2U;
        invalid_public[2][1] = 0U;
        invalid_public[3][1] =
            (uint8_t)ac_ttt_core_test_other_role(role);
        invalid_public[4][1] = 3U;
        invalid_public[5][2] = 1U;
        invalid_public[6][3] = 1U;
        for (case_index = 0U; case_index < 7U; ++case_index) {
            if (ac_ttt_core_batch_add(
                    &batch, &reference, invalid_public[case_index]) != 0) {
                return 1;
            }
        }
        if (ac_ttt_core_scalar_matches(
                circuit, role, &reference, invalid_public[0]) != 0) {
            return 1;
        }
    }

    if (ac_ttt_core_batch_execute(&batch) != 0 || batch.executed != 653U) {
        return 1;
    }
    return 0;
}

static int ac_ttt_core_run_role(ac_ttt_role role)
{
    ac_ttt_core_circuit_report requirements;
    ac_ttt_core_circuit_report report_a;
    ac_ttt_core_circuit_report report_b;
    ac_bool_circuit circuit_a;
    ac_bool_circuit circuit_b;
    ac_bool_circuit_analysis generic_analysis;
    ac_ttt_core_parsed_circuit parsed;
    size_t serialized_size_a = 0U;
    size_t serialized_size_b = 0U;

    if (ac_ttt_core_circuit_requirements(role, &requirements) != AC_OK ||
        ac_ttt_core_circuit_build(
            role,
            ac_ttt_core_gates_a,
            AC_TTT_CORE_MAX_GATE_COUNT,
            &ac_ttt_core_builder_memory,
            &circuit_a,
            &report_a) != AC_OK ||
        ac_ttt_core_circuit_build(
            role,
            ac_ttt_core_gates_b,
            AC_TTT_CORE_MAX_GATE_COUNT,
            &ac_ttt_core_builder_memory,
            &circuit_b,
            &report_b) != AC_OK) {
        return 1;
    }
    if (memcmp(&requirements, &report_a, sizeof report_a) != 0 ||
        memcmp(&report_a, &report_b, sizeof report_a) != 0 ||
        circuit_a.public_input_bits != circuit_b.public_input_bits ||
        circuit_a.witness_input_bits != circuit_b.witness_input_bits ||
        circuit_a.gate_count != circuit_b.gate_count ||
        circuit_a.wire_count != circuit_b.wire_count ||
        circuit_a.output_wire != circuit_b.output_wire ||
        memcmp(
            ac_ttt_core_gates_a,
            ac_ttt_core_gates_b,
            report_a.gate_storage_bytes) != 0) {
        return 1;
    }
    if (ac_bool_circuit_validate(&circuit_a) != AC_OK ||
        ac_bool_circuit_analyze(
            &circuit_a,
            ac_ttt_core_depths,
            AC_TTT_CORE_MAX_WIRE_COUNT,
            &generic_analysis) != AC_OK ||
        generic_analysis.and_gate_count != report_a.and_gate_count ||
        generic_analysis.xor_gate_count != report_a.xor_gate_count ||
        generic_analysis.output_depth != report_a.output_depth) {
        return 1;
    }
    if (ac_bool_circuit_serialize(
            &circuit_a,
            ac_ttt_core_serialized_a,
            sizeof ac_ttt_core_serialized_a,
            &serialized_size_a) != AC_OK ||
        ac_bool_circuit_serialize(
            &circuit_b,
            ac_ttt_core_serialized_b,
            sizeof ac_ttt_core_serialized_b,
            &serialized_size_b) != AC_OK ||
        serialized_size_a != report_a.serialized_bytes ||
        serialized_size_a != serialized_size_b ||
        memcmp(
            ac_ttt_core_serialized_a,
            ac_ttt_core_serialized_b,
            serialized_size_a) != 0) {
        return 1;
    }
    if (ac_ttt_core_parse_and_analyze(
            ac_ttt_core_serialized_a,
            serialized_size_a,
            &parsed) != 0 ||
        parsed.public_bits != report_a.public_input_bits ||
        parsed.witness_bits != report_a.witness_input_bits ||
        parsed.gate_count != report_a.gate_count ||
        parsed.wire_count != report_a.wire_count ||
        parsed.output_wire != circuit_a.output_wire ||
        parsed.and_count != report_a.and_gate_count ||
        parsed.xor_count != report_a.xor_gate_count ||
        parsed.maximum_depth != generic_analysis.maximum_depth ||
        parsed.output_depth != report_a.output_depth) {
        return 1;
    }
    return ac_ttt_core_run_bounded_corpus(
        role, &circuit_a, ac_ttt_core_serialized_a, serialized_size_a);
}

static int test_core_x_counts_serialization_and_differential(void)
{
    AC_TEST_ASSERT_EQ_INT(ac_ttt_core_run_role(AC_TTT_ROLE_X), 0);
    return 0;
}

static int test_core_o_counts_serialization_and_differential(void)
{
    AC_TEST_ASSERT_EQ_INT(ac_ttt_core_run_role(AC_TTT_ROLE_O), 0);
    return 0;
}

int main(void)
{
    ac_test_suite suite = {0U, 0U};

    AC_TEST_RUN(
        suite, test_requirements_encoder_and_argument_boundaries);
    AC_TEST_RUN(
        suite, test_core_x_counts_serialization_and_differential);
    AC_TEST_RUN(
        suite, test_core_o_counts_serialization_and_differential);
    return ac_test_finish(&suite);
}
