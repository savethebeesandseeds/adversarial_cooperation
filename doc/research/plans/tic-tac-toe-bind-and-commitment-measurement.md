# Execution Plan: Tic-Tac-Toe `Bind` and Commitment Measurement

## Status

Complete, with one recorded verification exception. This plan covers one
bounded bridge from the completed Tic-Tac-Toe `Core` circuits to an exact
external policy-commitment profile and an executable cost model for `Bind`.

It does not emit the full `Bind` circuit, implement MPC in the head, construct
a proof transcript, or establish zero knowledge, knowledge soundness,
extraction, privacy, fairness, authorship, or production security.

## Objective

Complete the roadmap gate immediately following `Core`:

1. select and version the external policy commitment;
2. define its exact context, payload, public input, witness, and opening;
3. implement the profile by reusing the repository's existing educational
   BLAKE2b-256/libsodium commitment primitive;
4. construct an exact symbolic XOR/AND cost model for one frozen,
   minimum-AND ripple-carry representation of `Bind`;
5. measure `Bind` separately and then its conjunction with `Core-X` and
   `Core-O`;
6. leave the proof-internal view-commitment profile unselected while recording
   only in the plan and manuscript a clearly conditional digest-and-nonce byte
   floor for the familiar three-view shape; and
7. stop before importing any proof backend.

No new runtime or build dependency is permitted. RFC 7693 and the official
libsodium documentation already present in the bibliography are sufficient
for the active implementation description.

## Scientific Boundary

The external commitment is an educational hash-and-nonce construction. Its
binding discussion relies on collision resistance for the exact encoding. Its
hiding discussion requires a fresh secret 32-byte nonce and a separately
stated random-oracle-style assumption; collision resistance alone does not
establish hiding.

The symbolic model walks the exact operation shape and depth of a frozen
Boolean representation, but it does not materialize, serialize, or evaluate
the roughly 32-million-gate circuit. It is an exact count for that
construction, not a lower bound, optimized backend estimate, formal
verification result, or proof cost.

The proof-internal commitments to simulated views are not the external policy
commitment checked by `Bind`. No proof-internal primitive, domain, encoding,
or profile is selected in this slice. The conditional byte floor below is not
a construction or proof-size estimate. The view encoding, full
malicious-verifier transformation, extractor, and total soundness reduction
remain open.

## Frozen External Profile

### Public instance

The packed public input is 72 bytes:

```text
[game version:1]
[claimant role:1]
[initial board index:u16be]
[session identifier:32]
[statement round:u32be]
[policy commitment digest:32]
```

Version 1 accepts only game version 1, role X or O, initial board zero, and a
nonzero session identifier. The first four bytes are shared with the existing
`Core` public input. The application, rather than this encoding, must enforce
session uniqueness and meaningful statement-round allocation.

### Witness

The packed witness is 19,715 bytes:

```text
[canonical policy:19,683]
[commitment nonce:32]
```

The policy prefix is shared with the existing `Core` witness input. Calling
these bytes a witness does not make them secret in the current C APIs.

### Commitment context

Use the existing `ac_commitment_context` encoding with:

```text
protocol identifier = "AC-TTT-POLICY-V1" (16 bytes)
protocol version    = 1
session identifier = public 32-byte session
round              = public statement round
committer role     = 1 (fixed PROVER protocol label)
recipient role     = 2 (fixed VERIFIER protocol label)
payload type       = 1 (canonical policy v1)
```

The commitment payload is exactly:

```text
[the four Core public bytes][the 19,683 policy bytes]
```

The existing generic commitment encoding therefore hashes exactly 19,809
bytes:

```text
16  domain bytes
 2  commitment-scheme version
 2  protocol-id length
16  protocol identifier
 2  protocol version
 2  session-id length
32  session identifier
 4  round
 1  committer role
 1  recipient role
 2  payload type
 2  nonce length
32  nonce
 8  payload length
19687 payload
```

Unkeyed BLAKE2b with a 32-byte digest and a 128-byte block therefore executes
155 compression calls for this fixed message length.

## Frozen Symbolic `Bind` Construction

Use the existing XOR/AND basis and packed least-significant-bit-first input
convention. Constants are aliases to wires zero and one. Message parsing,
fixed parameters, counters, rotations, and byte/word permutations rewire
signals and emit no gates.

Each 64-bit addition uses the minimum-AND ripple relation

```text
carry_out = ((a XOR carry) AND (b XOR carry)) XOR carry
sum       = (a XOR carry) XOR b
```

