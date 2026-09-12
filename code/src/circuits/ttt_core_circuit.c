#include "ac/ttt_core_circuit.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum {
    AC_TTT_CORE_KIND_UNREACHABLE = 0,
    AC_TTT_CORE_KIND_TERMINAL = 1,
    AC_TTT_CORE_KIND_X_TO_MOVE = 2,
    AC_TTT_CORE_KIND_O_TO_MOVE = 3
};

enum {
    AC_TTT_CORE_CONSTANT_WIRES = 2U,
    AC_TTT_CORE_PUBLIC_FIRST_WIRE = 2U,
    AC_TTT_CORE_WITNESS_FIRST_WIRE = 34U,
    AC_TTT_CORE_FIRST_GATE_WIRE = 157498U
};

typedef struct {
    uint32_t wire;
    uint32_t depth;
} ac_ttt_core_signal;

typedef struct {
    ac_bool_gate *gates;
    size_t capacity;
    uint32_t count;
    uint32_t and_count;
    uint32_t xor_count;
    ac_status status;
} ac_ttt_core_emitter;

static const uint32_t ac_ttt_core_powers_of_three[AC_TTT_BOARD_CELLS] = {
    1U, 3U, 9U, 27U, 81U, 243U, 729U, 2187U, 6561U
};

static const uint8_t ac_ttt_core_winning_lines[8][3] = {
    {0U, 1U, 2U},
    {3U, 4U, 5U},
    {6U, 7U, 8U},
    {0U, 3U, 6U},
    {1U, 4U, 7U},
    {2U, 5U, 8U},
    {0U, 4U, 8U},
    {2U, 4U, 6U}
};

static int ac_ttt_core_role_is_valid(ac_ttt_role role)
{
    return role == AC_TTT_ROLE_X || role == AC_TTT_ROLE_O;
}

static ac_ttt_role ac_ttt_core_other_role(ac_ttt_role role)
{
    return role == AC_TTT_ROLE_X ? AC_TTT_ROLE_O : AC_TTT_ROLE_X;
}

static void ac_ttt_core_clear_circuit(ac_bool_circuit *circuit)
{
    if (circuit != NULL) {
        (void)memset(circuit, 0, sizeof(*circuit));
    }
}

static void ac_ttt_core_clear_report(ac_ttt_core_circuit_report *report)
{
    if (report != NULL) {
        (void)memset(report, 0, sizeof(*report));
    }
}

static void ac_ttt_core_fill_requirements(
    ac_ttt_role role,
    ac_ttt_core_circuit_report *report)
{
    report->specialization_role = role;
    report->public_input_bits = AC_TTT_CORE_PUBLIC_INPUT_BITS;
    report->witness_input_bits = AC_TTT_CORE_WITNESS_INPUT_BITS;
    report->builder_scratch_bytes = sizeof(ac_ttt_core_builder_scratch);
    if (role == AC_TTT_ROLE_X) {
        report->required_policy_state_count = 2423U;
        report->selector_equality_count = 8631U;
        report->and_gate_count = AC_TTT_CORE_X_AND_GATE_COUNT;
        report->xor_gate_count = AC_TTT_CORE_X_XOR_GATE_COUNT;
        report->gate_count = AC_TTT_CORE_X_GATE_COUNT;
        report->wire_count = AC_TTT_CORE_X_WIRE_COUNT;
        report->output_depth = AC_TTT_CORE_X_OUTPUT_DEPTH;
        report->serialized_bytes = AC_TTT_CORE_X_SERIALIZED_BYTES;
    } else {
        report->required_policy_state_count = 2097U;
        report->selector_equality_count = 7536U;
        report->and_gate_count = AC_TTT_CORE_O_AND_GATE_COUNT;
        report->xor_gate_count = AC_TTT_CORE_O_XOR_GATE_COUNT;
        report->gate_count = AC_TTT_CORE_O_GATE_COUNT;
        report->wire_count = AC_TTT_CORE_O_WIRE_COUNT;
        report->output_depth = AC_TTT_CORE_O_OUTPUT_DEPTH;
        report->serialized_bytes = AC_TTT_CORE_O_SERIALIZED_BYTES;
    }
    report->gate_storage_bytes =
        (size_t)report->gate_count * sizeof(ac_bool_gate);
    report->evaluator_scratch_bytes = (size_t)report->wire_count;
}

