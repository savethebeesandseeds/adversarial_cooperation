#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ac/bool_circuit.h"
#include "test_support.h"

static ac_bool_gate make_gate(uint8_t opcode, uint32_t left, uint32_t right)
{
    ac_bool_gate gate;

    (void)memset(&gate, 0, sizeof(gate));
    gate.left = left;
    gate.right = right;
    gate.opcode = opcode;
    return gate;
}

static uint16_t load_u16be(const uint8_t *input)
{
    return (uint16_t)(((uint16_t)input[0] << 8) | (uint16_t)input[1]);
}

static uint32_t load_u32be(const uint8_t *input)
{
    return ((uint32_t)input[0] << 24)
        | ((uint32_t)input[1] << 16)
        | ((uint32_t)input[2] << 8)
        | (uint32_t)input[3];
}

/* Independent, deliberately small parser used only to inspect test output. */
static int parse_serialized(
    const uint8_t *input,
    size_t input_length,
    uint32_t expected_public_bits,
    uint32_t expected_witness_bits,
    uint32_t expected_gate_count,
    uint32_t expected_wire_count,
    uint32_t expected_output_wire)
{
    static const uint8_t magic[8] = {
        'A', 'C', 'B', 'C', 'I', 'R', '0', '1'
    };
    uint32_t public_bits;
    uint32_t witness_bits;
    uint32_t gate_count;
    uint32_t wire_count;
    uint32_t output_wire;
    uint64_t derived_wires;
    uint64_t expected_length;
    uint32_t first_gate_wire;
    uint32_t gate_index;

    if (input == NULL || input_length < AC_BOOL_CIRCUIT_HEADER_BYTES) {
        return 0;
    }
    if (memcmp(input, magic, sizeof(magic)) != 0
        || load_u16be(&input[8]) != AC_BOOL_CIRCUIT_FORMAT_VERSION
        || load_u16be(&input[10]) != AC_BOOL_CIRCUIT_BASIS_XOR_AND) {
        return 0;
    }
    public_bits = load_u32be(&input[12]);
    witness_bits = load_u32be(&input[16]);
    gate_count = load_u32be(&input[20]);
    wire_count = load_u32be(&input[24]);
    output_wire = load_u32be(&input[28]);
    derived_wires = UINT64_C(2)
        + (uint64_t)public_bits
        + (uint64_t)witness_bits
        + (uint64_t)gate_count;
    expected_length = (uint64_t)AC_BOOL_CIRCUIT_HEADER_BYTES
        + (uint64_t)AC_BOOL_CIRCUIT_GATE_BYTES * (uint64_t)gate_count;
    if (derived_wires > (uint64_t)UINT32_MAX
        || wire_count != (uint32_t)derived_wires
        || expected_length != (uint64_t)input_length
        || output_wire >= wire_count
        || public_bits != expected_public_bits
        || witness_bits != expected_witness_bits
        || gate_count != expected_gate_count
        || wire_count != expected_wire_count
        || output_wire != expected_output_wire) {
        return 0;
    }
    first_gate_wire = wire_count - gate_count;
    for (gate_index = 0U; gate_index < gate_count; ++gate_index) {
        size_t offset = (size_t)AC_BOOL_CIRCUIT_HEADER_BYTES
            + (size_t)gate_index * (size_t)AC_BOOL_CIRCUIT_GATE_BYTES;
        uint8_t opcode = input[offset];
        uint32_t left = load_u32be(&input[offset + 1U]);
        uint32_t right = load_u32be(&input[offset + 5U]);
        uint32_t gate_wire = first_gate_wire + gate_index;

        if ((opcode != (uint8_t)AC_BOOL_GATE_XOR
                && opcode != (uint8_t)AC_BOOL_GATE_AND)
            || left >= gate_wire
            || right >= gate_wire) {
            return 0;
        }
    }
    return 1;
}

