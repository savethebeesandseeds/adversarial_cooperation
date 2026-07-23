# Primitive Registry

## Status Language

This registry records exact constructions and their evidence boundaries. It
does not turn a component into a theorem merely by naming it a primitive.
“Approved” below means approved for the stated educational slice, not approved
for production deployment.

## AC Unkeyed Hash Profile Version 1

### Purpose

Provide one explicit, fixed digest profile for teaching byte-level hashing and
for internal protocol constructions that separately state their own security
arguments. The profile is not a generic cryptographic framework.

### Definition

For a byte string `m`, compute

```text
h = BLAKE2b-256(m).
```

The input is a length-known byte string and the output is exactly 32 bytes. A
caller that hashes structured data must first define a canonical typed or
length-delimited encoding and, where protocols share a backend, a distinct
domain. Raw concatenation does not supply either property.

### Security Notions and Evidence Status

- Preimage, second-preimage, and collision resistance are distinct
  computational notions whose exact games and challenge distributions must be
  stated where they matter.
- The familiar classical work factors near `2^256` for a specified idealized
  target and `2^128` for an idealized generic collision are model-dependent
  estimates, not proofs about BLAKE2b or a surrounding protocol.
- One-bit diffusion observations and published test vectors establish only
  concrete implementation behavior for the tested inputs.
- RFC 7693 specifies BLAKE2 and labels the 32-byte BLAKE2b parameter set with a
  `2^128` collision-security target; the RFC explicitly makes no independent
  security assertion.

### Assumptions and Non-Properties

Any security-relevant use assumes a correct backend and a computationally
bounded adversary under the use site's stated experiment. This unkeyed profile
does not by itself provide confidentiality, anonymity, sender authentication,
message authentication, signatures, commitments, password protection, key
derivation, truth, authorization, or safe serialization.

### Implementation Backend and Dependency Boundary

- Language baseline: C11.
- Sole external C library: the repository's existing libsodium dependency.
- Backend call: libsodium's explicit unkeyed BLAKE2b implementation with a
  fixed 32-byte output.
- Public interface: `include/ac/hash.h`.
- Implementation: `src/primitives/hash.c`.

No custom BLAKE2 or MD5 implementation, second cryptographic library, or test
framework is introduced. Papers, DOIs, RFCs, and standards are references, not
runtime dependencies.

### Approved Uses and Artifacts

- Teaching appendix: `document/content/aprendix/hash_functions/`.
- Fixed public demonstration: `src/hash_functions/hash_demo.c`.
- Deterministic vectors: `test-vectors/hash-foundations-v1.md`.
- Wrapper tests: `tests/test_hash.c`.
- Existing protocol constructions may call their own reviewed backend path;
  this repair does not silently refactor or strengthen them.

### Known Misuse Patterns

- hashing a password or other low-entropy secret and assuming it is hidden;
- treating an unkeyed digest as a MAC or signature;
- hashing ambiguous concatenations or native C structure images;
- omitting protocol domain and version context;
- equating digest equality with truth, authorship, or authorization;
- treating a diffusion example, vector, unit test, or sanitizer run as a
  cryptographic proof; and
- using MD5 where collision resistance is required.

## AC Commitment Version 1

### Purpose

Bind one participant to a byte string before disclosure while making recovery
of a low-entropy payload depend on a fresh, secret, high-entropy nonce. The
first user is the Two Oracles Play Rock--Paper--Scissors chapter.

### Definition

For public context `ctx`, payload `m`, and a fresh 32-byte nonce `r`, compute

```text
C = BLAKE2b-256(EncodeV1(ctx, r, m)).
```

`EncodeV1` is canonical and contains, in order:

1. the exact 16-byte scheme domain `AC-COMMITMENT-V1`;
2. scheme version;
3. length and value of the protocol identifier;
4. protocol version;
5. length and value of the session identifier;
6. round;
7. committer and recipient roles;
8. payload type;
9. nonce length and nonce; and
10. 64-bit payload length and payload.

Unsigned integers are big-endian. No C structure image, pointer, native
endianness, padding, NUL terminator, or ambiguous concatenation is hashed.

