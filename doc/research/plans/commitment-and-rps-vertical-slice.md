# Execution Plan: Commitment Primitive and Two-Oracles Rock--Paper--Scissors

- **Status:** Ready for author review; implementation has not started.
- **Plan date:** 2026-07-21
- **Authoritative repository:** `C:\Work\applied_cryptography\adversarial_cooperation`
- **Plan destination:** `research/plans/commitment-and-rps-vertical-slice.md`
- **Recorded baseline commit:** `2824992ee2802b0b72f67054f63189628eb15103`
- **Governing documents:** `AGENTS.md` and `research/RESEARCH_CHARTER.md`

This is a living execution plan. The implementing agent must update its
progress, discoveries, decisions, verification results, and remaining
uncertainties while working. A checked box means the corresponding result was
actually obtained, not merely attempted.

## 1. Intended Outcome

Produce the project's first complete vertical slice: a reusable educational
commitment primitive and a two-party Rock--Paper--Scissors commit--reveal
protocol whose manuscript, C implementation, tests, transcript, and stated
limitations describe the same system.

The narrow protocol objective is:

> Each honest participant fixes its own move before processing peer messages
> and emits its reveal only after sending its commitment and accepting a
> context-valid peer commitment. Any peer reveal later accepted must open that
> commitment in the same context.

The protocol replaces a trusted referee's early collection of plaintext moves.
It does **not** reproduce a trusted referee's ability to release both moves
fairly and simultaneously. Either participant may still withhold its opening
after learning the peer's move. That selective-abort defect is a required
result of the slice, not an implementation bug to conceal.

Accepting a context-valid commitment digest does not prove that a malicious
peer knows any valid opening. A peer may commit to an unopenable digest and
later fail or abort. Version 1 verifies openings only when they arrive and makes
no commit-time proof-of-knowledge claim.

After the literature gate in Section 6 succeeds, the target contribution
classification is:

> A known commit--reveal construction used as the project's first complete
> methodological and instructional example.

Until that gate closes, its status remains `unreviewed; no novelty claim`. No
novelty claim is authorized by this plan.

## 2. Progress Ledger

- [x] Phase 0 repository audit completed and preserved.
- [x] This implementation plan written without changing manuscript or C code.
- [x] Baseline and user-owned changes rechecked at implementation start.
- [x] Primary-source and exact-construction research gate completed.
- [x] Commitment specification and deterministic vector frozen.
- [x] Commitment implementation and tests completed.
- [x] RPS wire protocol, participant-local state machine, and tests completed.
- [x] Honest and selective-abort demonstrations completed.
- [x] Canonical RPS chapter and standalone PDF target completed.
- [x] Research registers updated.
- [x] Strict, sanitizer, adversarial, and LaTeX verification completed.
- [x] Final work report delivered with claims and limitations separated from
  implementation observations.

### 2.1 Implementation and Verification Record — 2026-07-21

The work was developed in an isolated copy and is ready for guarded copyback to
the authoritative checkout. The implementation retains C11 and uses one C
dependency, libsodium, only for BLAKE2b, random bytes, constant-time comparison,
and secret clearing. Protocol serialization, parsing, state transitions,
transcripts, outcome computation, demonstrations, and the test harness are
plain repository C.

The frozen 123-byte commitment preimage produced digest
`d533a9e7051d5ae4b87dd3aa8068dc1572edd621b3e0bf4e1226b37c4203ca5d`
both in C and independently with Python 3.12.13 `hashlib.blake2b`.

A fresh Debian disposable environment recorded GCC 12.2.0, GNU Make 4.3, and
libsodium 1.0.18. The existing poker target and the new RPS target built under
`-std=c11 -Wall -Wextra -Wpedantic -Werror -O2`. Twenty-seven grouped tests
passed: seven commitment groups, six honest/core RPS groups, and fourteen
adversarial groups. Those tests include all nine outcomes, 207 truncation
prefixes, exact transcript slots, context mutation, replay, malformed input,
abort, timeout, and selective abort. AddressSanitizer and
UndefinedBehaviorSanitizer passed with leak detection and halt-on-error enabled.

The honest demonstration completed with equal 385-byte local transcripts. The
selective-abort demonstration left the peers in typed local/peer abort states
with equal 353-byte delivered transcripts; the second revealer could derive the
result privately but the public outcome API remained gated, while the first
revealer learned neither the withheld move nor the result.

The first strict build exposed an overly strong array-size annotation on a
private encoder parameter. The declaration was narrowed to the actual pointer
contract before the clean rebuild. Independent review also found missing typed
abort-reason exposure, incomplete transcript-slot assertions, and missing
sanitizer runtime settings. The implementation and tests were corrected before
the recorded clean run.

The standalone chapter built with pdfTeX 1.40.24 / TeX Live 2022, latexmk 4.79,
and BibTeX 0.99d. Its five citations and internal references resolved; the final
18-page PDF had no LaTeX error, missing glyph, overfull box, or replacement
character. Every page was rendered and inspected. Two harmless underfull
vertical-box page-break warnings remain. The generated PDF was used only for
verification and is not part of the source copyback. As a regression check, the
pre-existing standalone poker section also rebuilt successfully as a three-page
PDF through the changed shared wrapper.

## 3. Preservation Baseline

At plan-writing time, the authoritative checkout remains on `main` at the
recorded baseline commit. The visible status is:

```text
## main
 D idyicyanere-linux-x64.vsix
?? AGENTS.md
?? research/
```

The deleted VSIX and unrelated pre-existing untracked work are user-owned. The
listed Phase 0 registers and this plan are in-scope research artifacts, but do
not restore or delete the VSIX, disturb unrelated files, or stage or commit any
file without an explicit request.

The current execution identity may trigger Git's dubious-ownership check. Use
the per-command read-only override when needed:

```bash
git -c safe.directory=C:/Work/applied_cryptography/adversarial_cooperation status --short --branch
```

Do not change global Git configuration merely to inspect this checkout.

The repository currently tracks generated objects, binaries, and PDFs under
`.temp/`, `temp/`, and `demostrations/.build/`. The only ignore rule covers
`scholarship/*`. Because no generated-artifact policy has been approved:

