#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ac/hash.h"

static void print_digest(const ac_hash_blake2b_256_digest *digest)
{
    size_t index;

    for (index = 0U; index < sizeof(digest->bytes); ++index) {
        (void)printf("%02x", (unsigned int)digest->bytes[index]);
    }
}

static size_t digest_bit_difference(
    const ac_hash_blake2b_256_digest *left,
    const ac_hash_blake2b_256_digest *right)
{
    size_t difference = 0U;
    size_t index;

    for (index = 0U; index < sizeof(left->bytes); ++index) {
        uint8_t value = (uint8_t)(left->bytes[index] ^ right->bytes[index]);

        while (value != 0U) {
            difference += (size_t)(value & 1U);
            value = (uint8_t)(value >> 1);
        }
    }
    return difference;
}

static int compute_or_report(
    const char *label,
    const uint8_t *input,
    size_t input_len,
    ac_hash_blake2b_256_digest *digest)
{
    ac_status status = ac_hash_blake2b_256(digest, input, input_len);

    if (status != AC_OK) {
        (void)fprintf(stderr, "%s failed with status %d\n", label, status);
        return 1;
    }
    return 0;
}

static int demonstrate_low_entropy_guessing(void)
{
    static const char *const candidates[] = {"rock", "paper", "scissors"};
    const uint8_t target[] = {'p', 'a', 'p', 'e', 'r'};
    ac_hash_blake2b_256_digest target_digest;
    ac_hash_blake2b_256_digest candidate_digest;
    size_t index;

    if (compute_or_report(
            "low-entropy target",
            target,
            sizeof(target),
            &target_digest) != 0) {
        return 1;
    }

    (void)printf("\nBare digest of a three-choice secret: ");
    print_digest(&target_digest);
    (void)printf("\nTrying the public choices: ");
    for (index = 0U; index < sizeof(candidates) / sizeof(candidates[0]); ++index) {
        const uint8_t *candidate = (const uint8_t *)candidates[index];
        size_t candidate_len = strlen(candidates[index]);

        if (compute_or_report(
                "candidate",
                candidate,
                candidate_len,
                &candidate_digest) != 0) {
            return 1;
        }
        if (memcmp(
                candidate_digest.bytes,
                target_digest.bytes,
                sizeof(target_digest.bytes)) == 0) {
            (void)printf("recovered '%s'\n", candidates[index]);
            return 0;
        }
    }

    (void)fprintf(stderr, "fixed dictionary example unexpectedly found no match\n");
    return 1;
}

int main(void)
{
    static const uint8_t abc[] = {'a', 'b', 'c'};
    static const uint8_t zero_message[32] = {0U};
    uint8_t changed_message[32] = {0U};
    ac_hash_blake2b_256_digest empty_digest;
    ac_hash_blake2b_256_digest abc_digest;
    ac_hash_blake2b_256_digest zero_digest;
    ac_hash_blake2b_256_digest changed_digest;

    changed_message[0] = 1U;

    if (compute_or_report("empty input", NULL, 0U, &empty_digest) != 0 ||
        compute_or_report("abc", abc, sizeof(abc), &abc_digest) != 0 ||
        compute_or_report(
            "all-zero message",
            zero_message,
            sizeof(zero_message),
            &zero_digest) != 0 ||
        compute_or_report(
            "one-bit-changed message",
            changed_message,
            sizeof(changed_message),
            &changed_digest) != 0) {
        return 1;
    }

    (void)printf(
        "EDUCATIONAL HASH DEMONSTRATION\n"
        "This program does not implement encryption, authentication,\n"
        "password storage, signatures, or a security proof.\n\n");

    (void)printf("BLAKE2b-256(empty) = ");
    print_digest(&empty_digest);
    (void)printf("\nBLAKE2b-256(abc)   = ");
    print_digest(&abc_digest);
    (void)printf("\n\nOne input bit differs between two 32-byte messages.\n");
    (void)printf("first digest  = ");
    print_digest(&zero_digest);
    (void)printf("\nsecond digest = ");
    print_digest(&changed_digest);
    (void)printf(
        "\nObserved differing digest bits: %zu of 256.\n"
        "This is one diffusion observation, not evidence of preimage or\n"
        "collision resistance.\n",
        digest_bit_difference(&zero_digest, &changed_digest));

    if (demonstrate_low_entropy_guessing() != 0) {
        return 1;
    }

    (void)printf(
        "\nStructured-input warning:\n"
        "  'ab' || 'c' and 'a' || 'bc' both produce the bytes 'abc'.\n"
        "  Equal raw bytes are the same hash input, not a hash collision.\n"
        "  Protocols must encode types, lengths, versions, and domains\n"
        "  unambiguously before hashing structured data.\n");
    return 0;
}
