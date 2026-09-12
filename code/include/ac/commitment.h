#ifndef AC_COMMITMENT_H
#define AC_COMMITMENT_H

#include <stddef.h>
#include <stdint.h>

#include "ac/status.h"

#define AC_COMMITMENT_SCHEME_VERSION 1U
#define AC_COMMITMENT_PROTOCOL_ID_BYTES 16U
#define AC_COMMITMENT_SESSION_ID_BYTES 32U
#define AC_COMMITMENT_NONCE_BYTES 32U
#define AC_COMMITMENT_DIGEST_BYTES 32U

/*
 * Educational hash-based construction, not a production commitment API.
 * Its hiding discussion requires a fresh secret nonce and an explicitly
 * stated random-oracle-style hash assumption; collision resistance alone is
 * not a hiding proof. Execution and tests establish no security theorem.
 */

typedef struct {
    uint8_t protocol_id[AC_COMMITMENT_PROTOCOL_ID_BYTES];
    uint16_t protocol_version;
    uint8_t session_id[AC_COMMITMENT_SESSION_ID_BYTES];
    uint32_t round;
    uint8_t committer_role;
    uint8_t recipient_role;
    uint16_t payload_type;
} ac_commitment_context;

typedef struct {
    uint8_t bytes[AC_COMMITMENT_NONCE_BYTES];
} ac_commitment_nonce;

typedef struct {
    uint8_t bytes[AC_COMMITMENT_DIGEST_BYTES];
} ac_commitment_digest;

/*
 * Creates a commitment with a fresh nonce from libsodium's randombytes API.
 * The nonce is an opening secret until the caller deliberately reveals it.
 * Digest and nonce outputs must be distinct and must not overlap context or
 * payload storage.
 */
ac_status ac_commitment_create(
    const ac_commitment_context *context,
    const uint8_t *payload,
    size_t payload_len,
    ac_commitment_digest *digest,
    ac_commitment_nonce *nonce);

/*
 * Deterministic computation seam for tests and published vectors.
 * Reusing, predicting, or exposing a nonce before reveal defeats the intended
 * hiding argument, especially for low-entropy payloads.
 * The digest output must not overlap the context, payload, or nonce inputs.
 */
ac_status ac_commitment_compute_with_nonce(
    const ac_commitment_context *context,
    const uint8_t *payload,
    size_t payload_len,
    const ac_commitment_nonce *nonce,
    ac_commitment_digest *digest);

ac_status ac_commitment_verify(
    const ac_commitment_context *context,
    const uint8_t *payload,
    size_t payload_len,
    const ac_commitment_nonce *nonce,
    const ac_commitment_digest *digest);

void ac_commitment_nonce_clear(ac_commitment_nonce *nonce);

#endif