- do not edit `.gitignore` in this slice;
- do not remove pre-existing tracked outputs;
- direct verification outputs to caller-supplied `/tmp` directories;
- remove only newly generated, precisely identified RPS/test outputs if a
  local build accidentally creates them; and
- record every unavoidable generated change.

Before implementation, repeat status, HEAD, file inventory, and relevant
build baselines. If the repository has advanced, update this plan rather than
blindly applying stale paths or assumptions.

## 4. Interpretation and Authorial Preservation

The current chapter contains one sentence:

> This section is on how do two people that can see eachother moves in advance
> can cooperate.

Preserve that sentence verbatim in a clearly marked `Author's Intuition`
passage. Do not silently claim that its literal or philosophical meaning is
settled.

This slice adopts one provisional operationalization:

> Interpret “seeing the other move in advance” as the ordinary simultaneous-
> choice problem in which learning the peer's move before fixing one's own
> creates an advantage. Use commitments to fix both moves before either is
> revealed.

The chapter must say that this is a narrow interpretation of the oracle
metaphor, not a general theory of prophecy, strategy prediction, or agents that
literally know future choices.

The proposed ASCII rename `RN-011` is **not approved by this plan**. Keep the
canonical manuscript at:

`document/content/two_oracles_play_rock–paper–scissors/two_oracles_play_rock–paper–scissors.tex`

Use ASCII names for new C and test paths. Retain typographic punctuation in the
display title. If the author later approves `RN-011`, execute that rename as a
separate approved task outside this execution plan, using `git mv` and a
complete reference update. It is not an optional implementation step here.

## 5. Scope

### 5.1 Required new artifacts

- `include/ac/status.h`
- `include/ac/commitment.h`
- `include/ac/rps.h`
- `src/primitives/commitment.c`
- `src/protocols/rps.c`
- `src/two_oracles_play_rock_paper_scissors/rps_demo.c`
- `tests/Makefile`
- `tests/test_support.h`
- `tests/test_commitment.c`
- `tests/test_rps.c`
- `tests/test_rps_adversarial.c`
- `test-vectors/commitment-v1.md`
- `document/references.bib`
- `research/PRIMITIVE_REGISTRY.md`
- `research/BIBLIOGRAPHY_QUEUE.md`
- `research/DECISIONS.md`

### 5.2 Existing files expected to change

- the canonical RPS chapter named above;
- `Makefile.config`;
- root `README.md`, limited to the already evidenced LaTeX dependency and
  command correction;
- `src/Makefile`;
- `demostrations/Makefile`;
- `book`;
- `src/README.md` and `demostrations/README.md`, only where the new commands
  require documentation;
- `research/CHAPTER_MATRIX.md`;
- `research/CLAIM_LEDGER.md`;
- `research/ASSUMPTION_LEDGER.md`;
- `research/OPEN_PROBLEMS.md`;
- `research/RENAME_MAP.md`;
- `research/ROADMAP.md`; and
- this plan.

The Phase 0 audit already proved that the documented LaTeX dependency set omits
`algorithm2e`/`texlive-science`, which the section wrapper loads through
`math_utils.tex`. Make the narrow corresponding root `README.md` correction and
reconfirm it in a clean disposable build. Touch dependency scripts only if that
recheck proves they also require correction.

### 5.3 Explicit non-goals

- no poker refactor or reuse of its current in-function hand commitment;
- no repair of Introduction, Trust Establishment, Hash Functions, or the full
  manuscript include graph;
- no networking, PKI, authenticated-transport, or identity-establishment
  implementation;
- no guaranteed fair output or guaranteed delivery;
- no deposits, penalties, blockchain, trusted hardware, or fair-exchange
  mechanism;
- no zero-knowledge proof;
- no production cryptography claim;
- no post-quantum, composable, adaptive-corruption, coercion-resistance, or
  side-channel theorem;
- no general solution to repeated-game strategy leakage;
- no fuzzing infrastructure or CI rollout beyond the deterministic adversarial
  harness in this slice;
- no mass rename or generated-artifact cleanup; and
- no claim that commit--reveal creates trust, rational play, cooperation,
  justice, peace, or a good objective.

## 6. Literature and Contribution Gate

Complete this gate before manuscript or register prose presents the candidate
hash-based construction as an established commitment scheme or makes a
security claim about it. The implementation plan may use “commitment” as the
target primitive name while keeping its evidence status explicit.

1. Verify primary prior art for bit commitment and commit--reveal. All metadata
   below is candidate metadata pending primary-source verification:
   - Manuel Blum, “Coin Flipping by Telephone: A Protocol for Solving
     Impossible Problems,” 1983, DOI `10.1145/1008908.1008911`,
     <https://doi.org/10.1145/1008908.1008911>.
   - Moni Naor, “Bit Commitment Using Pseudorandomness,” 1991, DOI
     `10.1007/BF00196774`,
     <https://research.ibm.com/publications/bit-commitment-using-pseudorandomness>.
2. Do not cite Naor's pseudorandom-generator construction as a proof of the
   exact nonce-and-hash construction used here. Find a primary source that
   directly supports the exact construction and assumptions, or retain the
   narrower label `instructional hash-based construction with a security
   argument`.
3. Verify the selected hash specification and implementation. The metadata
   below is likewise provisional until checked against the primary source:
   - RFC 7693, “The BLAKE2 Cryptographic Hash and Message Authentication Code
     (MAC),” DOI `10.17487/RFC7693`,
     <https://www.rfc-editor.org/rfc/rfc7693>.
   - the BLAKE2 design paper referenced by RFC 7693.
   - official libsodium generic-hash documentation,
     <https://libsodium.gitbook.io/doc/hashing/generic_hashing>, and random-data
     documentation, <https://libsodium.gitbook.io/doc/generating_random_data>,
     as implementation documentation, not theoretical evidence.
4. Use Richard Cleve's 1986 fairness result, DOI `10.1145/12130.12168`,
   <https://doi.org/10.1145/12130.12168>, only if the chapter makes a carefully
   matched general theorem claim. The concrete selective-abort trace below
   needs no inflated impossibility theorem.
5. For every source, record author, exact title, venue/version, DOI or stable
   URL, supporting page/section, claim supported, BibTeX key, and verification
   status in `research/BIBLIOGRAPHY_QUEUE.md`.
