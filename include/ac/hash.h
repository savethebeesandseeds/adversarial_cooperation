#ifndef AC_HASH_H
#define AC_HASH_H

#include <stddef.h>
#include <stdint.h>

#include "ac/status.h"

#define AC_HASH_BLAKE2B_256_BYTES 32U

typedef struct {
    uint8_t bytes[AC_HASH_BLAKE2B_256_BYTES];
} ac_hash_blake2b_256_digest;

/*
 * Computes an unkeyed 32-byte BLAKE2b digest through libsodium.
 *
 * This function supplies a deterministic fingerprint only. It is not
 * encryption, a MAC, a signature, password hashing, or a commitment scheme.
 * A NULL input is accepted only when input_len is zero. The digest output must
 * not overlap the input. On every detected failure, a non-NULL output is
 * cleared before the function returns.
 */
ac_status ac_hash_blake2b_256(
    ac_hash_blake2b_256_digest *digest,
    const uint8_t *input,
    size_t input_len);

#endif
