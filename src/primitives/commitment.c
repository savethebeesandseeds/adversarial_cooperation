#include "ac/commitment.h"

#include <string.h>

#include <sodium.h>

static const uint8_t ac_commitment_domain[16] = {
    'A', 'C', '-', 'C', 'O', 'M', 'M', 'I',
    'T', 'M', 'E', 'N', 'T', '-', 'V', '1'
};

static void ac_write_u16be(uint8_t out[2], uint16_t value)
{
    out[0] = (uint8_t)(value >> 8);
    out[1] = (uint8_t)value;
}

static void ac_write_u32be(uint8_t out[4], uint32_t value)
{
    out[0] = (uint8_t)(value >> 24);
    out[1] = (uint8_t)(value >> 16);
    out[2] = (uint8_t)(value >> 8);
    out[3] = (uint8_t)value;
}

static void ac_write_u64be(uint8_t out[8], uint64_t value)
{
    out[0] = (uint8_t)(value >> 56);
    out[1] = (uint8_t)(value >> 48);
    out[2] = (uint8_t)(value >> 40);
    out[3] = (uint8_t)(value >> 32);
    out[4] = (uint8_t)(value >> 24);
    out[5] = (uint8_t)(value >> 16);
    out[6] = (uint8_t)(value >> 8);
    out[7] = (uint8_t)value;
}

static ac_status ac_hash_update(
    crypto_generichash_blake2b_state *state,
    const uint8_t *bytes,
    size_t length)
{
    if (length == 0U) {
        return AC_OK;
    }
    if (crypto_generichash_blake2b_update(
            state, bytes, (unsigned long long)length) != 0) {
        return AC_ERR_CRYPTO;
    }
    return AC_OK;
}

static ac_status ac_hash_u16(
    crypto_generichash_blake2b_state *state,
    uint16_t value)
{
    uint8_t encoded[2];

    ac_write_u16be(encoded, value);
    return ac_hash_update(state, encoded, sizeof(encoded));
}

static ac_status ac_hash_u32(
    crypto_generichash_blake2b_state *state,
    uint32_t value)
{
    uint8_t encoded[4];

    ac_write_u32be(encoded, value);
    return ac_hash_update(state, encoded, sizeof(encoded));
}

static ac_status ac_hash_u64(
    crypto_generichash_blake2b_state *state,
    uint64_t value)
{
    uint8_t encoded[8];

    ac_write_u64be(encoded, value);
    return ac_hash_update(state, encoded, sizeof(encoded));
}

static ac_status ac_commitment_validate_arguments(
    const ac_commitment_context *context,
    const uint8_t *payload,
    size_t payload_len,
    const ac_commitment_nonce *nonce,
    const ac_commitment_digest *digest)
{
    uint64_t encoded_length;

    if (context == NULL || nonce == NULL || digest == NULL) {
        return AC_ERR_ARGUMENT;
    }
    if (payload == NULL && payload_len != 0U) {
        return AC_ERR_ARGUMENT;
    }

    encoded_length = (uint64_t)payload_len;
    if ((size_t)encoded_length != payload_len) {
        return AC_ERR_LENGTH;
    }
    return AC_OK;
}