6. Add only verified entries to `document/references.bib`. The chapter owns
   `\cite{...}` calls; it does not own `\bibliography{...}`.
7. Record the contribution as a known construction used to establish project
   method. Any stronger classification requires a new literature review and
   author approval.

If no source proves the exact hiding claim for the chosen construction, write
an explicit random-oracle-style heuristic argument and label it as such. Do not
substitute “preimage resistance” as if it alone were a complete hiding proof.

## 7. Formal Protocol Contract

### 7.1 Parties, inputs, context, and outputs

- Parties are fixed roles `Alice` and `Bob`, denoted `A` and `B`.
- Each holds one private move in `{Rock, Paper, Scissors}`.
- Canonical move encoding is `Rock = 0`, `Paper = 1`, `Scissors = 2`.
- Before execution, both agree on the RPS protocol version, commitment scheme
  version, one unique public 32-byte session identifier, role assignment,
  nonzero 32-bit round number, move encoding, and local timeout policy.
- The session identifier separates contexts. It is not a secret, an identity,
  a signature, or proof of freshness by itself.
- An honest participant uses a functioning operating-system CSPRNG and keeps
  its move and 32-byte commitment nonce private until reveal.
- `ac_rps_get_outcome` succeeds only after the local reveal is emitted and the
  peer reveal validates. This state-machine restriction cannot prevent a
  malicious host from computing the result immediately after receiving the
  peer reveal, because it already knows its own move.

For moves `m_A` and `m_B`, compute:

```text
d = (m_A - m_B + 3) mod 3
```

- `d = 0`: tie;
- `d = 1`: Alice wins;
- `d = 2`: Bob wins.

Terminal local observations are:

- `COMPLETE(outcome)` after two valid openings;
- `ABORTED_LOCAL(reason)`;
- `ABORTED_PEER(reason)` after receiving a valid explicit abort;
- `TIMED_OUT` after a host-supplied local timeout event; or
- `FAILED(reason)` after an accepted in-session protocol violation.

A timeout records a local observation of non-delivery. It does not prove the
peer's motive or create a globally agreed abort fact.

### 7.2 Baseline and ideal functionality

The strong baseline is a trusted referee that privately receives both moves,
fixes them, and releases both moves or the result to both parties
simultaneously.

Use that baseline to define an idealized trusted-referee comparison for
exposition. Then define the exact real-protocol contract separately as
`simultaneous choice with abort`:

1. each honest move is fixed before the peer's plaintext move is learned;
2. each valid opening is checked against the prior session-bound commitment;
3. the result is released by the API only after both openings validate; and
4. either party or the network can prevent completion.

These are comparison models, not a simulation-based claim that the construction
securely realizes an ideal functionality.

For this chapter, strong fairness means that either both parties obtain the
result or neither does. Guaranteed output delivery means that an honest party
eventually obtains its output despite peer deviation. This protocol provides
neither.

### 7.3 Adversary and communication model

Analyze a classical probabilistic polynomial-time, static malicious adversary
that corrupts at most one of the two parties. A corrupted party may:

- choose any legal move and any nonce;
- send malformed, duplicate, replayed, reordered, context-mismatched, or
  inconsistent messages;
- send a second conflicting commitment;
- reveal before the permitted phase;
- open with a different move or nonce;
- delay, suppress, or refuse messages; and
- abort at any time, including after learning the honest party's reveal.

The transport is assumed to provide origin authentication and integrity for
accepted peer messages, but it may expose, delay, drop, duplicate, or reorder
them. Transport and identity establishment are supplied externally and are not
implemented. The v1 state machine detects but does not recover from every
duplicate or reordering; this is an availability limitation.

The adversary is assumed unable to defeat authenticated transport, read or
alter honest local memory, predict an honestly generated nonce, or violate the
explicit hash assumptions.

Out of model are adaptive corruption, compromised randomness, endpoint
malware, memory disclosure, side channels beyond constant-time digest
comparison, traffic-analysis protection, coercion, denial-of-service
prevention, concurrency, crash recovery, quantum attackers, and universal
composition.

### 7.4 Required property classification

| Property or claim | Required status and boundary |
|---|---|
| Honest correctness | Proposition limited to two honest parties with identical agreed context, successful cryptographic operations, and eventual delivery of every canonical message before timeout; all nine move pairs are implementation evidence. |
| Computational hiding before reveal | Security argument under a fresh secret 256-bit nonce and explicitly stated hash model; context and timing still leak. |
| Computational binding after commit | Security argument/proof sketch under collision resistance for malicious commitment and second-preimage resistance for alternate opening of a fixed honest commitment. |
| Valid opening / proof of knowledge | Not achieved at commit time; an accepted digest may have no opening known to its sender. |
| Context separation | Protocol invariant from scheme domain, protocol/version, session, roles, round, and payload type. It is not authentication. |
| Canonical parsing and state enforcement | Implementation observation for the enumerated tests, not a cryptographic theorem. |
| Fairness | Not achieved. |
| Guaranteed output, liveness, availability | Not achieved. |
| Accountability or non-repudiation | Not achieved by an unsigned local transcript. |
| Random, rational, or cooperative move selection | Not achieved; a commitment fixes a choice but does not improve it. |
| Repeated-play strategy privacy | Not achieved; completed games reveal moves and may support inference. |
| Production, post-quantum, composable, or coercion-resistant security | Not claimed. |

## 8. Commitment Specification

### 8.1 Suite

Commitment scheme version 1 uses:

- keyless BLAKE2b with a 32-byte digest through libsodium's explicit
  BLAKE2b interface, so protocol bytes do not depend on an unnamed future
  generic-hash default;
- a fresh 32-byte nonce from `randombytes_buf` for ordinary execution;
- a caller-supplied fixed nonce only through the explicitly documented
  deterministic/test computation path; and
- `sodium_memcmp` for digest comparison.

Do not use keyed BLAKE2, `rand()`, a bare hash of the three-value move,
ambiguous string concatenation, native C struct serialization, or a custom hash
implementation.

The exact 16-byte scheme-domain value is the ASCII byte string:

```text
AC-COMMITMENT-V1
```

It has no terminating NUL in the hashed input.

The RPS protocol identifier is a fixed 16-byte field containing ASCII `AC-RPS`
followed by ten zero bytes.

### 8.2 Context and public API

