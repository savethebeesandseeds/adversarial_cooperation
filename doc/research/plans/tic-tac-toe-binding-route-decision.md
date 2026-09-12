# Execution Plan: Tic-Tac-Toe Binding Route Decision

## Status

Complete on 2026-07-31. This bounded manuscript-and-register slice decides
what must happen after the completed external Policy Commitment Profile
Version 1 and symbolic direct-`Bind` measurement.

It does not change C code, emit the measured `Bind` netlist, select or import a
proof system, introduce another commitment primitive, add a dependency, or
claim zero knowledge, knowledge soundness, extraction, privacy, production
security, or novelty.

## Objective

Resolve the immediate fork recorded by the previous slice:

1. state formally why proving `Core` while merely carrying a public digest
   does not prove anything about the policy named by that digest;
2. distinguish the claim "there exists a non-losing policy" from the intended
   claim "this committed policy is non-losing";
3. decide whether the next step should emit the roughly 32-million-gate direct
   BLAKE2b `Bind` circuit or replace it immediately with a new
   "proof-friendly" commitment;
4. preserve the minimal-library boundary; and
5. name the smallest evidence gate that may authorize later proof code.

## Current Evidence

The completed predecessor slice fixes:

- the external profile `AC-TTT-POLICY-V1`;
- a 72-byte public instance and 19,715-byte witness layout;
- an independently reproduced disclosed-opening vector;
- distinct `Core` and `Bind` duties;
- an exact symbolic direct-`Bind` shape with 32,003,200 gates and an estimated
  288,028,832-byte serialization if emitted; and
- separate Core-X/Bind and Core-O/Bind arithmetic totals.

The direct netlist is not emitted or checked against the hash implementation.
No proof backend, proof-internal view commitment, arithmetization, transcript,
extractor, or malicious-verifier transformation is selected.

Baseline for this plan, against an isolated copy of the authoritative working
tree:

- all 71 existing grouped C tests pass; and
- `bash book -pdf section_ttt` produces the stabilized 24-page standalone
  chapter.

## Scientific Boundary

The route decision must not confuse any of the following:

- a public value being present in a statement;
- that value being read by the proved relation;
- the relation enforcing an opening to that value;
- a proof system establishing knowledge of a relation witness;
- and the commitment scheme being computationally binding.

Even a perfect argument of knowledge for a relation that checks only `Core`
may correctly extract a non-losing policy while proving nothing about the
public commitment. Likewise, separate witnesses for `Core(pi)` and
`Bind(C, pi_prime, r)` are insufficient unless the relation enforces
`pi = pi_prime`.

The concrete hash profile has only the previously stated computational and
heuristic arguments. Its synthetic invalid-policy vector may illustrate a
mismatched statement under the stated binding assumption, but ordinary tests
do not establish unique opening and will not be used as a mathematical proof
of commitment security.

## Decision Criteria

Evaluate the immediate routes without adding candidate implementations:

### Emit the direct Boolean `Bind`

This preserves the existing external profile and adds no library. It would,
however, materialize hundreds of megabytes for one deliberately unoptimized
Boolean representation before any proof transformation is selected. It would
establish neither privacy nor proof cost, and it may be irrelevant to a later
arithmetic backend.

### Replace the commitment now

A different commitment might be cheaper inside a particular proof system, but
"proof-friendly" is relative to the chosen field, constraint language,
transcript, setup, and security assumptions. Selecting one before those are
fixed would add a primitive and likely a library without an end-to-end cost or
security comparison.

### Preserve the baseline and co-design the next gate

Keep Policy Commitment Profile Version 1 as the disclosed-opening baseline.
Do not promote it into a proof backend, and do not silently change its bytes.
Choose a proof backend, relation representation, external binding mechanism,
and proof-internal commitment layer together in a later paper-first plan. Only
then decide whether to emit direct `Bind`, replace the external profile under a
new version, or prove a precisely specified compatibility relation.

The third route is the expected decision unless new evidence contradicts the
current cost and dependency record.

## Exact File Scope

Permitted source changes are limited to:

- `document/content/tic_tac_toe_without_revealing_the_strategy/`
  `tic_tac_toe_without_revealing_the_strategy.tex`;
- `research/ASSUMPTION_LEDGER.md`;
- `research/CHAPTER_MATRIX.md`;
- `research/CLAIM_LEDGER.md`;
- `research/DECISIONS.md`;
- `research/OPEN_PROBLEMS.md`;
- `research/PRIMITIVE_REGISTRY.md`;
- `research/ROADMAP.md`; and
- this plan.

No C source, header, test, vector, build file, bibliography, backup, include
graph, or dependency declaration may change.

The authoritative preimage SHA-256 values for the eight existing permitted
files are recorded in the verification notes before copyback. The new plan
must remain absent from the authoritative tree until guarded copyback.

## Work Sequence

1. Audit the chapter, implementation contract, tests, registers, and existing
   primary-source bibliography.
2. Obtain independent read-only reviews of relation linkage, route options,
   and minimum file scope.
3. Add one compact linkage proposition, its proof, and the actual-profile
   illustration with the correct computational qualification.
4. Record the route decision: do not emit direct `Bind` now; do not select a
   replacement now; preserve Version 1; require backend/commitment co-design.
5. Update only the affected claim, assumption, decision, primitive, chapter,
   open-problem, and roadmap records.
6. Rebuild the standalone chapter until references stabilize.
7. Scan the log and extracted text for critical defects, render every PDF page,
   and inspect every page visually.
8. Re-run the 71-group C regression even though executable sources must remain
   byte-identical.
