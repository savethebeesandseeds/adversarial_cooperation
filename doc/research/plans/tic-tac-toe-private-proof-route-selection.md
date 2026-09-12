# Tic-Tac-Toe Private-Proof Route Selection

## Status

Complete. This is a design-selection slice, not a proof-system implementation.
A checked item records evidence actually obtained.

## Intended Outcome

Select the smallest scientifically defensible route from the completed public
Tic-Tac-Toe predicate toward the author's private-strategy aspiration.

This slice will:

1. preserve the already fixed game, policy encoding, and
   `PublicNonLose(role, policy)` predicate;
2. separate the one-shot global knowledge argument from private per-state
   strategy service;
3. compare a bounded set of general proof families against the exact relation;
4. select the next executable artifact without importing a proof stack;
5. identify which properties the leading candidate does and does not provide;
   and
6. record the assumptions and implementation gates that must be satisfied
   before any private-proof claim is made.

No C source, public API, Makefile, primitive, vector, proof transcript, or
runtime dependency is added in this slice. Papers and publication identifiers
remain bibliography only.

## Progress

- [x] Governing instructions, research charter, completed public-checker plan,
  chapter, implementation, tests, build files, bibliography, and relevant
  research registers read.
- [x] Live branch, HEAD, dirty status, chapter backup, and preservation boundary
  recorded.
- [x] Baseline 41-group C regression and standalone Tic-Tac-Toe PDF build pass
  in disposable Debian containers.
- [x] Primary-source comparison completed.
- [x] Exact relation decomposition and security target frozen below.
- [x] Immediate DAG-first route and conditional proof-family preference
  selected below.
- [x] Chapter and research registers updated.
- [x] Stabilized PDF, citation, visual, regression, and independent review
  completed.
- [x] Guarded copyback to the authoritative checkout completed.

## Preservation Baseline

The authoritative checkout is:

`C:\Work\applied_cryptography\adversarial_cooperation`

At the start of this slice it is on `main` at
`2824992ee2802b0b72f67054f63189628eb15103`. The Phase 0, RPS, Hash, Trust,
and public Tic-Tac-Toe work remains uncommitted. The pre-existing deletion of
`idyicyanere-linux-x64.vsix` is user-owned. Nothing in this slice may restore,
discard, stage, commit, or rewrite that work.

The isolated working copy is:

`C:\Users\santi\OneDrive\Documents\Adversarial-cooperation\.codex-ttt-proof-stage`

The live chapter SHA-256 at the start of this slice is:

`ABA517AAABE453A035B6D7080206FE009A9945C39CD2642AFFB81222734DE3B6`

The tracked chapter backup SHA-256 is:

`4F075F30383ABAB4906E81B2A39DB7A453BC6E80F18C564723129CAEBB609CC2`

The backup remains protected and byte-identical. Copyback must revalidate the
live hashes of every affected existing file and the absence of the new plan,
then copy only the explicit inventory below and verify byte equality.

## The Exact Goal Remains the Same

Let the public instance be

```text
x = (game_version, claimant_role, initial_state,
     policy_commitment, context)
```

and the private witness be

```text
w = (canonical_policy, commitment_opening).
```

The full relation is usefully decomposed as

```text
Core(x, policy) =
    game_version == 1
    AND claimant_role IN {X, O}
    AND initial_state == EMPTY_BOARD
    AND PublicNonLose(claimant_role, policy)

Bind(x, policy, opening) =
    Open(policy_commitment,
         context,
         Encode(game_version, claimant_role, initial_state, policy),
         opening)

R(x, w) = Core(x, canonical_policy)
          AND Bind(x, canonical_policy, commitment_opening).
```

This decomposition does not weaken the relation. It exposes two different
engineering costs:

- the finite-game computation in `Core`; and
- the commitment compatibility and cryptographic assumptions in `Bind`.

A proof of `Core` alone does not establish consistency with the public
commitment `C`. A proof of `Bind` alone says nothing about policy legality or
non-loss. The repository's instructional nonce-and-hash commitment is not
silently promoted into a formally proved commit-and-prove mechanism.

Omitting `policy_commitment` entirely would reduce the statement to knowledge
of some non-losing Tic-Tac-Toe policy. Because such policies are public and
easy to generate, that one-shot capability claim would carry little strategic
meaning. The commitment is retained to make a stable, context-bound policy the
subject of the statement, while authorship and identity remain separate.

## Selected Protocol Shape

