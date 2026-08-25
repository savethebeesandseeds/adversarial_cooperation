# Research and Engineering Decisions

## Reading Key

These records explain choices already made for a bounded slice. They do not
authorize unrelated refactors, renames, cleanup, or stronger research claims.
Material deviations require a new entry with rationale and evidence.

## Whole-Book Assembly

### D-BOOK-001 - Keep every live content source visible in one status-labeled draft

- **Status:** Accepted editorial and preservation decision.
- **Decision:** The main manuscript is the inclusive Complete Research Draft.
  Its include graph exposes all 27 live content sources: 22 chapters and
  research notes in four main parts, followed by five appendices. A front-
  matter reading guide and local research-status notices distinguish developed
  chapters from intuitions, open problems, incomplete fragments, and
  placeholders.
- **Rationale:** Excluding unfinished work made the PDF look like a book about
  only the few technically mature slices and hid the wider research program.
  Visibility lets the author and reader see the actual scope without forcing
  every idea into theorem or protocol form.
- **Consequence:** Presence in the main PDF is not evidence of maturity,
  correctness, security, novelty, citation completeness, or implementation.
  Existing claim, assumption, and verification boundaries remain in force.
  Later work deepens the visible chapter in place; it should not express
  scientific uncertainty by silently removing the chapter from the book.

## RPS Vertical Slice

### D-RPS-001 — Provisional interpretation of the oracle sentence

- **Status:** Accepted for this slice; original meaning remains open.
- **Decision:** Preserve the author's sentence verbatim and operationalize
  “seeing the other's move in advance” as the advantage obtained by learning a
  peer's plaintext move before fixing one's own.
- **Rationale:** This yields a finite, falsifiable first protocol without
  claiming that literal foreknowledge or prophecy has been solved.
- **Consequence:** The chapter distinguishes the narrow timing game from the
  larger oracle metaphor and keeps the latter as an open problem.

### D-RPS-002 — Contribution classification

- **Status:** Accepted.
- **Decision:** Classify the slice as a known commit--reveal pattern used to
  establish the project's method. Make no novelty claim.
- **Rationale:** Blum and later commitment literature predate this project;
  the contribution here is disciplined exposition and implementation, not an
  assertion of cryptographic novelty.

### D-RPS-003 — Minimal, replaceable implementation dependency

- **Status:** Accepted; reflects the author's explicit preference for purity
  and minimal libraries.
- **Decision:** Define the protocol independently of software libraries. Use
  C11 and the repository's existing libsodium dependency as the sole external
  cryptographic backend for BLAKE2b, operating-system randomness,
  constant-time comparison, and clearing helpers. Add no second cryptographic
  library and no testing framework dependency.
- **Rationale:** Protocol logic, state, messages, and arguments remain visible
  in ordinary C while security-sensitive low-level operations are delegated to
  one reviewed backend.
- **Clarification:** DOIs identify papers and RFC 7693 specifies BLAKE2; neither
  is a software dependency.

### D-RPS-004 — Exact commitment suite and claim boundary

- **Status:** Accepted as an educational construction.
- **Decision:** Use canonical context encoding, a fresh 32-byte nonce, and a
  32-byte unkeyed BLAKE2b digest. Describe hiding through an explicitly labeled
  random-oracle-style argument and binding through a collision/alternate-
  opening argument.
- **Rationale:** No primary source directly proving this exact encoded
  nonce-and-hash construction was identified. A related formal commitment
  construction must not be substituted as its proof.
- **Consequence:** No theorem, production, information-theoretic, post-quantum,
  or composability claim is permitted.

### D-RPS-005 — Canonical encoding ownership

- **Status:** Accepted.
- **Decision:** The protocol specification owns all byte layouts: fixed domain,
  version, session, round, roles, types, lengths, and big-endian integers. C
  structures are never wire encodings.
- **Rationale:** One byte sequence must have one meaning across platforms and
  implementations.

### D-RPS-006 — Channel and identity boundary

- **Status:** Accepted assumption, not implemented functionality.
- **Decision:** Assume origin-authenticated, integrity-protected transport for
  accepted messages. Permit delay, loss, duplication, and reordering in the
  adversary model, with fail-closed behavior rather than recovery in v1.
- **Rationale:** Implementing PKI, identity establishment, and networking would
  obscure the first protocol's chronology and enlarge the trust surface.
- **Consequence:** Session and role fields bind context but do not authenticate
  people.

### D-RPS-007 — Fairness and delivery are explicit non-goals

- **Status:** Accepted.
- **Decision:** Make the second-revealer's selective-abort power a mandatory
  counterexample in prose, tests, demo, claims, and open problems.
- **Rationale:** An API can withhold its result until both openings arrive, but
  it cannot stop a malicious host from combining a received reveal with its
  own private move and then refusing to reveal.
