# Execution Plan: Tic-Tac-Toe `Core` as a Boolean Circuit

## Status

Complete. This plan covers one bounded, dependency-free bridge from the
completed fixed-DAG evaluator to a backend-neutral Boolean representation of
`Core`. It does not implement `Bind`, a commitment, a proof system, or a
private protocol.

## Objective

Construct two deterministic Boolean circuits, `Core-X` and `Core-O`, that each
accept exactly when all of the following hold:

1. the public game version is 1;
2. the public claimant role equals the circuit specialization;
3. the public initial-board index is zero;
4. the complete 19,683-byte policy is Canonical Policy Version 1 for that role;
5. the established fixed-DAG root value is non-losing.

Then compare the circuit result with both existing public C evaluators over a
named bounded corpus. The existing recursive checker, fixed-DAG evaluator, and
their tests remain byte-identical so that they are independent semantic
baselines.

## Scientific Boundary

This slice establishes only that a particular public Boolean netlist executes
and agrees with the two existing C paths on specified tests. The policy is
passed in plaintext to every evaluator.

The word `witness` identifies an input-wire range; it does not imply secrecy.
Circuit serialization is not a commitment. Evaluating `Core` is not proving
the complete relation `R`. `Bind`, the external policy commitment, all
proof-internal commitments, proof transformations, soundness, extraction,
zero knowledge, privacy, proof size, interaction, and production security stay
out of scope. Gate counts and depth describe only this frozen IR construction;
they are not lower bounds or backend costs. Tests are implementation evidence,
not formal verification.

## Frozen Design Decisions

### Circuit family

Build two role-specialized circuits rather than one universal-role circuit.
Each specialization still receives and checks a public role byte. This keeps
the first artifact smaller and makes its recurrence and independent counts
direct. A future backend may motivate a separately versioned universal
circuit, but this plan does not pre-empt that decision.

### Inputs

The canonical packed public input is four bytes:

```text
[game version][claimant role][initial board index high][initial board index low]
```

The board index is an unsigned big-endian 16-bit integer. Version 1 accepts
only index zero. The witness-typed input is the existing 19,683 policy bytes,
unchanged.

Within each packed byte, bit zero is assigned first. Normal wire order is:

1. constant-zero wire 0;
2. constant-one wire 1;
3. 32 public input bits;
4. 157,464 witness input bits;
5. gate outputs in append order.

Thus policy byte `i`, bit `b`, is wire `34 + 8*i + b`, and the first gate
output is wire 157,498.

### Generic Boolean IR

The generic layer uses only two-input XOR and AND gates. A gate record contains
two 32-bit operand wire numbers, an 8-bit opcode, and three reserved zero bytes.
The in-memory record is required to occupy 12 bytes in the tested C11 ABI, but
raw structs are never serialized.

The circuit view borrows caller-owned gate storage. The library allocates
nothing. Validation requires exact derived wire counts, zero reserved bytes,
known opcodes, one output below the wire count, and operands strictly earlier
than the gate output. All count, byte/bit, wire, and serialization arithmetic is
checked before use.

The evaluator receives packed public and witness bytes, uses one caller-owned
byte per wire, and returns one acceptance bit. Exact byte lengths are required.
For a non-byte-aligned generic input, unused high bits of the last byte must be
zero. Inputs, scratch, output, gates, and serialization buffers are documented
as distinct. Scratch is cleared through a best-effort volatile C11 wipe before
return; this is not a guarantee about registers, compiler copies, the OS, or
caller-owned input buffers.

### Canonical serialization

Serialization is byte-defined and big-endian; it never copies a C struct.

The 32-byte header is:

```text
magic             8 bytes: "ACBCIR01"
format version    u16be: 1
basis             u16be: 1 (XOR/AND)
public bits       u32be
witness bits      u32be
gate count        u32be
wire count        u32be
output wire       u32be
```

Each gate is nine bytes:

```text
[opcode:u8][left:u32be][right:u32be]
```

The exact encoded length is `32 + 9 * gate_count`. The serializer rejects
invalid circuits, insufficient capacity, arithmetic overflow, and overlap with
borrowed circuit storage. An independent parser in the tests checks the
serialized format's magic, counts, references, opcodes, exact length, and
absence of trailing bytes. In-memory validation and generic negative tests
separately enforce the three reserved zero bytes of each 12-byte C gate record.
A public decoder is not added in this slice because no repository consumer
needs one.

### Frozen circuit construction