The selected target is a **one-shot global zero-knowledge argument of
knowledge** for `R`. Computational knowledge soundness makes “argument,” not
information-theoretic “proof,” the precise term for this target.

It is not a strategy oracle. The verifier should learn an accept/reject result
for the global statement, not a sequence of policy moves. This avoids creating
the exact unrestricted deterministic interface that the chapter proves is
functionally clonable.

Private per-state evaluation remains a different two-party computation:

```text
input from claimant: committed policy and opening
input from verifier: state
output: policy(state), or a derived legal action
```

That functionality has different input-privacy, output-leakage, fairness,
statefulness, and repeated-query questions. It is not bundled into this proof
selection.

## Provisional Security Target

Before an implementation may use the phrases below as claims, it must name and
meet all of these conditions:

- two roles, prover and verifier;
- classical probabilistic polynomial-time adversaries;
- one standalone session initially, with concurrency and reset security
  excluded;
- perfect or explicitly bounded completeness;
- computational knowledge soundness with an extractor for the complete
  witness required by `R`;
- computational zero knowledge against a malicious verifier, not merely an
  honest verifier;
- an explicit external policy-commitment construction and its binding/hiding
  assumptions;
- any proof-internal commitment to simulated views as a separate construction,
  with its own encoding, domain, binding/hiding assumptions, openings, and
  transcript cost;
- exact public-instance, version, role, initial-state, commitment, and context
  binding;
- an honest-verifier soundness calculation using independent uniform challenges
  generated only after the prover's first commitments, kept distinct from the
  arbitrary PPT challenge strategies covered by the malicious-verifier
  zero-knowledge target; and
- explicit soundness error and repetition count.

The definitions cover the direct abstract prover-verifier transcript. Human
identity, transport authentication, relaying, delivery, and timing are not
provided by the proof layer. They must be instantiated separately if an
application needs them.

The proof layer does not authenticate a human identity, force delivery, prevent
abort, establish authorship or originality, guarantee later policy use, resist
endpoint compromise, or provide concurrent/composable security.

## Bounded Route Comparison

### Classical Generic Zero Knowledge for NP

Goldreich, Micali, and Wigderson establish the conceptual route from an NP
relation to zero knowledge under their stated assumptions. For this project,
that result is foundational evidence of possibility, not an implementation
recipe. Reducing the relation through a generic NP-complete problem would add
representation machinery and large communication without illuminating the
Tic-Tac-Toe-specific predicate.

**Decision:** retain as theory; do not implement the generic reduction as the
chapter companion.

### MPC in the Head and ZKBoo

Ishai, Kushilevitz, Ostrovsky, and Sahai show how a secure multiparty
computation can be simulated by one prover and converted into zero knowledge.
ZKBoo specializes this idea to Boolean circuits using symmetric primitives.
In the commitment-hybrid model, for a correct 2-private
`(2,3)`-decomposition, ZKBoo Proposition 4.2 classifies its basic three-message
protocol as a Sigma protocol with special honest-verifier zero knowledge and
3-special soundness. The relevant bibliography keys are
`ishai2007zeroknowledgemp` and `giacomelli2016zkboo`.

Its proof-internal commitments to simulated views are not the external policy
commitment checked by `Bind`. A future experiment must select, serialize, and
analyze both layers independently. The view-commitment construction and cost
cannot be borrowed silently from the repository's instructional hash
commitment.

For one idealized repetition, a uniformly sampled challenge in `{1,2,3}` gives
a combinatorial soundness term at most `2/3`. Independent repetitions with
independently uniform challenges chosen after the first messages give
`(2/3)^r`; 219 makes only this term at most `2^-128`. Concrete soundness also
depends on the selected view commitment's binding reduction. This formula does
not estimate a proof size: that requires the actual Boolean-circuit gate and
view counts.

The basic Sigma protocol does not, merely by being named, satisfy this slice's
malicious-verifier zero-knowledge target. ZKBoo discusses additional
transformations to full zero knowledge/proof of knowledge, and a
Fiat--Shamir-derived non-interactive form only in the random-oracle model. This
project selects neither transformation yet.

**Decision:** leading family for a later explicitly educational, interactive
experiment, conditional on circuit measurement and a separate analysis of the
full-ZK/knowledge transformation. It is not selected as a production backend
and no current protocol claim follows.

### Transparent Succinct Arithmetic-Circuit Systems