- **Consequence:** The protocol provides neither strong fairness nor guaranteed
  output delivery. An abort or timeout does not repair this asymmetry.

### D-RPS-008 — Canonical manuscript and bibliography ownership

- **Status:** Accepted.
- **Decision:** Keep one canonical chapter at the existing Unicode path. The
  chapter owns citation commands; the standalone/book wrapper owns bibliography
  emission. Seed `document/references.bib` only with verified RPS sources.
- **Rationale:** Duplicate prose and bibliography declarations would drift.

### D-RPS-009 — RPS path rename deferred

- **Status:** Accepted deferral.
- **Decision:** Retain the current en-dash filesystem path for this slice and
  use ASCII for new C, test, and target names.
- **Rationale:** Approval to proceed is not approval for a filesystem rename.
  `RN-011` remains a separate migration decision.

### D-RPS-010 — Generated-artifact policy unchanged

- **Status:** Accepted deferral.
- **Decision:** Do not modify `.gitignore`, delete tracked outputs, or perform a
  global build-tree cleanup in this slice. Verification should use explicit
  disposable output directories.
- **Rationale:** Existing generated files and the pre-existing VSIX deletion
  are user-owned preservation concerns.

### D-RPS-011 — Transcript meaning

- **Status:** Accepted.
- **Decision:** Export accepted messages in canonical role/phase slots while
  keeping local timeout and failure observations separate.
- **Rationale:** Honest completed peers can compare identical canonical bytes,
  but dropped delivery can create different local views.
- **Consequence:** The unsigned transcript is not consensus, authentication,
  non-repudiation, proof of delivery, or proof of motive.

## Hash Functions Foundation Repair

### D-HASH-001 — One fixed, narrow wrapper

- **Status:** Accepted for the educational foundation.
- **Decision:** Expose only BLAKE2b-256 over a length-known byte string through
  `ac_hash_blake2b_256()`. Do not introduce a generic algorithm selector,
  streaming interface, MD5 implementation, or custom hash implementation.
- **Rationale:** A fixed profile keeps the byte contract inspectable and uses
  the repository's one existing cryptographic dependency without enlarging the
  runtime trust surface.

### D-HASH-002 — References are not dependencies

- **Status:** Accepted.
- **Decision:** Use primary papers, RFCs, a NIST publication, and official
  libsodium documentation only to support narrowly matched claims. Add no
  software library for citation or historical examples.
- **Rationale:** A DOI is a stable publication identifier. It neither executes
  nor enters the build. The project remains C11 plus libsodium.

### D-HASH-003 — Security notions remain separate

- **Status:** Accepted.
- **Decision:** Define preimage, second-preimage, and collision resistance as
  separate experiments; present `2^n` and `2^(n/2)` only as idealized generic
  classical scales; label diffusion measurements as non-proofs.
- **Rationale:** The original draft contained valuable intuitions but promoted
  them into absolute or universal claims.

### D-HASH-004 — MD5 is historical-only

- **Status:** Accepted.
- **Decision:** Retain one independently executed historical collision pair in
  the vector record and appendix, but do not add MD5 to project code. Withdraw
  the draft's incorrect `ello, World!` output.
- **Rationale:** The counterexample teaches the boundary clearly without
  making a broken algorithm part of the runtime.

### D-HASH-005 — Evidence boundary

- **Status:** Accepted.
- **Decision:** Treat deterministic vectors, unit tests, strict compilation,
  and sanitizers as evidence of specified implementation behavior only.
- **Consequence:** None of them proves collision resistance, preimage
  resistance, or the security of a protocol that calls the wrapper.

### D-HASH-006 — Existing protocol code is not silently strengthened

- **Status:** Accepted deferral.
- **Decision:** Do not refactor the RPS commitment or poker demo to use the new
  wrapper during this repair.
- **Rationale:** Sharing a primitive call does not automatically align domains,
  threat models, or security arguments. Each migration requires its own review.

## Trust Establishment Model Repair

### D-TRUST-001 — Taxonomy before protocol

- **Status:** Accepted for this foundation slice.
- **Decision:** Repair Trust Establishment as a model and boundary chapter, not
  as a new key-establishment protocol.
- **Rationale:** The repository has not selected a target identity setting or
  setup ceremony. Implementing exchange first would hide the very trust
  assumptions the chapter must expose.
- **Consequence:** Add no C code, primitive-registry entry, test framework, or
  runtime dependency. The chapter makes no novelty claim.

### D-TRUST-002 — Preserve entropy autonomy as open research

- **Status:** Accepted.
- **Decision:** Preserve the author's `Lambda`, covariance-eigenvalue,
  quantization, source-isolation, and participant-autonomy ideas in a visible
  Open Research Note, while removing their protocol/algorithm status.