static int ac_ttt_core_ranges_overlap(
    const void *first,
    size_t first_size,
    const void *second,
    size_t second_size)
{
    uintptr_t first_address;
    uintptr_t second_address;

    if (first == NULL || second == NULL || first_size == 0U ||
        second_size == 0U) {
        return 0;
    }
    first_address = (uintptr_t)first;
    second_address = (uintptr_t)second;
    if (first_address <= second_address) {
        return second_address - first_address < first_size;
    }
    return first_address - second_address < second_size;
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

static ac_ttt_role ac_ttt_core_board_winner(
    const uint8_t board[AC_TTT_BOARD_CELLS])
{
    size_t line;

    for (line = 0U; line < 8U; ++line) {
        uint8_t first = board[ac_ttt_core_winning_lines[line][0]];

        if (first != 0U &&
            board[ac_ttt_core_winning_lines[line][1]] == first &&
            board[ac_ttt_core_winning_lines[line][2]] == first) {
            return (ac_ttt_role)first;
        }
    }
    return (ac_ttt_role)0;
}

static int ac_ttt_core_board_is_full(
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

static ac_ttt_role ac_ttt_core_board_turn(
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

static void ac_ttt_core_build_classification(
    ac_ttt_core_builder_scratch *scratch)
{
    uint32_t index;

    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        scratch->kind_by_board[index] = AC_TTT_CORE_KIND_UNREACHABLE;
    }
    scratch->kind_by_board[0] = AC_TTT_CORE_KIND_X_TO_MOVE;

    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        ac_ttt_role turn;
        size_t cell;

        if (scratch->kind_by_board[index] == AC_TTT_CORE_KIND_UNREACHABLE) {
            continue;
        }
        ac_ttt_core_decode_board(index, scratch->board);
        if (ac_ttt_core_board_winner(scratch->board) != (ac_ttt_role)0 ||
            ac_ttt_core_board_is_full(scratch->board)) {
            scratch->kind_by_board[index] = AC_TTT_CORE_KIND_TERMINAL;
            continue;
        }

        turn = ac_ttt_core_board_turn(scratch->board);
        scratch->kind_by_board[index] =
            turn == AC_TTT_ROLE_X
                ? AC_TTT_CORE_KIND_X_TO_MOVE
                : AC_TTT_CORE_KIND_O_TO_MOVE;
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            uint32_t child_index;

            if (scratch->board[cell] != 0U) {
                continue;
            }
            child_index = index +
                          (uint32_t)turn * ac_ttt_core_powers_of_three[cell];
            if (scratch->kind_by_board[child_index] ==
                AC_TTT_CORE_KIND_UNREACHABLE) {
                scratch->kind_by_board[child_index] =
                    ac_ttt_core_other_role(turn) == AC_TTT_ROLE_X
                        ? AC_TTT_CORE_KIND_X_TO_MOVE
                        : AC_TTT_CORE_KIND_O_TO_MOVE;
            }
        }
    }
}

static ac_ttt_core_signal ac_ttt_core_emit_gate(
    ac_ttt_core_emitter *emitter,
    uint8_t opcode,
    ac_ttt_core_signal left,
    ac_ttt_core_signal right)
{
    ac_ttt_core_signal output = {0U, 0U};
    uint32_t output_wire;

    if (emitter->status != AC_OK) {
        return output;
    }
    if ((size_t)emitter->count >= emitter->capacity) {
        emitter->status = AC_ERR_CAPACITY;
        return output;
    }
    output_wire = AC_TTT_CORE_FIRST_GATE_WIRE + emitter->count;
    if (left.wire >= output_wire || right.wire >= output_wire ||
        (opcode != AC_BOOL_GATE_XOR && opcode != AC_BOOL_GATE_AND)) {
        emitter->status = AC_ERR_STATE;
        return output;
    }

    emitter->gates[emitter->count].left = left.wire;
    emitter->gates[emitter->count].right = right.wire;
    emitter->gates[emitter->count].opcode = opcode;
    emitter->gates[emitter->count].reserved[0] = 0U;
    emitter->gates[emitter->count].reserved[1] = 0U;
    emitter->gates[emitter->count].reserved[2] = 0U;
    ++emitter->count;
    if (opcode == AC_BOOL_GATE_XOR) {
        ++emitter->xor_count;
    } else {
        ++emitter->and_count;
    }

    output.wire = output_wire;
    output.depth =
        (left.depth > right.depth ? left.depth : right.depth) + 1U;
    return output;
}