### Security Notions and Evidence Status

- **Conditional correctness:** recomputing the same canonical bytes with the
  same nonce produces the same digest. This is established as an implementation
  invariant by deterministic vectors and tests.
- **Computational hiding:** argued only in a random-oracle-style model when the
  nonce is independently sampled, unpredictable, 256 bits long, secret until
  opening, and never reused. The three-value RPS move has insufficient entropy
  to hide itself. This is a security argument, not a formal reduction for the
  concrete hash function.
- **Computational binding:** argued from the difficulty of finding a collision
  or an alternate opening in the canonical encoding. This is not a completed
  reduction or theorem for the exact construction.
- **Knowledge of an opening:** not provided at commit time. Any 32-byte digest
  has valid syntax; its sender may know no opening.
- **Authentication, non-repudiation, fairness, delivery, and availability:**
  not provided.

The construction is never described as information-theoretically secure,
post-quantum secure, composable, or production-ready.

### Assumptions

- classical computationally bounded adversary for the stated arguments;
- fresh, secret, unpredictable 32-byte nonce;
- protected local move and nonce until reveal;
- correct BLAKE2b implementation and successful library initialization;
- collision/second-preimage behavior and the explicitly labeled
  random-oracle-style hiding model;
- canonical context agreed by both parties; and
- no endpoint-memory or relevant side-channel compromise.

### Implementation Backend and Dependency Boundary

- Language baseline: C11.
- Sole external C library for this primitive: libsodium, already used by the
  repository.
- Hash: unkeyed BLAKE2b with a 32-byte digest through libsodium.
- Randomness: `randombytes_buf()` in ordinary execution.
- Comparison/clearing: libsodium constant-time comparison and clearing helpers
  where applicable.
- Deterministic path: caller-supplied nonce, explicitly limited to tests and
  reproducible vectors.

The protocol definition is independent of libsodium. A DOI, paper, or RFC in
the bibliography is documentation, not executable code and not a dependency.
No second cryptographic library and no custom production hash implementation
is introduced.

RFC 7693 labels the 32-byte BLAKE2b parameter set with a `2^128` collision-
security target while explicitly making no independent security assertion for
the algorithm. This registry records the parameter label, not a proof of the
hash or commitment construction.

### Approved Uses

- educational RPS commit--reveal;
- deterministic protocol vectors and mutation tests; and
- later educational protocols only after their contexts and leakage are
  independently reviewed.

It is not approved as a general production commitment API or as a drop-in
replacement for a commitment scheme whose formal proof is required.

### Current Users and Artifacts

- Chapter: `document/content/two_oracles_play_rock–paper–scissors/`.
- Public interface: `include/ac/commitment.h`.
- Implementation: `src/primitives/commitment.c`.
- Deterministic vector: `test-vectors/commitment-v1.md`.
- Tests: `tests/test_commitment.c`.
- Future explanatory appendix: Commitments, after the Hash Functions appendix
  has been repaired.

### Required Test Evidence

- exact independently reproduced preimage and digest vector;
- successful create/verify path;
- mutations of every context field, payload, nonce, and digest reject;
- empty-payload and null/length policy;
- deterministic supplied-nonce path; and
- explicit nonce cleanup API.

Passing these tests establishes encoding and implementation behavior. It does
not prove hiding or binding.

### Known Misuse Patterns

- hashing `move` without a secret high-entropy nonce;
- nonce reuse, prediction, early disclosure, or logging;
- omitting protocol, version, session, round, role, or payload-type context;
- ambiguous string concatenation or native-structure serialization;
- using a keyed hash whose key the verifier does not possess;
- treating a digest as proof that its sender knows an opening;
- treating a session identifier or role byte as authentication;
- treating a failed reveal as evidence of motive;
- reporting a unit test as a cryptographic proof; and
- calling this educational construction production-ready.

### References

RFC 7693 specifies BLAKE2. Naor's 1991 work supplies formal commitment
background but studies a different construction. The exact v1
nonce-and-hash construction therefore retains the limited argument status
above; see `research/BIBLIOGRAPHY_QUEUE.md`.