- **Rationale:** The ideas are generative, but the current sketch supplies no
  stochastic/adversarial source model, conservative min-entropy bound,
  quantizer analysis, conditioner preconditions, health tests, or failure
  behavior.
- **Consequence:** No later chapter may consume the sketch as key material
  until those open requirements are met.

### D-TRUST-003 — Local entropy and remote identity are separate resources

- **Status:** Accepted.
- **Decision:** Treat a local entropy pipeline as capable of producing only
  local unpredictable material under stated source and endpoint assumptions.
  Peer authentication additionally requires a setup rule binding a credential
  to an intended identity or role.
- **Rationale:** A man-in-the-middle can substitute exchange material even when
  both endpoints use excellent randomness.
- **Consequence:** Challenges and key confirmation prove possession/session
  facts only relative to a pre-existing credential binding.

### D-TRUST-004 — Mutual information is not a secrecy shortcut

- **Status:** Accepted.
- **Decision:** Use only the exact `X,Y,Z,T` source model and Maurer upper bound
  cited in the chapter. Withdraw the draft's universal `I(X;Y)>0` and
  `length <= I` slogans.
- **Rationale:** Eve's side information, public reconciliation leakage,
  reliability, authentication, and the joint distribution determine the
  conclusion. Let `H(X)>0` and `X=Y=Z`; then `I(X;Y)=H(X)>0`, while Eve knows
  the same value. This counterexample defeats raw Alice--Bob correlation as a
  sufficient condition.
- **Consequence:** The market-source idea remains an empirical open problem.

### D-TRUST-005 — Public agreement is not shared secrecy

- **Status:** Accepted.
- **Decision:** Separate public beacons and deterministic public optimization
  from secret agreement. Retain the Dijkstra example only under identical
  public state, algorithm/version, and deterministic tie-breaking.
- **Rationale:** A capable observer obtains the same public value or result.

### D-TRUST-006 — Minimal sources and bibliography ownership

- **Status:** Accepted.
- **Decision:** Use four claim-scoped sources only: NIST SP 800-90B, RFC 9846,
  Maurer 1993, and official NIST beacon documentation. The chapter owns
  citations; standalone and main-book wrappers own bibliography emission.
- **Rationale:** These sources support every active external claim without
  importing a historical survey or software package. DOIs identify
  publications and remain unrelated to runtime dependencies.

### D-TRUST-007 — Main wrapper emits a final PDF

- **Status:** Accepted.
- **Decision:** Remove the LaTeX `draft` document-class option from the main
  wrapper after the included manuscript builds successfully, retain live PDF
  links, and hide only their visible border boxes.
- **Rationale:** Draft mode suppresses normal final-document behavior and was
  no longer serving a diagnosed build purpose. This changes no protocol claim,
  include path, dependency, or dormant chapter.

## Tic-Tac-Toe Public Checker Slice

### D-TTT-001 — Public predicate before private proof

- **Status:** Accepted and verified for this bounded public-checker slice.
- **Decision:** First define and implement an exhaustive checker over a fully
  disclosed deterministic policy. Call it the public disclosure baseline, not
  a solution to the aspirational chapter title.
- **Rationale:** Legality and non-loss need one exact finite meaning before a
  commitment, zero-knowledge, or private-computation layer can claim to hide
  the same witness.
- **Consequence:** The current executable may read the entire policy and makes
  no confidentiality or zero-knowledge claim.

### D-TTT-002 — Fixed game and canonical policy version 1

- **Status:** Accepted specification.
- **Decision:** Fix ordinary 3-by-3 play from the empty board, X first,
  alternating legal turns, and stop-on-win semantics. Encode boards in ternary
  row-major order and use a role-specific 19,683-byte policy with one legal
  move at every required claimant-turn state and `0xff` everywhere else.
- **Rationale:** A complete canonical table prevents unchecked bytes and makes
  exhaustive enumeration, negative tests, and a later committed witness
  unambiguous.
- **Consequence:** “Non-losing” means no opponent win under every legal
  continuation from this initial state. It is not silently renamed “perfect,”
  locally optimal, or uniquely minimax.

### D-TTT-003 — Plain C11 and no dependency expansion

- **Status:** Accepted; reflects the author's preference for a minimal and
  inspectable implementation.
- **Decision:** Implement the model, reference generator, checker, Core demo,
  and Core tests using only C11 fixed-size caller-owned storage. Preserve that
  dependency-free target. The separately named `ttt_bind` target reuses the
  repository's existing libsodium commitment backend and adds no library.
- **Rationale:** The public finite predicate requires no randomness, hashing,
  commitment, network, clock, file I/O, or external proof backend.