static int test_gate_record_shape_and_constants(void)
{
    ac_bool_circuit circuit;
    uint8_t scratch[2] = {0xa5U, 0xa5U};
    uint8_t acceptance = 0xa5U;

    AC_TEST_ASSERT_EQ_INT(sizeof(ac_bool_gate), 12U);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 0U, 0U, 0U, 0U, NULL),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(circuit.wire_count, 2U);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            NULL,
            0U,
            NULL,
            0U,
            scratch,
            sizeof(scratch),
            &acceptance),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(acceptance, 0U);
    AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch));

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 0U, 0U, 0U, 1U, NULL),
        AC_OK);
    (void)memset(scratch, 0xa5, sizeof(scratch));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            NULL,
            0U,
            NULL,
            0U,
            scratch,
            sizeof(scratch),
            &acceptance),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(acceptance, 1U);
    AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch));
    return 0;
}

static int test_complete_xor_and_truth_tables(void)
{
    ac_bool_gate gate;
    ac_bool_circuit circuit;
    uint8_t public_input;
    uint8_t scratch[5];
    uint8_t acceptance;
    unsigned int row;

    for (row = 0U; row < 4U; ++row) {
        gate = make_gate((uint8_t)AC_BOOL_GATE_XOR, 2U, 3U);
        AC_TEST_ASSERT_STATUS(
            ac_bool_circuit_init(&circuit, 2U, 0U, 1U, 4U, &gate),
            AC_OK);
        public_input = (uint8_t)row;
        (void)memset(scratch, 0xa5, sizeof(scratch));
        AC_TEST_ASSERT_STATUS(
            ac_bool_circuit_evaluate(
                &circuit,
                &public_input,
                1U,
                NULL,
                0U,
                scratch,
                sizeof(scratch),
                &acceptance),
            AC_OK);
        AC_TEST_ASSERT_EQ_INT(
            acceptance,
            ((row & 1U) ^ ((row >> 1) & 1U)));
        AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch));

        gate = make_gate((uint8_t)AC_BOOL_GATE_AND, 2U, 3U);
        AC_TEST_ASSERT_STATUS(
            ac_bool_circuit_init(&circuit, 2U, 0U, 1U, 4U, &gate),
            AC_OK);
        (void)memset(scratch, 0xa5, sizeof(scratch));
        AC_TEST_ASSERT_STATUS(
            ac_bool_circuit_evaluate(
                &circuit,
                &public_input,
                1U,
                NULL,
                0U,
                scratch,
                sizeof(scratch),
                &acceptance),
            AC_OK);
        AC_TEST_ASSERT_EQ_INT(
            acceptance,
            ((row & 1U) & ((row >> 1) & 1U)));
        AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch));
    }
    return 0;
}

static int test_lsb_first_public_and_witness_wire_order(void)
{
    static const uint8_t public_input[2] = {0xa5U, 0x02U};
    static const uint8_t witness_input[2] = {0x3cU, 0x01U};
    ac_bool_circuit circuit;
    uint8_t scratch[21];
    uint8_t acceptance;
    uint32_t bit;

    for (bit = 0U; bit < 10U; ++bit) {
        AC_TEST_ASSERT_STATUS(
            ac_bool_circuit_init(
                &circuit,
                10U,
                9U,
                0U,
                UINT32_C(2) + bit,
                NULL),
            AC_OK);
        (void)memset(scratch, 0xa5, sizeof(scratch));
        AC_TEST_ASSERT_STATUS(
            ac_bool_circuit_evaluate(
                &circuit,
                public_input,
                sizeof(public_input),
                witness_input,
                sizeof(witness_input),
                scratch,
                sizeof(scratch),
                &acceptance),
            AC_OK);
        AC_TEST_ASSERT_EQ_INT(
            acceptance,
            (public_input[bit / 8U] >> (bit % 8U)) & 1U);
        AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch));
    }
    for (bit = 0U; bit < 9U; ++bit) {
        AC_TEST_ASSERT_STATUS(
            ac_bool_circuit_init(
                &circuit,
                10U,
                9U,
                0U,
                UINT32_C(12) + bit,
                NULL),
            AC_OK);
        (void)memset(scratch, 0xa5, sizeof(scratch));
        AC_TEST_ASSERT_STATUS(
            ac_bool_circuit_evaluate(
                &circuit,
                public_input,
                sizeof(public_input),
                witness_input,
                sizeof(witness_input),
                scratch,
                sizeof(scratch),
                &acceptance),
            AC_OK);
        AC_TEST_ASSERT_EQ_INT(
            acceptance,
            (witness_input[bit / 8U] >> (bit % 8U)) & 1U);
        AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch));
    }
    return 0;
}

