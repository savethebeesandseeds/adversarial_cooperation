#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ac/hash.h"
#include "test_support.h"

static const uint8_t empty_digest[AC_HASH_BLAKE2B_256_BYTES] = {
    0x0eU, 0x57U, 0x51U, 0xc0U, 0x26U, 0xe5U, 0x43U, 0xb2U,
    0xe8U, 0xabU, 0x2eU, 0xb0U, 0x60U, 0x99U, 0xdaU, 0xa1U,
    0xd1U, 0xe5U, 0xdfU, 0x47U, 0x77U, 0x8fU, 0x77U, 0x87U,
    0xfaU, 0xabU, 0x45U, 0xcdU, 0xf1U, 0x2fU, 0xe3U, 0xa8U
};

static const uint8_t abc_digest[AC_HASH_BLAKE2B_256_BYTES] = {
    0xbdU, 0xddU, 0x81U, 0x3cU, 0x63U, 0x42U, 0x39U, 0x72U,
    0x31U, 0x71U, 0xefU, 0x3fU, 0xeeU, 0x98U, 0x57U, 0x9bU,
    0x94U, 0x96U, 0x4eU, 0x3bU, 0xb1U, 0xcbU, 0x3eU, 0x42U,
    0x72U, 0x62U, 0xc8U, 0xc0U, 0x68U, 0xd5U, 0x23U, 0x19U
};

static const uint8_t sequence_digest[AC_HASH_BLAKE2B_256_BYTES] = {
    0x39U, 0xa7U, 0xebU, 0x9fU, 0xedU, 0xc1U, 0x9aU, 0xabU,
    0xc8U, 0x34U, 0x25U, 0xc6U, 0x75U, 0x5dU, 0xd9U, 0x0eU,
    0x6fU, 0x9dU, 0x0cU, 0x80U, 0x49U, 0x64U, 0xa1U, 0xf4U,
    0xaaU, 0xeeU, 0xa3U, 0xb9U, 0xfbU, 0x59U, 0x98U, 0x35U
};

static const uint8_t zero_message_digest[AC_HASH_BLAKE2B_256_BYTES] = {
    0x89U, 0xebU, 0x0dU, 0x6aU, 0x8aU, 0x69U, 0x1dU, 0xaeU,
    0x2cU, 0xd1U, 0x5eU, 0xd0U, 0x36U, 0x99U, 0x31U, 0xceU,
    0x0aU, 0x94U, 0x9eU, 0xcaU, 0xfaU, 0x5cU, 0x3fU, 0x93U,
    0xf8U, 0x12U, 0x18U, 0x33U, 0x64U, 0x6eU, 0x15U, 0xc3U
};

static const uint8_t changed_message_digest[AC_HASH_BLAKE2B_256_BYTES] = {
    0xafU, 0xbcU, 0x1cU, 0x05U, 0x3cU, 0x2fU, 0x27U, 0x8eU,
    0x3cU, 0xbdU, 0x44U, 0x09U, 0xc1U, 0xc0U, 0x94U, 0xf1U,
    0x84U, 0xaaU, 0x45U, 0x9dU, 0xd2U, 0xf7U, 0xfcU, 0xa9U,
    0x6dU, 0x60U, 0x77U, 0x73U, 0x0aU, 0xb9U, 0xffU, 0xe3U
};

static size_t bit_difference(
    const uint8_t *left,
    const uint8_t *right,
    size_t length)
{
    size_t difference = 0U;
    size_t index;

    for (index = 0U; index < length; ++index) {
        uint8_t value = (uint8_t)(left[index] ^ right[index]);

        while (value != 0U) {
            difference += (size_t)(value & 1U);
            value = (uint8_t)(value >> 1);
        }
    }
    return difference;
}

static int test_published_vectors(void)
{
    static const uint8_t abc[] = {'a', 'b', 'c'};
    uint8_t sequence[256];
    ac_hash_blake2b_256_digest digest;
    size_t index;

    for (index = 0U; index < sizeof(sequence); ++index) {
        sequence[index] = (uint8_t)index;
    }

    AC_TEST_ASSERT_STATUS(ac_hash_blake2b_256(&digest, NULL, 0U), AC_OK);
    AC_TEST_ASSERT_MEMORY(digest.bytes, empty_digest, sizeof(empty_digest));
    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(&digest, abc, sizeof(abc)),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(digest.bytes, abc_digest, sizeof(abc_digest));
    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(&digest, sequence, sizeof(sequence)),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(
        digest.bytes,
        sequence_digest,
        sizeof(sequence_digest));
    return 0;
}