- **Consequence:** The original `ttt`/Core link remains free of `$(LDLIBS)`;
  `ttt_bind` links only the already configured libsodium backend. The generic
  commitment implementation remains byte-identical.

### D-TTT-004 — Five claims remain separate

- **Status:** Accepted research boundary.
- **Decision:** Treat knowledge of a commitment opening, policy legality,
  exhaustive non-loss, private per-state evaluation, and anti-cloning as five
  different claims. The public checker implements only disclosed legality and
  non-loss.
- **Rationale:** A commitment-shaped value cannot be scanned for policy
  quality, zero knowledge does not automatically provide extraction, and a
  revealed oracle answer remains revealed.
- **Consequence:** The external profile reuses the educational hash commitment
  for byte binding, but it is never promoted into a proof system or treated as
  proof-internal commitment machinery.

### D-TTT-005 — Unrestricted exact-oracle anti-cloning is impossible here

- **Status:** Accepted proposition scope.
- **Decision:** Record that an unrestricted exact deterministic oracle on a
  known finite domain can be functionally copied by querying every input and
  recording the outputs.
- **Rationale:** This is an information-revelation fact independent of
  cryptographic hardness.
- **Consequence:** Later anti-cloning work must restrict queries, responses, or
  the protected object and state the residual leakage; it cannot promise to
  conceal outputs the interface deliberately releases.

### D-TTT-006 — Preserve seed, challenge, Monte Carlo, and matrix ideas as open

- **Status:** Accepted preservation decision.
- **Decision:** Retain the session-seed, adversarial-initial-condition,
  all-scenarios, cloning-warning, and matrix-inversion phrases visibly as
  author's intuition or open questions. Do not put them into the current
  predicate without definitions.
- **Rationale:** The historical fragment is generative, but it defines no
  seed-to-policy rule, challenge-state contract, coverage proof, or matrix.
- **Consequence:** The bounded slice uses exhaustive traversal from the empty
  board; Monte Carlo and matrix inversion establish nothing in the current
  result.

### D-TTT-007 — Minimal sources and no novelty claim

- **Status:** Accepted.
- **Decision:** Reuse Naor only for commitment background and add GMR,
  Bellare--Goldreich, and GMW only for matched zero-knowledge,
  proof-of-knowledge, and general-existence distinctions. Classify the slice as
  a straightforward public finite-game model and verifier with no novelty
  claim.
- **Rationale:** These sources define later proof categories; none supplies a
  TTT proof backend or establishes this C implementation.
- **Consequence:** Sources remain bibliography only and add no dependency.
  Any stronger contribution classification requires a separate review.

### D-TTT-008 — Standalone target before main-book promotion

- **Status:** Superseded for current inclusion by D-TTT-022; retained as the
  correct boundary of the earlier public-checker slice.
- **Decision:** Add `section_ttt` with wrapper-owned bibliography emission, but
  leave `document/adversarial_cooperation.tex`, its include graph, the chapter
  backup, the tracked demonstration placeholder, and the primitive registry
  unchanged.
- **Rationale:** A standalone build enables exact verification without
  presenting an in-progress public baseline as part of the mature main-book
  spine.

### D-TTT-009 — Verification evidence remains subordinate to the claim

- **Status:** Accepted boundary; concrete results recorded in the execution
  plan.
- **Decision:** Treat state-count reproduction, reference policies, losing and
  malformed-policy tests, strict compilation, sanitizers, regression builds,
  and PDF inspection only as evidence for the enumerated implementation and
  manuscript behavior actually checked.
- **Rationale:** Execution cannot establish privacy, zero knowledge, extraction,
  commitment security, authorship, originality, or future obedience.
- **Consequence:** The final strict, focused sanitizer, regression, PDF, visual,
  dependency, and independent-review evidence is recorded without promoting
  execution into a cryptographic or formal proof.

### D-TTT-010 — Select a global knowledge argument, not a move oracle

- **Status:** Accepted research direction; not implemented.
- **Decision:** Target one zero-knowledge argument of knowledge that a single
  committed canonical policy satisfies the complete non-loss relation from the
  standard empty board. Computational knowledge soundness makes this an
  argument rather than an information-theoretic proof. Treat private evaluation
  of individual game states as a separate MPC service with a separate leakage
  analysis.
- **Rationale:** A move oracle proves a different statement, reveals one output
  per query, and revives the chapter's own functional-cloning concern. The
  global relation matches the present chapter title and public baseline more
  directly.
- **Consequence:** This slice adds no interactive move interface and makes no
  private-response or anti-cloning claim.

### D-TTT-011 — Build a fixed DAG evaluator before a proof circuit

- **Status:** Accepted sequence; fixed-DAG evaluator, equivalence argument,
  role-specialized Core circuits, external commitment profile, and symbolic
  direct-Bind measurement complete. An emitted Bind and proof protocol remain
  open.