Spartan represents the modern alternative: transparent succinct arguments of
knowledge for R1CS, with variants whose security and setup depend on their
polynomial-commitment instantiation. Its paper reports a substantial Rust
library and a stack involving R1CS, sum-check, computation commitments, and
polynomial commitments. The bibliography key is `setty2020spartan`.

Such a system could eventually give much shorter proofs and faster
verification than a linear MPC-in-the-head transcript. It would also require a
new circuit toolchain, field and group machinery, transcript rules, and a
reviewed implementation backend. Reimplementing that stack in small local C
would be unsafe and scientifically misleading.

**Decision:** credible long-term production-oriented comparison point; defer
implementation and dependency selection.

### Trusted Evaluator and Private Per-State Computation

A trusted evaluator can reuse the public checker but learns the policy and may
misreport or redistribute it. Private per-state computation returns policy
outputs and accumulates leakage. Neither is a public one-shot argument of
knowledge for `R`.

**Decision:** retain both as separate baselines, not proof substitutes.

## Immediate Route: Fixed DAG, Then Circuit, Then Cryptography

The next executable slice should first add a plain-C, proof-backend-neutral
fixed-DAG evaluator, not a proof system. The current recursive checker remains
the semantic baseline.

Independent enumeration gives the fixed public graph:

- 5,478 reachable states;
- 4,520 reachable nonterminal states and 958 terminal states;
- 626 terminal X wins, 316 terminal O wins, and 16 draws;
- 8,631 legal edges out of X-turn states and 7,536 out of O-turn states,
  for 16,167 public edges in total; and
- 549,946 nodes in the unmerged legal history tree, showing why the merged
  state DAG is the appropriate representation.

For every reachable state `i`, process states from greater ply to lesser ply
and derive a public-schedule Boolean value `q[i]`:

```text
q[i] = 1                         terminal, not opponent win
q[i] = 0                         terminal opponent win
q[i] = OR over legal moves a:    claimant turn
       (policy[i] == a) AND q[child(i, a)]
q[i] = AND q[child(i, move)]     opponent turn, over every legal move
```

Accept exactly when the empty-board value is one, after the existing canonical
policy-format checks pass. Every successor address in this selector is public.
The derived `q` values are deterministically constrained intermediates, not
independent semantic witness inputs; a backend may still encode them as
auxiliary wires or witness entries. Induction over remaining empty cells is the
required correctness argument connecting this recurrence to the current DFS.

The fixed-DAG work must:

1. preserve the full Canonical Policy Version 1 semantics;
2. use a deterministic public schedule and public successor indices;
3. avoid recursion, early success/failure exits, and secret table indices by
   enumerating a fixed public-successor selector whose equality and selection
   operations are counted;
4. differentially test the DAG verdict against `ac_ttt_verify_policy()` for
   reference, malformed, losing, and extensively mutated policies;
5. independently check the state, terminal, edge, and history counts above;
6. define a canonical role-specific compact move vector only if it has a
   checked deterministic expansion back to the complete 19,683-byte V1 table;
7. keep the external commitment message bound to the full canonical encoding
   unless a separately versioned policy encoding is approved; and
8. record exact operation, schedule, memory, and serialized-table counts.

Only after the DAG evaluator is equivalent should a later slice add a small
Boolean-circuit intermediate representation. That circuit work must give
public and witness inputs distinct typed wires, use a minimal gate basis,
define deterministic serialization and limits, express `Core` without secret-
dependent control flow, record `Bind` separately, and publish exact gate,
wire, depth, memory, and serialized-size counts before applying any proof-
family cost formula.

The current C checker uses recursion, early returns, and array access for
ordinary execution. Mechanically compiling that program would preserve
implementation accidents such as first-counterexample reporting and could
produce huge secret-index multiplexers. The bottom-up DAG instead expresses
the mathematical acceptance relation directly.

`Bind` remains an implementation gate. Either the existing BLAKE2b-based
encoding must be represented faithfully and its assumption mismatch kept
visible, or a different commitment must be selected through a separately
versioned design decision. The circuit slice may measure `Core` before `Bind`,
but it may not call `Core` the full relation `R`. Proof-internal commitments to
MPC views are a third, separate implementation gate if that family remains the
choice.

## Finite Enumeration Evidence

On 2026-07-21, Python 3.12.13 from the bundled Codex runtime executed the
following program from standard input. It writes no repository file. The
visited set merges transpositions for graph counts; `history()` deliberately
sums every child occurrence, so its node and terminal-game counts retain
distinct play histories.

