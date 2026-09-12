# Tic-Tac-Toe Model and Public Checker

## Status

Complete bounded finite-game vertical slice. A checked item records evidence
actually obtained; an unchecked item is not complete.

## Intended Outcome

Turn the current Tic-Tac-Toe fragment into an honest foundation for its central
research question. This slice will:

1. define ordinary 3-by-3 Tic-Tac-Toe, legal reachable states, roles, and a
   canonical deterministic positional policy;
2. implement a plain-C11 public checker that exhaustively verifies whether a
   fully disclosed policy prevents loss from the standard empty board against
   every legal opponent continuation;
3. produce a replayable losing game when a policy fails;
4. distinguish public verification, commitment, zero knowledge, proof of
   knowledge, private evaluation, and anti-cloning; and
5. state the exact private relation a later proof system would need to certify.

The executable result deliberately reveals the policy. It does not yet solve
the aspirational title. Defining and testing the public predicate comes before
choosing a zero-knowledge or private-computation backend.

This slice adds no cryptographic library, no cryptographic primitive, no
network code, and no dependency. The Tic-Tac-Toe target links only the C11
standard library. Existing RPS and Hash targets continue to use the repository's
sole external C backend, libsodium, unchanged.

## Progress

- [x] Governing instructions and current research registers read.
- [x] Live status, HEAD, file inventory, history, chapter, and backup audited.
- [x] Existing C test baseline passed in a disposable Debian container.
- [x] Minimal primary-source and claim-separation gate completed.
- [x] Public game, policy, checker, and evidence boundaries frozen below.
- [x] C API, checker, reference policy generator, demo, and tests implemented.
- [x] Chapter and research registers updated.
- [x] Strict, sanitizer, regression, and standalone-PDF verification completed.
- [x] Every final PDF page rendered and visually inspected.
- [x] Independent review has no unresolved P0/P1/P2 finding.
- [x] Guarded explicit copyback and final status verification completed.

## Preservation Baseline

The authoritative checkout is:

`C:\Work\applied_cryptography\adversarial_cooperation`

At the start of this slice it is on `main` at
`2824992ee2802b0b72f67054f63189628eb15103`. Completed Phase 0, RPS, Hash,
and Trust work remains uncommitted. The pre-existing deletion of
`idyicyanere-linux-x64.vsix` is user-owned. Do not restore, discard, stage,
commit, or alter unrelated work.

The isolated working copy was made from the complete live filesystem rather
than from HEAD. Its location is:

`C:\Users\santi\OneDrive\Documents\Adversarial-cooperation\.codex-ttt-stage`

The live chapter SHA-256 is:

`59a2b755e5a46823a9c8f322e4fc3f003ef790fc3151e88595fd2e938626081d`

The tracked backup SHA-256 is:

`4f075f30383abab4906e81b2a39db7a453bc6e80f18c564723129caebb609cc2`

History contains only the original fragment at `6523cb4` and spelling-only
corrections at `b3fe8dd`. The backup contains no unique scientific idea, but it
must remain byte-identical and must not be deleted.

Before copyback, revalidate HEAD, full status, every pre-existing destination
hash, the backup hash, protected out-of-scope hashes, and the absence of every
new destination. Copy only the explicit affected inventory, verify each file
byte-for-byte, and copy this completed plan last.

## Authorial Preservation

Retain these ideas visibly as `Author's Intuition` or open questions:

- prove possession of an undefeated strategy without revealing it;
- commit to a session seed;
- prevent an adversary from cloning the strategy;
- let an adversary choose initial conditions;
- inspect all scenarios rather than one friendly play;
- investigate whether matrix inversion has a useful role; and
- always include code for demonstration.

Do not silently convert those phrases into established mechanisms.

- A session identifier, a commitment nonce, and a policy-generation seed are
  different objects.
- A seed is related to a policy only after a public deterministic generation
  rule is specified and checked.
- Monte Carlo sampling cannot by itself prove a universal no-loss statement.
- No matrix is defined in the fragment; invertibility currently has no
  demonstrated connection to legality or non-loss.
- Arbitrary initial conditions may already be forced losses. This slice fixes
  the standard empty board; challenged-state certification remains open.

## Exact Game Model

The game version is ordinary 3-by-3 Tic-Tac-Toe:

- cells are numbered `0` through `8` in row-major order;
- each cell is `EMPTY=0`, `X=1`, or `O=2`;
- X moves first and roles alternate;
- a player wins after occupying a row, column, or diagonal;
- play stops immediately after a win;
- a full board without a winner is a draw; and
- the public initial state is the empty board.

Encode a board as

```text
index(board) = sum(board[cell] * 3^cell), cell = 0..8.
```