- **Decision:** Make the next executable artifact a dependency-free bottom-up
  evaluator over the fixed reachable-state DAG. Establish equivalence with the
  recursive checker by induction and a broad specified differential corpus before
  expressing the relation as a backend-neutral Boolean circuit.
- **Rationale:** The current DFS is a good public diagnostic checker but its
  recursion, secret-dependent early exits, table lookup, and counterexample
  reporting should not be mechanically mistaken for a proof circuit.
- **Consequence:** No proof library is selected until a proof representation
  and its external and proof-internal commitment layers are compared together
  under a complete cost and security model. Neither Core nor composed gate
  counts estimate a proof.

### D-TTT-012 — Retain MPC-in-the-head only as a conditional candidate

- **Status:** Accepted comparison result; no dependency or implementation.
- **Decision:** Keep MPC-in-the-head as the leading educational family after
  circuit measurement. Do not claim that base ZKBoo meets the target: its
  stated special honest-verifier zero knowledge and 3-special soundness require
  additional, exact analysis to obtain malicious-verifier zero knowledge and
  the chosen argument-of-knowledge statement. Defer transparent R1CS systems such
  as Spartan as a long-term comparison.
- **Rationale:** This preserves a small, inspectable research route without
  confusing a cited protocol family with a completed construction.
- **Consequence:** No Fiat-Shamir transform, random-oracle claim, pairing stack,
  R1CS framework, proof dependency, or production-security claim is added. The
  proof-internal commitments to simulated views remain distinct from the
  selected external policy profile. The proof-internal primitive, domain,
  encoding, assumptions, and cost remain unselected.

### D-TTT-013 — Use the board encoding as the public topological schedule

- **Status:** Accepted and implemented for the fixed-DAG slice.
- **Decision:** Exploit
  `child_index = parent_index + turn * 3^cell` for an empty cell. Build graph
  reachability by an ascending scan, validate every policy byte by a second
  ascending scan, and evaluate values by a descending scan. Keep the recursive
  checker and its replayable first-loss diagnostics as a separate baseline.
- **Rationale:** Every child index is strictly greater than its parent, so the
  evaluator needs no recursion, queue, edge table, ply table, heap, or external
  dependency. A separate DAG report avoids assigning DFS history counts or a
  counterexample trace to a relation that computes neither.
- **Consequence:** The explicit array and board payload is 39,375 bytes. The
  relation enumerates public legal successors and never selects a child address
  from the policy byte. The implementation is not described as constant-time,
  side-channel resistant, a Boolean circuit, or a private proof.

### D-TTT-014 — Bound differential evidence precisely

- **Status:** Accepted implementation-evidence boundary.
- **Decision:** Compare both evaluators on every legal one-entry alternative of
  the two deterministic reference policies (11,647 policies), 512
  deterministically generated multi-entry policies, known losses, every byte
  value at a required entry, and named malformed cases. Prove recurrence
  equivalence separately by induction on remaining empty cells.
- **Rationale:** A broad reproducible corpus is useful evidence but the full
  canonical policy spaces are enormous. Calling the corpus exhaustive without
  its Hamming-distance restriction would be false.
- **Consequence:** Test agreement is recorded as C implementation evidence,
  not formal verification, a probability statement, circuit equivalence, or a
  cryptographic property.

### D-TTT-015 — Specialize the first Core circuits by public role

- **Status:** Accepted and implemented for the Boolean-Core slice.
- **Decision:** Emit separate deterministic `Core-X` and `Core-O` circuits.
  Each circuit still receives a public role byte and rejects a mismatch.
- **Rationale:** Role specialization gives smaller, simpler recurrences and
  independently reproducible counts without guessing that a later backend
  requires one universal circuit identity.
- **Consequence:** A universal-role circuit, if later required, is a new
  versioned construction with its own counts and equivalence evidence.

### D-TTT-016 — Use a dependency-free caller-owned XOR/AND IR

- **Status:** Accepted and implemented.
- **Decision:** Reserve wires zero and one as Boolean constants; place public
  bits, witness-typed bits, and append-only gate outputs in fixed order; permit
  only two-input XOR and AND; and serialize a 32-byte header plus byte-defined
  nine-byte big-endian gate records. Allocate no library memory and expose no
  public decoder before a consumer needs one.
- **Rationale:** The smallest inspectable representation is enough to freeze
  `Core` without importing a proof framework or allowing C-struct padding and
  host endianness to define an artifact.
- **Consequence:** Callers provide disjoint, checked storage. The 12-byte C gate
  layout is measured memory, never a portable encoding.

### D-TTT-017 — Wire typing and serialization provide no secrecy

- **Status:** Accepted scientific boundary.
- **Decision:** Call the policy range witness input only to distinguish it from
  public input. State everywhere that evaluation receives those bytes in
  plaintext and that circuit serialization binds no policy opening.
