#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ac/bool_circuit.h"

typedef struct {
    const void *address;
    size_t length;
} ac_bool_region;

static void ac_bool_wipe(void *buffer, size_t length)
{
    volatile uint8_t *bytes = (volatile uint8_t *)buffer;
    size_t index;

    if (buffer == NULL) {
        return;
    }
    for (index = 0U; index < length; ++index) {
        bytes[index] = 0U;
    }
}

static ac_status ac_bool_size_multiply(
    size_t left,
    size_t right,
    size_t *product_out)
{
    if (product_out == NULL) {
        return AC_ERR_ARGUMENT;
    }
    *product_out = 0U;
    if (left != 0U && right > SIZE_MAX / left) {
        return AC_ERR_LENGTH;
    }
    *product_out = left * right;
    return AC_OK;
}

static ac_status ac_bool_derived_wire_count(
    uint32_t public_input_bits,
    uint32_t witness_input_bits,
    uint32_t gate_count,
    uint32_t *wire_count_out)
{
    uint64_t count;

    if (wire_count_out == NULL) {
        return AC_ERR_ARGUMENT;
    }
    *wire_count_out = 0U;
    count = UINT64_C(2)
        + (uint64_t)public_input_bits
        + (uint64_t)witness_input_bits
        + (uint64_t)gate_count;
    if (count > (uint64_t)UINT32_MAX) {
        return AC_ERR_LENGTH;
    }
    *wire_count_out = (uint32_t)count;
    return AC_OK;
}

static ac_status ac_bool_input_bytes(uint32_t bits, size_t *bytes_out)
{
    uint64_t rounded;

    if (bytes_out == NULL) {
        return AC_ERR_ARGUMENT;
    }
    *bytes_out = 0U;
    rounded = (uint64_t)bits + UINT64_C(7);
    rounded /= UINT64_C(8);
    if (rounded > (uint64_t)SIZE_MAX) {
        return AC_ERR_LENGTH;
    }
    *bytes_out = (size_t)rounded;
    return AC_OK;
}

static ac_status ac_bool_region_end(
    const ac_bool_region *region,
    uintptr_t *begin_out,
    uintptr_t *end_out)
{
    uintptr_t begin;

    if (region == NULL || begin_out == NULL || end_out == NULL) {
        return AC_ERR_ARGUMENT;
    }
    *begin_out = 0U;
    *end_out = 0U;
    if (region->length == 0U) {
        return AC_OK;
    }
    if (region->address == NULL) {
        return AC_ERR_ARGUMENT;
    }
    begin = (uintptr_t)region->address;
    if (region->length > (size_t)(UINTPTR_MAX - begin)) {
        return AC_ERR_LENGTH;
    }
    *begin_out = begin;
    *end_out = begin + (uintptr_t)region->length;
    return AC_OK;
}

static ac_status ac_bool_require_disjoint(
    const ac_bool_region *regions,
    size_t region_count)
{
    size_t left_index;

    if (regions == NULL && region_count != 0U) {
        return AC_ERR_ARGUMENT;
    }
    for (left_index = 0U; left_index < region_count; ++left_index) {
        uintptr_t left_begin;
        uintptr_t left_end;
        size_t right_index;
        ac_status status = ac_bool_region_end(
            &regions[left_index],
            &left_begin,
            &left_end);

        if (status != AC_OK) {
            return status;
        }
        if (regions[left_index].length == 0U) {
            continue;
        }
        for (right_index = left_index + 1U;
             right_index < region_count;
             ++right_index) {
            uintptr_t right_begin;
            uintptr_t right_end;

            status = ac_bool_region_end(
                &regions[right_index],
                &right_begin,
                &right_end);
            if (status != AC_OK) {
                return status;
            }
            if (regions[right_index].length != 0U
                && left_begin < right_end
                && right_begin < left_end) {
                return AC_ERR_ARGUMENT;
            }
        }
    }
    return AC_OK;
}

static int ac_bool_unused_bits_are_zero(
    const uint8_t *input,
    size_t input_bytes,
    uint32_t input_bits)
{
    uint32_t remainder = input_bits % UINT32_C(8);
    uint8_t used_mask;

    if (remainder == 0U) {
        return 1;
    }
    if (input == NULL || input_bytes == 0U) {
        return 0;
    }
    used_mask = (uint8_t)((UINT32_C(1) << remainder) - UINT32_C(1));
    return (input[input_bytes - 1U] & (uint8_t)~used_mask) == 0U;
}

static void ac_bool_store_u16be(uint8_t *output, uint16_t value)
{
    output[0] = (uint8_t)(value >> 8);
    output[1] = (uint8_t)value;
}