There are exactly `3^9 = 19,683` byte-addressable board encodings. Exhaustive
enumeration from the empty board, stopping after every terminal position,
produces these independently checked state counts:

- 5,478 reachable boards;
- 2,423 reachable nonterminal X-to-move boards;
- 2,097 reachable nonterminal O-to-move boards; and
- 958 terminal boards.

The implementation must reproduce these counts. They are mathematical and
implementation cross-checks, not cryptographic claims.

## Canonical Policy Version 1

For a fixed claimant role `p` in `{X,O}`, a deterministic positional policy is
a 19,683-byte table `policy`.

- If index `i` represents a legal reachable nonterminal board where `p` moves,
  `policy[i]` is one legal empty cell in `0..8`.
- Every other entry is exactly `0xff` (`AC_TTT_NO_MOVE`).

Thus the policy supplies a legal action at every claimant decision state,
including legal off-path states that the policy would not reach from the empty
board. It contains no unchecked bytes in invalid, opponent-turn, or terminal
positions.

The role is a separate public input. The table does not encode identity,
authorship, originality, randomness, a session, or future obedience.

## Public Verification Predicate

For role `p` and canonical policy `pi`, define `PublicNonLose(p,pi)=1` exactly
when:

1. every required policy entry is present and selects an empty cell;
2. every non-required table entry is `0xff`; and
3. starting from the empty board, following `pi` at claimant turns and
   branching over every legal opponent move always ends in either a claimant
   win or a draw.

The checker first constructs the complete legal reachable-state classification
without expanding terminal states. It rejects missing, out-of-range, occupied,
or stray entries. It then performs depth-first exhaustive verification:

- at a claimant turn, follow the table's single move;
- at an opponent turn, try every empty cell in ascending order;
- stop a branch at a win or full-board draw; and
- on the first opponent win, return the complete chronological move trace.

The traversal order is deterministic. A successful return says only that the
fully disclosed table satisfies this finite predicate under the fixed rules
and initial state.

## Public C API

Create `include/ac/ttt.h` with:

- constants for nine cells, 19,683 table entries, nine plies, no-move `0xff`,
  and game version 1;
- roles `AC_TTT_ROLE_X=1` and `AC_TTT_ROLE_O=2`;
- `ac_ttt_policy` containing the fixed byte table;
- verdicts `AC_TTT_VERDICT_NON_LOSING`, `AC_TTT_VERDICT_INVALID_POLICY`, and
  `AC_TTT_VERDICT_LOSING`;
- policy-format reasons for missing, out-of-range, occupied, and unexpected
  entries;
- an `ac_ttt_report` containing verdict, reason, invalid board index/move,
  reachable and required-state counts, explored/terminal node counts, and a
  zero-to-nine-move counterexample;
- `void ac_ttt_policy_init(ac_ttt_policy *policy)`; and
- `ac_status ac_ttt_verify_policy(ac_ttt_role role, const ac_ttt_policy *policy,
  ac_ttt_report *report)`.

`ac_ttt_verify_policy` returns an `ac_status` for API execution and puts the
mathematical verdict in the report. It clears a non-null report before every
argument check. Invalid roles or null pointers are API errors; malformed or
losing policies are successfully computed verdicts.

Implement a deterministic minimax/backward-induction reference-policy builder
for the teaching demo and tests. It may be a public convenience function if
that keeps the implementation singular, but it must be documented as a public,
non-secret reference generator rather than a cryptographic mechanism. Policy
generation and policy verification remain logically separate.

No heap allocation, randomness, clock, file I/O, network, hash, commitment, or
libsodium call belongs in the checker.

## Five Claims That Must Remain Separate

1. **Knowledge of an opening.** A commitment-shaped value does not show that
   its sender knows an opening. A proof of knowledge requires an extractor-
   based definition and an instantiated protocol.
2. **Legality.** A disclosed table can be checked publicly. An unopened
   commitment cannot be scanned for legal moves.
3. **Non-loss.** Exhaustive public traversal can check a disclosed policy.
   Random samples do not prove every branch.
4. **Private per-state evaluation.** Revealing a move leaks that policy output.
   Hiding the query, the output, or both is a separate private-computation
   problem.
5. **Anti-cloning.** An unrestricted exact deterministic strategy oracle can
   be cloned information-theoretically by querying every finite-domain input
   and recording the outputs. Cryptography cannot conceal answers that the
   interface deliberately releases.

## Future Private Relation

A later construction may define a public instance

```text
x = (game_version, claimant_role, initial_state, policy_commitment, context)
```

and witness

```text
w = (canonical_policy, commitment_opening).
```