static ac_ttt_core_signal ac_ttt_core_fold(
    ac_ttt_core_emitter *emitter,
    ac_ttt_core_signal *signals,
    size_t signal_count,
    uint8_t opcode)
{
    while (signal_count > 1U && emitter->status == AC_OK) {
        size_t read_index;
        size_t write_index = 0U;

        for (read_index = 0U; read_index + 1U < signal_count;
             read_index += 2U) {
            signals[write_index] = ac_ttt_core_emit_gate(
                emitter,
                opcode,
                signals[read_index],
                signals[read_index + 1U]);
            ++write_index;
        }
        if (read_index < signal_count) {
            signals[write_index] = signals[read_index];
            ++write_index;
        }
        signal_count = write_index;
    }
    if (signal_count == 0U || emitter->status != AC_OK) {
        ac_ttt_core_signal failed = {0U, 0U};
        return failed;
    }
    return signals[0];
}

static ac_ttt_core_signal ac_ttt_core_equal_byte(
    ac_ttt_core_emitter *emitter,
    uint32_t first_bit_wire,
    uint8_t constant)
{
    ac_ttt_core_signal literals[8];
    size_t bit;

    for (bit = 0U; bit < 8U; ++bit) {
        ac_ttt_core_signal input = {
            first_bit_wire + (uint32_t)bit,
            0U
        };

        if (((constant >> bit) & 1U) != 0U) {
            literals[bit] = input;
        } else {
            ac_ttt_core_signal one = {1U, 0U};
            literals[bit] = ac_ttt_core_emit_gate(
                emitter, AC_BOOL_GATE_XOR, input, one);
        }
    }
    return ac_ttt_core_fold(
        emitter, literals, 8U, AC_BOOL_GATE_AND);
}

static ac_ttt_core_signal ac_ttt_core_build_public_check(
    ac_ttt_role role,
    ac_ttt_core_emitter *emitter)
{
    ac_ttt_core_signal equalities[4];

    equalities[0] = ac_ttt_core_equal_byte(
        emitter, AC_TTT_CORE_PUBLIC_FIRST_WIRE, AC_TTT_GAME_VERSION);
    equalities[1] = ac_ttt_core_equal_byte(
        emitter,
        AC_TTT_CORE_PUBLIC_FIRST_WIRE + 8U,
        (uint8_t)role);
    equalities[2] = ac_ttt_core_equal_byte(
        emitter, AC_TTT_CORE_PUBLIC_FIRST_WIRE + 16U, 0U);
    equalities[3] = ac_ttt_core_equal_byte(
        emitter, AC_TTT_CORE_PUBLIC_FIRST_WIRE + 24U, 0U);
    return ac_ttt_core_fold(
        emitter, equalities, 4U, AC_BOOL_GATE_AND);
}

static ac_ttt_core_signal ac_ttt_core_policy_byte_is_ff(
    uint32_t board_index,
    ac_ttt_core_emitter *emitter)
{
    ac_ttt_core_signal literals[8];
    uint32_t first_wire = AC_TTT_CORE_WITNESS_FIRST_WIRE +
                          board_index * 8U;
    size_t bit;

    for (bit = 0U; bit < 8U; ++bit) {
        literals[bit].wire = first_wire + (uint32_t)bit;
        literals[bit].depth = 0U;
    }
    return ac_ttt_core_fold(
        emitter, literals, 8U, AC_BOOL_GATE_AND);
}