```python
from functools import lru_cache

EMPTY, X, O = 0, 1, 2
LINES = ((0,1,2),(3,4,5),(6,7,8),(0,3,6),(1,4,7),(2,5,8),
         (0,4,8),(2,4,6))

def winner(b, mark):
    return any(all(b[i] == mark for i in line) for line in LINES)

def terminal(b):
    return winner(b, X) or winner(b, O) or EMPTY not in b

def turn(b):
    return X if b.count(X) == b.count(O) else O

seen = set()
edges_x = edges_o = 0

def visit(b):
    global edges_x, edges_o
    if b in seen:
        return
    seen.add(b)
    if terminal(b):
        return
    mark = turn(b)
    for i, cell in enumerate(b):
        if cell == EMPTY:
            child = b[:i] + (mark,) + b[i+1:]
            if mark == X:
                edges_x += 1
            else:
                edges_o += 1
            visit(child)

@lru_cache(None)
def history(b):
    if terminal(b):
        return (1, 1)
    nodes, games = 1, 0
    mark = turn(b)
    for i, cell in enumerate(b):
        if cell == EMPTY:
            child = b[:i] + (mark,) + b[i+1:]
            child_nodes, child_games = history(child)
            nodes += child_nodes
            games += child_games
    return nodes, games

root = (EMPTY,) * 9
visit(root)
terminal_states = [b for b in seen if terminal(b)]
x_turn = sum(not terminal(b) and turn(b) == X for b in seen)
o_turn = sum(not terminal(b) and turn(b) == O for b in seen)
x_wins = sum(winner(b, X) for b in terminal_states)
o_wins = sum(winner(b, O) for b in terminal_states)
draws = sum(not winner(b, X) and not winner(b, O)
            for b in terminal_states)
history_nodes, terminal_games = history(root)

print(f"reachable={len(seen)}")
print(f"x_turn={x_turn} o_turn={o_turn} terminal={len(terminal_states)}")
print(f"x_wins={x_wins} o_wins={o_wins} draws={draws}")
print(f"edges_x={edges_x} edges_o={edges_o} "
      f"edges_total={edges_x + edges_o}")
print(f"history_nodes={history_nodes} terminal_games={terminal_games}")
```

Recorded output:

```text
reachable=5478
x_turn=2423 o_turn=2097 terminal=958
x_wins=626 o_wins=316 draws=16
edges_x=8631 edges_o=7536 edges_total=16167
history_nodes=549946 terminal_games=255168
```

This is reproducible finite-enumeration evidence, not a proof-system cost,
formal verification result, or substitute for the planned C DAG tests.

## Why This Preserves Minimalism

- No new library is added before the computation is measured.
- The existing public checker remains the semantic oracle.
- The fixed DAG and later Boolean circuit are reusable by more than one proof
  family.
- The commitment cost cannot disappear behind an attractive proof-size claim.
- A later production path may use a reviewed external backend without forcing
  that dependency into the book's foundational C examples.
- Any locally written MPC-in-the-head code must live in an unmistakably
  educational/toy area and print a non-production warning.

Minimalism here means minimizing hidden assumptions and trust surfaces, not
reimplementing sophisticated cryptography from scratch.

## Source Gate

Add only three route-selection sources:

1. Ishai--Kushilevitz--Ostrovsky--Sahai 2007 for the MPC-in-the-head
   construction and circuit-size relationship.
2. Giacomelli--Madsen--Orlandi 2016 for the concrete Boolean-circuit Sigma
   protocol, its exact base properties, and its Fiat--Shamir boundary.
3. Setty 2020 for a transparent R1CS argument-of-knowledge comparison and its
   implementation complexity.

GMW, GMR, Bellare--Goldreich, and Naor are already present for the existing
definition-level discussion. No survey, product documentation, package, or
implementation repository is needed for an active manuscript claim.

Contribution classification remains:

> A literature-backed route-selection and relation-decomposition note for an
> existing public finite-game predicate; no new proof system, construction, or
> novelty claim.

## Affected Files

### New

- `research/plans/tic-tac-toe-private-proof-route-selection.md`

### Existing

- `document/content/tic_tac_toe_without_revealing_the_strategy/tic_tac_toe_without_revealing_the_strategy.tex`
- `document/references.bib`
- `research/BIBLIOGRAPHY_QUEUE.md`
- `research/CHAPTER_MATRIX.md`
- `research/CLAIM_LEDGER.md`
- `research/ASSUMPTION_LEDGER.md`
- `research/OPEN_PROBLEMS.md`
- `research/DECISIONS.md`
- `research/ROADMAP.md`