The public context contains:

```c
#define AC_COMMITMENT_PROTOCOL_ID_BYTES 16U
#define AC_COMMITMENT_SESSION_ID_BYTES 32U
#define AC_COMMITMENT_NONCE_BYTES 32U
#define AC_COMMITMENT_DIGEST_BYTES 32U

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
```

Expose typed nonce and digest wrappers and return a shared `ac_status` error
enumeration. The minimum functions are:

```c
ac_status ac_commitment_create(
    const ac_commitment_context *context,
    const uint8_t *payload,
    size_t payload_len,
    ac_commitment_digest *digest,
    ac_commitment_nonce *nonce);

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
```

`compute_with_nonce` is the deterministic injection point and must warn that
reusing, predicting, or exposing the nonce destroys the intended hiding
argument. Allow `(payload == NULL, payload_len == 0)` for the generic primitive;
reject every other inconsistent null/length combination. RPS always commits to
exactly one move byte.

Check `sodium_init` and every hash return. Clear partial outputs on error. Wipe
temporary digests with `sodium_memzero`. Wipe an unrevealed nonce when a session
terminates without reveal, and give non-RPS callers the explicit nonce cleanup
function above. The public API is educational and not declared ABI-stable.

The generic commitment primitive validates buffer/length consistency and
canonical field widths, but does not impose RPS semantics such as nonzero
rounds, role values `1`/`2`, or a nonzero session ID. RPS initialization and
message parsing enforce those rules.

### 8.3 Canonical commitment preimage

Hash the following byte sequence incrementally and exactly:

```text
scheme-domain[16]                    = "AC-COMMITMENT-V1"
scheme-version:u16be                 = 1
protocol-id-length:u16be             = 16
protocol-id[16]
protocol-version:u16be
session-id-length:u16be              = 32
session-id[32]
round:u32be
committer-role:u8
recipient-role:u8
payload-type:u16be
nonce-length:u16be                   = 32
nonce[32]
payload-length:u64be
payload[payload-length]
```

All integers are unsigned big-endian. Lengths are included even where v1 fixes
them so the byte sequence has one canonical meaning. Do not hash padding,
pointer values, host endianness, or a C structure image.

`test-vectors/commitment-v1.md` must contain every input field, the complete
preimage hex, expected digest hex, provenance of the independent computation,
and tool/version information. Cross-check at least one vector with an
independent BLAKE2b implementation. A vector establishes encoding stability;
it is not a security proof.

### 8.4 Security-argument boundary

For binding, explain the different-message collision that a malicious
committer would need in the canonical encoding, and distinguish this from an
alternate opening against a fixed honest commitment.

For hiding, the three-value move has negligible entropy by itself. The argument
depends on a fresh secret 256-bit nonce and a clearly named random-oracle-style
or equivalent hash assumption. Never say the bare hash hides the move. Never
call the construction information-theoretically secure.

RFC 7693 lists a `2^128` collision-security target for its 32-byte BLAKE2b
parameter set. Record that as a cited algorithm parameter, not as proof of this
commitment construction. Tests cannot establish hiding or binding.

## 9. RPS Wire Protocol

### 9.1 Envelope

Every message has this canonical byte layout:

```text
magic[4]                             = "ACRP"
wire-version:u16be                   = 1
session-id[32]
round:u32be
sender-role:u8                       = 1 (Alice) or 2 (Bob)
recipient-role:u8                    = 1 (Alice) or 2 (Bob)
message-type:u8
payload-length:u16be
payload[payload-length]
```

The header is exactly 47 bytes. Message types and canonical sizes are:

| Type | Value | Payload | Total bytes |
|---|---:|---|---:|
| `COMMIT` | 1 | 32-byte commitment digest | 79 |
| `REVEAL` | 2 | one move byte followed by a 32-byte nonce | 80 |
| `ABORT` | 3 | one enumerated reason byte | 48 |

Abort reason codes are `UNSPECIFIED = 1`, `APPLICATION_REQUEST = 2`, and
`PROTOCOL_POLICY = 3`. They are sender-supplied declarations, not verified
evidence of cause or motive.

RPS protocol version 1 fixes commitment scheme version 1. A later scheme must
increment the RPS protocol version or add an explicitly specified negotiation;
v1 has no algorithm agility claim.

Reject truncated input, trailing input, mismatched declared length, unknown
version/type/role/move/abort reason, equal sender and recipient, wrong
recipient, zero round, all-zero session identifier, or a noncanonical payload
length. Copy validated fields into owned fixed-size storage; retain no pointer
into a caller's buffer.

No MAC or signature is present. Sender and recipient fields are contextual
bindings whose identity meaning depends entirely on the external authenticated
channel assumption.

### 9.2 Participant-local API

The protocol library must never place both parties' private moves in one
session object. The demo creates two independent `ac_rps_session` instances and
passes serialized messages between them.

Initialization fixes and locks the local role, peer role, move, session,
round, nonce, and commitment before any peer message is processed. Provide at
least:

```c
#define AC_RPS_SESSION_ID_BYTES 32U
#define AC_RPS_MESSAGE_MAX_BYTES 80U
#define AC_RPS_TRANSCRIPT_MAX_BYTES 481U

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

ac_status ac_rps_export_transcript(
    const ac_rps_session *session,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_length);

void ac_rps_session_clear(ac_rps_session *session);
```

`AC_RPS_TRANSCRIPT_MAX_BYTES` is `43` bytes of transcript header plus `24`
bytes of six slot headers plus `2*79 + 2*80 + 2*48` message bytes, totaling
`481`. Keep a compile-time assertion or equivalent test tied to this layout.

Every writer takes an output capacity and returns the exact encoded length.
Output remains unavailable if the capacity is insufficient. `make_commit`,
`make_reveal`, and `make_abort` do not transition state unless capacity checks,
encoding, and all required cryptographic work succeed. Local API misuse returns
`AC_ERR_STATE` without pretending the peer deviated.

### 9.3 State machine

Required states are:

- `READY`;
- `COMMIT_SENT`;
- `COMMIT_RECEIVED`;
- `BOTH_COMMITTED`;
- `REVEAL_SENT`;
- `REVEAL_RECEIVED`;
- `COMPLETE`;
- `ABORTED_LOCAL`;
- `ABORTED_PEER`;
- `TIMED_OUT`; and
- `FAILED` with a retained reason.