- **Rationale:** A circuit is a computation description, not a commitment,
  zero-knowledge protocol, or proof of knowledge.
- **Consequence:** A separate external profile and disclosed-opening wrapper
  may bind bytes, but the Core serialization itself still provides no
  commitment. Proof-internal commitments, soundness, extraction, zero
  knowledge, and privacy stay open.

### D-TTT-018 — Preserve independent baselines and bound circuit evidence

- **Status:** Accepted implementation-evidence boundary.
- **Decision:** Keep `ttt.h`, `ttt.c`, and `test_ttt.c` byte-identical. Compare
  only the circuit acceptance bit with both existing evaluators over the named
  deterministic corpus. Keep the production evaluator scalar; any 64-lane
  serialized interpreter is test machinery only.
- **Rationale:** Three differently represented paths expose more translation
  errors than replacing the old implementation, while batching makes broad
  comparison practical without changing the API.
- **Consequence:** Agreement remains bounded implementation evidence, not
  exhaustive policy-space coverage or formal C/circuit equivalence.

### D-TTT-019 — Reuse one minimal external commitment under a fixed profile

- **Status:** Accepted and implemented as an educational disclosed-opening
  baseline.
- **Decision:** Reuse AC Commitment Version 1 under protocol identifier
  `AC-TTT-POLICY-V1`. Bind the four Core bytes and all 19,683 policy bytes to a
  nonzero public session and public statement round. Use fixed protocol-role
  labels `PROVER` and `VERIFIER`; keep the claimant's X/O role in the payload.
- **Rationale:** This fixes the exact statement with no new library or custom
  production cryptography, while preventing game-role and sender-role concepts
  from being conflated.
- **Consequence:** The profile inherits only the generic commitment's limited
  collision/alternate-opening and random-oracle-style arguments. It provides
  no knowledge, authentication, replay database, privacy during verification,
  or production guarantee.

### D-TTT-020 — Measure direct Bind symbolically before emitting it

- **Status:** Accepted and implemented as a cost model, not a circuit.
- **Decision:** Walk one frozen BLAKE2b XOR/AND operation shape, using the
  explicit bit-zero half-adder and discarding the final carry, count depth and
  gates exactly, combine its counts arithmetically with each Core, and do not
  allocate or serialize its 5,625,151 AND plus
  26,378,049 XOR gates: 32,003,200 total gates, 32,161,498 wires, and depth
  1,558,999.
- **Rationale:** The direct result is already sufficient to show that the
  external hash dominates this Boolean route. Emitting hundreds of megabytes
  before selecting a proof transformation would add machinery without adding
  privacy or a proof.
- **Consequence:** Every reported size is labeled modeled or “if emitted.”
  No circuit-equivalence, proof-size, transcript, runtime, lower-bound, or
  optimized-backend claim follows. No proof-internal view commitment is
  selected; its 35,040-byte illustrative value is only a conditional
  digest-and-nonce floor recorded in the plan and manuscript, not a C API.

### D-TTT-021 — Preserve Version 1 and co-design the proof binding

- **Status:** Accepted paper-first route decision; no code or dependency.
- **Decision:** Keep `AC-TTT-POLICY-V1`, its implementation, vector, and direct
  symbolic counts as the disclosed-opening and comparison baseline. Do not
  emit its 32,003,200-gate Bind circuit as the next artifact. Select neither an
  unspecified replacement nor a proof representation outside an end-to-end
  comparison of candidate pairs. Any replacement must receive a new profile
  and relation version.
- **Rationale:** Emitting the frozen direct netlist would add hundreds of
  megabytes of representation evidence but no privacy or proof. Conversely,
  “proof-friendly” is backend-relative: selecting a commitment independently
  of its field, constraint language, setup, transcript, and adversary model
  would add assumptions and machinery without a comparable total cost.
- **Consequence:** The next gate is a source-backed, end-to-end comparison that
  enforces one shared policy witness across Core and the opening relation. It
  must distinguish an externally published and recorded policy commitment,
  under an explicit lifecycle, from proof-internal commitments; define
  migration and exact bytes for any new profile; and include total relation,
  transcript, prover, and verifier costs.
  No alternative is claimed cheaper, safer, selected, or implemented; no new
  library is authorized.

### D-TTT-022 — Adopt Tic-Tac-Toe as the canonical Core-Bind reference

- **Status:** Accepted after standalone and main-book build verification.
- **Decision:** Promote the chapter into the main include graph and make its
  concept ladder, statement modes, shared-witness proposition, and publication
  lifecycle the book's reusable reference contract. Later chapters may cite
  these distinctions only after replacing their problem-specific model,
  witness, relation, leakage, setup, adversary, and lifecycle.