static void ac_bool_store_u32be(uint8_t *output, uint32_t value)
{
    output[0] = (uint8_t)(value >> 24);
    output[1] = (uint8_t)(value >> 16);
    output[2] = (uint8_t)(value >> 8);
    output[3] = (uint8_t)value;
}

ac_status ac_bool_circuit_init(
    ac_bool_circuit *circuit,
    uint32_t public_input_bits,
    uint32_t witness_input_bits,
    uint32_t gate_count,
    uint32_t output_wire,
    const ac_bool_gate *gates)
{
    ac_bool_region regions[2];
    size_t gate_bytes;
    uint32_t wire_count;
    ac_status status;

    if (circuit == NULL) {
        return AC_ERR_ARGUMENT;
    }
    status = ac_bool_size_multiply(
        (size_t)gate_count,
        sizeof(ac_bool_gate),
        &gate_bytes);
    if (status != AC_OK) {
        return status;
    }
    regions[0].address = circuit;
    regions[0].length = sizeof(*circuit);
    regions[1].address = gates;
    regions[1].length = gate_bytes;
    status = ac_bool_require_disjoint(regions, 2U);
    if (status != AC_OK) {
        return status;
    }
    (void)memset(circuit, 0, sizeof(*circuit));
    status = ac_bool_derived_wire_count(
        public_input_bits,
        witness_input_bits,
        gate_count,
        &wire_count);
    if (status != AC_OK) {
        return status;
    }
    circuit->public_input_bits = public_input_bits;
    circuit->witness_input_bits = witness_input_bits;
    circuit->gate_count = gate_count;
    circuit->wire_count = wire_count;
    circuit->output_wire = output_wire;
    circuit->gates = gates;
    status = ac_bool_circuit_validate(circuit);
    if (status != AC_OK) {
        (void)memset(circuit, 0, sizeof(*circuit));
    }
    return status;
}