static int test_compound_circuit_and_depth_analysis(void)
{
    ac_bool_gate gates[2];
    ac_bool_circuit circuit;
    ac_bool_circuit_analysis analysis;
    uint32_t depths[7];
    uint8_t public_input = 0x03U;
    uint8_t witness_input = 0x01U;
    uint8_t scratch[7];
    uint8_t acceptance;

    /* (public bit 0 XOR witness bit 0) AND public bit 1. */
    gates[0] = make_gate((uint8_t)AC_BOOL_GATE_XOR, 2U, 4U);
    gates[1] = make_gate((uint8_t)AC_BOOL_GATE_AND, 5U, 3U);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 2U, 1U, 2U, 6U, gates),
        AC_OK);
    AC_TEST_ASSERT_STATUS(ac_bool_circuit_validate(&circuit), AC_OK);
    (void)memset(&analysis, 0xa5, sizeof(analysis));
    (void)memset(depths, 0xa5, sizeof(depths));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_analyze(
            &circuit,
            depths,
            sizeof(depths) / sizeof(depths[0]),
            &analysis),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(analysis.xor_gate_count, 1U);
    AC_TEST_ASSERT_EQ_INT(analysis.and_gate_count, 1U);
    AC_TEST_ASSERT_EQ_INT(analysis.maximum_depth, 2U);
    AC_TEST_ASSERT_EQ_INT(analysis.output_depth, 2U);
    AC_TEST_ASSERT_EQ_INT(depths[0], 0U);
    AC_TEST_ASSERT_EQ_INT(depths[4], 0U);
    AC_TEST_ASSERT_EQ_INT(depths[5], 1U);
    AC_TEST_ASSERT_EQ_INT(depths[6], 2U);

    (void)memset(scratch, 0xa5, sizeof(scratch));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            &public_input,
            1U,
            &witness_input,
            1U,
            scratch,
            sizeof(scratch),
            &acceptance),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(acceptance, 0U);
    AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch));
    witness_input = 0U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            &public_input,
            1U,
            &witness_input,
            1U,
            scratch,
            sizeof(scratch),
            &acceptance),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(acceptance, 1U);
    AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch));
    return 0;
}

static int test_structural_validation_rejections(void)
{
    ac_bool_gate gates[2];
    ac_bool_circuit circuit;
    ac_bool_circuit cleared;

    gates[0] = make_gate((uint8_t)AC_BOOL_GATE_XOR, 2U, 3U);
    circuit.public_input_bits = 2U;
    circuit.witness_input_bits = 0U;
    circuit.gate_count = 1U;
    circuit.wire_count = 5U;
    circuit.output_wire = 4U;
    circuit.gates = gates;
    AC_TEST_ASSERT_STATUS(ac_bool_circuit_validate(&circuit), AC_OK);

    gates[0].opcode = 99U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_validate(&circuit),
        AC_ERR_FORMAT);
    gates[0].opcode = (uint8_t)AC_BOOL_GATE_XOR;
    gates[0].reserved[2] = 1U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_validate(&circuit),
        AC_ERR_FORMAT);
    gates[0].reserved[2] = 0U;
    gates[0].left = 4U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_validate(&circuit),
        AC_ERR_FORMAT);
    gates[0].left = 2U;

    gates[1] = make_gate((uint8_t)AC_BOOL_GATE_AND, 4U, 1U);
    circuit.gate_count = 2U;
    circuit.wire_count = 6U;
    circuit.output_wire = 5U;
    gates[0].right = 5U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_validate(&circuit),
        AC_ERR_FORMAT);
    gates[0].right = 3U;
    AC_TEST_ASSERT_STATUS(ac_bool_circuit_validate(&circuit), AC_OK);

    circuit.wire_count = 7U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_validate(&circuit),
        AC_ERR_FORMAT);
    circuit.wire_count = 6U;
    circuit.output_wire = 6U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_validate(&circuit),
        AC_ERR_FORMAT);
    circuit.output_wire = 5U;
    circuit.gates = NULL;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_validate(&circuit),
        AC_ERR_ARGUMENT);

    (void)memset(&circuit, 0xa5, sizeof(circuit));
    (void)memset(&cleared, 0, sizeof(cleared));
    gates[0].opcode = 99U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 2U, 0U, 1U, 4U, gates),
        AC_ERR_FORMAT);
    AC_TEST_ASSERT_MEMORY(&circuit, &cleared, sizeof(circuit));
    (void)memset(&circuit, 0xa5, sizeof(circuit));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(
            &circuit,
            UINT32_MAX,
            0U,
            0U,
            0U,
            NULL),
        AC_ERR_LENGTH);
    AC_TEST_ASSERT_MEMORY(&circuit, &cleared, sizeof(circuit));
    AC_TEST_ASSERT_STATUS(ac_bool_circuit_validate(NULL), AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(NULL, 0U, 0U, 0U, 0U, NULL),
        AC_ERR_ARGUMENT);
    return 0;
}