- **Rationale:** Tic-Tac-Toe is finite enough for exhaustive public checking
  while still exposing the exact conceptual mistakes that recur in larger
  applications: confusing a commitment with a semantic predicate, proving two
  facts about different witnesses, mistaking witness-typed wires for secrecy,
  and treating context bytes as an authenticated chronological record.
- **Consequence:** Stable labels and a parameterized future security target are
  part of the chapter. Promotion creates no proof, privacy, commitment-security,
  transfer, or novelty claim. The include is retained only if both PDFs build
  and pass visual inspection.

### D-TTT-023 — Publish one deterministic positive cross-layer fixture

- **Status:** Accepted as test and teaching material only.
- **Decision:** Keep fresh randomness as the Bind demo default and add an
  explicit `--reference-vector` mode using fixed public constants and the valid
  X reference policy. Require those exact policy bytes to pass the recursive
  evaluator, fixed DAG, actual Core-X Boolean evaluation, and Bind opening in a
  single test group. Preserve the older invalid-policy vector because it
  demonstrates that Bind does not imply Core.
- **Rationale:** A canonical example needs one replayable positive story across
  all existing layers. A fixed fixture improves conformance evidence without
  pretending that public test randomness supplies hiding.
- **Consequence:** No public API, primitive, proof backend, or cryptographic
  library is added. The fixture establishes deterministic implementation
  agreement only; it does not prove commitment security, circuit equivalence,
  or policy privacy.

### D-WEB-001 --- Keep the browser edition derived and source-addressable

- **Status:** Superseded for the public reader by D-WEB-003; retained as the
  record of the first static-browser attempt.
- **Decision:** Keep LaTeX and C as the only authored book and protocol
  sources. Generate the browser payload from the main include graph, attach
  exact source ranges and hashes to rendered blocks, and compile the existing
  disclosed-policy Tic-Tac-Toe checker to WebAssembly through a thin ABI.
  Browser editing operates on the corresponding raw LaTeX block after an
  explicit local-directory grant; it never converts arbitrary rendered HTML
  back into LaTeX.
- **Rationale:** The browser should make the book inspectable and executable
  without becoming a competing manuscript or a JavaScript reimplementation of
  the research code.
- **Consequence:** Generated HTML, JSON, JavaScript glue, and WebAssembly are
  disposable artifacts. Static hosting can enforce revision and source-range
  checks but cannot establish that an edited manuscript compiles; the ordinary
  LaTeX build remains authoritative. The browser checker remains fully
  disclosed and educational. It supplies no zero knowledge, policy privacy,
  authorship, cryptographic proof, or stronger security guarantee.

### D-WEB-002 --- Keep browser feedback read-only and standardize chapter views

- **Status:** Superseded by D-WEB-003; retained as the record of the second
  browser-review attempt.
- **Decision:** Give every chapter Standard, Demo, and Document views. Register
  demonstrations by exact canonical source path and show an explicit
  unavailable state when none is ready. Remove browser directory grants and
  manuscript writes. A text selection may instead produce a copyable change
  indication containing complete enclosing source-block provenance, exact raw
  LaTeX, and a reader-authored note.
- **Rationale:** The browser is a useful place to point at wording and explore
  executable companions, but a hosted reading surface should not silently
  acquire authority over the research record. A rendered selection also
  cannot be reverse-mapped character-for-character after LaTeX commands and
  whitespace have been transformed.
- **Consequence:** Change indications are editorial requests, not accepted
  edits. They write no file, upload no content, run no compiler, and claim only
  complete-block provenance. The compiled PDF remains the typographic
  authority. The dependency-free Standard renderer handles common constructs
  and must visibly label unsupported ones; exact LaTeX remains available in
  Document view. The Tic-Tac-Toe browser lab remains a fully disclosed
  educational checker with no additional cryptographic claim.

### D-WEB-003 --- Make the compiled PDF the reader and standardize demos

- **Status:** Accepted replacement for the public browser interface.
- **Decision:** Expose only Read, Demos, and About. Read embeds the PDF newly
  compiled from the canonical LaTeX manuscript. Demos lists only registered,
  working WebAssembly companions. Remove the custom LaTeX renderer,
  serialized manuscript, raw-document view, source map, change indications,
  comments, and every browser manuscript-write path. Standardize a validated,
  immutable demo definition with named examples and a runner that returns one
  presentation-neutral result shape.
- **Rationale:** A second partial renderer obscured the manuscript, duplicated
  source at runtime, and made an ordinary reading surface look like an editor.
  The PDF already gives complete LaTeX rendering. A small demo contract lets
  later chapters reuse navigation and presentation without copying protocol
  code or inventing a new page structure.
