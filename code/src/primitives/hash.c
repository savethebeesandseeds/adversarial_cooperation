#include "ac/hash.h"

#include <string.h>

#include <sodium.h>

ac_status ac_hash_blake2b_256(
    ac_hash_blake2b_256_digest *digest,
    const uint8_t *input,
    size_t input_len)
{
    static const uint8_t empty_input[1] = {0U};
    const uint8_t *source;
    unsigned long long sodium_length;

    if (digest != NULL) {
        (void)memset(digest, 0, sizeof(*digest));
    }
    if (digest == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (input == NULL && input_len != 0U) {
        return AC_ERR_ARGUMENT;
    }

    sodium_length = (unsigned long long)input_len;
    if ((size_t)sodium_length != input_len) {
        return AC_ERR_LENGTH;
    }
    if (sodium_init() < 0) {
        return AC_ERR_CRYPTO;
    }

    source = input != NULL ? input : empty_input;
    if (crypto_generichash_blake2b(
            digest->bytes,
            sizeof(digest->bytes),
            source,
            sodium_length,
            NULL,
            0U) != 0) {
        (void)memset(digest, 0, sizeof(*digest));
        return AC_ERR_CRYPTO;
    }
    return AC_OK;
}
