# Tic-Tac-Toe Fixed-DAG Evaluator

## Status

Complete. This was a bounded non-cryptographic implementation slice. Every
checked item records evidence actually obtained.

## Intended Outcome

Add a plain-C11, proof-backend-neutral evaluator for the already defined
`PublicNonLose(role, policy)` predicate. The new evaluator will:

1. preserve Canonical Policy Version 1 exactly;
2. build the fixed reachable game graph without recursion;
3. validate all 19,683 policy bytes without early failure or a
   policy-selected memory address;
4. evaluate the complete reachable-state DAG in a public topological order;
5. match the existing recursive checker's mathematical verdict;
6. record exact finite schedule, operation, storage, and graph counts; and
7. leave replayable counterexample diagnosis with the existing recursive
   checker.

This slice adds no commitment, hash, proof system, transcript, randomness,
network code, cryptographic library, or external dependency. It does not
implement the private-strategy goal. It prepares the finite game computation
for a later Boolean-circuit slice.

## Progress

- [x] Governing instructions, charter, completed Tic-Tac-Toe plans, chapter,
  implementation, tests, build files, and relevant registers read.
- [x] Live branch, HEAD, dirty status, protected backup, and preservation
  hashes recorded.
- [x] Isolated staging copy created from the complete dirty live filesystem.
- [x] Strict 8-group Tic-Tac-Toe baseline, complete 41-group C regression, and
  public demonstration pass in a disposable Debian container.
- [x] API, recurrence, induction, fixed-schedule, mutation-corpus, and build
  audits completed independently.
- [x] Exact implementation and evidence contract frozen below.
- [x] Public API, evaluator, demonstration, and tests implemented.
- [x] Chapter and research registers updated.
- [x] Strict, sanitizer, regression, dependency, and manuscript verification
  completed.
- [x] Independent final review has no unresolved P0/P1/P2 finding.
- [x] Guarded explicit copyback and final live-tree verification completed.

## Preservation Baseline

The authoritative dirty checkout is:

`C:\Work\applied_cryptography\adversarial_cooperation`

At the start of this slice it is on `main` at
`2824992ee2802b0b72f67054f63189628eb15103`, and the Git index is empty.
Completed Phase 0, RPS, Hash, Trust, public Tic-Tac-Toe, and proof-route work is
uncommitted. The pre-existing deletion of `idyicyanere-linux-x64.vsix` is
user-owned. Nothing in this slice may restore, discard, stage, commit, or alter
that work.

The isolated staging copy is:

`C:\Users\santi\OneDrive\Documents\Adversarial-cooperation\.codex-ttt-dag-stage`

Affected existing-file SHA-256 values at the start are:

```text
BAEC55A36092F2AB1E4CE576372E6666534AB9133827543E1E6D4558334123B9  include/ac/ttt.h
1274588BDD14960C48B43446E03BA8BF83DC232B98E1C5533B0D34761F3F7373  src/protocols/ttt.c
2D90C25E3A8CE7A5BBF4EBA45E14058564501D5F57FAD52F5FDDADFB204379D9  src/tic_tac_toe_without_revealing_the_strategy/ttt_demo.c
D62E8D8734A5E82CB92E0EF74826742E562557895EDC6428E4ADE30EC100DC84  tests/test_ttt.c
3CFFFBA224EE3F2293827170A5C65373BB38DF34FCFC0191CE64FC4452A6B2F9  document/content/tic_tac_toe_without_revealing_the_strategy/tic_tac_toe_without_revealing_the_strategy.tex
F5C15EA24F8CF7896455972FE97FC9083CA0190769D60F05F314C26B7173B885  research/CHAPTER_MATRIX.md
B854F1AA1F0FA5ED614BC9989B7FAC671E1D0CFDD8343561511FFF8635F785FC  research/CLAIM_LEDGER.md
DE1AFD839A4C9E3E8511F87AAABD1074689CDE79D10DB0645118C692F11B12B2  research/ASSUMPTION_LEDGER.md
90C0E7CDDC6F187A0D6D6F459C7226C97816200F0460C6A00E9135FD78FA42B3  research/OPEN_PROBLEMS.md
64AAEA89BADF2FB8D2DFEBAE24472F0068405E53B56A055522D3DADC62976C24  research/DECISIONS.md
F639FFB7D5BC17FB1FCC12FED519CD9FB986CC3B9CC7305795E174160871C571  research/ROADMAP.md
```

