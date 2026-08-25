# Execution Plan: Tic-Tac-Toe Canonical Reference Contract

## Status

Complete on 2026-07-31. The existing Tic-Tac-Toe work is now the book's
canonical running example for separating a hidden object, a semantic
predicate, temporal binding, a private proof, and protocol lifecycle.

The revision combines a manuscript reference contract with one bounded
executable fixture. It changes no protocol bytes, public C API, circuit,
cryptographic primitive, or proof backend. It adds no library. It adds a
deterministic valid-policy vector and demo mode so one exact policy can be
followed through the recursive checker, fixed DAG, Boolean Core, and disclosed
Bind opening. It may remove the already-unused OpenSSL development package
from `setup.sh` after a repository-wide source and linkage check confirms that
libsodium remains the sole external C backend.

## Objective

Make the chapter safe and useful for later chapters to cite without allowing a
reader to inherit claims that Tic-Tac-Toe does not establish:

1. present a plain-language map before implementation detail;
2. define the reusable `Core`--`Bind`--proof separation;
3. distinguish an existential one-shot policy claim from a committed-policy
   claim;
4. state the future completeness, extraction, and zero-knowledge experiments
   as targets rather than achieved theorems;
5. define the external publication lifecycle needed for "fixed earlier";
6. add stable labels and numbered propositions;
7. add one concrete board-index/policy-lookup example and an exercise on-ramp;
8. correct documentation and ledger drift;
9. add one reproducible positive shared-witness fixture while preserving the
   existing invalid-policy separation vector;
10. promote the chapter into the current main-manuscript include graph only if
   both standalone and full-book builds stabilize; and
11. preserve the minimal dependency boundary.

## Scientific Boundary

The chapter remains an educational research example. The revision must not
claim that the repository currently provides:

- policy privacy;
- zero knowledge;
- knowledge extraction;
- commitment binding or hiding as a theorem;
- authenticated chronology;
- fairness, delivery, identity, or replay prevention;
- a selected proof backend;
- production security; or
- novelty.

The fixed Version 1 BLAKE2b-256 profile is one concrete educational point, not
an asymptotic security family. A backend-neutral target may be parameterized by
`lambda`, but no concrete Version 1 security theorem follows from that target.

The external commitment is optional for the weaker one-shot claim that the
prover knows some policy satisfying `Core`. It is required only when the
statement must identify a policy fixed before a later event, reused across
declared proofs, or opened later. Such temporal meaning additionally requires
an externally enforced ordered publication record; session and round bytes do
not create chronology or identity by themselves.

## Canonical Reference Contract

The chapter will expose five reusable layers:

1. **Policy:** the exact canonical hidden object.
2. **Core:** the public semantic predicate applied to that object.
3. **Bind:** the relation connecting the same object to a public commitment
   when temporal identity is required.
4. **Proof:** a future argument that establishes the selected relation while
   meeting separately stated knowledge and zero-knowledge experiments.
5. **Lifecycle:** publication, authentication, replay handling, reuse, abort,
   opening, expiration, and released-output leakage outside the relation.

Later chapters may reuse this decomposition only by defining their own object,
model, predicate, leakage, adversary, setup, and lifecycle. They inherit no
Tic-Tac-Toe security conclusion by analogy.

## Exact File Scope

Create:

1. `research/plans/tic-tac-toe-canonical-reference-contract.md`
2. `test-vectors/tic-tac-toe-reference-v1.txt`

Modify:

3. `document/content/tic_tac_toe_without_revealing_the_strategy/`
   `tic_tac_toe_without_revealing_the_strategy.tex`
4. `document/utils/math_utils.tex`
5. `document/adversarial_cooperation.tex`
6. `book`
7. `README.md`
8. `demostrations/README.md`
9. `src/README.md`
10. `setup.sh`
11. `src/tic_tac_toe_without_revealing_the_strategy/ttt_bind_demo.c`
12. `src/Makefile`
13. `demostrations/Makefile`
14. `tests/test_ttt_bind.c`
15. `tests/Makefile`
16. `research/BIBLIOGRAPHY_QUEUE.md`
17. `research/ASSUMPTION_LEDGER.md`
18. `research/CLAIM_LEDGER.md`
19. `research/CHAPTER_MATRIX.md`
20. `research/DECISIONS.md`
21. `research/OPEN_PROBLEMS.md`
22. `research/ROADMAP.md`
23. `document/utils/text_utils.tex`
24. `compile_latex.sh`