For byte `z` and public constant `c`, equality is a balanced AND of eight bit
literals. A zero literal is `z_b XOR 1`; a one literal is `z_b`. Required
policy bytes share only the complemented bit wires demanded by at least one
legal cell constant. Each legal-action equality is built once and reused in
canonical validation and the claimant recurrence. Non-required bytes use a
balanced AND of all eight raw bits to test equality with `0xff`.

At a required state, canonical validity is the balanced XOR of its distinct
legal-action equality bits. XOR equals OR here because equality with distinct
byte constants is mutually exclusive even for malformed bytes. Policy validity
is the balanced AND of all 19,683 per-entry validity bits.

The fixed-DAG recurrence is emitted in descending board-index order. A safe
terminal aliases constant one and an opponent-win terminal aliases constant
zero. At a claimant state, each action equality is ANDed with the corresponding
child value and the mutually exclusive terms are folded with XOR. At an
opponent state, every child value is folded with AND. The final acceptance wire
ANDs the public checks, complete policy validity, and `q[0]`, pairing the two
shallower terms before the root value so that the deepest input receives one
final gate.

The generator uses an ascending public reachability classification equivalent
to the established game graph. It has no recursion, heap, randomness, crypto,
clock, file, network, table address chosen by a policy byte, or external
dependency. Caller-owned builder scratch contains only the classification,
first selector wire per state, one reusable wire/depth signal per state, and a
nine-byte board.

### Independently derived and confirmed counts

Let `M=19683`, `N_X=2423`, `E_X=8631`, `N_O=2097`, and `E_O=7536`.
Policy equality AND gates for claimant `p` are
`7 * (E_p + M - N_p)`. Recurrence AND gates are
`E_p + (E_other - N_other)`. The policy reduction uses `M-1` AND gates.
The three public equalities and their combination use 31 AND gates, and final
acceptance uses two more. Demand-shared policy complements are 18,904 for X and
16,436 for O. Selector/validity XOR folds contribute
`2 * (E_p - N_p)` gates, and the four public bytes contribute 30 XOR gates.

Independent derivation and emitted-gate tests agree on these values:

| Quantity | Core-X | Core-O |
|---|---:|---:|
| Public input bits | 32 | 32 |
| Witness input bits | 157,464 | 157,464 |
| AND gates | 215,022 | 209,313 |
| XOR gates | 31,350 | 27,344 |
| Total gates | 246,372 | 236,657 |
| Total wires, including two constants | 403,870 | 394,155 |
| Output depth | 31 | 30 |
| Gate storage at 12 bytes/gate | 2,956,464 | 2,839,884 |
| Evaluator scratch bytes | 403,870 | 394,155 |
| Canonical serialized bytes | 2,217,380 | 2,129,945 |

The named builder-scratch struct occupies 255,888 bytes on the tested ABI:
three
19,683-entry 32-bit arrays, one 19,683-byte classification array, and a
nine-byte board. The implementation reports `sizeof` rather than assuming an
ABI-independent padding rule.

## Exact File Scope

Create:

1. `research/plans/tic-tac-toe-core-boolean-circuit.md`
2. `include/ac/bool_circuit.h`
3. `include/ac/ttt_core_circuit.h`
4. `src/circuits/bool_circuit.c`
5. `src/circuits/ttt_core_circuit.c`
6. `tests/test_bool_circuit.c`
7. `tests/test_ttt_core_circuit.c`

Modify:

8. `src/Makefile`
9. `tests/Makefile`
10. `src/README.md`
11. `src/tic_tac_toe_without_revealing_the_strategy/ttt_demo.c`
12. `document/content/tic_tac_toe_without_revealing_the_strategy/tic_tac_toe_without_revealing_the_strategy.tex`
13. `research/CHAPTER_MATRIX.md`
14. `research/CLAIM_LEDGER.md`
15. `research/ASSUMPTION_LEDGER.md`
16. `research/OPEN_PROBLEMS.md`
17. `research/DECISIONS.md`
18. `research/ROADMAP.md`

No other file is in scope.

## Preservation Baseline

The isolated stage was byte-identical to the live non-generated tree before
this plan. The following stage hashes freeze all existing files in the slice:

| File | SHA-256 before this slice |
|---|---|
| `src/Makefile` | `0b8d5d00b25f155d3d2094145a70e4ee3d6f7db8238fa3988182b93cbdf53f2a` |
| `tests/Makefile` | `f6dfd9038e9bf4d2b0787e929d8c8487dd44de4b7b0f9dd50780e95f01fe1f4d` |
| `src/README.md` | `fff2b73490e712bfde31e667ed4ddad89e06a5973215946690bc4aea4a1e96bc` |
| `src/tic_tac_toe_without_revealing_the_strategy/ttt_demo.c` | `d40a60a0cbd15912bc1241791188d829431e779c627a03fb5bdbda88c99e60c4` |
| TTT chapter | `ba59807f76c7ae083919f138079926b498730d5c9617e8f116a39c1099cef1a7` |
| `research/CHAPTER_MATRIX.md` | `2c594fc9ebc7a324b52c7bc13eddea4beb5250867c3f66679934e5fc3f594af5` |
| `research/CLAIM_LEDGER.md` | `666d0889e35014f066181dcf12a8e3839ec825aedc81c66b78492a81299b747b` |
| `research/ASSUMPTION_LEDGER.md` | `e6339c5af75f1c9fdc19ffd93662d3f6c3491c0214edadf21298a4d5cffd3e29` |
| `research/OPEN_PROBLEMS.md` | `7cdc3989cb53ad7ba1feac196459044916697dc60ebb15b168ce57966971bd0e` |
| `research/DECISIONS.md` | `fa5b162a43bee1a93cb1cea0ab5674d6bbb5c6a000b44d3885b89ee169605169` |
| `research/ROADMAP.md` | `5926a60b51254a58f524ae1329c6369d9f214067a3b4b508aa53eeab5f066683` |

Keep these semantic baselines byte-identical:

| Protected file | Frozen SHA-256 |
|---|---|
| `include/ac/ttt.h` | `4fc5d6a78920c9395cd70384368a5419f57ef1ba3fdc8ba4beb7a79b4f388b48` |
| `src/protocols/ttt.c` | `c24614d86b090a1a86ae5a426d864fc117e32a01dd5970a0a0acdf2c6f8d513e` |
| `tests/test_ttt.c` | `a55211c6b702f27e263ef719d335d05c412c0fa000336287aa14bdfd4dad1b27` |
| `Makefile.config` | `2adc1bcf6c37efc9563cafc67f39edf3053867b46143818dff66b99333f826e0` |
| `demostrations/Makefile` | `68dd65d5ce737ff78784c49603e7355c2fccee7b688db18f83c095739fa284e0` |
| `book` | `09a90cf6f85842b781992c99183bcc9344aca1d4c4ec9ff8ef607a42ef4b8cd1` |

Also protect the completed TTT plans, the chapter backup, main include graph,
bibliography, bibliography queue, primitive registry, audit, rename map, every
other chapter and implementation, generated files, Git metadata, and the
pre-existing deleted `idyicyanere-linux-x64.vsix` status.

## Implementation Sequence

1. Add and test the generic Boolean IR in isolation.
2. Add the deterministic TTT Core generator and public-input encoder.
3. Independently recount and parse both emitted circuits.
4. Differentially compare circuit acceptance with both preserved C evaluators.
5. Link the circuit into the existing TTT demonstration without `$(LDLIBS)`.
6. Update only the chapter and six research registers named above.
7. Rebuild and inspect the standalone TTT PDF.
8. Re-run strict, sanitizer, dependency, forbidden-call, scope, and hash checks.
9. Copy back only after every live destination still matches its frozen hash or
   remains absent as recorded.

## Verification Corpus

The generic tests cover complete XOR/AND truth tables, constants, public and
witness bit ordering, a compound circuit, independently computed depth,
malformed opcodes, reserved bytes, self/future references, wrong wire/output
counts, null and capacity errors, exact lengths, non-bit trailing input,
overlap rejection, best-effort scratch clearing, deterministic serialization,
a hand-computed tiny serialization vector, and serialization boundaries.

For each role, the TTT tests build one circuit and compare its acceptance bit
with both preserved C evaluators on:

- the reference policy;
- a known canonical losing policy;
- all 256 byte values at board index zero;
- named missing, occupied, out-of-range, and unexpected-entry policies;
- 256 deterministic stratified legal one-entry mutations;
- 128 deterministic canonical multi-entry policies;
- invalid game version, role, and initial-board public inputs.

The test may use an independent 64-lane interpreter over canonical serialized
gates to keep the bounded corpus practical. Scalar evaluation remains the
production API and is exercised on representative positive and negative cases.
Every comparison is only the final Boolean result; diagnostics and traces are
not forced into equivalence.

Two complete builds and serializations must be byte-identical. Independent
arithmetic must match emitted opcode counts, wires, depth, storage, and encoded
length. The demo and new tests must have no libsodium linkage and the new source
must contain no crypto, randomness, clock, file, or network call.

