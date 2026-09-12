#ifndef AC_RPS_H
#define AC_RPS_H

#include <stddef.h>
#include <stdint.h>

#include "ac/commitment.h"
#include "ac/status.h"

#define AC_RPS_PROTOCOL_VERSION 1U
#define AC_RPS_COMMITMENT_PAYLOAD_TYPE_MOVE 1U

#define AC_RPS_SESSION_ID_BYTES 32U
#define AC_RPS_MESSAGE_HEADER_BYTES 47U
#define AC_RPS_COMMIT_MESSAGE_BYTES 79U
#define AC_RPS_REVEAL_MESSAGE_BYTES 80U
#define AC_RPS_ABORT_MESSAGE_BYTES 48U
#define AC_RPS_MESSAGE_MAX_BYTES 80U

#define AC_RPS_TRANSCRIPT_HEADER_BYTES 43U
#define AC_RPS_TRANSCRIPT_SLOT_COUNT 6U
#define AC_RPS_TRANSCRIPT_SLOT_HEADER_BYTES 4U
#define AC_RPS_TRANSCRIPT_MAX_BYTES 481U

/* Educational protocol API. It assumes authenticated transport externally
 * and deliberately provides neither fair exchange nor guaranteed output. */

typedef enum {
    AC_RPS_ROLE_ALICE = 1,
    AC_RPS_ROLE_BOB = 2
} ac_rps_role;

typedef enum {
    AC_RPS_MOVE_ROCK = 0,
    AC_RPS_MOVE_PAPER = 1,
    AC_RPS_MOVE_SCISSORS = 2
} ac_rps_move;

typedef enum {
    AC_RPS_OUTCOME_TIE = 0,
    AC_RPS_OUTCOME_ALICE_WINS = 1,
    AC_RPS_OUTCOME_BOB_WINS = 2
} ac_rps_outcome;

typedef enum {
    AC_RPS_ABORT_UNSPECIFIED = 1,
    AC_RPS_ABORT_APPLICATION_REQUEST = 2,
    AC_RPS_ABORT_PROTOCOL_POLICY = 3
} ac_rps_abort_reason;

typedef enum {
    AC_RPS_MESSAGE_COMMIT = 1,
    AC_RPS_MESSAGE_REVEAL = 2,
    AC_RPS_MESSAGE_ABORT = 3
} ac_rps_message_type;

typedef enum {
    AC_RPS_STATE_UNINITIALIZED = 0,
    AC_RPS_STATE_READY,
    AC_RPS_STATE_COMMIT_SENT,
    AC_RPS_STATE_COMMIT_RECEIVED,
    AC_RPS_STATE_BOTH_COMMITTED,
    AC_RPS_STATE_REVEAL_SENT,
    AC_RPS_STATE_REVEAL_RECEIVED,
    AC_RPS_STATE_COMPLETE,
    AC_RPS_STATE_ABORTED_LOCAL,
    AC_RPS_STATE_ABORTED_PEER,
    AC_RPS_STATE_TIMED_OUT,
    AC_RPS_STATE_FAILED
} ac_rps_state;

/*
 * Fixed-size participant-local state for the educational v1 protocol.
 * Callers may allocate this object directly but must treat every field as
 * private implementation state and use only the functions below.
 */
typedef struct {
    uint8_t internal_session_id[AC_RPS_SESSION_ID_BYTES];
    uint32_t internal_round;
    ac_rps_role internal_local_role;
    ac_rps_role internal_peer_role;
    ac_rps_move internal_local_move;
    ac_rps_move internal_peer_move;
    ac_rps_state internal_state;
    ac_status internal_failure_reason;
    ac_rps_abort_reason internal_abort_reason;
    ac_commitment_nonce internal_local_nonce;
    ac_commitment_digest internal_local_commitment;
    ac_commitment_digest internal_peer_commitment;
    uint8_t internal_transcript_present[AC_RPS_TRANSCRIPT_SLOT_COUNT];
    uint16_t internal_transcript_lengths[AC_RPS_TRANSCRIPT_SLOT_COUNT];
    uint8_t internal_transcript_messages[AC_RPS_TRANSCRIPT_SLOT_COUNT]
                                        [AC_RPS_MESSAGE_MAX_BYTES];
    uint8_t internal_initialized;
    uint8_t internal_local_commit_sent;
    uint8_t internal_peer_commit_received;
    uint8_t internal_local_reveal_sent;
    uint8_t internal_peer_reveal_received;
    uint8_t internal_local_secret_cleared;
} ac_rps_session;

ac_status ac_rps_session_init(
    ac_rps_session *session,
    ac_rps_role local_role,
    ac_rps_move local_move,
    const uint8_t session_id[AC_RPS_SESSION_ID_BYTES],
    uint32_t round);

ac_status ac_rps_session_init_with_nonce_for_test(
    ac_rps_session *session,
    ac_rps_role local_role,
    ac_rps_move local_move,
    const uint8_t session_id[AC_RPS_SESSION_ID_BYTES],
    uint32_t round,
    const ac_commitment_nonce *nonce);

/* Message output and length storage must not overlap the session object. */
ac_status ac_rps_make_commit(
    ac_rps_session *session,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_length);

ac_status ac_rps_make_reveal(
    ac_rps_session *session,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_length);

ac_status ac_rps_make_abort(
    ac_rps_session *session,
    ac_rps_abort_reason reason,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_length);

ac_status ac_rps_receive(
    ac_rps_session *session,
    const uint8_t *message,
    size_t message_length);

ac_status ac_rps_mark_timeout(ac_rps_session *session);

ac_status ac_rps_get_state(
    const ac_rps_session *session,
    ac_rps_state *state);

ac_status ac_rps_get_peer_move_if_revealed(
    const ac_rps_session *session,
    ac_rps_move *peer_move);

ac_status ac_rps_get_outcome(
    const ac_rps_session *session,
    ac_rps_outcome *outcome);

ac_status ac_rps_get_failure(
    const ac_rps_session *session,
    ac_status *failure_reason);

ac_status ac_rps_get_abort_reason(
    const ac_rps_session *session,
    ac_rps_abort_reason *abort_reason);

ac_status ac_rps_export_transcript(
    const ac_rps_session *session,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_length);

void ac_rps_session_clear(ac_rps_session *session);

#endif
