# Adversarial Cooperation Roadmap

## Governing Principle

Advance by complete vertical slices in which the question, model, prose, C
state machine, transcript, tests, attacks, limitations, and citations agree.
Do not polish every fragment in parallel and do not use a green demo as a
substitute for a security argument.

## Phase 0 — Preservation and Audit

**Objective:** establish live repository evidence without rewriting source.

Deliverables:

- repository/build audit;
- chapter matrix;
- claim and assumption ledgers;
- backup comparison;
- open-problem register;
- rename map;
- this staged roadmap.

Exit condition: all audit artifacts agree with the live worktree; every build
path and backup pair has a recorded disposition; no chapter/C source was
changed.

## Decision Gate A — Author and Preservation Questions

Resolve only decisions that cannot safely be inferred:

1. Does `idyicyanere-linux-x64.vsix` remain intentionally deleted?
2. In Cryptographic Meta-Mathematics, is “Gödel's **unprovable** truths” from
   the backup intended, rather than current “improvable truths”?
3. Should the ignored `scholarship/` work receive its own preservation plan,
   especially the corrupted closing punctuation in `chinese_v.tex:36`?
4. Approve or defer the small ASCII rename for the Rock-Paper-Scissors path.

These questions do not block research planning, but they must be answered
before touching the affected files.

## Phase 1 — Reproducible Spine and First Complete Protocol (Provisionally Complete)

### Current Slice Status — 2026-07-21

The reviewed plan has been executed. The narrow protocol specification, public C API,
implementation, teaching driver, deterministic/adversarial tests, commitment
vector, canonical RPS chapter, minimal bibliography seed, primitive registry,
bibliography queue, decision records, and claim/assumption/open-problem updates
now form one vertical slice. The original oracle sentence remains
verbatim, the Unicode path rename is deferred, and selective abort remains the
central fairness counterexample.

Fresh strict compilation, the independently checked vector, 27 grouped tests,
ASan/UBSan, both demonstrations, standalone PDF generation, and visual
inspection of all 18 pages passed in the recorded disposable environments.
This establishes the enumerated implementation behavior, not computational
hiding, binding, production readiness, fairness, or general protocol security.

### 1A. Build and Test Spine

Repair only what the first slice needs:

- make README commands match the actual paths and Bash requirements;
- define a disposable Debian build environment with pinned-enough tool output;
- add the missing LaTeX package dependency used by `algorithm2e`;
- keep C11, the standard already selected by `Makefile.config`;
- define one non-interactive manuscript/section build path;
- introduce a test target and sanitizer target without reorganizing poker;
- decide where new generated outputs go and keep them out of source control.

Success means a clean environment can build the new slice without relying on
tracked objects, binaries, PDFs, or logs. It does not require repairing every
dormant chapter first.

### 1B. Reusable Commitment Primitive

Build the smallest primitive required by the first game:

- libsodium-backed hash commitment;
- explicit protocol/domain/version identifiers;
- fixed or length-delimited canonical serialization;
- session and role binding;
- operating-system randomness for normal runs;
- injected deterministic nonce for tests;
- constant-time verification;
- explicit error handling and nonce lifecycle;
- computational hiding/binding claims only, with assumptions stated;
- deterministic test vectors and corruption tests.

No production-readiness claim and no custom cryptographic hash implementation.

### 1C. Two Oracles Play Rock-Paper-Scissors

Turn the one-line fragment into the first end-to-end chapter and executable
protocol:

1. two fixed parties and three legal moves;
2. trusted-simultaneous-choice baseline;
3. authenticated-channel assumption stated but transport left out of scope;
4. commit/commit/reveal/reveal chronology;
5. versioned session-bound messages and a public transcript;
6. explicit states for invalid input, duplicate/reordered messages, reveal
   before both commitments, verification failure, timeout, and selective abort;
7. outcome computation only after valid reveals;
8. a clear statement that basic commit-reveal does **not** guarantee fair output
   when a losing party can refuse to reveal;
9. a “What Cryptography Does Not Solve” section;
10. primary citations and contribution classification as a known construction
    used to establish the book's method.

Required tests:

- all nine honest move pairs;
- deterministic commitment vectors;
- wrong move, nonce, session, version, sender, and round;
- truncated and oversized messages;
- duplicate, replayed, and reordered messages;
- premature reveal;
- non-reveal/selective abort;
- transcript disagreement;
- compiler warnings and available sanitizers.

Exit condition: prose, state machine, C behavior, tests, transcript examples,
claim ledger, and assumptions describe exactly the same narrow protocol.

## Phase 2 — Repair the Foundations Around the First Slice

1. **Hash Functions foundation repaired and verified:** rebuilt from primary
   references and independently reproduced test vectors; separates hashes from
   MACs, password hashing, signatures, and commitments; adds one fixed
   C11/libsodium wrapper and teaching companion.
2. **Trust Establishment model repaired and verified:** entropy generation is
   not peer authentication; the active chapter now separates unauthenticated
   exchange, authenticated establishment, identity binding, correlated
   observations, public coordination, and setup choices without adding a
   protocol or runtime dependency.
3. Rewrite the Introduction's technical promises as a method of inquiry while
   preserving the moral voice and adversary/enemy distinction.
4. Expand the RPS seed primitive registry and bibliography queue into
   project-wide records; create the glossary and thesis; settle book-level
   bibliography ownership.
5. Add the intended include graph gradually, using honest stubs for open
   chapters rather than hiding them or pretending they are mature.

Exit condition: the foundations no longer teach false blanket guarantees, and
the complete book skeleton compiles without turning open problems into claims.

## Phase 3 — Finite Strategy and Mental Poker

### Tic-Tac-Toe

