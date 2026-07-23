#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ac/commitment.h"
#include "test_support.h"

static const uint8_t rps_protocol_id[AC_COMMITMENT_PROTOCOL_ID_BYTES] = {
    'A', 'C', '-', 'R', 'P', 'S', 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U
};

static const uint8_t vector_digest[AC_COMMITMENT_DIGEST_BYTES] = {
    0xd5U, 0x33U, 0xa9U, 0xe7U, 0x05U, 0x1dU, 0x5aU, 0xe4U,
    0xb8U, 0x7dU, 0xd3U, 0xaaU, 0x80U, 0x68U, 0xdcU, 0x15U,
    0x72U, 0xedU, 0xd6U, 0x21U, 0xb3U, 0xe0U, 0xbfU, 0x4eU,
    0x12U, 0x26U, 0xb3U, 0x7cU, 0x42U, 0x03U, 0xcaU, 0x5dU
};

static void fill_sequence(uint8_t *out, size_t length, uint8_t first)
{
    size_t index;

    for (index = 0U; index < length; ++index) {
        out[index] = (uint8_t)(first + (uint8_t)index);
    }
}

static void make_vector_context(ac_commitment_context *context)
{
    (void)memset(context, 0, sizeof *context);
    (void)memcpy(
        context->protocol_id,
        rps_protocol_id,
        sizeof context->protocol_id);
    context->protocol_version = 1U;
    fill_sequence(context->session_id, sizeof context->session_id, 0U);
    context->round = 1U;
    context->committer_role = 1U;
    context->recipient_role = 2U;
    context->payload_type = 1U;
}

static void make_vector_nonce(ac_commitment_nonce *nonce)
{
    fill_sequence(nonce->bytes, sizeof nonce->bytes, 0xa0U);
}

static int test_published_vector(void)
{
    const uint8_t payload[1] = {0U};
    ac_commitment_context context;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;

    make_vector_context(&context);
    make_vector_nonce(&nonce);
    (void)memset(&digest, 0, sizeof digest);

    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            payload,
            sizeof payload,
            &nonce,
            &digest),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(digest.bytes, vector_digest, sizeof vector_digest);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_verify(
            &context,
            payload,
            sizeof payload,
            &nonce,
            &digest),
        AC_OK);
    return 0;
}

static int test_deterministic_and_distinct_nonces(void)
{
    const uint8_t payload[1] = {2U};
    ac_commitment_context context;
    ac_commitment_nonce first_nonce;
    ac_commitment_nonce second_nonce;
    ac_commitment_digest first_digest;
    ac_commitment_digest repeated_digest;
    ac_commitment_digest second_digest;

    make_vector_context(&context);
    make_vector_nonce(&first_nonce);
    second_nonce = first_nonce;
    second_nonce.bytes[0] ^= 0x80U;

    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            payload,
            sizeof payload,
            &first_nonce,
            &first_digest),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            payload,
            sizeof payload,
            &first_nonce,
            &repeated_digest),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(
        first_digest.bytes,
        repeated_digest.bytes,
        sizeof first_digest.bytes);

    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            payload,
            sizeof payload,
            &second_nonce,
            &second_digest),
        AC_OK);
    AC_TEST_ASSERT(
        memcmp(
            first_digest.bytes,
            second_digest.bytes,
            sizeof first_digest.bytes) != 0);
    return 0;
}

static int expect_context_mutation_rejected(
    const ac_commitment_context *mutated,
    const ac_commitment_nonce *nonce,
    const ac_commitment_digest *digest)
{
    const uint8_t payload[1] = {0U};

    AC_TEST_ASSERT_STATUS(
        ac_commitment_verify(
            mutated,
            payload,
            sizeof payload,
            nonce,
            digest),
        AC_ERR_INVALID_OPENING);
    return 0;
}

static int test_every_context_field_is_bound(void)
{
    const uint8_t payload[1] = {0U};
    ac_commitment_context context;
    ac_commitment_context changed;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;

    make_vector_context(&context);
    make_vector_nonce(&nonce);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            payload,
            sizeof payload,
            &nonce,
            &digest),
        AC_OK);

    changed = context;
    changed.protocol_id[0] ^= 1U;
    AC_TEST_ASSERT_EQ_INT(
        expect_context_mutation_rejected(&changed, &nonce, &digest), 0);
    changed = context;
    changed.protocol_version += 1U;
    AC_TEST_ASSERT_EQ_INT(
        expect_context_mutation_rejected(&changed, &nonce, &digest), 0);
    changed = context;
    changed.session_id[31] ^= 1U;
    AC_TEST_ASSERT_EQ_INT(
        expect_context_mutation_rejected(&changed, &nonce, &digest), 0);
    changed = context;
    changed.round += 1U;
    AC_TEST_ASSERT_EQ_INT(
        expect_context_mutation_rejected(&changed, &nonce, &digest), 0);
    changed = context;
    changed.committer_role ^= 3U;
    AC_TEST_ASSERT_EQ_INT(
        expect_context_mutation_rejected(&changed, &nonce, &digest), 0);
    changed = context;
    changed.recipient_role ^= 3U;
    AC_TEST_ASSERT_EQ_INT(
        expect_context_mutation_rejected(&changed, &nonce, &digest), 0);
    changed = context;
    changed.payload_type += 1U;
    AC_TEST_ASSERT_EQ_INT(
        expect_context_mutation_rejected(&changed, &nonce, &digest), 0);
    return 0;
}