9. Obtain a final independent review of the exact diff.
10. Guardedly copy back only files whose authoritative preimages still match;
    then verify every destination hash.

## Acceptance Criteria

The slice is complete only if:

- the chapter explains, with a proof, why unused `C` cannot bind the extracted
  `Core` witness;
- the chapter says exactly when a commitment is and is not necessary for the
  intended claim;
- the immediate fork is resolved without adding code or a dependency;
- `AC-TTT-POLICY-V1` remains byte-compatible and explicitly limited to the
  disclosed-opening baseline;
- the next gate names backend-relative comparison criteria rather than a
  product or library;
- all ledger statements match the chapter and do not promote evidence;
- all 71 existing C test groups still pass;
- the stabilized PDF has a clean critical-issue scan and every page is
  visually inspected; and
- protected files are byte-identical and guarded copyback reports exact hash
  agreement.

## Non-Claims

Completing this plan will not establish:

- commitment binding or hiding for the concrete hash profile;
- a zero-knowledge proof or argument;
- witness extraction;
- proof-system compatibility;
- end-to-end proof size, runtime, or memory;
- superiority of Boolean or arithmetic arithmetization;
- security against quantum adversaries;
- production readiness; or
- novelty.

## Verification Record

The baseline isolated tree passed `make -C tests test` with all 71 grouped C
tests and produced a stabilized 24-page chapter with
`bash book -pdf section_ttt`.

Against the final staged chapter and unchanged executable sources:

- `make -C tests test` again passed all 71 grouped tests with zero failures;
- `bash book -pdf section_ttt` produced a stabilized 27-page, 345,903-byte
  standalone PDF with SHA-256
  `8a697d46d2f3142ab4a28f3c3ee6c90ccde4eccff4ecc58c00acf43ddbae2eb9`;
- the stabilized log had zero matches for LaTeX/package warnings, overfull
  boxes, undefined control sequences, fatal errors, or LaTeX errors;
- `pdftotext` output had zero unresolved-reference, undefined-citation, or
  `??` placeholder matches; and
- all 27 rendered pages were inspected individually with no clipping,
  overflow, collision, or illegible content found.

Independent reviews covered minimum file scope, candidate-route evidence,
formal witness linkage, the exact chapter diff, and register consistency.
Their substantive findings were incorporated. Follow-up review left no
unresolved P0, P1, or P2 issue.

The pre-copy inventory contained 199 authoritative and 200 staged source
files after excluding version-control and generated build/render directories.
It found exactly nine differences: the eight permitted modified files and
this new plan. No protected file differed. No C source, header, test, vector,
build file, bibliography, dependency declaration, backup, or include graph
changed.

The guarded copy required these authoritative preimages:

- chapter:
  `2587781a1ad9d841da38ac59e1095976f1494ff4f02f127d352e5ec7eef1175d`;
- `ASSUMPTION_LEDGER.md`:
  `b8fd72eb970cb73c5bd1b664e57060dc0e694497f7fec28d6b471926046ffeca`;
- `CHAPTER_MATRIX.md`:
  `59bfec023cc3f5afced2f91e82acd12936eebbcbc8a461e9944cd887fff5b5ad`;
- `CLAIM_LEDGER.md`:
  `4fae24bbc43a739e995287e6c5bd4f0dd3083965eeea74fbccb89cc8baeccc4e`;
- `DECISIONS.md`:
  `d202bdff008d11d396a3f5668b7606176fca0a5f95493acc507e94c063fb40a5`;
- `OPEN_PROBLEMS.md`:
  `0c8cf5e2df304afbf6c4c5f8ee8c874815f75b0df24434496ea12102f35bcecf`;
- `PRIMITIVE_REGISTRY.md`:
  `6d60940116efe1888ee09dde580929b188d3cf7ce83132c415b6c8d0728d786d`;
  and
- `ROADMAP.md`:
  `411da19cc82d22cf6a027839dbb604aeb9e5c0caa065b1de0dd817ce16e4ba16`.

The new plan was absent as required. The first guarded copyback matched all
nine staged snapshot hashes. After adding the final evidence row to the claim
ledger, the eight material source hashes are:

- chapter:
  `17a7a5b2fa8e5af367cc74727bfebcdf25973928dd6af496cad4c00354f2caf8`;
- `ASSUMPTION_LEDGER.md`:
  `9279c7547fcefbba157fd94e27f1caf7ac007c9da571b8164a4d8ee3e6c4d436`;
- `CHAPTER_MATRIX.md`:
  `a5bf49ba466a2472d967bef6a9066855785672c81a26331655b827df76d83b1b`;
- `CLAIM_LEDGER.md`:
  `02dda83c97d214003732a4e2d6cc5c8a71602ca1cdb1690b060aa58656063517`;
- `DECISIONS.md`:
  `423b283f857870fa7fb4935a30516cced612d45d34440471a2063318f856f0eb`;
- `OPEN_PROBLEMS.md`:
  `6af1843a39d7f834d6ca2593c8613f0b0f323c68cc34c7a60126a1012f0d98df`;
- `PRIMITIVE_REGISTRY.md`:
  `af778d9063ba120416a9c97bf4d0d007391e5416d816e0249da2c937fef289bd`;
  and
- `ROADMAP.md`:
  `0ae4f3e816618b9e68adfffe362acc405c94c384560a254da2f48b1593e3cb03`.

The plan's pre-closure snapshot was
`e67704d336cf1f9ddb4268e54bb061a967a539a03b33939b70f5380544451f43`.
Because a file cannot contain its own final hash without changing it, the
final ledger-and-plan destination equality check is recorded by the executing
work report rather than recursively embedded here.

None of this verification promotes the claims excluded under Non-Claims.