static int test_analysis_errors_and_overlap(void)
{
    ac_bool_gate gate = make_gate((uint8_t)AC_BOOL_GATE_AND, 2U, 3U);
    ac_bool_circuit circuit;
    ac_bool_circuit_analysis analysis;
    uint32_t depths[5];
    uint8_t untouched_analysis[sizeof(analysis)];

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 2U, 0U, 1U, 4U, &gate),
        AC_OK);
    (void)memset(&analysis, 0xa5, sizeof(analysis));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_analyze(&circuit, depths, 4U, &analysis),
        AC_ERR_CAPACITY);
    AC_TEST_ASSERT_ZERO(&analysis, sizeof(analysis));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_analyze(&circuit, NULL, 5U, &analysis),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_analyze(&circuit, depths, 5U, NULL),
        AC_ERR_ARGUMENT);
    (void)memset(&analysis, 0xa5, sizeof(analysis));
    (void)memset(untouched_analysis, 0xa5, sizeof(untouched_analysis));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_analyze(
            &circuit,
            (uint32_t *)(void *)&gate,
            5U,
            &analysis),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_MEMORY(
        &analysis,
        untouched_analysis,
        sizeof(analysis));
    return 0;
}

static int test_input_lengths_unused_bits_and_clearing(void)
{
    ac_bool_circuit circuit;
    uint8_t public_input[2] = {1U, 0U};
    uint8_t witness_input[2] = {0U, 0U};
    uint8_t scratch[20];
    uint8_t acceptance = 0xa5U;

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 9U, 9U, 0U, 2U, NULL),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            public_input,
            1U,
            witness_input,
            sizeof(witness_input),
            scratch,
            sizeof(scratch),
            &acceptance),
        AC_ERR_LENGTH);
    AC_TEST_ASSERT_EQ_INT(acceptance, 0U);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            NULL,
            sizeof(public_input),
            witness_input,
            sizeof(witness_input),
            scratch,
            sizeof(scratch),
            &acceptance),
        AC_ERR_ARGUMENT);

    public_input[1] = 0x02U;
    (void)memset(scratch, 0xa5, sizeof(scratch));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            public_input,
            sizeof(public_input),
            witness_input,
            sizeof(witness_input),
            scratch,
            sizeof(scratch),
            &acceptance),
        AC_ERR_FORMAT);
    AC_TEST_ASSERT_EQ_INT(acceptance, 0U);
    AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch));
    public_input[1] = 0U;
    witness_input[1] = 0x80U;
    (void)memset(scratch, 0xa5, sizeof(scratch));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            public_input,
            sizeof(public_input),
            witness_input,
            sizeof(witness_input),
            scratch,
            sizeof(scratch),
            &acceptance),
        AC_ERR_FORMAT);
    AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch));
    witness_input[1] = 0U;

    (void)memset(scratch, 0xa5, sizeof(scratch));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            public_input,
            sizeof(public_input),
            witness_input,
            sizeof(witness_input),
            scratch,
            sizeof(scratch) - 1U,
            &acceptance),
        AC_ERR_CAPACITY);
    AC_TEST_ASSERT_ZERO(scratch, sizeof(scratch) - 1U);
    AC_TEST_ASSERT_EQ_INT(scratch[sizeof(scratch) - 1U], 0xa5U);
    return 0;
}

