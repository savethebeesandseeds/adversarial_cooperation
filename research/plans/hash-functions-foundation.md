# Hash Functions Foundation Repair

## Status

Active execution plan for the bounded Hash Functions appendix repair. This
document records preservation, sources, design decisions, test evidence, and
remaining limits. A checked item means the result was actually obtained.

## Intended Outcome

Repair the project's Hash Functions appendix so that a reader can distinguish:

- a hash function from a one-way-function claim;
- preimage, second-preimage, and collision resistance;
- idealized generic work factors from guarantees about a concrete algorithm;
- an empirical diffusion example from a security proof;
- an unkeyed digest from a MAC, signature, commitment, password-hashing
  scheme, KDF, anonymity mechanism, or encryption scheme; and
- executable implementation evidence from cryptographic evidence.

Add the smallest C companion that demonstrates the repository's already
selected BLAKE2b-256 backend. It must add no external library: C11 plus the
existing libsodium dependency remains the complete C dependency set.

## Progress

- [x] Live status and affected-file hashes recorded.
- [x] Active appendix compared with its `.bak` file.
- [x] Isolated writable staging copy created.
- [x] Primary-source gate completed for definitions, BLAKE2, MD5, HMAC, and
  the implementation API.
- [x] Exact C API and deterministic vectors frozen.
- [x] Hash wrapper, demonstration, and tests implemented.
- [x] Appendix rewritten and bibliography/registers updated.
- [x] Strict build, regression tests, sanitizers, and independent vectors pass.
- [x] Standalone appendix PDF builds and every page passes visual inspection.
- [x] Guarded copyback and final status verification complete.
- [x] Final report delivered with claims separated from observations.

## Preservation Baseline

The authoritative checkout is:

`C:\Work\applied_cryptography\adversarial_cooperation`

At implementation start it remains on `main` at
`2824992ee2802b0b72f67054f63189628eb15103` with the completed RPS slice
uncommitted, the Phase 0 research files untracked, and the older VSIX deletion
already present. All of that state is user-owned. Do not restore the VSIX,
discard or stage any file, create a commit, or alter unrelated material.

The active hash appendix differs from `hash_functions.tex.bak` only in two
minor spelling corrections (`pre-image` and `easier`). The backup contains no
unique substantive idea, but it remains the preserved original draft and must
not be modified or deleted.

Do not rename `aprendix` in this task. Do not change the global generated-file
policy. Verification output belongs in disposable directories.

Before copyback, every pre-existing destination must still match its recorded
SHA-256 baseline. New destinations must still be absent. Copy only an explicit
file list and verify every copied file byte-for-byte.

## Scope

### New artifacts

- `include/ac/hash.h`
- `src/primitives/hash.c`
- `src/hash_functions/hash_demo.c`
- `tests/test_hash.c`
- `test-vectors/hash-foundations-v1.md`
- `research/plans/hash-functions-foundation.md`

### Existing artifacts that may change

- `document/content/aprendix/hash_functions/hash_functions.tex`
- `document/references.bib`
- `book`
- `src/Makefile`
- `src/README.md`
- `demostrations/Makefile`
- `demostrations/README.md`
- `tests/Makefile`
- `README.md`
- `research/PRIMITIVE_REGISTRY.md`
- `research/BIBLIOGRAPHY_QUEUE.md`
- `research/DECISIONS.md`
- `research/CHAPTER_MATRIX.md`
- `research/CLAIM_LEDGER.md`
- `research/ASSUMPTION_LEDGER.md`
- `research/OPEN_PROBLEMS.md`
- `research/ROADMAP.md`

### Explicit non-goals

- no custom implementation of BLAKE2, MD5, SHA-2, SHA-3, HMAC, or a password
  hashing scheme;
- no second cryptographic library or test framework;
- no production cryptography or benchmark claim;
- no refactor of the existing commitment or poker hashing code;
- no Trust Establishment rewrite;
- no full-book include-graph repair;
- no filename migration or backup deletion; and
- no claim that one example, test vector, or sanitizer run proves security.

## Source and Contribution Gate

This is a literature-backed foundation and executable teaching companion. It
makes no novelty claim.

Use only each source's narrow supported role:

1. Phillip Rogaway and Thomas Shrimpton, “Cryptographic Hash-Function Basics:
   Definitions, Implications, and Separations for Preimage Resistance,
   Second-Preimage Resistance, and Collision Resistance,” FSE 2004,
   DOI `10.1007/978-3-540-25937-4_24`, for the fact that the three intuitions
   have multiple precise games and should not be conflated.
2. RFC 7693 for the BLAKE2b algorithm, digest-size parameter, and the RFC's
   stated target labels. Preserve the RFC's explicit refusal to make an
   independent security assertion.