The new plan destination is absent. The chapter backup is protected and has
SHA-256
`4F075F30383ABAB4906E81B2A39DB7A453BC6E80F18C564723129CAEBB609CC2`.

Before copyback, revalidate HEAD, branch, empty index, full status, every
affected destination hash, plan absence, and the protected-file hashes. Require
the staged-versus-live delta to be exactly the twelve-file inventory below.
Copy only explicit files, byte-verify each destination, and copy this completed
plan last.

## Existing Semantic Baseline

The current function `ac_ttt_verify_policy()` remains the diagnostic semantic
baseline. It:

- validates Canonical Policy Version 1;
- follows the policy's one move on claimant turns;
- recursively explores every legal opponent move in ascending-cell order;
- returns a replayable first losing trace; and
- reports DFS history-node counts.

Those diagnostics are not part of the Boolean acceptance relation. The new
evaluator must not call the old verifier, copy its recursive traversal, or
manufacture equivalent exploration counts. It must independently compute the
same canonical-format and losing/non-losing verdicts.

## Public API Contract

Add a dedicated `ac_ttt_dag_report` and:

```text
ac_status ac_ttt_evaluate_policy_dag(
    ac_ttt_role role,
    const ac_ttt_policy *policy,
    ac_ttt_dag_report *report);
```

The report contains:

- verdict and first canonical-format error evidence;
- reachable, X-turn, O-turn, and terminal-state counts;
- X-win, O-win, and draw terminal counts;
- X-edge, O-edge, and total-edge counts;
- graph-scan, policy-scan, value-scan, and evaluated-state counts;
- claimant selector equality, term-AND, and OR-fold counts;
- opponent AND-fold count;
- serialized policy bytes; and
- explicit classification/value-array and board-scratch bytes.

The report deliberately contains no DFS explored-history counts and no losing
trace. For diagnosis, callers retain `ac_ttt_verify_policy()`.

As with the existing API, execution status and mathematical verdict remain
separate. A null pointer or invalid role is `AC_ERR_ARGUMENT`; a malformed
readable policy is an `AC_OK` execution with `INVALID_POLICY`. A non-null report
is cleared before argument validation. Policy and report storage must not
overlap.

## Fixed Public Graph Schedule

The board encoding supplies a topological order without a stored edge table.
For an empty cell `c` and public turn value `t` in `{1,2}`:

```text
child_index = parent_index + t * 3^c.
```

Therefore every legal child index is strictly greater than its parent index.
The implementation uses a fixed powers-of-three table and three complete
public scans:

1. scan indices `0..19682` to propagate reachability and classify the graph;
2. scan indices `0..19682` to validate every canonical policy byte; and
3. scan indices `19682..0` to derive every reachable `q` value.

The graph and value scans may skip arithmetic for publicly unreachable slots,
but they visit all 19,683 slots. The value order is reverse topological because
every child's index is larger. No recursion, queue, heap, randomness, clock,
file, network, cryptographic call, or stored successor table is needed.

The explicit array payload is:

```text
kind_by_board[19683]  = 19,683 bytes
q_by_board[19683]     = 19,683 bytes
board[9]              =      9 bytes
total                 = 39,375 bytes
```

The first two arrays alone are 39,366 bytes. These are explicit C scratch
payload counts, not a complete compiler stack-frame measurement.

## Canonical Validation Without Policy-Selected Addressing

The validator must scan every table entry and must not access `board[move]`.
At each required claimant state, enumerate all public cells `0..8` to derive
whether the move is in range and whether the selected cell is empty. Preserve
the existing reason precedence:

1. `0xff`: missing move;
2. any other byte outside `0..8`: out-of-range move;
3. a byte in `0..8` naming an occupied cell: occupied-cell move; and
4. any non-`0xff` byte at a non-required index: unexpected entry.

Record only the first error in ascending board-index order, for deterministic
diagnosis, but continue the complete scan and DAG evaluation. Final verdict
precedence is:

```text
INVALID_POLICY, if any canonical-format error exists;
NON_LOSING,      otherwise if q[empty_board] == 1;
LOSING,          otherwise.
```