static int test_evaluator_overlap_rejections(void)
{
    ac_bool_gate gate = make_gate((uint8_t)AC_BOOL_GATE_XOR, 2U, 3U);
    ac_bool_circuit circuit;
    uint8_t storage[8] = {0U};
    uint8_t scratch[5] = {0U};
    uint8_t public_input = 0U;
    uint8_t acceptance = 0xa5U;

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 2U, 0U, 1U, 4U, &gate),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            storage,
            1U,
            NULL,
            0U,
            storage,
            5U,
            &acceptance),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(acceptance, 0xa5U);
    scratch[1] = 0xa5U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            &public_input,
            1U,
            NULL,
            0U,
            scratch,
            sizeof(scratch),
            &scratch[1]),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(scratch[1], 0xa5U);
    acceptance = 0xa5U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            &public_input,
            1U,
            NULL,
            0U,
            (uint8_t *)(void *)&gate,
            5U,
            &acceptance),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(acceptance, 0xa5U);
    return 0;
}

static int test_tiny_canonical_serialization_vector(void)
{
    static const uint8_t expected[50] = {
        0x41U, 0x43U, 0x42U, 0x43U, 0x49U, 0x52U, 0x30U, 0x31U,
        0x00U, 0x01U, 0x00U, 0x01U,
        0x00U, 0x00U, 0x00U, 0x02U,
        0x00U, 0x00U, 0x00U, 0x01U,
        0x00U, 0x00U, 0x00U, 0x02U,
        0x00U, 0x00U, 0x00U, 0x07U,
        0x00U, 0x00U, 0x00U, 0x06U,
        0x01U, 0x00U, 0x00U, 0x00U, 0x02U,
        0x00U, 0x00U, 0x00U, 0x04U,
        0x02U, 0x00U, 0x00U, 0x00U, 0x05U,
        0x00U, 0x00U, 0x00U, 0x03U
    };
    ac_bool_gate gates[2];
    ac_bool_circuit circuit;
    uint8_t first[50];
    uint8_t second[50];
    size_t required_size = 0U;
    size_t written = 0U;

    gates[0] = make_gate((uint8_t)AC_BOOL_GATE_XOR, 2U, 4U);
    gates[1] = make_gate((uint8_t)AC_BOOL_GATE_AND, 5U, 3U);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 2U, 1U, 2U, 6U, gates),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialized_size(&circuit, &required_size),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(required_size, sizeof(expected));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialize(
            &circuit,
            first,
            sizeof(first),
            &written),
        AC_OK);
    AC_TEST_ASSERT_EQ_INT(written, sizeof(first));
    AC_TEST_ASSERT_MEMORY(first, expected, sizeof(expected));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialize(
            &circuit,
            second,
            sizeof(second),
            &written),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(first, second, sizeof(first));
    AC_TEST_ASSERT(parse_serialized(first, sizeof(first), 2U, 1U, 2U, 7U, 6U));
    return 0;
}