For this fixed slice, the relation accepts exactly when `game_version=1`,
`initial_state=empty`, the commitment opens under `context` to a canonical
encoding that binds the version, role, initial state, and policy, and
`PublicNonLose(role,policy)=1`. A protocol claiming the author's full goal
would need both a zero-knowledge property and a proof/argument-of-knowledge
property for that relation under named setup, adversary, composition, and
commitment assumptions.

This slice implements none of that proof machinery. The repository's existing
instructional hash commitment is not silently promoted into a proof system.

## Minimal Source Gate

Sources are bibliography entries, never software dependencies.

- Reuse Naor 1991 only for formal commitment background; it does not prove the
  repository's hash construction or a Tic-Tac-Toe predicate.
- Add Goldwasser--Micali--Rackoff 1989 for the zero-knowledge definition.
- Add Bellare--Goldreich 1992/1993 for the extractor-based proof-of-knowledge
  distinction.
- Add Goldreich--Micali--Wigderson 1991 for the conceptual result that NP
  statements have zero-knowledge proof systems under its stated assumptions.
  It does not select or implement a practical proof backend here.

No mental-poker, obfuscation, modern SNARK, or private-computation survey is
needed for an active claim in this slice. Reopen the bibliography only when a
later protocol needs it.

Contribution classification:

> A straightforward public finite-game model, exhaustive verifier, and
> educational boundary analysis; no novelty claim.

## Affected Files

### New

- `research/plans/tic-tac-toe-model-and-public-checker.md`
- `include/ac/ttt.h`
- `src/protocols/ttt.c`
- `src/tic_tac_toe_without_revealing_the_strategy/ttt_demo.c`
- `tests/test_ttt.c`

### Existing

- `document/content/tic_tac_toe_without_revealing_the_strategy/tic_tac_toe_without_revealing_the_strategy.tex`
- `book`
- `README.md`
- `src/Makefile`
- `src/README.md`
- `demostrations/Makefile`
- `demostrations/README.md`
- `tests/Makefile`
- `document/references.bib`
- `research/BIBLIOGRAPHY_QUEUE.md`
- `research/CHAPTER_MATRIX.md`
- `research/CLAIM_LEDGER.md`
- `research/ASSUMPTION_LEDGER.md`
- `research/OPEN_PROBLEMS.md`
- `research/DECISIONS.md`
- `research/ROADMAP.md`

### Protected and unchanged

- the chapter `.bak`;
- `document/adversarial_cooperation.tex` and the main include graph;
- `Makefile.config`;
- `research/PRIMITIVE_REGISTRY.md` and `research/REPOSITORY_AUDIT.md`;
- every existing commitment, hash, RPS, poker, and Trust source;
- the tracked demonstration `.gitkeep`;
- generated files, repository metadata, and rename map.

## Build Integration

- Add `section_ttt` to `book`, using the shared bibliography wrapper.
- Add `ttt` and `run_ttt` to `src/Makefile`.
- Link the TTT binary from only `ttt.c` and `ttt_demo.c`; omit `$(LDLIBS)`.
- Add `build_ttt` and `demo_ttt` to `demostrations/Makefile`.
- Add the TTT test binary and target to `tests/Makefile`; omit `$(LDLIBS)` from
  its link command.
- Leave repository-wide C11 and libsodium configuration untouched.
- Keep all verification output outside tracked directories.

## Required Tests

At minimum:

- independently expected state counts: 5,478 / 2,423 / 2,097 / 958;
- deterministic public reference policies for X and O pass;
- a fully canonical but forceably losing policy returns `LOSING` and a
  replayable opponent-win trace;
- a loss hidden on a late opponent branch is still found;
- missing, out-of-range, occupied, and unexpected entries return
  `INVALID_POLICY` with deterministic evidence;
- entries on opponent-turn, terminal, unreachable, wrong-count, simultaneous-
  winner, and post-win boards are rejected as unexpected;
- traversal never expands a terminal state;
- repeated verification returns an identical report and uses no randomness;
- invalid role and null-pointer calls return argument errors and clear a
  non-null report; and
- the original commitment, RPS, adversarial RPS, and hash tests still pass.

Tests establish enumerated implementation behavior only. They do not establish
privacy, zero knowledge, knowledge extraction, commitment security, optimality,
authorship, or future behavior.

## Manuscript Contract

The chapter should contain:

1. Author's Intuition;
2. a prominent Current Result box saying that this slice reveals the policy;
3. Abstract;
4. Cooperation Problem and classical disclosure/trusted-evaluator baselines;
5. fixed game, role, state, policy, and encoding definitions;
6. non-losing versus locally optimal/minimax terminology;
7. public exhaustive predicate and algorithm;
8. exact C mapping and evidence;
9. the five separated claims;
10. future committed-policy private relation;
11. repeated-query reconstruction proposition and proof;
12. attacks, abort, leakage, and composition boundaries;
13. ethical/institutional interpretation;
14. What Cryptography Does Not Solve;
15. limitations and open problems; and
16. exercises and references.

