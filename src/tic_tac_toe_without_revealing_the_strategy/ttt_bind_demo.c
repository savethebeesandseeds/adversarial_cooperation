#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sodium.h>

#include "ac/bool_circuit.h"
#include "ac/commitment.h"
#include "ac/ttt.h"
#include "ac/ttt_bind.h"
#include "ac/ttt_core_circuit.h"

static ac_bool_gate core_gates[AC_TTT_CORE_X_GATE_COUNT];
static ac_ttt_core_builder_scratch core_builder_scratch;
static uint8_t core_wire_scratch[AC_TTT_CORE_X_WIRE_COUNT];

static void print_bytes(const uint8_t *bytes, size_t length)
{
    size_t index;

    for (index = 0U; index < length; ++index) {
        (void)printf("%02x", (unsigned int)bytes[index]);
    }
    (void)putchar('\n');
}

static void fill_sequence(
    uint8_t *output,
    size_t length,
    uint8_t first)
{
    size_t index;

    for (index = 0U; index < length; ++index) {
        output[index] = (uint8_t)(first + (uint8_t)index);
    }
}

static int evaluate_public_layers(
    const ac_ttt_policy *policy,
    ac_ttt_report *recursive_report,
    ac_ttt_dag_report *dag_report,
    uint8_t *core_acceptance)
{
    ac_bool_circuit circuit;
    ac_ttt_core_circuit_report core_report;
    uint8_t public_input[AC_TTT_CORE_PUBLIC_INPUT_BYTES];

    *core_acceptance = 0U;
    if (ac_ttt_verify_policy(
            AC_TTT_ROLE_X, policy, recursive_report) != AC_OK ||
        recursive_report->verdict != AC_TTT_VERDICT_NON_LOSING ||
        ac_ttt_evaluate_policy_dag(
            AC_TTT_ROLE_X, policy, dag_report) != AC_OK ||
        dag_report->verdict != AC_TTT_VERDICT_NON_LOSING ||
        ac_ttt_core_encode_public_input(
            AC_TTT_ROLE_X, public_input) != AC_OK ||
        ac_ttt_core_circuit_build(
            AC_TTT_ROLE_X,
            core_gates,
            AC_TTT_CORE_X_GATE_COUNT,
            &core_builder_scratch,
            &circuit,
            &core_report) != AC_OK ||
        ac_bool_circuit_evaluate(
            &circuit,
            public_input,
            sizeof public_input,
            policy->move_by_board,
            sizeof policy->move_by_board,
            core_wire_scratch,
            sizeof core_wire_scratch,
            core_acceptance) != AC_OK ||
        *core_acceptance != 1U) {
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    const uint32_t reference_round = UINT32_C(0x11223344);
    uint32_t statement_round = 1U;
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES];
    ac_ttt_policy policy;
    ac_ttt_report recursive_report;
    ac_ttt_dag_report dag_report;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;
    ac_ttt_bind_circuit_report bind;
    ac_ttt_relation_circuit_report relation;
    uint8_t core_acceptance = 0U;
    int reference_mode = 0;
    int result = 1;

    if (argc == 2 && strcmp(argv[1], "--reference-vector") == 0) {
        reference_mode = 1;
        statement_round = reference_round;
    } else if (argc != 1) {
        (void)fprintf(
            stderr,
            "usage: %s [--reference-vector]\n",
            argv[0]);
        return 2;
    }

    (void)puts(
        "EDUCATIONAL MEASUREMENT ONLY -- NOT A PRIVATE PROOF OR "
        "PRODUCTION PROTOCOL");
    (void)puts(
        "The policy is present in this process. The commitment and tests "
        "do not establish zero knowledge.");

    if (sodium_init() < 0) {
        (void)fputs("libsodium initialization failed\n", stderr);
        return 1;
    }
    (void)memset(&policy, 0, sizeof policy);
    (void)memset(&nonce, 0, sizeof nonce);
    (void)memset(&digest, 0, sizeof digest);

    if (ac_ttt_build_reference_policy(AC_TTT_ROLE_X, &policy) != AC_OK ||
        evaluate_public_layers(
            &policy,
            &recursive_report,
            &dag_report,
            &core_acceptance) != 0) {
        (void)fputs("TTT public-layer demonstration failed\n", stderr);
        goto cleanup;
    }

    if (reference_mode != 0) {
        fill_sequence(session_id, sizeof session_id, 0x10U);
        fill_sequence(nonce.bytes, sizeof nonce.bytes, 0x80U);
        if (ac_ttt_bind_commitment_compute_with_nonce(
                AC_TTT_ROLE_X,
                session_id,
                statement_round,
                &policy,
                &nonce,
                &digest) != AC_OK) {
            (void)fputs("reference commitment computation failed\n", stderr);
            goto cleanup;
        }
    } else {
        randombytes_buf(session_id, sizeof session_id);
        if (ac_ttt_bind_commitment_create(
                AC_TTT_ROLE_X,
                session_id,
                statement_round,
                &policy,
                &digest,
                &nonce) != AC_OK) {
            (void)fputs("randomized commitment creation failed\n", stderr);
            goto cleanup;
        }
    }

    if (
        ac_ttt_bind_commitment_verify(
            AC_TTT_ROLE_X,
            session_id,
            statement_round,
            &policy,
            &nonce,
            &digest) != AC_OK ||
        ac_ttt_bind_circuit_measure(&bind) != AC_OK ||
        ac_ttt_relation_circuit_measure(
            AC_TTT_ROLE_X, &relation) != AC_OK) {
        (void)fputs("TTT Bind demonstration failed\n", stderr);
        goto cleanup;
    }

    if (reference_mode != 0) {
        (void)puts("mode: deterministic valid-policy reference vector");
        (void)fputs("session identifier: ", stdout);
        print_bytes(session_id, sizeof session_id);
        (void)printf(
            "statement round: 0x%08" PRIx32 "\n",
            statement_round);
        (void)fputs("opening nonce: ", stdout);
        print_bytes(nonce.bytes, sizeof nonce.bytes);
    } else {
        (void)puts("mode: randomized live teaching run");
    }

    (void)puts("recursive checker: non-losing");
    (void)puts("fixed-DAG checker: non-losing");
    (void)printf("Core-X circuit acceptance: %u\n", core_acceptance);
    (void)printf(
        "external profile: %u-byte framed message, %u BLAKE2b blocks\n",
        bind.commitment_message_bytes,
        bind.blake2b_block_count);
    (void)fputs("policy commitment digest: ", stdout);
    print_bytes(digest.bytes, sizeof digest.bytes);
    (void)puts("opening verification: accepted");
    (void)printf(
        "Bind model: %" PRIu32 " AND + %" PRIu32
        " XOR = %" PRIu32 " gates; depth %" PRIu32 "\n",
        bind.modeled_and_gate_count,
        bind.modeled_xor_gate_count,
        bind.modeled_gate_count,
        bind.modeled_output_depth);
    (void)printf(
        "Core-X AND Bind model: %" PRIu32
        " gates; serialization if emitted %zu bytes\n",
        relation.modeled_gate_count,
        relation.serialized_bytes_if_emitted);
    (void)puts(
        "No proof-internal commitment is selected or exposed by this API.");
    result = 0;

cleanup:
    ac_commitment_nonce_clear(&nonce);
    sodium_memzero(&policy, sizeof policy);
    sodium_memzero(session_id, sizeof session_id);
    return result;
}