Core transitions are:

| Current state | Local action or accepted peer message | Next state |
|---|---|---|
| `READY` | send local commit | `COMMIT_SENT` |
| `READY` | receive peer commit | `COMMIT_RECEIVED` |
| `COMMIT_SENT` | receive peer commit | `BOTH_COMMITTED` |
| `COMMIT_RECEIVED` | send local commit | `BOTH_COMMITTED` |
| `BOTH_COMMITTED` | send local reveal | `REVEAL_SENT` |
| `BOTH_COMMITTED` | receive valid peer reveal | `REVEAL_RECEIVED` |
| `REVEAL_SENT` | receive valid peer reveal | `COMPLETE` |
| `REVEAL_RECEIVED` | send local reveal | `COMPLETE` |
| any nonterminal state | send local abort | `ABORTED_LOCAL` |
| any nonterminal state | accept peer abort | `ABORTED_PEER` |
| any nonterminal state | host marks timeout | `TIMED_OUT` |

Receiving a peer commit in `READY` is safe because initialization has already
fixed the local move, nonce, and commitment. An honest reveal may be emitted
only after both context-valid commitment messages exist. This phase condition
does not prove that a malicious peer knows an opening.

Parsing and state mutation must be atomic:

- framing errors and messages clearly belonging to another session return a
  format/routing error without mutating the session;
- local API misuse returns an error without mutating the session;
- a syntactically valid in-session premature reveal, duplicate/replay,
  conflicting commitment, or invalid opening enters `FAILED` only after the
  complete violation is established;
- rejected packets never overwrite an accepted transcript slot; and
- terminal states accept no further protocol message.

This v1 policy rejects rather than buffers out-of-order messages. It can
therefore lose availability under duplication or reordering, which must remain
visible in the chapter.

`ac_rps_get_outcome` succeeds only in `COMPLETE`. A malicious host can still
read a valid peer reveal and combine it with its own move before sending its own
reveal. An API cannot erase that selective-abort power.

### 9.4 Canonical transcript

Store accepted raw messages in logical role/phase slots, not local arrival
order:

1. `A_COMMIT = 1`;
2. `B_COMMIT = 2`;
3. `A_REVEAL = 3`;
4. `B_REVEAL = 4`;
5. `A_ABORT = 5`; and
6. `B_ABORT = 6`.

Export:

```text
magic[4]                             = "ACRT"
transcript-version:u16be             = 1
session-id[32]
round:u32be
slot-count:u8                        = 6
for each slot in fixed order:
    slot-id:u8
    present:u8
    message-length:u16be
    message[message-length]
```

An absent slot is encoded as `present = 0`, `message-length = 0`, and no
message bytes. A present slot is encoded as `present = 1` and must contain the
exact canonical length for that slot's message type. Reject every other
presence value or absent/present length combination.

Do not include local `sent`/`received` direction in the canonical bytes. Do not
include rejected packets. Honest completed peers must export identical bytes
regardless of legal delivery order, provided every locally emitted commit and
reveal was delivered and accepted by the peer. Dropped or selectively delivered
messages may yield different local transcripts.

Keep local timeout/failure observations separate from the shared message-slot
transcript. The transcript is a canonical record of locally observed accepted
bytes, not consensus, a signature, non-repudiation, or proof of delivery.

## 10. Mandatory Selective-Abort Counterexample

The demo, adversarial test, chapter, claim ledger, and open-problem register
must all contain this trace:

1. Alice and Bob accept both commitments.
2. Alice reveals first.
3. Bob validates Alice's opening and combines it with Bob's private move,
   learning the result.
4. `ac_rps_get_outcome(&bob, ...)` still fails because Bob has not emitted his
   own reveal; this API gate does not erase Bob's protocol-level knowledge.
5. If the result is unfavorable, Bob withholds his reveal or sends `ABORT`.
6. Alice has only Bob's hiding commitment. Alice times out or records the abort
   without learning Bob's move or the result, and neither session reaches
   `COMPLETE`.

Bob has learned more than Alice and can bias the set of games that reach
`COMPLETE`. Alice can attribute a received abort to the peer within the assumed
authenticated channel session. The exported unsigned transcript cannot prove
that attribution, delivery, or motive to a third party. Silence produces only
Alice's local timeout observation.

Do not “fix” this trace by declaring the incomplete game a tie or awarding a
winner. Such a rule would be a new enforcement or utility mechanism outside
this cryptographic slice.

## 11. C Implementation Discipline

- Retain C11 and the existing `-Wall -Wextra -Wpedantic` baseline.
- Use only fixed-size protocol messages and bounded transcript storage; no heap
  allocation is needed for v1.
- Add shared, explicit status codes for bad argument, length, format, context,
  state, duplicate, invalid opening, terminal state, crypto initialization, and
  output capacity.
- Encode and decode integers with checked byte helpers. Never cast a byte buffer
  to a C struct.
- Check all lengths before arithmetic and all return values before mutation.
- Clear output buffers on failure where doing so cannot hide the diagnostic.
- Use constant-time digest comparison. Do not claim that the whole protocol is
  constant-time or side-channel resistant.
- Do not log a move or nonce before its reveal message. A revealed nonce is
  public; an aborted unrevealed nonce remains secret and should be wiped.
- `ac_rps_session_clear` wipes the complete session object. Terminal cleanup
  wipes unrevealed local secret material while preserving already-public raw
  transcript bytes.
- Keep poker code and behavior unchanged.
- Add no production dependency beyond the existing libsodium dependency.

## 12. Demonstration Contract

Add an honest command such as:

```bash
make -C demostrations demo_rps RPS_ARGS='--alice rock --bob scissors'
```

and a selectable selective-abort scenario, either through `RPS_ARGS` or a
separate narrow Make target.

The driver must:

- print an unmistakable educational/non-production warning;
- state that authenticated transport is assumed rather than implemented;
- state that fair exchange and guaranteed output are absent;
- generate one random 32-byte session identifier and pass it to two local
  participant states;
- show commit/commit/reveal/reveal delivery without printing secrets early;
- print the result only after both valid openings;
- export and compare canonical transcripts; and
- demonstrate the asymmetric selective-abort trace without relabeling it as a
  successful fair game.