### Protected and Unchanged

- the completed public-checker execution plan;
- `include/ac/ttt.h`, `src/protocols/ttt.c`, the demo, and all tests;
- every Makefile, README, test vector, and other chapter;
- `book` and the main manuscript include graph;
- `research/PRIMITIVE_REGISTRY.md`, `REPOSITORY_AUDIT.md`, and `RENAME_MAP.md`;
- the chapter `.bak` and every other backup;
- generated files and repository metadata.

## Verification Contract

Because no executable source changes, proportionate verification is:

- run the complete 41-group C regression once and require all existing groups
  to pass;
- rebuild `section_ttt` until citations stabilize;
- scan the final log for LaTeX errors, undefined references/citations, missing
  glyphs, and overfull boxes;
- render and inspect every final Tic-Tac-Toe page after the last meaningful
  manuscript change;
- rebuild `section_rps`, `section_hash`, and `section_trust` because the shared
  bibliography changes;
- verify every protected C/build file remains byte-identical; and
- obtain independent scientific/claim and repository-scope review with no
  unresolved P0, P1, or P2 finding.

Sanitizer evidence is not rerun because no C source changes. Existing
sanitizer evidence remains attached only to the completed public-checker slice.

## Verification Record

The final staged sources produced the following evidence in disposable Debian
containers:

- `make -C tests test` passed all 41 existing groups: 7 commitment, 6 core
  RPS, 14 adversarial RPS, 6 hash, and 8 Tic-Tac-Toe;
- no C source, public header, test, Makefile, primitive, vector, or runtime
  dependency changed, so the completed slice's sanitizer evidence was not
  relabeled as evidence for this manuscript-only work;
- the final `section_ttt` PDF is 16 pages and 275,640 bytes;
- `section_rps`, `section_hash`, and `section_trust` rebuilt as fresh PDFs of
  18 pages / 245,232 bytes, 10 pages / 214,224 bytes, and 12 pages / 197,238
  bytes respectively after the shared bibliography changed;
- the four stabilized final logs contain no LaTeX error, undefined citation or
  reference, missing glyph, overfull box, emergency stop, fatal error, missing
  bibliography, or multiply defined reference;
- every page of the final Tic-Tac-Toe PDF was rendered at 144 dpi and visually
  inspected after the last manuscript change; no clipping, overlap, broken
  equation, unreadable text, bad page transition, or citation defect remains;
- the finite enumeration program and its exact output are recorded above; an
  independent reviewer also reproduced the counts and the idealized 219-term
  calculation; and
- independent scientific, editorial/LaTeX, and repository/register reviews
  report no unresolved P0, P1, or P2 finding. Their corrections included the
  proof-versus-argument distinction, public-instance leakage, separate
  commitment layers, Proposition 4.2 conditions, total-soundness boundary,
  honest- versus malicious-verifier challenge behavior, fixed-selector
  recurrence, auxiliary-wire semantics, communication boundary, open-problem
  status, and evidence language.

This evidence establishes only the route selection, finite enumeration,
manuscript rendering, unchanged-code regression, and recorded scope. It does
not establish any private argument or cryptographic property listed below.

The guarded copyback then wrote only the nine existing files and one new plan
listed in the affected-file inventory. All ten authoritative SHA-256 hashes
matched the reviewed staging files afterward. The checkout remained on `main`
at `2824992ee2802b0b72f67054f63189628eb15103`, the Git index remained clean,
and the chapter backup remained byte-identical with SHA-256
`4F075F30383ABAB4906E81B2A39DB7A453BC6E80F18C564723129CAEBB609CC2`.

## Acceptance and Evidence Boundary

This slice is complete only when one immediate route is selected for named
reasons, alternatives are deferred for named reasons, the full relation is not
silently reduced to `Core`, ZKBoo's honest-verifier property is not promoted to
malicious-verifier zero knowledge, no proof or efficiency number is claimed
without a circuit, no dependency or primitive is added, all final pages are
visually clean, and copyback preserves the dirty authoritative tree.

Completion establishes that the project has a disciplined next experiment. It
does not establish policy privacy, zero knowledge, knowledge extraction,
commitment security, a sound proof transcript, circuit equivalence, proof
efficiency, production readiness, or novelty.