- **Consequence:** LaTeX remains the sole manuscript source, C remains the
  computational source of demonstrations, and JavaScript is adapter and
  presentation code only. The first registry entry is the disclosed-policy
  Tic-Tac-Toe checker with three fixed examples. A successful run is an
  implementation observation under that fixed model; it adds no privacy,
  zero-knowledge, commitment-security, authorship, or novelty claim.

## Dedicated Container Toolchain

### D-CONTAINER-001 --- Make provisioning visible and project-owned

- **Status:** Accepted operational boundary; not a security theorem.
- **Decision:** Use the Docker Official Debian stable-slim image pinned by
  digest in `compose.yaml`, and provision each fresh disposable container by
  running the checked-in `setup.sh`. Do not add a Dockerfile or reuse an
  ambient container belonging to this or another project.
- **Rationale:** The package request, APT policy, privilege transition, build
  commands, and version record remain visible in ordinary repository files.
  A hidden prebuilt layer is not required to understand what the environment
  requests from Debian.
- **Consequence:** Provisioning is slower and is not bit-for-bit frozen. The
  base image is fixed by digest, but Debian stable packages can advance through
  signed security and update repositories. Every run records the platform,
  architecture, packages, and tools actually used. The record supplies
  provenance; it does not prove that those packages are harmless or
  vulnerability-free, nor that different architectures produce identical
  bytes.

### D-CONTAINER-002 --- Minimize the package and host-access surfaces

- **Status:** Accepted implementation boundary.
- **Decision:** Install explicit, mode-specific top-level Debian package
  profiles with recommended and suggested packages disabled, require
  authenticated APT repository metadata, and reject insecure or
  unauthenticated APT modes. Mount only the enumerated
  repository inputs read-only, omit `.git`, Docker sockets, explicitly mapped
  host devices, deliberately passed secrets, and unrelated host paths, and
  make `.container-output/` the only writable project bind mount.
- **Rationale:** Fewer packages and narrower mounts reduce accidental authority
  and the amount of third-party code in the build environment.
- **Consequence:** `--no-install-recommends` and related controls reduce the
  dependency surface; they do not make a selected package intrinsically safe.
  APT authenticates repository metadata and package hashes but does not audit
  package behavior for this project.

### D-CONTAINER-003 --- Separate privileged setup from unprivileged builds

- **Status:** Accepted implementation boundary.
- **Decision:** Restrict root authority to provisioning the disposable
  container with APT. Run compilation, tests, manuscript assembly, WebAssembly
  generation, and preview serving as a numeric non-root user under
  no-new-privileges with an empty Linux capability set. Ordinary network access
  remains throughout this one-container run. Publish the preview only on host
  loopback and deliberately pass no host credentials into the container.
- **Rationale:** Package installation requires modifying the container root
  filesystem, while ordinary project builds do not require root authority.
- **Consequence:** The later build phase still has ordinary outbound network
  reachability; the one-container, no-Dockerfile design cannot remove its
  network after provisioning. Docker also shares the host kernel and trusts
  the Docker daemon. This environment is not a sandbox for hostile source,
  kernel exploits, daemon compromise, or a malicious dependency.

### D-CONTAINER-004 --- Use the same path for local verification and Pages

- **Status:** Accepted deployment boundary.
- **Decision:** Local hosts and GitHub Actions invoke the same Compose service.
  The Pages workflow runs `docker compose run --rm toolchain web` and uploads
  only `.container-output/pages-<run-id>-<attempt>/web-dist` as a static
  artifact.
- **Rationale:** A single platform-neutral command avoids a special Windows
  hosting path and reduces differences between local and continuous builds.
- **Consequence:** GitHub Pages receives static HTML, CSS, JavaScript,
  WebAssembly, and the compiled manuscript PDF. It receives no serialized
  LaTeX manuscript, running container, build-time privilege, Docker socket,
  server-side editor, or host filesystem authority. The published reader
  requests no local-checkout directory grant and exposes no client-side
  manuscript write path.

### D-CONTAINER-005 --- Keep preview ephemeral and artifact publication write-once

- **Status:** Accepted lifecycle boundary.
- **Decision:** Build and serve preview entirely in disposable container
  storage. Do not publish a preview run directory. Continue to publish `web`
  and `verify` results only to explicit, write-once artifact destinations that
  are assembled under a hidden sibling and atomically renamed into place. An
  existing run ID is rejected.
- **Rationale:** Preview is transient inspection, whereas a published artifact
  is evidence to preserve. A fixed preview artifact name prevented ordinary
  restart after the first run and conflated those lifecycles.
- **Consequence:** `docker compose up preview` may be stopped and started
  without deleting or renaming host evidence. The preview remains reachable
  only through host loopback port 4173. This separation changes no package,
  source-mount, non-root, capability, network, or scientific claim boundary.