No public header, protocol/circuit implementation, existing test vector,
bibliography entry, backup, or other chapter is in scope. The published
synthetic invalid-policy vector remains unchanged because it demonstrates the
important fact that Bind acceptance does not imply Core acceptance.

## Authoritative Preimages

Before this pass, the authoritative destination had the following SHA-256
preimages. Guarded copyback must refuse any existing file whose hash changes:

```text
17a7a5b2fa8e5af367cc74727bfebcdf25973928dd6af496cad4c00354f2caf8  document/content/tic_tac_toe_without_revealing_the_strategy/tic_tac_toe_without_revealing_the_strategy.tex
63c2413aad5ddd10c88c831c32821f6e47a59496f958ed67a5d577010b1759bc  document/utils/math_utils.tex
5ea562cb9149a1a9b20226a89cb1d122747fafd6204c0b6ed3c4527dea7654be  document/adversarial_cooperation.tex
09a90cf6f85842b781992c99183bcc9344aca1d4c4ec9ff8ef607a42ef4b8cd1  book
21dd0bf58ba0c58635d876d0d04c105349528acd045f586d0a04353bd4de3f57  README.md
9c6599daa041ec24d444000e62407e17471694a8ad70965e16e6756ffbfadf51  demostrations/README.md
ecf5f301b46df52140ded8f50c63475e78ad738a6947baa1b5bdb98bf29a8a0e  src/README.md
565150a7370cba5cd1d3ba5217c6e2dd5a71db3cc8a414a6baf51160cdaf1eb0  setup.sh
d67b157c51d548cdfbad6feaf163581be128df3657bc678e03c77c87a0b551d2  src/tic_tac_toe_without_revealing_the_strategy/ttt_bind_demo.c
2af8e26a21791c8504ffbb73ecbd17c02444cca22f60c6965a9886cd2fe51a4e  src/Makefile
711968caf0adfbb48b1cf8c8a341b54bfaf42c7aeea983359a7e0a28b98df0b0  demostrations/Makefile
b04061699a3ccf2ca350ee054af025e757c510eb980351318c708f2a8886ac27  tests/test_ttt_bind.c
9d267d085a75e23c93975197cc847a7c3945ae3099c131462831f4dbc6b641ab  tests/Makefile
30226ccaedd1e8f72ca2006bb3f4becdb69059b42037dc597eb0cb7962dcdb36  research/BIBLIOGRAPHY_QUEUE.md
9279c7547fcefbba157fd94e27f1caf7ac007c9da571b8164a4d8ee3e6c4d436  research/ASSUMPTION_LEDGER.md
02dda83c97d214003732a4e2d6cc5c8a71602ca1cdb1690b060aa58656063517  research/CLAIM_LEDGER.md
a5bf49ba466a2472d967bef6a9066855785672c81a26331655b827df76d83b1b  research/CHAPTER_MATRIX.md
423b283f857870fa7fb4935a30516cced612d45d34440471a2063318f856f0eb  research/DECISIONS.md
6af1843a39d7f834d6ca2593c8613f0b0f323c68cc34c7a60126a1012f0d98df  research/OPEN_PROBLEMS.md
0ae4f3e816618b9e68adfffe362acc405c94c384560a254da2f48b1593e3cb03  research/ROADMAP.md
c14ef0604f9ec7991d764049d8852a7e04e4d09b3851b11ac846866a51ef289b  document/utils/text_utils.tex
f53bfa41484e694dc2366bcd0ce6e99ca7b72015c92c8554a9ebc10c2e96d1a2  compile_latex.sh
```

The plan and positive reference vector were absent at the destination.

## Work Sequence

1. Preserve and hash the authoritative preimages.
2. Add the concept ladder, concrete board example, stable labels, numbered
   propositions, two statement modes, target experiments, and publication
   lifecycle.
3. Add a concise reusable shared-witness box and intuitive cost comparison.
4. Group the exercises by entry level without removing the existing research
   questions.
5. Add a deterministic valid X reference-policy fixture. Require the same
   policy bytes to pass the recursive checker, fixed DAG, emitted Core-X
   circuit, and fixed Bind opening. Preserve fresh randomness as the default
   demo mode and add an explicit reproducible `--reference-vector` mode.
6. Correct the two overbroad commitment rows and all discovered documentation
   drift.
7. Record the chapter's canonical role and the rule against transferring its
   assumptions to other problems.
8. Remove `libssl-dev` from setup only after confirming there is no OpenSSL C
   use or dynamic linkage.