The single process necessarily receives both command-line moves. Say plainly
that it is a teaching harness, not a process-isolation or transport simulation.

## 13. Test Matrix

Use a tiny repository-local assertion harness; add no testing framework
dependency. Tests establish implementation behavior and invariants, not
cryptographic security.

### 13.1 Commitment tests

- independently reproduced deterministic vector with exact preimage bytes;
- successful computation and verification;
- same inputs and nonce produce the recorded digest;
- fresh/different nonce changes the digest;
- mutation of protocol ID, protocol version, session, round, committer role,
  recipient role, payload type, payload, nonce, or digest rejects;
- empty-payload policy and inconsistent null/length arguments;
- deterministic supplied-nonce path;
- explicit nonce cleanup API; and
- ordinary argument failures clear partial outputs.

Do not pretend an ordinary unit test can force `sodium_init()` failure or prove
that compiler-generated stack storage was wiped. Without a deliberate internal
test seam or linker instrumentation, checked initialization paths and temporary
`sodium_memzero` calls are reviewed implementation invariants, not test results.
`randombytes_buf()` itself has no failure return to simulate.

### 13.2 Honest RPS tests

- all nine move pairs, checked from both participants' perspectives;
- Alice-first and Bob-first legal commit/reveal interleavings;
- exact outcome formula and agreement;
- outcome unavailable before `COMPLETE`;
- honest participant cannot emit reveal before both commitments;
- commit, reveal, and abort parser round trips;
- honest canonical transcript equality independent of legal arrival order; and
- explicit session clearing.

### 13.3 Parser and adversarial tests

- every truncation prefix for every message type;
- trailing bytes, oversized input, and declared-length mismatch;
- invalid magic, version, message type, role, move, abort reason, round, and
  all-zero session;
- sender equal to recipient and wrong recipient;
- duplicate commit, reveal, and abort;
- replay across session and round;
- reflected roles and changed sender/recipient;
- reveal before both commitments;
- second conflicting commitment;
- context-valid random commitment digest followed by failure to open, showing
  that commit-time knowledge of an opening is not established;
- changed move, nonce, or commitment digest;
- malformed or invalid opening with atomic state behavior;
- explicit abort before commit, after both commits, and after receiving the
  peer reveal;
- timeout/missing reveal;
- selective-abort asymmetry: the party in the second-reveal position derives
  the result from its private move and the peer's accepted reveal, its own
  `ac_rps_get_outcome` call still fails because it has not revealed, the first
  party cannot derive the withheld move or result, and neither session reaches
  `COMPLETE`;
- transcript disagreement after dropped delivery;
- behavior after each terminal state; and
- local API misuse distinguished from peer protocol failure.

A deterministic mutation harness is sufficient here. Record libFuzzer or a
network fault simulator as future work rather than quietly expanding scope.

## 14. Manuscript Contract

Use the existing RPS source as the sole canonical chapter. Do not create a
second demonstration manuscript.

The mature chapter should contain, in this order unless a clearer LaTeX
structure emerges:

1. `\chapter{Two Oracles Play Rock--Paper--Scissors}`;
2. Author's Intuition;
3. Abstract;
4. Cooperation Problem;
5. Trusted-Referee and Simultaneous-Choice Baselines;
6. Parties, Moves, Secrets, and Outputs;
7. Threat Model, Channels, and Setup;
8. Idealized Trusted-Referee Baseline;
9. Exact Real-Protocol Contract with Abort;
10. Non-Goals;
11. Commitment Primitive;
12. Commit/Commit/Reveal/Reveal Protocol;
13. State Machine and Canonical Transcript;
14. Conditional Correctness;
15. Hiding and Binding Claims;
16. Security-Argument Status;
17. Attacks and Counterexamples;
18. Selective Abort, Timeout, and Fairness;
19. C Implementation Mapping;
20. Tests and Experiments;
21. Ethical and Institutional Analysis;
22. What Cryptography Does Not Solve;
23. Limitations;
24. Open Problems; and
25. Exercises; and
26. References, emitted by the standalone/book bibliography owner rather than
    declared inside the chapter.

The chapter must include:

- the original sentence as authorial intuition;
- the distinction between literal oracles and the operationalized timing game;
- exact players, inputs, outputs, setup, leakage, and non-goals;
- ideal and real-with-abort functionalities side by side;
- an executable protocol description matching the C state machine;
- a claim/evidence table matching Section 7.4 of this plan;
- a short correctness proof for the outcome formula;
- a hiding/binding argument whose status and assumptions are visible;
- the selective-abort counterexample;
- a table mapping C files, public functions, tests, and protocol steps;
- an ethical note that cryptography can fix chronology but cannot determine a
  good move, make participants cooperate, or justify enforcement; and
- the mandatory `What Cryptography Does Not Solve` section.

Never title the construction “Secure RPS” without an immediately visible
qualification and named property. Do not use test results as evidence of
hiding or binding.

## 15. Bibliography and Standalone PDF Ownership

Create `document/references.bib` as the initial shared bibliography seed for
the RPS slice, containing only verified RPS-slice entries. Do not add placeholder
Trust Establishment citations to mask the existing full-book failure. The RPS
chapter contains citation commands but no bibliography declaration, and this
slice does not settle bibliography ownership for the current Trust chapter or
the full book.

Extend `book` with:

```text
section_rps -> document/content/two_oracles_play_rock–paper–scissors/two_oracles_play_rock–paper–scissors.tex
```

The audited Git mode for `book` is not executable. Keep the invocation
portable as `bash book ...` and update its usage examples accordingly; changing
the executable bit is not required by this slice.

Only the generated RPS standalone wrapper owns `\bibliographystyle{plain}` and
`\bibliography{...}`. Resolve `document/references.bib` from the repository
root—either emit an absolute/root-derived bibliography path or set `BIBINPUTS`
explicitly—so bibliography lookup still works when `BOOK_TMP_DIR` points to
`/tmp`. Do not add bibliography commands globally to the poker wrapper unless
a no-citation regression test proves identical `section_poker` behavior. Have
`book` accept optional `BOOK_OUT_DIR` and `BOOK_TMP_DIR` environment overrides
so verification can write outside the tracked output tree.