static int test_opening_mutations_are_rejected(void)
{
    uint8_t payload[1] = {0U};
    ac_commitment_context context;
    ac_commitment_nonce nonce;
    ac_commitment_nonce changed_nonce;
    ac_commitment_digest digest;
    ac_commitment_digest changed_digest;

    make_vector_context(&context);
    make_vector_nonce(&nonce);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            payload,
            sizeof payload,
            &nonce,
            &digest),
        AC_OK);

    payload[0] = 1U;
    AC_TEST_ASSERT_STATUS(
        ac_commitment_verify(
            &context,
            payload,
            sizeof payload,
            &nonce,
            &digest),
        AC_ERR_INVALID_OPENING);
    payload[0] = 0U;

    changed_nonce = nonce;
    changed_nonce.bytes[17] ^= 1U;
    AC_TEST_ASSERT_STATUS(
        ac_commitment_verify(
            &context,
            payload,
            sizeof payload,
            &changed_nonce,
            &digest),
        AC_ERR_INVALID_OPENING);

    changed_digest = digest;
    changed_digest.bytes[9] ^= 1U;
    AC_TEST_ASSERT_STATUS(
        ac_commitment_verify(
            &context,
            payload,
            sizeof payload,
            &nonce,
            &changed_digest),
        AC_ERR_INVALID_OPENING);
    return 0;
}

static int test_empty_payload_policy(void)
{
    ac_commitment_context context;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;

    make_vector_context(&context);
    make_vector_nonce(&nonce);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            NULL,
            0U,
            &nonce,
            &digest),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_verify(&context, NULL, 0U, &nonce, &digest),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            NULL,
            1U,
            &nonce,
            &digest),
        AC_ERR_ARGUMENT);
    return 0;
}

static int test_randomized_create_and_cleanup(void)
{
    const uint8_t payload[1] = {1U};
    ac_commitment_context context;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;

    make_vector_context(&context);
    (void)memset(&nonce, 0, sizeof nonce);
    (void)memset(&digest, 0, sizeof digest);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_create(
            &context,
            payload,
            sizeof payload,
            &digest,
            &nonce),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_verify(
            &context,
            payload,
            sizeof payload,
            &nonce,
            &digest),
        AC_OK);

    ac_commitment_nonce_clear(&nonce);
    AC_TEST_ASSERT_ZERO(nonce.bytes, sizeof nonce.bytes);
    ac_commitment_nonce_clear(NULL);
    return 0;
}

static int test_argument_failures_and_output_clearing(void)
{
    const uint8_t payload[1] = {0U};
    ac_commitment_context context;
    ac_commitment_nonce nonce;
    ac_commitment_digest digest;

    make_vector_context(&context);
    make_vector_nonce(&nonce);

    (void)memset(&digest, 0xa5, sizeof digest);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            NULL,
            payload,
            sizeof payload,
            &nonce,
            &digest),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(digest.bytes, sizeof digest.bytes);

    (void)memset(&digest, 0xa5, sizeof digest);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            NULL,
            sizeof payload,
            &nonce,
            &digest),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(digest.bytes, sizeof digest.bytes);

    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            payload,
            sizeof payload,
            NULL,
            &digest),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_compute_with_nonce(
            &context,
            payload,
            sizeof payload,
            &nonce,
            NULL),
        AC_ERR_ARGUMENT);

    AC_TEST_ASSERT_STATUS(
        ac_commitment_verify(
            NULL,
            payload,
            sizeof payload,
            &nonce,
            &digest),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_verify(
            &context,
            payload,
            sizeof payload,
            NULL,
            &digest),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_verify(
            &context,
            payload,
            sizeof payload,
            &nonce,
            NULL),
        AC_ERR_ARGUMENT);

    (void)memset(&digest, 0xa5, sizeof digest);
    (void)memset(&nonce, 0x5a, sizeof nonce);
    AC_TEST_ASSERT_STATUS(
        ac_commitment_create(
            NULL,
            payload,
            sizeof payload,
            &digest,
            &nonce),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(digest.bytes, sizeof digest.bytes);
    AC_TEST_ASSERT_ZERO(nonce.bytes, sizeof nonce.bytes);
    return 0;
}

int main(void)
{
    ac_test_suite suite = {0U, 0U};

    AC_TEST_RUN(suite, test_published_vector);
    AC_TEST_RUN(suite, test_deterministic_and_distinct_nonces);
    AC_TEST_RUN(suite, test_every_context_field_is_bound);
    AC_TEST_RUN(suite, test_opening_mutations_are_rejected);
    AC_TEST_RUN(suite, test_empty_payload_policy);
    AC_TEST_RUN(suite, test_randomized_create_and_cleanup);
    AC_TEST_RUN(suite, test_argument_failures_and_output_clearing);
    return ac_test_finish(&suite);
}