static ac_ttt_core_signal ac_ttt_core_build_required_policy_entry(
    uint32_t board_index,
    ac_ttt_core_emitter *emitter,
    ac_ttt_core_builder_scratch *scratch,
    uint32_t *selector_equality_count)
{
    ac_ttt_core_signal complements[8];
    ac_ttt_core_signal equalities[AC_TTT_BOARD_CELLS];
    uint32_t first_wire = AC_TTT_CORE_WITNESS_FIRST_WIRE +
                          board_index * 8U;
    size_t equality_count = 0U;
    size_t bit;
    size_t cell;

    ac_ttt_core_decode_board(board_index, scratch->board);
    for (bit = 0U; bit < 8U; ++bit) {
        int complement_is_needed = 0;

        complements[bit].wire = 0U;
        complements[bit].depth = 0U;
        for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
            if (scratch->board[cell] == 0U &&
                ((((uint8_t)cell >> bit) & 1U) == 0U)) {
                complement_is_needed = 1;
                break;
            }
        }
        if (complement_is_needed != 0) {
            ac_ttt_core_signal input = {
                first_wire + (uint32_t)bit,
                0U
            };
            ac_ttt_core_signal one = {1U, 0U};
            complements[bit] = ac_ttt_core_emit_gate(
                emitter, AC_BOOL_GATE_XOR, input, one);
        }
    }

    for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
        ac_ttt_core_signal literals[8];

        if (scratch->board[cell] != 0U) {
            continue;
        }
        for (bit = 0U; bit < 8U; ++bit) {
            if ((((uint8_t)cell >> bit) & 1U) != 0U) {
                literals[bit].wire = first_wire + (uint32_t)bit;
                literals[bit].depth = 0U;
            } else {
                literals[bit] = complements[bit];
            }
        }
        equalities[equality_count] = ac_ttt_core_fold(
            emitter, literals, 8U, AC_BOOL_GATE_AND);
        if (equality_count == 0U) {
            scratch->first_selector_wire_by_board[board_index] =
                equalities[equality_count].wire;
        }
        ++equality_count;
        ++*selector_equality_count;
    }
    return ac_ttt_core_fold(
        emitter, equalities, equality_count, AC_BOOL_GATE_XOR);
}

static ac_ttt_core_signal ac_ttt_core_build_policy_validity(
    ac_ttt_role role,
    ac_ttt_core_emitter *emitter,
    ac_ttt_core_builder_scratch *scratch,
    uint32_t *required_state_count,
    uint32_t *selector_equality_count)
{
    uint8_t required_kind = role == AC_TTT_ROLE_X
                                ? AC_TTT_CORE_KIND_X_TO_MOVE
                                : AC_TTT_CORE_KIND_O_TO_MOVE;
    uint32_t index;

    *required_state_count = 0U;
    *selector_equality_count = 0U;
    for (index = 0U; index < AC_TTT_STATE_COUNT; ++index) {
        ac_ttt_core_signal validity;

        scratch->first_selector_wire_by_board[index] = 0U;
        if (scratch->kind_by_board[index] == required_kind) {
            validity = ac_ttt_core_build_required_policy_entry(
                index, emitter, scratch, selector_equality_count);
            ++*required_state_count;
        } else {
            validity = ac_ttt_core_policy_byte_is_ff(index, emitter);
        }
        scratch->signal_wire_by_board[index] = validity.wire;
        scratch->signal_depth_by_board[index] = validity.depth;
    }

    {
        size_t count = AC_TTT_STATE_COUNT;

        while (count > 1U && emitter->status == AC_OK) {
            size_t read_index;
            size_t write_index = 0U;

            for (read_index = 0U; read_index + 1U < count;
                 read_index += 2U) {
                ac_ttt_core_signal left = {
                    scratch->signal_wire_by_board[read_index],
                    scratch->signal_depth_by_board[read_index]
                };
                ac_ttt_core_signal right = {
                    scratch->signal_wire_by_board[read_index + 1U],
                    scratch->signal_depth_by_board[read_index + 1U]
                };
                ac_ttt_core_signal folded = ac_ttt_core_emit_gate(
                    emitter, AC_BOOL_GATE_AND, left, right);

                scratch->signal_wire_by_board[write_index] = folded.wire;
                scratch->signal_depth_by_board[write_index] = folded.depth;
                ++write_index;
            }
            if (read_index < count) {
                scratch->signal_wire_by_board[write_index] =
                    scratch->signal_wire_by_board[read_index];
                scratch->signal_depth_by_board[write_index] =
                    scratch->signal_depth_by_board[read_index];
                ++write_index;
            }
            count = write_index;
        }
    }

    {
        ac_ttt_core_signal result = {
            scratch->signal_wire_by_board[0],
            scratch->signal_depth_by_board[0]
        };
        return result;
    }
}