Bit zero uses the one-AND/one-XOR half-adder implied by carry-in zero. Bits 1
through 62 use the full relation, and bit 63 emits only its two-XOR sum because
the final carry is discarded. One addition therefore emits 63 AND gates and
251 XOR gates. This choice minimizes AND count for the selected transparent
construction at the cost of great depth; it is not asserted to be globally
optimal.

For each BLAKE2b compression:

- 96 `G` applications execute six 64-bit additions and four 64-bit XORs each;
- final feed-forward emits 1,024 XOR gates;
- the result is 36,288 AND and 170,176 XOR gates.

After 155 blocks, compare the first 256 state bits with the public digest,
reduce the 256 equality bits, check that the 256-bit session identifier is
nonzero, and AND the two results.

The independently derived target report is:

| Quantity | `Bind` |
|---|---:|
| Public input bits | 576 |
| Witness input bits | 157,720 |
| BLAKE2b blocks | 155 |
| AND gates | 5,625,151 |
| XOR gates | 26,378,049 |
| Total gates | 32,003,200 |
| Wires, including two constants | 32,161,498 |
| Output depth | 1,558,999 |
| Flat 12-byte gate storage | 384,038,400 bytes |
| One-byte-per-wire evaluator scratch | 32,161,498 bytes |
| Canonical nine-byte-gate serialization | 288,028,832 bytes |

The conjunction adds one AND gate to a role-specialized `Core`:

| Quantity | `Core-X AND Bind` | `Core-O AND Bind` |
|---|---:|---:|
| AND gates | 5,840,174 | 5,834,465 |
| XOR gates | 26,409,399 | 26,405,393 |
| Total gates | 32,249,573 | 32,239,858 |
| Wires | 32,407,871 | 32,398,156 |
| Output depth | 1,559,000 | 1,559,000 |
| Flat gate storage | 386,994,876 bytes | 386,878,296 bytes |
| Canonical serialization | 290,246,189 bytes | 290,158,754 bytes |

These counts make the central result visible: in this direct Boolean route,
faithfully binding the policy costs far more than checking the finite game.

## Unselected Proof-Internal Commitment

This slice does not select a proof-internal commitment primitive or assign it
a protocol identifier. External policy binding and future simulated-view
commitments have different meanings, lifetimes, encodings, openings, and
security requirements; they must not silently share this profile.

Conditioned on a future base three-view construction using one 32-byte digest
per view and one 32-byte opening nonce per revealed view, `r` repetitions
would contain at least:

- `3r` first-message digests, or `96r` bytes;
- `2r` opened nonces, or `64r` bytes;
- `2r` opened view payloads, whose lengths are not yet known; and
- `r` challenges before any transform-specific packing.

At the already recorded illustrative `r=219`, the conditional digest bytes are
21,024 and the fixed opened-nonce bytes are 14,016. The 35,040-byte sum
excludes challenges, views, metadata, the external policy commitment, and any
full-zero-knowledge/knowledge transformation. It is not a proof-size claim.
It is deliberately not exposed as a C API because no proof-internal
commitment profile has been selected.

## Exact File Scope

Create:

1. `research/plans/tic-tac-toe-bind-and-commitment-measurement.md`
2. `include/ac/ttt_bind.h`
3. `src/protocols/ttt_bind.c`
4. `src/tic_tac_toe_without_revealing_the_strategy/ttt_bind_demo.c`
5. `tests/test_ttt_bind.c`
6. `test-vectors/tic-tac-toe-bind-v1.txt`

Modify:

7. `src/Makefile`
8. `tests/Makefile`
9. `demostrations/Makefile`
10. `src/README.md`
11. `document/content/tic_tac_toe_without_revealing_the_strategy/tic_tac_toe_without_revealing_the_strategy.tex`
12. `research/PRIMITIVE_REGISTRY.md`
13. `research/CHAPTER_MATRIX.md`
14. `research/CLAIM_LEDGER.md`
15. `research/ASSUMPTION_LEDGER.md`
16. `research/OPEN_PROBLEMS.md`
17. `research/DECISIONS.md`
18. `research/ROADMAP.md`

No other file is in scope. In particular, preserve byte-for-byte:

- the generic commitment implementation and its vectors;
- the recursive and fixed-DAG TTT evaluators;
- `bool_circuit` and both `Core` circuits;
- every completed plan;
- bibliography and bibliography queue;
- main-book include graph and `book`;
- backups, generated artifacts, and repository metadata.

## Implementation Sequence

1. Freeze the exact external context and encoding in a narrow public header.
2. Implement create, deterministic compute, verify, and public/witness
   encoders by composing the existing commitment primitive.