The fixed C schedule is not claimed to be constant-time or side-channel safe.
It is an inspectable relation-oriented computation whose successor addresses
do not depend on the policy.

## Bottom-Up Recurrence

For every reachable state `i`, derive:

```text
q[i] = 1                         terminal, not opponent win
q[i] = 0                         terminal opponent win
q[i] = OR over legal moves a:    claimant turn
       (policy[i] == a) AND q[child(i,a)]
q[i] = AND q[child(i,a)]         opponent turn, over all legal moves
```

At claimant states, enumerate every public legal successor. Never compute a
child address from `policy[i]`. Use bitwise Boolean folds so evaluation never
stops after a success or failure. At opponent states, likewise fold every
public legal child without early failure.

The implementation-operation counts, explicitly not circuit gates or proof
costs, are:

```text
claimant X: 8,631 selector equalities, term-ANDs, and OR folds;
            7,536 opponent AND folds
claimant O: 7,536 selector equalities, term-ANDs, and OR folds;
            8,631 opponent AND folds
```

Both roles scan 19,683 graph slots, 19,683 policy bytes, and 19,683 value
slots; evaluate 5,478 reachable values; and visit 16,167 public graph edges.
These counts describe the C recurrence only. A later circuit must separately
define its gate basis and may simplify folds differently.

## Equivalence Argument

Let `Canon_p(pi)` be Canonical Policy Version 1 validity. For a reachable state
`s`, let `F(s)` be the semantic Boolean returned by the recursive checker after
format validation: a terminal state is false exactly for an opponent win; a
claimant state follows `pi(s)`; and an opponent state is the conjunction over
all legal children.

For every canonical policy, prove `q(s)=F(s)` by strong induction on the number
of empty cells:

- At a terminal state both definitions return false exactly for an opponent
  win and true for a claimant win or draw.
- At a nonterminal claimant state, canonical validity supplies exactly one
  legal selected move. Every other equality bit is zero, so the selector OR
  reduces to the same one child used by `F`; the induction hypothesis applies
  to that child.
- At a nonterminal opponent state, both definitions are the conjunction of
  every legal child value; the induction hypothesis applies to every child.

Thus `q(empty)=F(empty)`. Every move removes one empty cell, and every child
index is larger than its parent, so the descending-index implementation is a
valid reverse topological evaluation of the same recurrence.

Malformed-policy agreement is a separate validation lemma: the new complete
scan and old ascending first-error scan implement the same canonical-format
definition and diagnostic precedence. The induction does not claim equality of
DFS trace or exploration-report side effects.

This is an ordinary mathematical argument plus implementation tests. It is not
machine-checked formal verification of the C program.

## Exact Finite Census

Independent test-local enumeration must reproduce:

```text
reachable states:   5,478
nonterminal states: 4,520
terminal states:      958
X wins:               626
O wins:               316
draws:                 16
X-turn edges:        8,631
O-turn edges:        7,536
total edges:        16,167
history-tree nodes: 549,946
terminal games:     255,168
```

The independent per-ply state/nonterminal/terminal/edge census is:

```text
ply 0:    1 /    1 /   0 /    9
ply 1:    9 /    9 /   0 /   72
ply 2:   72 /   72 /   0 /  504
ply 3:  252 /  252 /   0 / 1512
ply 4:  756 /  756 /   0 / 3780
ply 5: 1260 / 1140 / 120 / 4560
ply 6: 1520 / 1372 / 148 / 4116
ply 7: 1140 /  696 / 444 / 1392
ply 8:  390 /  222 / 168 /  222
ply 9:   78 /    0 /  78 /    0
```

These finite counts are not circuit gates, proof size, security evidence, or
performance claims.

## Differential Test Corpus

The final tests must cover:

1. both deterministic reference policies;
2. the existing explicit X-loss and late O-loss policies, retaining recursive
   replay checks while requiring DAG verdict agreement;
3. malformed missing, byte-9 and byte-254 out-of-range, occupied, opponent-
   turn, terminal, wrong-count, simultaneous-winner, post-win, boundary-index,
   and multiple-defect policies with deterministic first-error evidence;
4. repeated identical evaluations and argument-error report clearing;
5. every legal Hamming-distance-one mutation of each generated reference
   policy: 6,208 X policies and 5,439 O policies, 11,647 total; and