static ac_ttt_core_signal ac_ttt_core_build_recurrence(
    ac_ttt_role role,
    ac_ttt_core_emitter *emitter,
    ac_ttt_core_builder_scratch *scratch)
{
    uint8_t claimant_kind = role == AC_TTT_ROLE_X
                                ? AC_TTT_CORE_KIND_X_TO_MOVE
                                : AC_TTT_CORE_KIND_O_TO_MOVE;
    uint32_t reverse_index;

    for (reverse_index = AC_TTT_STATE_COUNT; reverse_index > 0U;
         --reverse_index) {
        uint32_t index = reverse_index - 1U;
        ac_ttt_core_signal value = {0U, 0U};
        uint8_t kind = scratch->kind_by_board[index];

        if (kind == AC_TTT_CORE_KIND_UNREACHABLE) {
            value.wire = 0U;
        } else if (kind == AC_TTT_CORE_KIND_TERMINAL) {
            ac_ttt_role winner;

            ac_ttt_core_decode_board(index, scratch->board);
            winner = ac_ttt_core_board_winner(scratch->board);
            value.wire =
                winner == ac_ttt_core_other_role(role) ? 0U : 1U;
        } else {
            ac_ttt_core_signal children[AC_TTT_BOARD_CELLS];
            size_t child_count = 0U;
            size_t cell;

            ac_ttt_core_decode_board(index, scratch->board);
            for (cell = 0U; cell < AC_TTT_BOARD_CELLS; ++cell) {
                uint32_t child_index;
                ac_ttt_core_signal child;

                if (scratch->board[cell] != 0U) {
                    continue;
                }
                child_index = index +
                              (uint32_t)(kind == AC_TTT_CORE_KIND_X_TO_MOVE
                                             ? AC_TTT_ROLE_X
                                             : AC_TTT_ROLE_O) *
                                  ac_ttt_core_powers_of_three[cell];
                child.wire = scratch->signal_wire_by_board[child_index];
                child.depth = scratch->signal_depth_by_board[child_index];
                if (kind == claimant_kind) {
                    ac_ttt_core_signal selector = {
                        scratch->first_selector_wire_by_board[index] +
                            (uint32_t)(child_count * 7U),
                        4U
                    };
                    children[child_count] = ac_ttt_core_emit_gate(
                        emitter, AC_BOOL_GATE_AND, selector, child);
                } else {
                    children[child_count] = child;
                }
                ++child_count;
            }
            value = ac_ttt_core_fold(
                emitter,
                children,
                child_count,
                kind == claimant_kind
                    ? AC_BOOL_GATE_XOR
                    : AC_BOOL_GATE_AND);
        }

        scratch->signal_wire_by_board[index] = value.wire;
        scratch->signal_depth_by_board[index] = value.depth;
    }

    {
        ac_ttt_core_signal root = {
            scratch->signal_wire_by_board[0],
            scratch->signal_depth_by_board[0]
        };
        return root;
    }
}

ac_status ac_ttt_core_encode_public_input(
    ac_ttt_role role,
    uint8_t output[AC_TTT_CORE_PUBLIC_INPUT_BYTES])
{
    size_t index;

    if (output != NULL) {
        for (index = 0U; index < AC_TTT_CORE_PUBLIC_INPUT_BYTES; ++index) {
            output[index] = 0U;
        }
    }
    if (output == NULL || !ac_ttt_core_role_is_valid(role)) {
        return AC_ERR_ARGUMENT;
    }
    output[0] = AC_TTT_GAME_VERSION;
    output[1] = (uint8_t)role;
    return AC_OK;
}