static int test_independent_parser_rejects_format_mutations(void)
{
    ac_bool_gate gate = make_gate((uint8_t)AC_BOOL_GATE_XOR, 2U, 3U);
    ac_bool_circuit circuit;
    uint8_t encoded[41];
    uint8_t changed[42];
    size_t written;

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 2U, 0U, 1U, 4U, &gate),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialize(
            &circuit,
            encoded,
            sizeof(encoded),
            &written),
        AC_OK);
    AC_TEST_ASSERT(parse_serialized(encoded, sizeof(encoded), 2U, 0U, 1U, 5U, 4U));
    AC_TEST_ASSERT(!parse_serialized(encoded, sizeof(encoded) - 1U, 2U, 0U, 1U, 5U, 4U));
    (void)memcpy(changed, encoded, sizeof(encoded));
    changed[sizeof(encoded)] = 0U;
    AC_TEST_ASSERT(!parse_serialized(changed, sizeof(changed), 2U, 0U, 1U, 5U, 4U));
    (void)memcpy(changed, encoded, sizeof(encoded));
    changed[0] ^= 1U;
    AC_TEST_ASSERT(!parse_serialized(changed, sizeof(encoded), 2U, 0U, 1U, 5U, 4U));
    (void)memcpy(changed, encoded, sizeof(encoded));
    changed[11] = 2U;
    AC_TEST_ASSERT(!parse_serialized(changed, sizeof(encoded), 2U, 0U, 1U, 5U, 4U));
    (void)memcpy(changed, encoded, sizeof(encoded));
    changed[32] = 99U;
    AC_TEST_ASSERT(!parse_serialized(changed, sizeof(encoded), 2U, 0U, 1U, 5U, 4U));
    (void)memcpy(changed, encoded, sizeof(encoded));
    changed[36] = 4U;
    AC_TEST_ASSERT(!parse_serialized(changed, sizeof(encoded), 2U, 0U, 1U, 5U, 4U));
    return 0;
}

static int test_serialization_errors_capacity_and_overlap(void)
{
    ac_bool_gate gate = make_gate((uint8_t)AC_BOOL_GATE_AND, 2U, 3U);
    ac_bool_circuit circuit;
    uint8_t output[41];
    size_t size = 123U;
    size_t written = 123U;

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 2U, 0U, 1U, 4U, &gate),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialize(
            &circuit,
            output,
            sizeof(output) - 1U,
            &written),
        AC_ERR_CAPACITY);
    AC_TEST_ASSERT_EQ_INT(written, 0U);
    written = 123U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialize(
            &circuit,
            NULL,
            sizeof(output),
            &written),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(written, 0U);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialize(
            &circuit,
            output,
            sizeof(output),
            NULL),
        AC_ERR_ARGUMENT);
    written = 123U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialize(
            &circuit,
            (uint8_t *)(void *)&gate,
            sizeof(output),
            &written),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(written, 123U);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialized_size(NULL, &size),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(size, 0U);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialized_size(&circuit, NULL),
        AC_ERR_ARGUMENT);

    gate.reserved[0] = 1U;
    size = 123U;
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialized_size(&circuit, &size),
        AC_ERR_FORMAT);
    AC_TEST_ASSERT_EQ_INT(size, 0U);
    return 0;
}

static int test_null_and_output_clearing_policy(void)
{
    ac_bool_circuit circuit;
    uint8_t scratch[2] = {0U};
    uint8_t acceptance = 0xa5U;

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 0U, 0U, 0U, 1U, NULL),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            NULL,
            NULL,
            0U,
            NULL,
            0U,
            scratch,
            sizeof(scratch),
            &acceptance),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(acceptance, 0U);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            NULL,
            0U,
            NULL,
            0U,
            NULL,
            sizeof(scratch),
            &acceptance),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            NULL,
            0U,
            NULL,
            0U,
            scratch,
            sizeof(scratch),
            NULL),
        AC_ERR_ARGUMENT);
    return 0;
}