9. Build the standalone chapter and full manuscript until references and
   citations stabilize.
10. Run the focused tests, strict complete C regression, both demo modes, and
    focused sanitizers.
11. Render and inspect every page of both final PDFs; obtain independent final
    scientific, scope, and visual reviews.
12. Copy back only the permitted files whose authoritative preimages still
    match.

## Acceptance Criteria

The slice is complete only if:

- a new reader can locate every layer of the canonical example from one early
  table;
- the chapter states both existential and committed modes without conflation;
- target completeness, extraction, and simulation experiments are explicit
  and visibly unimplemented;
- an ordered authenticated publication event is required for temporal claims;
- important definitions, equations, propositions, cost tables, and limitation
  sections have stable labels;
- one deterministic vector uses a semantically valid non-losing X policy, and
  its exact bytes are shared across recursive, DAG, Core, and Bind checks;
- the default Bind demo still uses fresh randomness while its explicit
  reference mode is byte-reproducible;
- the chapter is included in the main manuscript and both PDFs build cleanly;
- the public/Core targets remain dependency-free and Bind remains the only TTT
  target using libsodium;
- OpenSSL is neither referenced by source nor installed by `setup.sh`;
- all existing and new grouped tests pass under strict warnings; and
- no claim, assumption, protocol byte, executable source, or dependency is
  silently strengthened.

## Verification Record

Completed on 2026-07-31 against the final staged sources.

- **Scientific review:** independent review returned GO with no unresolved
  P0/P1/P2/P3 finding. The final statement relation is not itself called a
  knowledge claim; completeness quantifies parameters sampled by
  `Setup(1^lambda)`; the argument-of-knowledge target requires an efficient
  extractor, a stated knowledge/extraction error, and a valid extracted
  witness; and the zero-knowledge target requires a PPT simulator receiving
  the auxiliary input and declared public outputs.
- **C regression:**
  `make -C tests clean && make -C tests test
  CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'` passed all 72
  grouped tests. The focused canonical path passed 39/39 checks, and the
  focused ASan/UBSan Bind run passed 10/10. A later complete-suite sanitizer
  rerun exceeded ten minutes without a diagnostic, so no fresh complete-suite
  sanitizer result is claimed.
- **Reference fixture:** the same valid X policy passed the recursive checker,
  fixed DAG, emitted Core-X Boolean evaluator, and disclosed Bind opening.
  Independent standard-library computation reproduced 5,478 reachable states,
  2,423 required X entries, the 19,683-byte policy, 19,687-byte payload,
  19,809-byte framed message, and digest
  `4538c875f4e919665030e7950b0ea92c2df4f194c10f2e021fd70363e722f378`.
  The ordinary demo still obtains a fresh nonce; only `--reference-vector`
  selects the public deterministic fixture.
- **Dependencies:** public and Core executables link only libc; Bind links libc
  and the already-existing libsodium. Repository source and dynamic-link scans
  found no OpenSSL use, and `setup.sh` no longer installs `libssl-dev`.
- **Documents:** `bash book -pdf section_ttt` produced 32 pages, 408,308 bytes,
  SHA-256
  `f1e3bde9d419011735eb8f1f08e4857ef8e51d7aa127243333c2f60a76104d7b`.
  `LATEX_OUTDIR=.temp-main-final4 bash compile_latex.sh -s
  document/adversarial_cooperation.tex` produced 49 pages, 472,601 bytes,
  SHA-256
  `5742fab83e9ee8b72ab243218d7c263758890085cc10bec4d08e8a48582e541c`.
  Both final logs had zero matches for critical LaTeX warnings, overfull boxes,
  undefined citations/references, duplicate destinations, or fatal errors.
  All 81 rendered pages were visually inspected without clipping, overlap, or
  illegible content.
- **Scope and copyback:** independent scope review found exactly the 24
  permitted paths, zero missing or extra deltas, all 22 authoritative
  preimages intact, both additions absent, all backups and excluded sources
  unchanged, and no introduced whitespace, conflict-marker, NUL, or generated
  artifact defect. The 24 paths were copied individually with preimage guards;
  every destination hash matched its staged source. No recursive
  synchronization was used.

The evidence establishes the documented public semantics, executable
conformance path, builds, and dependency boundary. It does not establish
commitment binding or hiding, policy privacy, zero knowledge, knowledge
extraction, authenticated chronology, all-policy circuit equivalence,
production security, or novelty.