Run `latexmk` noninteractively with file/line errors and halt-on-error. The RPS
section succeeds only if BibTeX resolves every citation and the fresh output is
nonempty. A stale or partial PDF is failure.

The full main-book include graph remains unchanged in this slice because it has
known unrelated failures. Do not repair those failures to make the RPS target
appear successful.

## 16. Research Register Updates

### 16.1 Primitive registry

Add the v1 commitment with:

- exact definition and encoding;
- hiding and binding notions;
- guarantee type and proof/argument status;
- BLAKE2b/libsodium backend;
- nonce and context requirements;
- approved educational uses;
- chapter users;
- deterministic vectors;
- known misuse patterns; and
- the future appendix destination.

### 16.2 Claim ledger

Preserve `AC-RPS-01` verbatim and mark it as operationalized into a narrower
timing model. Add distinct entries for:

- conditional honest correctness;
- computational hiding argument;
- computational binding argument;
- context/replay enforcement as a protocol invariant;
- canonical parser/state observations;
- the selective-abort counterexample;
- local transcript observability without identity proof; and
- executed demo/test results as implementation observations only.

### 16.3 Assumption ledger

Expand `AS-RPS-01` and `AS-RPS-02`, or add narrowly scoped entries, for:

- exactly two fixed pre-identified roles;
- at most one corrupted party for nontrivial analysis;
- classical computational bounds;
- external origin-authenticated, integrity-protected public transport;
- delay, loss, duplication, and reordering without recovery guarantees;
- unique session, protocol version, role assignment, round, and rules;
- protected move and fresh unpredictable nonce until reveal;
- honest endpoint memory and no side-channel compromise;
- eventual valid delivery as a condition for honest completion;
- host-defined local timeouts; and
- the same-process demo's absence of a real isolation boundary.

### 16.4 Open problems and decisions

Update rather than delete `OP-R01` through `OP-R07`. Mark only the portions for
which evidence now exists. `OP-R04` remains open because this slice demonstrates
selective abort and does not solve fairness.

Record in `research/DECISIONS.md`:

- the provisional interpretation of the oracle sentence;
- contribution classification;
- C11/libsodium and exact commitment-suite choice;
- canonical encoding ownership;
- authenticated-channel assumption;
- fairness and output-delivery non-goals;
- canonical manuscript and bibliography ownership;
- deferral of `RN-011`;
- no `.gitignore` policy change; and
- all material implementation deviations from this plan.

Update `RN-011` in `research/RENAME_MAP.md` from “proposed before first slice”
to “deferred; current Unicode path retained for this slice.” This records the
decision without performing the rename.

Update the chapter matrix and roadmap only with actually completed evidence.
The RPS row remains `Included: No`; a standalone section target does not alter
the main manuscript include graph. Do not create `research/THESIS.md` or
`research/GLOSSARY.md` merely to enlarge this slice.

## 17. Ordered Execution

### Phase A: Re-baseline and freeze the specification

1. Read `AGENTS.md`, the charter, this plan, the RPS source, current build
   files, and affected registers in full.
2. Record HEAD and status without changing global Git configuration.
3. Confirm the pre-existing deletion and all unrelated untracked files.
4. Run existing narrow C and section baselines with `/tmp` output overrides
   where available.
5. Freeze the exact domain bytes, context fields, wire layouts, state table,
   transcript slots, status codes, and output semantics in this plan and
   `research/DECISIONS.md`.

**Exit gate:** no source edit begins until every byte layout has one meaning and
the preservation baseline is recorded.

### Phase B: Establish scholarship and claims

1. Complete the literature gate in Section 6.
2. Create the bibliography queue and verified `.bib` entries.
3. Draft the formal model, claim/evidence table, assumptions, and selective-
   abort trace before implementation prose.
4. Decide whether the exact hash construction has a literature-backed security
   statement or only the explicitly limited heuristic argument.

**Exit gate:** every planned security sentence has a claim type, assumptions,
and evidence or is labeled open.

### Phase C: Implement and test the commitment primitive

1. Add status and commitment headers.
2. Implement incremental canonical hashing and checked randomness.
3. Create one independently reproduced vector.
4. Add unit and mutation tests before RPS integration.
5. Run base-warning, `-Werror`, AddressSanitizer, and UndefinedBehaviorSanitizer
   builds with outputs outside the repository.

**Exit gate:** deterministic vectors and every commitment mutation test pass;
no RPS code is needed to verify the primitive.

### Phase D: Implement and test participant-local RPS

1. Add the exact parser/encoder and participant-local state object.
2. Implement commit, reveal, abort, timeout, result, transcript, and clear
   operations.
3. Add all nine honest games and both legal delivery orders.
4. Add parser, replay, context, invalid-opening, terminal-state, and transcript
   tests.
5. Add the mandatory selective-abort test.

**Exit gate:** no API releases a shared result before two valid openings, and
the selective-abort test proves that this restriction still does not provide
fairness.

### Phase E: Add the teaching driver and build targets

1. Add only the narrow Makefile/config paths needed for RPS and tests.
2. Add honest and selective-abort demo modes.
3. Preserve all poker targets and behavior.
4. Document exact commands and educational warnings.
5. Keep outputs outside the tracked directories during verification.

**Exit gate:** a fresh environment can build and run both scenarios from the
documented commands with no compiler warning.

### Phase F: Complete the chapter and registers

1. Write the canonical chapter against the frozen, tested implementation.
2. Add verified citations and the standalone `section_rps` target.
3. Update primitive, claim, assumption, open-problem, decision, chapter, and
   roadmap registers.
4. Compile the RPS PDF and inspect its log and rendered pages.

**Exit gate:** prose, math, code, vectors, tests, and registers use identical
contexts, chronology, states, and claim boundaries.

### Phase G: Final adversarial verification and review

1. Run the complete deterministic, strict, sanitizer, and LaTeX command set.
2. Re-run the selective-abort scenario and inspect both local states and
   transcripts.
3. Inspect the final diff for scope creep, accidental generated files, and
   unsupported claims.
4. Update this plan with exact commands, versions, results, and remaining
   uncertainties.
5. Deliver the required work report. Do not commit unless the user explicitly
   requests it.

## 18. Verification Commands

Adapt paths only if the live build files require it, and record every change.
The intended native/disposable-environment commands are:

```bash
make -C demostrations build_rps \
  TEMP_DIR=/tmp/ac-rps-default-temp \
  BUILD_DIR=/tmp/ac-rps-default-build

make -C demostrations demo_rps \
  TEMP_DIR=/tmp/ac-rps-default-temp \
  BUILD_DIR=/tmp/ac-rps-default-build \
  RPS_ARGS='--alice rock --bob scissors'

make -C tests test \
  TEMP_DIR=/tmp/ac-rps-test-temp \
  BUILD_DIR=/tmp/ac-rps-test-build

make -C demostrations build_rps \
  TEMP_DIR=/tmp/ac-rps-strict-temp \
  BUILD_DIR=/tmp/ac-rps-strict-build \
  CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'

make -C tests test \
  TEMP_DIR=/tmp/ac-rps-strict-test-temp \
  BUILD_DIR=/tmp/ac-rps-strict-test-build \
  CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'

make -C tests sanitize \
  TEMP_DIR=/tmp/ac-rps-san-temp \
  BUILD_DIR=/tmp/ac-rps-san-build

BOOK_OUT_DIR=/tmp/ac-rps-pdf \
BOOK_TMP_DIR=/tmp/ac-rps-tex \
  bash book -pdf section_rps
```

The sanitizer target must use a separate build variant equivalent to:

```text
-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address,undefined
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1
UBSAN_OPTIONS=halt_on_error=1
```

For the LaTeX result:

- require exit zero and a fresh nonempty `section_rps.pdf`;
- fail on undefined citations/references, LaTeX errors, missing glyphs, or
  overfull boxes originating in the touched chapter;
- render the PDF to images and inspect every page; if no renderer is available,
  keep the PDF verification gate incomplete rather than silently skipping
  visual QA;
- confirm that the retained Unicode source path opens in the documented Debian
  environment and produces no mojibake, replacement character, or missing
  glyph; stop on failure rather than performing an unapproved rename; and
- do not accept a stale PDF left by a failed run.

Record compiler, Make, libsodium, TeX, latexmk, BibTeX, sanitizer, and container
versions. A clean disposable build should install only the documented C and
LaTeX dependencies and keep repository mounts read-only where practical.

## 19. Acceptance Criteria

The slice is provisionally complete only when all of the following are true:

1. Existing user changes, backups, poker behavior, and tracked outputs are
   preserved.
2. The RPS path rename and global output policy remain unchanged unless the
   author separately approves them.
3. The commitment encoding, RPS messages, and transcript have exact canonical
   byte definitions and deterministic vectors.
4. Default and `-Werror` C11 builds emit no warning.
5. Commitment, all-nine-game, parser, state, replay, abort, timeout, mutation,
   and transcript tests pass.
6. AddressSanitizer and UndefinedBehaviorSanitizer runs pass where the chosen
   toolchain supports them; any unsupported sanitizer is recorded, not hidden.
7. No participant-local API returns a result before two valid openings.
8. The selective-abort scenario shows that the party in the second-reveal
   position can derive the result and withhold its opening while its own outcome
   API still fails; the first party learns no result, neither session completes,
   and fairness and guaranteed output remain explicit non-properties.
9. Honest completed peers export identical canonical transcripts after every
   emitted canonical message is delivered and accepted, while local timeout or
   dropped-delivery disagreement is not mislabeled consensus.
10. `section_rps` compiles noninteractively with resolved citations and no
    touched-chapter LaTeX defect; every rendered page is inspected, and the
    retained Unicode source path produces no mojibake or replacement glyph.
11. The chapter contains `What Cryptography Does Not Solve` and every security
    statement has a claim type, assumptions, and proof/argument/evidence status.
12. Tests are reported only as implementation evidence.
13. `AC-RPS-01` and the author's original sentence remain visible; the narrow
    operationalization is identified as an interpretation.
14. `OP-R04` remains open and records what stronger fairness work would require.
15. The final status contains no unexpected generated artifact or unrelated
    source change.

## 20. Principal Risks and Stop Conditions

- **Unsupported hiding language.** If the exact nonce-and-hash construction
  lacks a matching proof/source, weaken the claim and retain the heuristic
  model; do not launder a related commitment paper into evidence.
- **Nonce misuse.** Prediction, early disclosure, or compromised randomness can
  defeat hiding for a three-value move. Reuse invalidates the stated
  fresh-randomness assumption and can reveal another low-entropy commitment
  once any reused nonce is opened. Treat freshness as a hard precondition and
  negative-test reuse where possible.
- **Authentication confusion.** Session IDs and role bytes do not authenticate
  people. Stop any prose that implies otherwise.
- **Fairness confusion.** Detection of non-reveal, an abort byte, or a timeout
  does not deliver the missing result. Keep the counterexample central.
- **Transcript overclaim.** Local accepted bytes are not signatures, global
  consensus, non-repudiation, or proof of motive.
- **Same-process illusion.** Two state objects in one demo improve architecture
  and testing but do not establish real confidentiality between processes.
- **Availability.** The fail-closed v1 state machine rejects reordering and
  duplication instead of recovering. Do not call it robust transport.
- **Authorial drift.** If the literal oracle idea proves materially different,
  preserve it as an open problem rather than replacing it with commit--reveal.
- **Filesystem drift.** Do not treat plan approval as rename approval.
- **Build-tree contamination.** Stop and inspect before deleting or replacing
  any tracked generated file.
- **Scope expansion.** If implementation seems to require networking, fair
  exchange, poker refactoring, a general serialization framework, or a second
  manuscript, record it as future work and keep this slice narrow.

## 21. Required Final Work Report

At completion, report:

1. every file read;
2. every file created, changed, moved, or intentionally left untouched;
3. exact claims introduced, strengthened, weakened, or removed;
4. every assumption introduced or made explicit;
5. primary sources verified and claims they support;
6. exact build, test, sanitizer, adversarial, and LaTeX commands;
7. tool versions, results, warnings, failures, and skipped checks;
8. the selective-abort observation from each participant's perspective;
9. known cryptographic, implementation, transport, institutional, and
   research limitations;
10. worktree status relative to the preservation baseline; and
11. the next smallest coherent step.

Do not summarize the result merely as “secure” or “complete.” State exactly
what now executes, what is conditionally argued, and what remains open.