ac_status ac_bool_circuit_validate(const ac_bool_circuit *circuit)
{
    uint32_t expected_wire_count;
    uint32_t first_gate_wire;
    uint32_t index;
    size_t representable_bytes;
    ac_status status;

    if (circuit == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (circuit->gate_count != 0U && circuit->gates == NULL) {
        return AC_ERR_ARGUMENT;
    }
    status = ac_bool_derived_wire_count(
        circuit->public_input_bits,
        circuit->witness_input_bits,
        circuit->gate_count,
        &expected_wire_count);
    if (status != AC_OK) {
        return status;
    }
    status = ac_bool_input_bytes(
        circuit->public_input_bits,
        &representable_bytes);
    if (status != AC_OK) {
        return status;
    }
    status = ac_bool_input_bytes(
        circuit->witness_input_bits,
        &representable_bytes);
    if (status != AC_OK) {
        return status;
    }
    status = ac_bool_size_multiply(
        (size_t)circuit->gate_count,
        sizeof(ac_bool_gate),
        &representable_bytes);
    if (status != AC_OK) {
        return status;
    }
    if (circuit->wire_count != expected_wire_count) {
        return AC_ERR_FORMAT;
    }
    if (circuit->output_wire >= circuit->wire_count) {
        return AC_ERR_FORMAT;
    }
    first_gate_wire = circuit->wire_count - circuit->gate_count;
    for (index = 0U; index < circuit->gate_count; ++index) {
        const ac_bool_gate *gate = &circuit->gates[index];
        uint32_t gate_wire = first_gate_wire + index;

        if (gate->opcode != (uint8_t)AC_BOOL_GATE_XOR
            && gate->opcode != (uint8_t)AC_BOOL_GATE_AND) {
            return AC_ERR_FORMAT;
        }
        if (gate->reserved[0] != 0U
            || gate->reserved[1] != 0U
            || gate->reserved[2] != 0U) {
            return AC_ERR_FORMAT;
        }
        if (gate->left >= gate_wire || gate->right >= gate_wire) {
            return AC_ERR_FORMAT;
        }
    }
    return AC_OK;
}

ac_status ac_bool_circuit_analyze(
    const ac_bool_circuit *circuit,
    uint32_t *depth_scratch,
    size_t depth_capacity,
    ac_bool_circuit_analysis *analysis)
{
    ac_bool_region regions[4];
    size_t gate_bytes;
    size_t depth_bytes;
    size_t depth_entries;
    uint32_t first_gate_wire;
    uint32_t index;
    ac_status status;

    if (analysis == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (circuit == NULL) {
        depth_entries = depth_scratch == NULL ? 0U : depth_capacity;
        status = ac_bool_size_multiply(
            depth_entries,
            sizeof(*depth_scratch),
            &depth_bytes);
        if (status != AC_OK) {
            return status;
        }
        regions[0].address = depth_scratch;
        regions[0].length = depth_bytes;
        regions[1].address = analysis;
        regions[1].length = sizeof(*analysis);
        status = ac_bool_require_disjoint(regions, 2U);
        if (status != AC_OK) {
            return status;
        }
        (void)memset(analysis, 0, sizeof(*analysis));
        return AC_ERR_ARGUMENT;
    }
    status = ac_bool_size_multiply(
        (size_t)circuit->gate_count,
        sizeof(ac_bool_gate),
        &gate_bytes);
    if (status != AC_OK) {
        return status;
    }
    depth_entries = depth_scratch == NULL
        ? 0U
        : (depth_capacity < (size_t)circuit->wire_count
            ? depth_capacity
            : (size_t)circuit->wire_count);
    status = ac_bool_size_multiply(
        depth_entries,
        sizeof(*depth_scratch),
        &depth_bytes);
    if (status != AC_OK) {
        return status;
    }
    regions[0].address = circuit;
    regions[0].length = sizeof(*circuit);
    regions[1].address = circuit->gates;
    regions[1].length = gate_bytes;
    regions[2].address = depth_scratch;
    regions[2].length = depth_bytes;
    regions[3].address = analysis;
    regions[3].length = sizeof(*analysis);
    status = ac_bool_require_disjoint(regions, 4U);
    if (status != AC_OK) {
        return status;
    }
    (void)memset(analysis, 0, sizeof(*analysis));
    if (depth_scratch == NULL) {
        return AC_ERR_ARGUMENT;
    }
    status = ac_bool_circuit_validate(circuit);
    if (status != AC_OK) {
        return status;
    }
    if (depth_capacity < (size_t)circuit->wire_count) {
        return AC_ERR_CAPACITY;
    }

    (void)memset(depth_scratch, 0, depth_bytes);
    first_gate_wire = circuit->wire_count - circuit->gate_count;
    for (index = 0U; index < circuit->gate_count; ++index) {
        const ac_bool_gate *gate = &circuit->gates[index];
        uint32_t left_depth = depth_scratch[gate->left];
        uint32_t right_depth = depth_scratch[gate->right];
        uint32_t depth = (left_depth > right_depth
            ? left_depth
            : right_depth) + UINT32_C(1);

        depth_scratch[first_gate_wire + index] = depth;
        if (depth > analysis->maximum_depth) {
            analysis->maximum_depth = depth;
        }
        if (gate->opcode == (uint8_t)AC_BOOL_GATE_XOR) {
            analysis->xor_gate_count += UINT32_C(1);
        } else {
            analysis->and_gate_count += UINT32_C(1);
        }
    }
    analysis->output_depth = depth_scratch[circuit->output_wire];
    return AC_OK;
}

ac_status ac_bool_circuit_evaluate(
    const ac_bool_circuit *circuit,
    const uint8_t *public_input,
    size_t public_input_bytes,
    const uint8_t *witness_input,
    size_t witness_input_bytes,
    uint8_t *wire_scratch,
    size_t wire_scratch_capacity,
    uint8_t *acceptance_out)
{
    ac_bool_region regions[6];
    size_t required_public_bytes;
    size_t required_witness_bytes;
    size_t gate_bytes;
    size_t wipe_bytes = 0U;
    uint32_t witness_start;
    uint32_t first_gate_wire;
    uint32_t index;
    int wipe_scratch = 0;
    ac_status status;

    if (acceptance_out == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (circuit == NULL) {
        regions[0].address = public_input;
        regions[0].length = public_input == NULL ? 0U : public_input_bytes;
        regions[1].address = witness_input;
        regions[1].length = witness_input == NULL ? 0U : witness_input_bytes;
        regions[2].address = wire_scratch;
        regions[2].length = wire_scratch == NULL
            ? 0U
            : wire_scratch_capacity;
        regions[3].address = acceptance_out;
        regions[3].length = sizeof(*acceptance_out);
        status = ac_bool_require_disjoint(regions, 4U);
        if (status != AC_OK) {
            return status;
        }
        *acceptance_out = 0U;
        return AC_ERR_ARGUMENT;
    }
    status = ac_bool_size_multiply(
        (size_t)circuit->gate_count,
        sizeof(ac_bool_gate),
        &gate_bytes);
    if (status != AC_OK) {
        return status;
    }
    wipe_bytes = wire_scratch == NULL
        ? 0U
        : (wire_scratch_capacity < (size_t)circuit->wire_count
            ? wire_scratch_capacity
            : (size_t)circuit->wire_count);
    regions[0].address = circuit;
    regions[0].length = sizeof(*circuit);
    regions[1].address = circuit->gates;
    regions[1].length = gate_bytes;
    regions[2].address = public_input;
    regions[2].length = public_input_bytes;
    regions[3].address = witness_input;
    regions[3].length = witness_input_bytes;
    regions[4].address = wire_scratch;
    regions[4].length = wipe_bytes;
    regions[5].address = acceptance_out;
    regions[5].length = sizeof(*acceptance_out);
    status = ac_bool_require_disjoint(regions, 6U);
    if (status != AC_OK) {
        return status;
    }
    *acceptance_out = 0U;
    if (wire_scratch == NULL) {
        return AC_ERR_ARGUMENT;
    }
    status = ac_bool_circuit_validate(circuit);
    if (status != AC_OK) {
        return status;
    }
    status = ac_bool_input_bytes(
        circuit->public_input_bits,
        &required_public_bytes);
    if (status != AC_OK) {
        return status;
    }
    status = ac_bool_input_bytes(
        circuit->witness_input_bits,
        &required_witness_bytes);
    if (status != AC_OK) {
        return status;
    }
    if (public_input_bytes != required_public_bytes
        || witness_input_bytes != required_witness_bytes) {
        return AC_ERR_LENGTH;
    }
    if ((required_public_bytes != 0U && public_input == NULL)
        || (required_witness_bytes != 0U && witness_input == NULL)) {
        return AC_ERR_ARGUMENT;
    }
    wipe_scratch = 1;
    if (wire_scratch_capacity < (size_t)circuit->wire_count) {
        status = AC_ERR_CAPACITY;
        goto cleanup;
    }
    if (!ac_bool_unused_bits_are_zero(
            public_input,
            public_input_bytes,
            circuit->public_input_bits)
        || !ac_bool_unused_bits_are_zero(
            witness_input,
            witness_input_bytes,
            circuit->witness_input_bits)) {
        status = AC_ERR_FORMAT;
        goto cleanup;
    }

    wire_scratch[0] = 0U;
    wire_scratch[1] = 1U;
    for (index = 0U; index < circuit->public_input_bits; ++index) {
        wire_scratch[UINT32_C(2) + index] = (uint8_t)(
            (public_input[index / UINT32_C(8)]
                >> (index % UINT32_C(8))) & UINT8_C(1));
    }
    witness_start = UINT32_C(2) + circuit->public_input_bits;
    for (index = 0U; index < circuit->witness_input_bits; ++index) {
        wire_scratch[witness_start + index] = (uint8_t)(
            (witness_input[index / UINT32_C(8)]
                >> (index % UINT32_C(8))) & UINT8_C(1));
    }
    first_gate_wire = circuit->wire_count - circuit->gate_count;
    for (index = 0U; index < circuit->gate_count; ++index) {
        const ac_bool_gate *gate = &circuit->gates[index];
        uint8_t value;

        if (gate->opcode == (uint8_t)AC_BOOL_GATE_XOR) {
            value = (uint8_t)(
                wire_scratch[gate->left] ^ wire_scratch[gate->right]);
        } else {
            value = (uint8_t)(
                wire_scratch[gate->left] & wire_scratch[gate->right]);
        }
        wire_scratch[first_gate_wire + index] = value;
    }
    *acceptance_out = wire_scratch[circuit->output_wire];
    status = AC_OK;

cleanup:
    if (wipe_scratch != 0) {
        ac_bool_wipe(wire_scratch, wipe_bytes);
    }
    return status;
}

ac_status ac_bool_circuit_serialized_size(
    const ac_bool_circuit *circuit,
    size_t *serialized_size_out)
{
    ac_bool_region regions[3];
    size_t gate_storage_bytes;
    size_t gates_size;
    ac_status status;

    if (serialized_size_out == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (circuit == NULL) {
        *serialized_size_out = 0U;
        return AC_ERR_ARGUMENT;
    }
    status = ac_bool_size_multiply(
        (size_t)circuit->gate_count,
        sizeof(ac_bool_gate),
        &gate_storage_bytes);
    if (status != AC_OK) {
        return status;
    }
    regions[0].address = circuit;
    regions[0].length = sizeof(*circuit);
    regions[1].address = circuit->gates;
    regions[1].length = gate_storage_bytes;
    regions[2].address = serialized_size_out;
    regions[2].length = sizeof(*serialized_size_out);
    status = ac_bool_require_disjoint(regions, 3U);
    if (status != AC_OK) {
        return status;
    }
    *serialized_size_out = 0U;
    status = ac_bool_circuit_validate(circuit);
    if (status != AC_OK) {
        return status;
    }
    status = ac_bool_size_multiply(
        (size_t)circuit->gate_count,
        (size_t)AC_BOOL_CIRCUIT_GATE_BYTES,
        &gates_size);
    if (status != AC_OK) {
        return status;
    }
    if (gates_size > SIZE_MAX - (size_t)AC_BOOL_CIRCUIT_HEADER_BYTES) {
        return AC_ERR_LENGTH;
    }
    *serialized_size_out = (size_t)AC_BOOL_CIRCUIT_HEADER_BYTES + gates_size;
    return AC_OK;
}

ac_status ac_bool_circuit_serialize(
    const ac_bool_circuit *circuit,
    uint8_t *output,
    size_t output_capacity,
    size_t *written_out)
{
    static const uint8_t magic[8] = {
        'A', 'C', 'B', 'C', 'I', 'R', '0', '1'
    };
    ac_bool_region regions[4];
    size_t required_size;
    size_t gate_storage_bytes;
    size_t serialized_gate_bytes;
    size_t output_bytes;
    uint32_t index;
    ac_status status;

    if (written_out == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (circuit == NULL) {
        regions[0].address = output;
        regions[0].length = output == NULL ? 0U : output_capacity;
        regions[1].address = written_out;
        regions[1].length = sizeof(*written_out);
        status = ac_bool_require_disjoint(regions, 2U);
        if (status != AC_OK) {
            return status;
        }
        *written_out = 0U;
        return AC_ERR_ARGUMENT;
    }
    status = ac_bool_size_multiply(
        (size_t)circuit->gate_count,
        sizeof(ac_bool_gate),
        &gate_storage_bytes);
    if (status != AC_OK) {
        return status;
    }
    status = ac_bool_size_multiply(
        (size_t)circuit->gate_count,
        (size_t)AC_BOOL_CIRCUIT_GATE_BYTES,
        &serialized_gate_bytes);
    if (status != AC_OK) {
        return status;
    }
    if (serialized_gate_bytes
        > SIZE_MAX - (size_t)AC_BOOL_CIRCUIT_HEADER_BYTES) {
        return AC_ERR_LENGTH;
    }
    required_size = (size_t)AC_BOOL_CIRCUIT_HEADER_BYTES
        + serialized_gate_bytes;
    output_bytes = output == NULL
        ? 0U
        : (output_capacity < required_size
            ? output_capacity
            : required_size);
    regions[0].address = circuit;
    regions[0].length = sizeof(*circuit);
    regions[1].address = circuit->gates;
    regions[1].length = gate_storage_bytes;
    regions[2].address = output;
    regions[2].length = output_bytes;
    regions[3].address = written_out;
    regions[3].length = sizeof(*written_out);
    status = ac_bool_require_disjoint(regions, 4U);
    if (status != AC_OK) {
        return status;
    }
    *written_out = 0U;
    if (output == NULL) {
        return AC_ERR_ARGUMENT;
    }
    status = ac_bool_circuit_serialized_size(circuit, &required_size);
    if (status != AC_OK) {
        return status;
    }
    if (output_capacity < required_size) {
        return AC_ERR_CAPACITY;
    }

    (void)memcpy(output, magic, sizeof(magic));
    ac_bool_store_u16be(
        &output[8],
        (uint16_t)AC_BOOL_CIRCUIT_FORMAT_VERSION);
    ac_bool_store_u16be(
        &output[10],
        (uint16_t)AC_BOOL_CIRCUIT_BASIS_XOR_AND);
    ac_bool_store_u32be(&output[12], circuit->public_input_bits);
    ac_bool_store_u32be(&output[16], circuit->witness_input_bits);
    ac_bool_store_u32be(&output[20], circuit->gate_count);
    ac_bool_store_u32be(&output[24], circuit->wire_count);
    ac_bool_store_u32be(&output[28], circuit->output_wire);
    for (index = 0U; index < circuit->gate_count; ++index) {
        size_t offset = (size_t)AC_BOOL_CIRCUIT_HEADER_BYTES
            + (size_t)index * (size_t)AC_BOOL_CIRCUIT_GATE_BYTES;

        output[offset] = circuit->gates[index].opcode;
        ac_bool_store_u32be(&output[offset + 1U], circuit->gates[index].left);
        ac_bool_store_u32be(&output[offset + 5U], circuit->gates[index].right);
    }
    *written_out = required_size;
    return AC_OK;
}