3. Implement the symbolic depth/count walk with checked final arithmetic.
4. Publish and independently reproduce one deterministic profile vector.
5. Add negative tests for role, session, policy, nonce, digest, context, null
   arguments, and output clearing.
6. Add a separate human-readable demonstration that prints an unmistakable
   non-production/no-proof warning.
7. Update the chapter and named research registers.
8. Run strict tests, complete regression, sanitizers, dependency inspection,
   forbidden-call scans, standalone chapter build, and full-page visual review.
9. Guard copyback by preimage hash and exact-scope checks.

## Verification Contract

Run in the existing disposable Debian 12 environment:

```text
make -C tests ttt_bind CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C tests test CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C demostrations demo_ttt_bind CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C tests sanitize CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O1 -g'
```

Also:

- independently reproduce the vector with Python's standard-library
  `hashlib.blake2b` using the byte contract recorded in the vector file;
- independently reproduce the count formulas outside the C implementation;
- require the existing dependency-free `demo_ttt` and Core tests to remain
  free of libsodium linkage;
- inspect the new binary's dynamic dependencies and source calls;
- build `section_ttt` until citations stabilize;
- scan the final log for errors, undefined references/citations, missing
  glyphs, and overfull boxes;
- render and inspect every final page;
- verify protected hashes and exact file scope before guarded copyback.

## Acceptance Boundary

Completion establishes the exact external profile, executable profile
behavior, one deterministic vector, and exact symbolic counts for the frozen
construction. It does not establish a functioning private proof or any
cryptographic property by execution alone.

## Staged Completion Record

Acceptance verification ran on 2026-07-24 in the disposable Debian 12
x86-64 environment with GCC 12.2.0 and libsodium 1.0.18.

- The strict focused Bind target passed 9 test groups under C11 warnings as
  errors.
- The complete strict regression passed all 71 grouped tests: commitment 7,
  RPS 6, adversarial RPS 14, hash 6, Tic-Tac-Toe 13, Boolean circuit 13,
  Tic-Tac-Toe Core 3, and Bind 9.
- The focused Bind target passed the same 9 groups under ASan/UBSan. GCC
  `-fanalyzer` also compiled and ran those 9 groups without a diagnostic.
- The optional complete ASan/UBSan regression was attempted, but exceeded the
  120-second acceptance window while executing the pre-existing
  `test_bool_circuit` binary. It was terminated with the disposable container.
  No full-suite sanitizer result is claimed; the touched Bind path is covered
  by the completed focused sanitizer run.
- Both strict demonstrations ran. The original dependency-free Tic-Tac-Toe
  binary links only libc; the separate Bind binary links only the existing
  libsodium backend and libc.
- The new C/vector files contain no `rand`, `srand`, MD5, SHA-1, heap-allocation
  call, or non-ASCII byte. No three-view estimator or proof-internal
  commitment profile remains in the public C API.
- Independent Python `hashlib.blake2b` reconstruction produced a 19,687-byte
  payload, a 19,809-byte framed message, 155 blocks, and digest
  `b53130b6c3c2ebf8ca9ffb05358a61de820ee465587dafac7720060e06b51a00`.
  Independent arithmetic reproduced 36,288 AND and 170,176 XOR gates per
  compression.
- The final standalone chapter PDF has SHA-256
  `de210700ffb9dc5bdaca202e47a1b919fd312ca7a51e4e11f2cfdc4ffed733e6`,
  24 pages, and 320,819 bytes. Its stabilized log has no LaTeX/package
  warning, undefined reference/citation, overfull box, missing glyph, or fatal
  match. Three underfull-vbox notices remain; every page was rendered and
  inspected, and none produces clipping, overlap, or an unreadable layout.
- The full staged-tree comparison found 188 byte-identical unscoped files and
  zero unexpected changed or new files.
- The final independent review found no remaining P0, P1, or P2 defect. An
  earlier review prompted two conservative corrections: the bit-zero
  half-adder is now explicit, and hypothetical three-view arithmetic remains
  in research prose rather than becoming a public API.

## Guarded Delivery Record

Guarded copyback completed on 2026-07-24. Immediately before delivery, the
authoritative repository was clean at
`ce3af795940b05b79b4d50cc28b6e97ad188acbb`; all 12 recorded preimage hashes
matched; all six new destinations were absent; and the staged-tree comparison
reported zero unscoped differences. Exactly 18 files were copied. Every
authoritative destination then matched its staged SHA-256 hash, and Git status
showed exactly the planned 12 modified and six new files, with no generated
artifact. No commit or push was performed.