ac_status ac_ttt_core_circuit_requirements(
    ac_ttt_role role,
    ac_ttt_core_circuit_report *report)
{
    ac_ttt_core_clear_report(report);
    if (report == NULL || !ac_ttt_core_role_is_valid(role)) {
        return AC_ERR_ARGUMENT;
    }
    ac_ttt_core_fill_requirements(role, report);
    return AC_OK;
}

ac_status ac_ttt_core_circuit_build(
    ac_ttt_role role,
    ac_bool_gate *gates,
    size_t gate_capacity,
    ac_ttt_core_builder_scratch *scratch,
    ac_bool_circuit *circuit,
    ac_ttt_core_circuit_report *report)
{
    ac_ttt_core_circuit_report required;
    ac_ttt_core_emitter emitter;
    ac_ttt_core_signal public_check;
    ac_ttt_core_signal policy_validity;
    ac_ttt_core_signal root_value;
    ac_ttt_core_signal final_inputs[2];
    ac_ttt_core_signal shallow_pair;
    ac_ttt_core_signal acceptance;
    uint32_t required_state_count;
    uint32_t selector_equality_count;
    ac_status status;

    ac_ttt_core_clear_circuit(circuit);
    ac_ttt_core_clear_report(report);
    if (!ac_ttt_core_role_is_valid(role) || gates == NULL || scratch == NULL ||
        circuit == NULL || report == NULL) {
        return AC_ERR_ARGUMENT;
    }
    ac_ttt_core_fill_requirements(role, &required);
    *report = required;
    if (gate_capacity < (size_t)required.gate_count) {
        return AC_ERR_CAPACITY;
    }
    if (ac_ttt_core_ranges_overlap(
            gates, required.gate_storage_bytes, scratch, sizeof(*scratch)) ||
        ac_ttt_core_ranges_overlap(
            gates, required.gate_storage_bytes, circuit, sizeof(*circuit)) ||
        ac_ttt_core_ranges_overlap(
            gates, required.gate_storage_bytes, report, sizeof(*report)) ||
        ac_ttt_core_ranges_overlap(
            scratch, sizeof(*scratch), circuit, sizeof(*circuit)) ||
        ac_ttt_core_ranges_overlap(
            scratch, sizeof(*scratch), report, sizeof(*report)) ||
        ac_ttt_core_ranges_overlap(
            circuit, sizeof(*circuit), report, sizeof(*report))) {
        return AC_ERR_ARGUMENT;
    }

    emitter.gates = gates;
    emitter.capacity = gate_capacity;
    emitter.count = 0U;
    emitter.and_count = 0U;
    emitter.xor_count = 0U;
    emitter.status = AC_OK;

    ac_ttt_core_build_classification(scratch);
    public_check = ac_ttt_core_build_public_check(role, &emitter);
    policy_validity = ac_ttt_core_build_policy_validity(
        role,
        &emitter,
        scratch,
        &required_state_count,
        &selector_equality_count);
    root_value = ac_ttt_core_build_recurrence(role, &emitter, scratch);

    final_inputs[0] = public_check;
    final_inputs[1] = policy_validity;
    shallow_pair = ac_ttt_core_fold(
        &emitter, final_inputs, 2U, AC_BOOL_GATE_AND);
    acceptance = ac_ttt_core_emit_gate(
        &emitter, AC_BOOL_GATE_AND, shallow_pair, root_value);

    if (emitter.status != AC_OK) {
        ac_ttt_core_clear_circuit(circuit);
        return emitter.status;
    }
    if (required_state_count != required.required_policy_state_count ||
        selector_equality_count != required.selector_equality_count ||
        emitter.and_count != required.and_gate_count ||
        emitter.xor_count != required.xor_gate_count ||
        emitter.count != required.gate_count ||
        acceptance.wire + 1U != required.wire_count ||
        acceptance.depth != required.output_depth) {
        ac_ttt_core_clear_circuit(circuit);
        return AC_ERR_STATE;
    }

    status = ac_bool_circuit_init(
        circuit,
        AC_TTT_CORE_PUBLIC_INPUT_BITS,
        AC_TTT_CORE_WITNESS_INPUT_BITS,
        emitter.count,
        acceptance.wire,
        gates);
    if (status != AC_OK) {
        ac_ttt_core_clear_circuit(circuit);
        return status;
    }
    return AC_OK;
}