3. RFC 6151 for MD5's historical collision failure and the conclusion that it
   is unsuitable where collision resistance is required.
4. RFC 2104 only to distinguish HMAC as a keyed construction for message
   authentication. It does not make a bare digest a MAC.
5. NIST SP 800-185 only as a concrete standards example of function naming,
   customization, and tuple-aware hashing. It does not require this project to
   adopt SHA-3 and does not validate an ad hoc encoding.
6. Official libsodium Generic Hashing documentation only for API/backend
   behavior and its explicit warning that BLAKE2b is not a password-hashing
   scheme.
7. Wang and Yu, “How to Break MD5 and Other Hash Functions,” EUROCRYPT 2005,
   DOI `10.1007/11426639_2`, for historical collision-attack background if the
   corrected collision exercise is retained. The exact vector bytes must also
   be independently executed before publication.

The bibliography and queue must say what each source does not establish.

## Scientific Contract

Use the pedagogical fixed-function notation

`H : {0,1}* -> {0,1}^n`

while warning that formal work often models a keyed family and that exact
challenge distributions matter. Give separate experiments:

- **Preimage:** a challenge digest is generated under a stated experiment;
  the adversary succeeds by returning any input with that digest.
- **Second preimage:** the adversary receives an input and succeeds by finding
  a different input with the same digest.
- **Collision:** the adversary chooses two distinct inputs with the same
  digest.

Never say inversion is impossible. Use “computationally difficult under the
stated experiment and resource bound.”

For an idealized uniformly behaving `n`-bit output, derive rather than merely
assert the generic classical scales: about `2^n` trials for a specified target
and about `2^(n/2)` sampled outputs for a constant collision probability.
These are model-dependent generic estimates, not automatic properties of an
arbitrary concrete hash function.

Treat the avalanche example as one measured Hamming distance. It illustrates
diffusion for those two messages only and establishes none of the three
resistance notions.

State these composition boundaries explicitly:

- a bare digest can reveal a low-entropy input by enumeration;
- an unkeyed digest cannot authenticate an active sender who can replace both
  message and digest;
- a MAC needs a secret key and a defined construction;
- a signature needs a signature scheme and signing key;
- a commitment needs hiding and binding mechanisms plus context;
- password storage needs a dedicated salted, costed password-hashing scheme;
- a KDF has a separate key-derivation contract; and
- hashing supplies neither encryption, confidentiality, nor anonymity.

The required section `What Cryptography Does Not Solve` must explain that a
correct digest cannot establish truth, authorship, authorization, justice,
safe parsing, trusted reference storage, or correct surrounding protocol use.

## C Companion Contract

### Public API

`include/ac/hash.h` exposes only:

- `AC_HASH_BLAKE2B_256_BYTES = 32`; and
- `ac_hash_blake2b_256(out, input, input_len)`.

The algorithm and output size are in the name. The wrapper must:

- call libsodium's explicit BLAKE2b implementation;
- accept the empty byte string without requiring a non-null input pointer;
- reject a null output or a null nonempty input;
- define non-overlapping input/output buffers as a precondition;
- clear the output on every detected failure;
- check `sodium_init()` and the hashing return value; and
- make no secrecy, password, MAC, signature, or commitment promise.

Do not expose libsodium state types or invent a second generic abstraction.
The existing commitment remains unchanged in this task.

### Teaching demonstration

`hash_demo` uses only fixed public examples and prints an unmistakable
educational warning. It must show:

1. BLAKE2b-256 of the empty byte string;
2. BLAKE2b-256 of `abc`;
3. two messages differing by one bit, their digests, and measured digest-bit
   difference, followed by the non-proof warning;
4. why `"ab" || "c"` and `"a" || "bc"` are the same raw bytes rather than a
   hash collision; and
5. why a protocol must define typed or length-delimited encoding before
   hashing structured data.

The demo must not accept passwords or claim to audit arbitrary files.

### Deterministic vectors and tests

Freeze and independently reproduce:

- BLAKE2b-256 of the empty string;
- BLAKE2b-256 of `abc`;
- BLAKE2b-256 of bytes `00` through `ff`;
- the two demonstration messages and measured bit difference; and
- if retained, the corrected historical 128-byte MD5 collision pair, its
  equal MD5 digest, and the two distinct BLAKE2b-256 digests.

The ordinary C test suite verifies the wrapper and BLAKE2b results only. It
must not add or implement MD5. A disposable independent standard-library check
may verify the historical MD5 vector and is verification tooling, not a runtime
project dependency.

Test groups must cover known vectors, empty/null policy, binary input,
determinism, output clearing on invalid input, and the exact demonstration
observations. `make -C tests test` must continue to execute commitment and RPS
tests unchanged.