## Commands

Run in the disposable Linux environment with strict flags:

```text
make -C tests bool_circuit CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C tests ttt_core_circuit CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C tests ttt CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C tests test CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C demostrations demo_ttt CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
make -C tests sanitize CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O1 -g'
```

Also run the standalone chapter build prescribed by `book`, render every page,
and inspect the images. Record the exact environment, full group count, elapsed
time for the new bounded corpus, and any unavailable tool separately from code
failures.

## Completion Record

- **Environment:** Disposable Debian GNU/Linux 12 container, GCC 12.2.0 on the
  tested x86-64 flat-address ABI. LaTeX and Poppler were installed only inside
  that disposable container for rendering; they are not project dependencies.
- **Files changed:** Exactly the 18 paths in `Exact File Scope`: seven new and
  eleven modified. No bibliography, primitive registry, protected evaluator,
  completed earlier plan, backup, or generated artifact is in the slice.
- **Exact emitted counts:** `Core-X` emits 215,022 AND plus 31,350 XOR gates,
  246,372 gates total, 403,870 wires, depth 31, and 2,217,380 serialized bytes.
  `Core-O` emits 209,313 AND plus 27,344 XOR gates, 236,657 gates total,
  394,155 wires, depth 30, and 2,129,945 serialized bytes. Gate storage is
  2,956,464/2,839,884 bytes, evaluator scratch is 403,870/394,155 bytes, and
  builder-scratch `sizeof` is 255,888 bytes on the tested ABI.
- **Differential corpus executed:** 653 executions per specialization, 1,306
  total. Category overlap leaves 649 distinct X tuples and 651 distinct O
  tuples, 1,300 distinct tuples total. The independent serialized 64-lane
  interpreter agrees with both protected C evaluators throughout; the scalar
  production evaluator separately agrees on four representative cases per
  role. One timed strict run of the three Core groups completed in three
  whole seconds in the otherwise idle container.
- **Strict build/test results:** All 62 project groups passed under C11,
  `-Wall -Wextra -Wpedantic -Werror -O2`. The affected final generic/Core
  subset passed 13+3 groups again after the last alias-safety edit. The strict
  TTT demo passed and printed both exact reports and rejection of the known
  losing policy. GCC `-fanalyzer` reported no finding for the four new source
  and test translation units.
- **Sanitizer results:** All 62 project groups passed with AddressSanitizer and
  UndefinedBehaviorSanitizer when run sequentially. The final affected
  generic/Core binaries passed 13+3 groups after the last edit. A combined
  background invocation once hit the tool window/transient ASan launcher
  behavior; every binary passed when rerun normally, so this was recorded as
  an execution-environment event rather than hidden as a code result.
- **Dependency/forbidden-call results:** `readelf` reports only `libc.so.6` for
  the TTT demo and both new tests. The final source scan found no libsodium,
  crypto, randomness, heap, clock, file, or network call. The TTT link rules
  use no `$(LDLIBS)`.
- **PDF build and page inspection:** The standalone chapter stabilized at 20
  letter-size pages, 303,714 bytes, PDF 1.5, SHA-256
  `a88dda5698a0ebed532b113feefbaafec0243bafff8cce82e97a3fa013e42cdc`.
  The issue scan was clean, all 20 rendered pages were inspected, and the
  circuit-count table and surrounding pages were additionally checked at full
  size.
- **Protected-hash check:** Before copyback, the six explicitly protected
  files match their frozen hashes in both trees; the eleven live modification
  destinations match their frozen preimages; all seven new destinations are
  absent; 163 other common non-generated files are byte-identical; and the
  non-generated delta is exactly the planned 18 paths. The pre-existing
  deleted `idyicyanere-linux-x64.vsix` remains untouched.
- **Independent review:** The C/API, mathematical/protocol, and scope reviews
  have no unresolved P0/P1/P2 finding after corrections. Residual limits are
  the bounded corpus, lack of formal C verification, and tested-ABI rather
  than universal ISO C11 portability.
- **Guarded copyback result:** Passed. Immediately before copyback, all eleven
  preimages, seven absence conditions, and six protected hashes were checked
  again. Only `src/circuits/` was created, only the 18 named files were copied,
  and every destination hash then matched its staged source. No commit, stage,
  rename, dependency installation, generated artifact, or unrelated live file
  was changed.
- **Remaining open work:** `Bind`, selection and measurement of the external
  and proof-internal commitments, the proof transformation, soundness,
  extraction, zero knowledge, privacy, transcript cost, and production
  security all remain separate future work.