static int test_aliased_outputs_are_rejected_without_mutation(void)
{
    _Alignas(size_t) ac_bool_gate gate = make_gate(
        (uint8_t)AC_BOOL_GATE_AND,
        0U,
        1U);
    _Alignas(size_t) ac_bool_circuit circuit;
    ac_bool_circuit init_target;
    uint8_t saved_gate[sizeof(gate)];
    uint8_t saved_circuit[sizeof(circuit)];
    uint8_t saved_init_target[sizeof(init_target)];
    uint32_t depths[3] = {0U};
    uint8_t scratch[3] = {0U};
    uint8_t output[41] = {0U};
    ac_bool_circuit_analysis null_analysis;
    uint8_t saved_null_analysis[sizeof(null_analysis)];
    uint8_t null_acceptance = 0xa5U;
    size_t null_written = (size_t)123U;

    (void)memset(&null_analysis, 0xa5, sizeof(null_analysis));
    (void)memcpy(
        saved_null_analysis,
        &null_analysis,
        sizeof(saved_null_analysis));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_analyze(
            NULL,
            (uint32_t *)(void *)&null_analysis,
            sizeof(null_analysis) / sizeof(uint32_t),
            &null_analysis),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_MEMORY(
        &null_analysis,
        saved_null_analysis,
        sizeof(null_analysis));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            NULL,
            &null_acceptance,
            sizeof(null_acceptance),
            NULL,
            0U,
            scratch,
            sizeof(scratch),
            &null_acceptance),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(null_acceptance, 0xa5U);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialize(
            NULL,
            (uint8_t *)(void *)&null_written,
            sizeof(null_written),
            &null_written),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_EQ_INT(null_written, 123U);

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(&circuit, 0U, 0U, 1U, 2U, &gate),
        AC_OK);
    (void)memcpy(saved_gate, &gate, sizeof(saved_gate));
    (void)memcpy(saved_circuit, &circuit, sizeof(saved_circuit));

    (void)memset(&init_target, 0xa5, sizeof(init_target));
    (void)memcpy(
        saved_init_target,
        &init_target,
        sizeof(saved_init_target));
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_init(
            &init_target,
            0U,
            0U,
            1U,
            2U,
            (const ac_bool_gate *)(const void *)&init_target),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_MEMORY(
        &init_target,
        saved_init_target,
        sizeof(init_target));

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_analyze(
            &circuit,
            depths,
            sizeof(depths) / sizeof(depths[0]),
            (ac_bool_circuit_analysis *)(void *)&circuit),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_MEMORY(&circuit, saved_circuit, sizeof(circuit));
    AC_TEST_ASSERT_MEMORY(&gate, saved_gate, sizeof(gate));

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            NULL,
            0U,
            NULL,
            0U,
            scratch,
            sizeof(scratch),
            (uint8_t *)(void *)&circuit),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_evaluate(
            &circuit,
            NULL,
            0U,
            NULL,
            0U,
            scratch,
            sizeof(scratch),
            (uint8_t *)(void *)&gate),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_MEMORY(&circuit, saved_circuit, sizeof(circuit));
    AC_TEST_ASSERT_MEMORY(&gate, saved_gate, sizeof(gate));

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialized_size(
            &circuit,
            (size_t *)(void *)&circuit),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialized_size(
            &circuit,
            (size_t *)(void *)&gate),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_MEMORY(&circuit, saved_circuit, sizeof(circuit));
    AC_TEST_ASSERT_MEMORY(&gate, saved_gate, sizeof(gate));

    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialize(
            &circuit,
            output,
            sizeof(output),
            (size_t *)(void *)&circuit),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_bool_circuit_serialize(
            &circuit,
            output,
            sizeof(output),
            (size_t *)(void *)&gate),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_MEMORY(&circuit, saved_circuit, sizeof(circuit));
    AC_TEST_ASSERT_MEMORY(&gate, saved_gate, sizeof(gate));
    return 0;
}

int main(void)
{
    ac_test_suite suite = {0U, 0U};

    AC_TEST_RUN(suite, test_gate_record_shape_and_constants);
    AC_TEST_RUN(suite, test_complete_xor_and_truth_tables);
    AC_TEST_RUN(suite, test_lsb_first_public_and_witness_wire_order);
    AC_TEST_RUN(suite, test_compound_circuit_and_depth_analysis);
    AC_TEST_RUN(suite, test_structural_validation_rejections);
    AC_TEST_RUN(suite, test_analysis_errors_and_overlap);
    AC_TEST_RUN(suite, test_input_lengths_unused_bits_and_clearing);
    AC_TEST_RUN(suite, test_evaluator_overlap_rejections);
    AC_TEST_RUN(suite, test_tiny_canonical_serialization_vector);
    AC_TEST_RUN(suite, test_independent_parser_rejects_format_mutations);
    AC_TEST_RUN(suite, test_serialization_errors_capacity_and_overlap);
    AC_TEST_RUN(suite, test_null_and_output_clearing_policy);
    AC_TEST_RUN(suite, test_aliased_outputs_are_rejected_without_mutation);
    return ac_test_finish(&suite);
}