- enumerate the reachable game graph;
- define a legal committed policy and the non-losing relation;
- build a public exhaustive checker first;
- establish an equivalent fixed public game-DAG recurrence;
- distinguish proof of knowledge, legality, non-loss, response privacy, and
  repeated-query extraction;
- only then evaluate a zero-knowledge construction.

**Public checker, fixed-DAG equivalence, and role-specialized Boolean Core gates
complete; `Bind` and the private protocol remain open.** The recursive checker
retains replayable diagnostics, the fixed DAG supplies the public topological
recurrence and induction argument, and `Core-X`/`Core-O` now have fixed typed
inputs, deterministic XOR/AND netlists, canonical serialization, and exact
construction counts independently derived and recounted from the emitted
gates. Circuit acceptance is checked against both preserved C evaluators on a
bounded corpus. All three paths still receive the policy in plaintext. The immediate
next gate is to select and measure `Bind` and both commitment layers before
choosing a proof transformation or importing proof machinery. MPC-in-the-head
remains only the leading conditional educational family. No proof library,
cryptographic implementation, private-proof claim, or novelty claim has been
added.

### Poker

- preserve current four-stage decomposition as the strongest technical anchor;
- label the current C program as a centralized educational animation;
- replace trace checksums with explicit semantics;
- define participant-local state and messages;
- study verifiable shuffles and malicious models before selecting one;
- restore backup-only questions about joining, full-deck checks,
  anti-collusion versus betting, private winner evaluation, and other games;
- add betting or winner proofs only after the deal's properties are honest.

Exit condition: each chapter states precisely which privacy/integrity properties
are implemented, argued, proved, or still open.

## Phase 4 — Private Joint Computation

Develop increasingly general examples:

1. one-bit secret-dependent error notification;
2. bounded two-dimensional private obstacle intersection;
3. a small optimization problem with public constraints and private scalar
   inputs;
4. only then a general secure multi-party optimization chapter.

For every step, measure output leakage and separate secure evaluation from
truthful input, global optimality, incentive alignment, fairness, and justice.

## Phase 5 — Institutions, Dynamics, and Repair

Return to Hidden Law, Cryptographic Meta-Mathematics, Conflict Arbitrage,
Aggregation Disadvantage, Momentum-Zero Traps, Friendly Fire, Fighting
Dissipates Strength, the Helmsman's Game, Defining Victory, and Adversarial
Aftermath.

For each chapter, first define:

- parties and power asymmetries;
- actions and information;
- the blocked desirable transition;
- baseline institution;
- exit, coercion, audit, appeal, and succession;
- whether cryptography changes the feasible set at all.

A rigorous design essay is an acceptable endpoint when cryptography is not the
right instrument.

## Phase 6 — Goodhart's Law

Treat Goodhart as a synthesis problem, not a promised cryptographic solution.
Study one mechanism at a time—randomized evaluation, rotating committed
objectives, multi-metric review, challenge/audit, flaw-discovery rewards,
versioning, and sunset clauses—and test how each can itself be gamed.

Goodhart remains the northern star of the first arc, not the first campsite.

## Completed Scope of the First Implementation Plan

The completed first implementation plan is
`research/plans/commitment-and-rps-vertical-slice.md` and is limited to:

### Proposed new artifacts

- a small public commitment interface and libsodium-backed implementation;
- an RPS protocol state machine with participant-local state;
- a human-readable demo;
- unit, protocol, attack, and transcript tests;
- deterministic test vectors;
- the completed RPS manuscript chapter;
- a standalone RPS section-PDF target;
- claim, assumption, primitive, bibliography, and decision-register updates.

### Proposed touched infrastructure

- the narrow Makefile/config paths needed to build and test those artifacts;
- README/build dependency corrections proven by a clean disposable container;
- `.gitignore`/output-directory changes only after the tracked-artifact policy
  is explicitly approved.

### Explicit non-goals

- networking or authenticated transport implementation;
- deposits, penalties, blockchains, trusted hardware, or fair-exchange claims;
- zero knowledge;
- production cryptography claims;
- poker refactoring;
- mass directory rename;
- global chapter cleanup;
- solving Goodhart, war, justice, or adversarial alignment by declaration.

### Acceptance criteria

1. Clean C build with the repository's warning flags.
2. Deterministic tests and adversarial state-transition tests pass.
3. Sanitizer run passes where the toolchain supports it.
4. RPS section PDF compiles non-interactively.
5. Every manuscript claim maps to a test, an explicit assumption, a proof or
   security argument status, or a visible open problem.
6. Selective abort is demonstrated and documented as unresolved fairness.
7. No existing source, backup, user deletion, or ignored scholarship work is
   overwritten.

This scope yields one scientifically honest, compiling, tested chapter before
the project expands laterally.

## Completed Foundation Plan — Trust Establishment

The completed plan is
`research/plans/trust-establishment-model-repair.md`. It is deliberately
manuscript-only:

- preserve the author's local-entropy, challenge, active-interposer, denial-of-
  service, and market intuitions as clearly classified material;
- replace the unfinished entropy algorithm with an Open Research Note and a
  source-validation checklist;
- separate six different tasks and their initial resources;
- state one exact correlated-source upper bound under its cited model and keep
  computational key agreement outside that impossibility claim;
- add a standalone Trust target and settle bibliography ownership for the
  currently included main chapter;
- update only the affected research registers; and
- add no C code, primitive, runtime library, identity-system choice, or novelty
  claim.

Exit condition: the standalone and main-book Trust outputs compile with
resolved citations, every final page passes visual inspection, regressions for
the existing section targets pass, the former claims remain traceable, and the
authoritative copyback preserves all unrelated work.