The source fragment remains traceable in the claim ledger. Do not claim that
the current checker is zero knowledge or that it proves knowledge.

## Verification Commands

Use a disposable Debian environment. Intended commands are:

```text
make -C demostrations build_ttt \
  BUILD_DIR=/tmp/ac-ttt-build TEMP_DIR=/tmp/ac-ttt-temp
make -C demostrations demo_ttt \
  BUILD_DIR=/tmp/ac-ttt-build TEMP_DIR=/tmp/ac-ttt-temp
make -C tests test \
  BUILD_DIR=/tmp/ac-ttt-test-build TEMP_DIR=/tmp/ac-ttt-test-temp
make -C tests ttt \
  BUILD_DIR=/tmp/ac-ttt-strict-build TEMP_DIR=/tmp/ac-ttt-strict-temp \
  CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C tests sanitize \
  BUILD_DIR=/tmp/ac-ttt-san-build TEMP_DIR=/tmp/ac-ttt-san-temp
BOOK_OUT_DIR=/tmp/ac-ttt-pdf BOOK_TMP_DIR=/tmp/ac-ttt-tex \
  bash book -pdf section_ttt
```

Also rebuild `section_rps`, `section_hash`, `section_trust`, and
`section_poker` after changing the shared book helper. Require fresh nonempty
PDFs, stabilized citations, and no touched-file LaTeX error, undefined
reference/citation, missing glyph, or overfull box. Render and inspect every
final TTT page after the last meaningful change.

## Verification Record

The final staged sources produced the following evidence in disposable Debian
containers:

- the public demonstration built and ran, reporting the fixed counts and
  accepting the X and O reference policies while returning a replayable loss
  for a naive canonical X policy;
- strict C11 compilation with `-Wall -Wextra -Wpedantic -Werror -O2` passed
  all 8 Tic-Tac-Toe test groups;
- focused AddressSanitizer and UndefinedBehaviorSanitizer execution passed the
  same 8 groups;
- the complete C regression passed 41 grouped tests: 7 commitment, 6 core RPS,
  14 adversarial RPS, 6 hash, and 8 Tic-Tac-Toe;
- an independent test-local bottom-up oracle matched every byte of both
  19,683-byte reference policies, including ascending-cell tie-breaking;
- losing traces were replayed and every claimant ply was checked against the
  rejected policy;
- all source companions compiled, and dynamic inspection of the Tic-Tac-Toe
  demonstration showed only the C runtime, with no libsodium dependency;
- the stabilized `section_ttt` PDF is 13 pages and 235,690 bytes; its final log
  has no LaTeX error, undefined citation/reference, missing glyph, or overfull
  box, and every page was rendered at 144 dpi and visually inspected after the
  last manuscript change;
- `section_rps`, `section_hash`, `section_trust`, and `section_poker` rebuilt as
  fresh nonempty PDFs, and each stabilized final log passed the same issue
  scan; and
- independent C, manuscript/scientific, and repository-scope reviews have no
  unresolved P0, P1, or P2 finding. Two test-coverage findings and seven
  notation, relation, evidence-language, and scope findings were corrected and
  re-reviewed.

One aggregate full-suite sanitizer invocation stalled without useful output
and is not counted as positive evidence. The final focused sanitizer run covers
the new Tic-Tac-Toe source and its complete 8-group suite; earlier slices retain
their own separately recorded sanitizer evidence.

## Copyback Record

Guarded copyback to the authoritative dirty checkout completed on 2026-07-21.
Immediately before writing, the guard revalidated `main` at
`2824992ee2802b0b72f67054f63189628eb15103`, all 16 pre-existing destination
hashes, the chapter backup, the absence of all 5 new destinations, and an exact
21-file staged-versus-live delta. It then copied and hash-verified the 16
existing files and 4 implementation files. This completed plan was copied last.
All 21 authoritative files were then byte-compared with staging; all backups
and protected files remained unchanged, and no file was staged, committed,
reset, restored, or deleted.

## Acceptance and Evidence Boundary

The slice is provisionally complete only when the public policy representation
has one meaning, both roles have accepted reference policies, malformed and
losing policies fail with deterministic evidence, the chapter calls the
checker a disclosure baseline, all sources support only matched claims, no new
dependency exists, existing targets regress, all final TTT pages are visually
clean, independent review finds no unresolved P0/P1/P2 issue, and copyback
preserves the live dirty tree.

An accepting checker run reports that the supplied public table satisfies the
implemented finite predicate. It does not prove C-to-specification equivalence,
who authored the table, that a person knows or will follow it, that the table
remained secret, that an unopened commitment contains it, or that any private
proof system is sound or zero knowledge.