## Manuscript Structure

1. Author's preserved intention and abstract
2. Function and digest definition
3. Three different resistance experiments
4. Idealized generic work factors
5. Diffusion is not a security proof
6. What a bare hash can and cannot do
7. Structured inputs, canonical encoding, and domain separation
8. BLAKE2b-256 companion profile
9. Historical MD5 failure and corrected collision exercise
10. C implementation companion
11. Tests and evidence boundary
12. What Cryptography Does Not Solve
13. Limitations, exercises, and references

The old prose is not silently promoted. Its claims remain traceable in the
claim ledger and the untouched `.bak`; the repaired chapter records which
claims were withdrawn, split, or qualified.

## Verification

Use a clean disposable Debian environment and record versions. Required checks:

```bash
make -C src CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C tests test CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C tests sanitize CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror'
make -C demostrations demo_hash
bash book -pdf section_hash
bash book -pdf section_rps
bash book -pdf section_poker
```

All new vector values must be independently recomputed. Inspect the chapter
log for errors, undefined citations/references, missing glyphs, and overfull
boxes. Render every page to PNG and inspect typography, equations, code blocks,
tables, bibliography, headers, footers, and page transitions. Delete disposable
PDF/render/build artifacts afterward.

## Verification Record

All checks below used disposable output directories or containers; no generated
artifact was written into the staged or authoritative checkout.

- Environment: Debian 12, GCC 12.2.0, GNU Make 4.3, libsodium 1.0.18.
- Fresh all-target C build: poker, RPS, and Hash Functions compiled with
  `-std=c11 -Wall -Wextra -Wpedantic -Werror -O2`.
- Ordinary tests: 33 grouped tests passed with zero failures: 7 commitment,
  6 RPS core, 14 RPS adversarial, and 6 hash tests.
- Sanitizers: the same 33 groups passed under AddressSanitizer and
  UndefinedBehaviorSanitizer with leak detection and halt-on-error enabled.
- Demonstrations: hash, honest RPS, selective-abort RPS, and the unchanged
  poker demo all exited successfully. The hash demo printed the frozen values,
  the RPS abort demo retained the explicit fairness counterexample, and poker
  reached valid showdown verification.
- Dynamic-link inspection showed only libsodium and ordinary system runtime
  libraries; the slice added no second cryptographic or test dependency.
- Independent Python 3.12 standard-library reproduction passed for the empty,
  `abc`, and `00`--`ff` BLAKE2b-256 vectors; both historical inputs produced
  MD5 `79054025255fb1a26e4bc422aef54eb4`; their BLAKE2b-256 digests were
  distinct; and the inputs differed in six bits.
- LaTeX: `section_hash` compiled to a ten-page PDF with no final-pass warnings,
  undefined citations/references, underfull/overfull boxes, or errors. All ten
  rendered pages were inspected at 150 DPI after the final layout adjustment.
- A separate read-only adversarial review checked the final C/API, manuscript,
  vectors, bibliography, build integration, and registers. Its scientific and
  traceability findings were corrected; the final pass reported no remaining
  P0, P1, or P2 finding.
- Regressions: `section_rps` compiled to 18 pages and `section_poker` to three.
  Poker's final log was clean. RPS retained two pre-existing underfull-vbox
  notices but had no undefined citation/reference or build error.
- Backup: `hash_functions.tex.bak` remained byte-identical at SHA-256
  `5c3e476bac68424f20959d4e0541ab0cdd94e7329569c5c98f377f95992731ad`.
- Guarded copyback: 18 pre-existing live destinations matched their recorded
  SHA-256 baselines, all six new destinations were absent, and 23 explicit
  files were copied and then matched byte-for-byte. Live HEAD remained
  `2824992ee2802b0b72f67054f63189628eb15103`; no file was staged or committed,
  and the pre-existing VSIX deletion was left untouched.

These observations establish build, vector, state-machine, and layout behavior
in the recorded environment. They do not strengthen the chapter's conditional
cryptographic claims.

## Acceptance and Evidence Boundary

The slice is provisionally complete only when:

- no new dependency exists;
- the `.bak`, RPS behavior, poker behavior, and user-owned status are preserved;
- every printed digest matches the frozen independently checked vectors;
- strict builds, all old and new tests, and supported sanitizers pass;
- the MD5 material is historical and never linked as production code;
- the standalone appendix has resolved citations and visually clean pages;
- registers state actual evidence rather than planned evidence; and
- the final report separates definitions, conditional security statements,
  implementation observations, and remaining open questions.

Passing all checks proves only that the tested implementation produced the
specified bytes and handled the enumerated cases in the recorded environment.
It does not prove that BLAKE2b is collision resistant, preimage resistant, or
appropriate for an unstated higher-level protocol.