6. 256 deterministic multi-entry policies per role, derived from an explicitly
   fixed integer mixer and legal-move ordinal selection.

The single-entry sweep is exhaustive only over legal one-entry alternatives of
the two generated reference tables. It is not exhaustive over all canonical
policies. Their spaces are much larger:

```text
X: 9 * 7^72 * 5^756 * 3^1372
O: 8^9 * 6^252 * 4^1140 * 2^696.
```

The 512 multi-entry policies form a deterministic corpus, not random sampling;
no probability or coverage bound follows. For every canonical member, require
both functions to return `AC_OK`, neither to return `INVALID_POLICY`, and their
losing/non-losing verdicts to agree. For malformed members, additionally
compare reason, first index, and move evidence. Never compare DFS-specific
history counts or traces with the DAG report.

Tests remain implementation evidence and may miss shared low-level bugs. The
induction and independent census carry different evidentiary roles.

## Affected Files

### New

- `research/plans/tic-tac-toe-fixed-dag-evaluator.md`

### Existing

- `include/ac/ttt.h`
- `src/protocols/ttt.c`
- `src/tic_tac_toe_without_revealing_the_strategy/ttt_demo.c`
- `tests/test_ttt.c`
- `document/content/tic_tac_toe_without_revealing_the_strategy/tic_tac_toe_without_revealing_the_strategy.tex`
- `research/CHAPTER_MATRIX.md`
- `research/CLAIM_LEDGER.md`
- `research/ASSUMPTION_LEDGER.md`
- `research/OPEN_PROBLEMS.md`
- `research/DECISIONS.md`
- `research/ROADMAP.md`

### Protected and Unchanged

- `Makefile.config`, all Makefiles, and all READMEs;
- `book` and the main manuscript include graph;
- `document/references.bib` and `research/BIBLIOGRAPHY_QUEUE.md`;
- `research/PRIMITIVE_REGISTRY.md`, `REPOSITORY_AUDIT.md`, and
  `RENAME_MAP.md`;
- both completed Tic-Tac-Toe plans;
- the chapter `.bak` and every other backup;
- all commitment, RPS, Hash, Poker, and Trust files;
- generated files and repository metadata.

No bibliography change is needed. The recurrence, induction, and finite
census concern the project-defined model and require no external source.

## Verification Contract

Use the existing disposable Debian verification container. Require:

```text
make -C tests ttt \
  BUILD_DIR=/tmp/ac-ttt-dag-strict \
  TEMP_DIR=/tmp/ac-ttt-dag-strict-temp \
  CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'

make -C demostrations demo_ttt \
  BUILD_DIR=/tmp/ac-ttt-dag-demo \
  TEMP_DIR=/tmp/ac-ttt-dag-demo-temp \
  CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'

ASAN_OPTIONS='detect_leaks=1:halt_on_error=1' \
UBSAN_OPTIONS='halt_on_error=1' \
make -C tests ttt \
  BUILD_DIR=/tmp/ac-ttt-dag-san \
  TEMP_DIR=/tmp/ac-ttt-dag-san-temp \
  CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer' \
  LDFLAGS='-fsanitize=address,undefined'

make -C tests test \
  BUILD_DIR=/tmp/ac-ttt-dag-regression \
  TEMP_DIR=/tmp/ac-ttt-dag-regression-temp
```

Run the complete differential corpus under strict optimization and measure it
externally without a timing assertion. Attempt the same complete focused
corpus under ASan/UBSan. If that proves impractical, run and name a fixed
stratified sanitizer subset and keep its evidence separate; do not imply that
the full sweep was sanitized.

Inspect the final demo with `ldd`, `readelf -d`, and `nm -u`; require no
libsodium dependency. Scan touched C files for accidental cryptographic,
randomness, heap, file, clock, or network calls.

Because only the Tic-Tac-Toe chapter changes, rebuild `section_ttt` until
citations stabilize. Require a fresh nonempty PDF and a final log without a
LaTeX error, undefined citation/reference, missing glyph, overfull box,
emergency stop, fatal error, or multiply defined reference. Render and inspect
every final page after the last meaningful manuscript change. No other
standalone section requires rebuilding while shared book and bibliography
files remain protected.

## Baseline Verification Record

Before source edits, the staged checkout produced:

- 8/8 strict Tic-Tac-Toe test groups passing with GCC 12.2.0-compatible C11
  flags in Debian;
- all 41 existing grouped tests passing: 7 commitment, 6 core RPS, 14
  adversarial RPS, 6 Hash, and 8 Tic-Tac-Toe; and
- the public demonstration accepting both reference policies and returning a
  replayable loss for a canonical naive X policy.

The container's libsodium 1.0.18 development package exists only to compile
the repository's pre-existing commitment, RPS, and Hash regression targets.
The Tic-Tac-Toe targets omit `$(LDLIBS)` and this slice adds no dependency.

## Final Staged Verification Record

Verification ran against an exact copy of the final staged source in the
disposable Debian container:

- the strict C11 Tic-Tac-Toe target passed all 13 grouped tests with
  `-Wall -Wextra -Wpedantic -Werror -O2`;
- the complete 13-group corpus passed AddressSanitizer and
  UndefinedBehaviorSanitizer with leak detection enabled and optimized
  instrumentation;
- all 46 project groups passed under `-Werror`: 7 commitment, 6 core RPS, 14
  adversarial RPS, 6 Hash, and 13 Tic-Tac-Toe;
- the strict public demo accepted both reference policies, rejected the
  canonical naive X policy, and printed matching recursive/DAG verdicts;
- `ldd`, `readelf -d`, and `nm -u` on the demo showed only libc and ordinary
  runtime symbols, with no libsodium or added-library dependency;
- a source scan found no cryptographic, randomness, heap, file, clock, or
  network call in the four touched C/API files; and
- the standalone chapter rebuilt to 18 letter-size pages and 285,727 bytes,
  SHA-256
  `9507AD43F3D115BBA53BB4E754C3C3CEF1E3A9B769A9B777C3EA98B18C592688`.
  Its final log has no LaTeX error, undefined citation/reference, overfull box,
  emergency stop, or fatal error. Every page was rendered and inspected after
  the last manuscript change.

The initial sanitizer attempt used the plan's `-O1` command. The external
five-minute command window expired while the instrumented complete corpus was
still running; it had emitted no failure and remained alive in the disposable
container. That orphaned test was stopped, and the same complete corpus was
rebuilt and run to completion with `-O2` sanitizer instrumentation. The passing
record above refers only to the completed run.

Three independent read-only reviews covered C/API safety, recurrence and
finite claims, and manuscript/register/scope integrity. After three P3 wording
clarifications in the registers, all reviewers reported no unresolved
P0/P1/P2 finding. The staged-versus-live delta was exactly the planned twelve
files, and protected Makefiles, `Makefile.config`, book include graph,
bibliography, bibliography queue, completed Tic-Tac-Toe plans, primitive/audit/
rename registers, and chapter backup remained byte-identical at that review.

## Guarded Copyback Record

Immediately before copyback, the authoritative checkout was still `main` at
`2824992ee2802b0b72f67054f63189628eb15103`, its Git index was empty, all eleven
affected destination files retained their preservation-baseline hashes, and
the new-plan destination was absent. Protected-file hashes also matched the
baseline. A complete non-generated tree comparison found exactly the planned
twelve-file delta.

The eleven existing files were copied by explicit name and byte-verified
against the staged source. A second complete tree comparison then found only
this new plan absent from the live checkout. Protected hashes, branch, HEAD,
empty index, and the user's pre-existing dirty status remained unchanged. This
completed plan was copied last and byte-verified. A final comparison found no
non-generated staged/live difference. No file was staged, committed, deleted,
renamed, or restored.

## Acceptance and Evidence Boundary

The slice is complete only when the separate DAG API has one documented
meaning, all fixed counts are reproduced, canonical and malformed verdicts
match the recursive baseline across the specified corpus, the induction is
visible, the original checker retains replay diagnostics, the complete C
regression and focused sanitizer evidence pass, the final chapter renders
cleanly, independent review has no unresolved P0/P1/P2 issue, and guarded
copyback preserves the dirty live tree.

Completion will establish an ordinary mathematical equivalence argument and
strong finite implementation evidence for this fixed public evaluator. It will
not establish formal C verification, constant-time execution, policy privacy,
commitment security, zero knowledge, knowledge extraction, Boolean-circuit
equivalence, proof efficiency, production readiness, or novelty.