ac_status ac_commitment_compute_with_nonce(
    const ac_commitment_context *context,
    const uint8_t *payload,
    size_t payload_len,
    const ac_commitment_nonce *nonce,
    ac_commitment_digest *digest)
{
    crypto_generichash_blake2b_state state;
    uint8_t computed[AC_COMMITMENT_DIGEST_BYTES];
    ac_status status;

    if (digest != NULL) {
        memset(digest, 0, sizeof(*digest));
    }
    status = ac_commitment_validate_arguments(
        context, payload, payload_len, nonce, digest);
    if (status != AC_OK) {
        return status;
    }
    if (sodium_init() < 0) {
        return AC_ERR_CRYPTO;
    }

    memset(&state, 0, sizeof(state));
    memset(computed, 0, sizeof(computed));
    if (crypto_generichash_blake2b_init(
            &state, NULL, 0U, sizeof(computed)) != 0) {
        sodium_memzero(&state, sizeof(state));
        return AC_ERR_CRYPTO;
    }

#define AC_HASH_OR_FAIL(expression)               \
    do {                                           \
        status = (expression);                     \
        if (status != AC_OK) {                     \
            goto fail;                             \
        }                                          \
    } while (0)

    AC_HASH_OR_FAIL(ac_hash_update(
        &state, ac_commitment_domain, sizeof(ac_commitment_domain)));
    AC_HASH_OR_FAIL(ac_hash_u16(&state, AC_COMMITMENT_SCHEME_VERSION));
    AC_HASH_OR_FAIL(ac_hash_u16(
        &state, AC_COMMITMENT_PROTOCOL_ID_BYTES));
    AC_HASH_OR_FAIL(ac_hash_update(
        &state, context->protocol_id, sizeof(context->protocol_id)));
    AC_HASH_OR_FAIL(ac_hash_u16(&state, context->protocol_version));
    AC_HASH_OR_FAIL(ac_hash_u16(
        &state, AC_COMMITMENT_SESSION_ID_BYTES));
    AC_HASH_OR_FAIL(ac_hash_update(
        &state, context->session_id, sizeof(context->session_id)));
    AC_HASH_OR_FAIL(ac_hash_u32(&state, context->round));
    AC_HASH_OR_FAIL(ac_hash_update(
        &state, &context->committer_role, sizeof(context->committer_role)));
    AC_HASH_OR_FAIL(ac_hash_update(
        &state, &context->recipient_role, sizeof(context->recipient_role)));
    AC_HASH_OR_FAIL(ac_hash_u16(&state, context->payload_type));
    AC_HASH_OR_FAIL(ac_hash_u16(&state, AC_COMMITMENT_NONCE_BYTES));
    AC_HASH_OR_FAIL(ac_hash_update(
        &state, nonce->bytes, sizeof(nonce->bytes)));
    AC_HASH_OR_FAIL(ac_hash_u64(&state, (uint64_t)payload_len));
    AC_HASH_OR_FAIL(ac_hash_update(&state, payload, payload_len));

#undef AC_HASH_OR_FAIL

    if (crypto_generichash_blake2b_final(
            &state, computed, sizeof(computed)) != 0) {
        status = AC_ERR_CRYPTO;
        goto fail;
    }

    memcpy(digest->bytes, computed, sizeof(digest->bytes));
    sodium_memzero(computed, sizeof(computed));
    sodium_memzero(&state, sizeof(state));
    return AC_OK;

fail:
    sodium_memzero(computed, sizeof(computed));
    sodium_memzero(&state, sizeof(state));
    memset(digest, 0, sizeof(*digest));
    return status;
}

ac_status ac_commitment_create(
    const ac_commitment_context *context,
    const uint8_t *payload,
    size_t payload_len,
    ac_commitment_digest *digest,
    ac_commitment_nonce *nonce)
{
    ac_status status;

    if (digest != NULL) {
        memset(digest, 0, sizeof(*digest));
    }
    if (nonce != NULL) {
        memset(nonce, 0, sizeof(*nonce));
    }
    status = ac_commitment_validate_arguments(
        context, payload, payload_len, nonce, digest);
    if (status != AC_OK) {
        return status;
    }
    if (sodium_init() < 0) {
        return AC_ERR_CRYPTO;
    }

    randombytes_buf(nonce->bytes, sizeof(nonce->bytes));
    status = ac_commitment_compute_with_nonce(
        context, payload, payload_len, nonce, digest);
    if (status != AC_OK) {
        ac_commitment_nonce_clear(nonce);
        memset(digest, 0, sizeof(*digest));
    }
    return status;
}

ac_status ac_commitment_verify(
    const ac_commitment_context *context,
    const uint8_t *payload,
    size_t payload_len,
    const ac_commitment_nonce *nonce,
    const ac_commitment_digest *digest)
{
    ac_commitment_digest expected;
    ac_status status;

    status = ac_commitment_validate_arguments(
        context, payload, payload_len, nonce, digest);
    if (status != AC_OK) {
        return status;
    }
    memset(&expected, 0, sizeof(expected));
    status = ac_commitment_compute_with_nonce(
        context, payload, payload_len, nonce, &expected);
    if (status != AC_OK) {
        sodium_memzero(&expected, sizeof(expected));
        return status;
    }

    if (sodium_memcmp(
            expected.bytes, digest->bytes, sizeof(expected.bytes)) != 0) {
        sodium_memzero(&expected, sizeof(expected));
        return AC_ERR_INVALID_OPENING;
    }
    sodium_memzero(&expected, sizeof(expected));
    return AC_OK;
}

void ac_commitment_nonce_clear(ac_commitment_nonce *nonce)
{
    if (nonce != NULL) {
        sodium_memzero(nonce, sizeof(*nonce));
    }
}