static int test_binary_length_is_part_of_the_input(void)
{
    static const uint8_t abc[] = {'a', 'b', 'c'};
    static const uint8_t abc_nul[] = {'a', 'b', 'c', 0U};
    ac_hash_blake2b_256_digest first;
    ac_hash_blake2b_256_digest second;

    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(&first, abc, sizeof(abc)),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(&second, abc_nul, sizeof(abc_nul)),
        AC_OK);
    AC_TEST_ASSERT(
        memcmp(first.bytes, second.bytes, sizeof(first.bytes)) != 0);
    return 0;
}

static int test_determinism(void)
{
    static const uint8_t message[] = {0U, 1U, 2U, 0U, 255U};
    ac_hash_blake2b_256_digest first;
    ac_hash_blake2b_256_digest second;

    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(&first, message, sizeof(message)),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(&second, message, sizeof(message)),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(first.bytes, second.bytes, sizeof(first.bytes));
    return 0;
}

static int test_single_bit_observation(void)
{
    static const uint8_t zero_message[32] = {0U};
    uint8_t changed_message[32] = {0U};
    ac_hash_blake2b_256_digest first;
    ac_hash_blake2b_256_digest second;

    changed_message[0] = 1U;
    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(&first, zero_message, sizeof(zero_message)),
        AC_OK);
    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(
            &second,
            changed_message,
            sizeof(changed_message)),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(
        first.bytes,
        zero_message_digest,
        sizeof(zero_message_digest));
    AC_TEST_ASSERT_MEMORY(
        second.bytes,
        changed_message_digest,
        sizeof(changed_message_digest));
    AC_TEST_ASSERT_EQ_INT(
        bit_difference(first.bytes, second.bytes, sizeof(first.bytes)),
        130U);
    return 0;
}

static int test_low_entropy_dictionary_recovery(void)
{
    static const char *const candidates[] = {"rock", "paper", "scissors"};
    static const uint8_t target[] = {'p', 'a', 'p', 'e', 'r'};
    ac_hash_blake2b_256_digest target_digest;
    ac_hash_blake2b_256_digest candidate_digest;
    size_t index;
    size_t match = sizeof(candidates) / sizeof(candidates[0]);

    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(&target_digest, target, sizeof(target)),
        AC_OK);
    for (index = 0U; index < sizeof(candidates) / sizeof(candidates[0]); ++index) {
        AC_TEST_ASSERT_STATUS(
            ac_hash_blake2b_256(
                &candidate_digest,
                (const uint8_t *)candidates[index],
                strlen(candidates[index])),
            AC_OK);
        if (memcmp(
                candidate_digest.bytes,
                target_digest.bytes,
                sizeof(target_digest.bytes)) == 0) {
            match = index;
            break;
        }
    }
    AC_TEST_ASSERT_EQ_INT(match, 1U);
    return 0;
}

static int test_argument_policy_and_output_clearing(void)
{
    static const uint8_t byte = 0U;
    ac_hash_blake2b_256_digest digest;

    (void)memset(&digest, 0xa5, sizeof(digest));
    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(&digest, NULL, 1U),
        AC_ERR_ARGUMENT);
    AC_TEST_ASSERT_ZERO(digest.bytes, sizeof(digest.bytes));

    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(NULL, &byte, 1U),
        AC_ERR_ARGUMENT);

    (void)memset(&digest, 0xa5, sizeof(digest));
    AC_TEST_ASSERT_STATUS(
        ac_hash_blake2b_256(&digest, NULL, 0U),
        AC_OK);
    AC_TEST_ASSERT_MEMORY(digest.bytes, empty_digest, sizeof(empty_digest));
    return 0;
}

int main(void)
{
    ac_test_suite suite = {0U, 0U};

    AC_TEST_RUN(suite, test_published_vectors);
    AC_TEST_RUN(suite, test_binary_length_is_part_of_the_input);
    AC_TEST_RUN(suite, test_determinism);
    AC_TEST_RUN(suite, test_single_bit_observation);
    AC_TEST_RUN(suite, test_low_entropy_dictionary_recovery);
    AC_TEST_RUN(suite, test_argument_policy_and_output_clearing);
    return ac_test_finish(&suite);
}
