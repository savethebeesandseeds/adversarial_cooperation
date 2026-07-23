# Research Charter for Adversarial Cooperation

## 1. Purpose

Adversarial Cooperation is an applied cryptography book, a collection of
protocol constructions, and an investigation into the conditions under which
parties with incomplete trust can still act together.

Its central question is:

> Given parties whose interests overlap but are not identical, and who may
> possess private information or strategic advantages, can a protocol realize
> a jointly valuable outcome while minimizing required trust, preserving
> legitimate privacy, and making harmful deviation detectable or containable?

The project studies cryptography not merely as secrecy machinery, but as a
language for promises that can be verified without surrendering every secret.

The book should remain hospitable to philosophy, game theory, political
economy, distributed systems, mechanism design, formal methods, and ethics.
Cryptography is the principal instrument, not the only source of truth.

## 2. Working Definition

Use the following as a provisional definition, not as a final theorem:

An adversarial-cooperation protocol is a protocol among parties with partially
aligned interests that:

1. realizes an explicitly defined joint functionality;
2. minimizes the assumptions parties must make about one another;
3. preserves specified private information or strategic autonomy;
4. detects, proves, limits, or prices specified deviations;
5. improves upon a stated non-cryptographic baseline;
6. and defines what happens under disagreement, abort, corruption, and exit.

A party is not called an adversary because it is evil. A party is adversarial
because the protocol must remain meaningful when that party protects its own
interests, tests the boundaries, or deviates within the stated model.

Reserve the word `enemy` for an actor whose utility includes defeating the
joint purpose itself. This distinction is philosophically important, but it
must eventually be represented by precise behavior or utility assumptions.

## 3. Foundational Discipline

The project must protect two things simultaneously:

1. The originality and moral force of the author's intuitions.
2. The falsifiability and precision required of serious research.

Neither may consume the other.

A beautiful sentence is not evidence.
A running program is not a proof.
A proof under unnamed assumptions is not a security theorem.
A security theorem is not a guarantee of justice.
A protocol that prevents one abuse may create another center of power.

Humility is part of the security model.

## 4. Claim Taxonomy

Every substantial statement should be recognizable as one of the following:

### Definition

A precise meaning assigned to a term.

### Construction

A protocol, algorithm, game, state machine, or composition of primitives.

### Theorem or Proposition

A formal statement with explicit assumptions and a proof.

### Lemma

A supporting formal result.

### Proof Sketch

A structured but incomplete argument. It must identify what remains unproved.

### Security Argument

An informal or semi-formal explanation tied to an explicit threat model.

### Conjecture

A statement believed to be true but not proved.

### Open Problem

A question for which the project does not yet have a satisfactory construction
or impossibility result.

### Design Principle

Guidance derived from examples, experience, or ethical preference.

### Author's Intuition

A generative thought, metaphor, or philosophical observation.

### Historical or Technical Fact

A statement requiring a reliable citation.

### Implementation Observation

A result established only for a particular program, configuration, or test.

Codex must never silently promote a statement from a weaker category into a
stronger one.

Maintain `research/CLAIM_LEDGER.md` with columns similar to:

| ID | Chapter | Exact claim | Claim type | Assumptions | Evidence | Counterexample status | Citation status | Confidence |
|----|---------|-------------|------------|-------------|----------|-----------------------|-----------------|------------|

## 5. Formal Model

For each protocol, define a model sufficient to answer the following questions.

### Parties

- Who participates?
- Are roles symmetric?
- Can parties join or leave?
- Can identities be duplicated?
- Is there a trusted dealer, judge, hardware device, beacon, or authority?

### Information

- What is public?
- What is private?
- What must remain hidden forever?
- What may be selectively disclosed later?
- What leakage is unavoidable?
- Does the transcript itself reveal behavior or relationships?

### Preferences

Where useful, define:

- each party's private type or information;
- permitted actions;
- utility or ordering over outcomes;
- the shared objective;
- and the point at which interests diverge.

Do not force a numerical utility function where an ordinal or logical model is
more honest.

### Adversary

Specify:

- honest-but-curious, covert, rational, malicious, or Byzantine behavior;
- static or adaptive corruption;
- collusion threshold;
- computational power;
- quantum or classical model, when relevant;
- access to side channels;
- ability to delay, reorder, replay, suppress, or fabricate messages;
- ability to abort;
- ability to coerce a participant;
- and ability to compromise long-term keys.

### Communication

Specify:

- authenticated or unauthenticated channels;
- confidential or public channels;
- broadcast assumptions;
- synchrony or asynchrony;
- message delivery guarantees;
- and transcript consistency.

### Setup

Specify every trusted or common resource:

- public parameters;
- common reference strings;
- PKI;
- pre-shared secrets;
- physical meetings;
- human-verifiable fingerprints;
- trusted hardware;
- randomness beacons;
- clock assumptions;
- and secure storage.

### Required Properties

Choose and define only those that apply:

- correctness;
- privacy;
- soundness;
- completeness;
- zero knowledge;
- binding;
- hiding;
- authenticity;
- integrity;
- input independence;
- fairness;
- guaranteed output delivery;
- robustness;
- accountability;
- non-repudiation;
- deniability;
- liveness;
- availability;
- recoverability;
- forward secrecy;
- post-compromise security;
- coercion resistance;
- and composability.

Do not bundle all desirable properties into the vague word `secure`.

## 6. The Counterfactual Requirement

Every applied-cryptography chapter must explain what cryptography changes.

Compare the proposed construction against at least one baseline:

- trusted intermediary;
- full public disclosure;
- contractual promise;
- conventional distributed algorithm;
- ordinary game-theoretic mechanism;
- repeated interaction and reputation;
- physical ceremony;
- or no cooperation.

Then answer:

1. What becomes possible?
2. What becomes safer?
3. What becomes more private?
4. What new cost or failure mode is introduced?
5. Is cryptography essential, or merely decorative?

When cryptography does not materially alter the feasible set, classify the
chapter as philosophy, game theory, governance, or mechanism design instead of
forcing it into a false cryptographic mold.

## 7. Canonical Chapter Structure

Use the following structure for mature technical chapters. Early fragments may
remain shorter until their central problem is understood.

1. **Epigraph or Author's Intuition**
2. **Abstract**
3. **Cooperation Problem**
4. **Why the Classical Solution Is Insufficient**
5. **Players, Inputs, Secrets, and Outputs**
6. **Threat Model and Setup**
7. **Ideal Functionality or Formal Objective**
8. **Non-Goals**
9. **Cryptographic Primitives Used**
10. **Protocol Construction**
11. **State Machine and Transcript**
12. **Correctness**
13. **Security Claims**
14. **Proof, Proof Sketch, or Security Argument**
15. **Attacks and Counterexamples**
16. **Abort, Fairness, and Recovery**
17. **C Implementation Companion**
18. **Tests and Experiments**
19. **Ethical and Institutional Analysis**
20. **Limitations**
21. **Open Problems**
22. **Exercises**
23. **References**

Every chapter should contain a short section titled:

> What Cryptography Does Not Solve

This is mandatory.

## 8. Protocol Notation

Prefer consistent notation across chapters.

A useful starting model is:

- \(P = \{P_1,\ldots,P_n\}\): parties;
- \(\theta_i\): private information or type of party \(P_i\);
- \(s\): public state;
- \(a_i\): action of party \(P_i\);
- \(u_i\): utility or preference relation, when modeled;
- \(\mathcal{F}\): ideal functionality or desired joint computation;
- \(\Pi\): real protocol;
- \(\mathcal{A}\): adversary;
- \(\lambda\): security parameter;
- \(\tau\): public or partially public transcript;
- \(\Sigma\): setup assumptions.

Do not introduce notation that is never used.
Do not hide ambiguity behind symbols.

## 9. Primitive Discipline

Create a primitive registry in `research/PRIMITIVE_REGISTRY.md`.

For each primitive, record:

- definition;
- security notion;
- assumptions;
- approved implementation backend;
- chapters that use it;
- whether its use is educational or security-relevant;
- test vectors;
- known misuse patterns;
- and relevant appendix section.

Expected primitives may include:

- cryptographic hash functions;
- message authentication codes;
- authenticated encryption;
- digital signatures;
- commitments;
- coin flipping;
- secret sharing;
- oblivious transfer;
- zero-knowledge proofs;
- secure multi-party computation;
- verifiable shuffles;
- threshold cryptography;
- private set operations;
- verifiable computation;
- randomness extraction;
- key agreement;
- password-authenticated key exchange;
- and transcript hashing.

Do not add a primitive merely because it sounds relevant. Each primitive must
remove a specific trust assumption, protect a specific secret, or enforce a
specific invariant.

## 10. C Companion Architecture

Do not reorganize the existing tree before auditing it. After the audit, work
toward a structure conceptually similar to:

- `include/ac/` for public interfaces;
- `src/primitives/` for reusable primitive wrappers;
- `src/protocols/` for protocol state machines;
- `demos/` for chapter demonstrations;
- `tests/` for unit, integration, attack, and transcript tests;
- `test-vectors/` for deterministic vectors;
- `document/` for the book;
- `research/` for claims, assumptions, plans, and open problems.

Every protocol implementation should expose:

1. initialization;
2. message generation;
3. message parsing and validation;
4. state transition;
5. final output;
6. transcript export;
7. explicit abort and error states;
8. deterministic test mode;
9. and a human-readable demonstration.

Protocol messages should contain:

- protocol identifier;
- protocol version;
- session identifier;
- sender and intended role;
- state or round number;
- length-delimited payload;
- and authentication or commitment data where required.

A byte sequence must have one canonical meaning.

## 11. Test Philosophy

Tests must include more than the cooperative path.

For every protocol, seek tests for:

- honest execution;
- invalid input;
- truncated input;
- oversized input;
- duplicate messages;
- replay;
- reordering;
- equivocation;
- incorrect nonce;
- corrupted commitment;
- wrong session identifier;
- wrong protocol version;
- premature reveal;
- selective abort;
- timeout or missing message;
- collusion where modeled;
- and transcript disagreement.

Where feasible, implement a simple adversarial harness that mutates messages
and protocol order.

The test suite should distinguish:

- implementation correctness;
- protocol invariant checking;
- and cryptographic security claims.

Only the first two can generally be established by ordinary testing.

## 12. Manuscript Voice

The author's voice is part of the work.

Do not sterilize statements such as:

- "Never make victory perpetual."
- "Victory is not the defeat of the adversaries."
- "Hard problems require humble solutions."
- "An immortal and inflexible system betrays the natural world."

Instead, connect them to operational mechanisms.

For example, "Never make victory perpetual" may motivate:

- expiration;
- key rotation;
- protocol termination;
- negotiated exit;
- sunset clauses;
- appeal;
- rollback;
- succession;
- state handoff;
- restorative settlement;
- and a prohibition against permanent exceptional powers.

The philosophical statement should remain visible, followed by a precise
question:

> Which protocol state, invariant, or governance mechanism gives this principle
> operational meaning?

## 13. Ethical Review

Every consequential protocol must examine:

- who may be excluded;
- who controls setup parameters;
- who can force an abort;
- who bears the cost of privacy;
- who can appeal;
- who can audit;
- who may be coerced;
- whether participation is voluntary;
- whether weaker parties can exit;
- whether the protocol freezes an unjust initial state;
- and whether secrecy protects dignity or hides domination.

Privacy and accountability must be considered together.

A zero-knowledge proof can demonstrate that a hidden statement satisfies a
predicate. It cannot decide whether the predicate is just.

No protocol receives moral credit merely because it is elegant.

## 14. Project Registers

Create and maintain:

### `research/THESIS.md`

The evolving central thesis and explicit non-claims of the book.

### `research/GLOSSARY.md`

Definitions of adversary, enemy, cooperation, victory, fairness, privacy,
secrecy, trust, proof, strategy, law, and related terms.

### `research/CHAPTER_MATRIX.md`

Suggested columns:

| Chapter | Problem | Classical baseline | Primitive | Threat model | Formal status | Code status | Novelty status | Priority |

### `research/CLAIM_LEDGER.md`

Every major scientific and historical claim.

### `research/ASSUMPTION_LEDGER.md`

Every trust, hardware, network, randomness, identity, and adversarial
assumption.

### `research/OPEN_PROBLEMS.md`

Questions not yet solved, including failed approaches and partial results.

### `research/DECISIONS.md`

Important editorial, architectural, and cryptographic decisions with rationale.

### `research/RENAME_MAP.md`

Current filename, proposed ASCII filename, display title, affected includes,
and migration status.

### `research/BIBLIOGRAPHY_QUEUE.md`

Claims needing primary literature, standards, prior constructions, or
historical verification.

## 15. Provisional Book Architecture

Do not enforce this structure before auditing the repository. Treat it as a
working map.

### Part I: Foundations

1. Introduction
2. What Is Adversarial Cooperation?
3. Adversaries, Enemies, and Partially Aligned Interests
4. Defining Victory
5. Threat Models and Protocol Anatomy
6. Cryptographic Primitives
7. Trust Establishment
8. Byzantine Agreement and Its Limits

### Part II: Finite Games and Verifiable Commitment

9. Two Oracles Play Rock-Paper-Scissors
10. Tic-Tac-Toe Without Revealing the Strategy
11. Poker Without Revealing the Cards

### Part III: Private Joint Action

12. Telling an Adversary It Is Wrong Without Learning Its Secret
13. Interactive Secure Stag Hunt
14. Joint Secure Obstacle Avoidance
15. Secure Multi-Party Optimization
16. Law Without Referencing the Law
17. Cryptographic Meta-Mathematics

### Part IV: Institutions, Dynamics, and Repair

18. Conflict Arbitrage
19. Aggregation Disadvantage
20. Momentum-Zero Traps
21. Friendly Fire
22. Fighting Dissipates Strength
23. The Helmsman's Game
24. Addressing Goodhart's Law
25. Adversarial Aftermath

### Part V: Appendices

- Hash functions
- Commitments
- Authentication and signatures
- Non-repudiation and its limits
- Zero-knowledge proofs
- Secure multi-party computation
- Verifiable shuffles
- Randomness
- Cryptographic erasure
- Standards
- C implementation guidance
- AST, VHDL, and high-level synthesis

## 16. Current Chapter Triage

### Strongest Existing Technical Anchor

`poker_without_revealing_the_cards`

The chapter already has a meaningful decomposition and explicit limitations.
Preserve it as an instructional construction, but do not imply malicious
security, collusion resistance, fairness, or production readiness without
additional mechanisms and proofs.

The toy commutative-encryption construction must remain visibly labeled as a
teaching model.

### Foundational Chapters Requiring Early Correction

#### `introduction.tex`

Preserve its moral voice, but remove or qualify blanket claims about
information-theoretic security, quantum resistance, adversary intelligence,
and the inevitability of kindness.

The introduction should promise a method of inquiry, not announce that every
war has already been solved.

#### `trust_establishment.tex`

Local randomness alone does not establish the identity of a remote party.

The chapter must distinguish:

- entropy generation;
- key derivation;
- unauthenticated key exchange;
- authenticated key exchange;
- identity binding;
- trust-on-first-use;
- pre-shared verification;
- certificates;
- password-authenticated exchange;
- human-verifiable short strings;
- physical channels;
- and correlated randomness.

Statements about independent randomness and common secrets must specify the
communication and computational model.

#### `hash_functions.tex`

Correct the definitions and examples.

Do not describe a hash function as encryption, anonymity, or a general means of
expressing a secret safely.

Separate:

- preimage resistance;
- second-preimage resistance;
- collision resistance;
- domain separation;
- password hashing;
- MACs;
- commitments;
- signatures;
- and checksums.

Treat MD5 only as a historical and attack demonstration. Verify every command
and expected digest.

#### `primitives.tex`

Turn this into the book's primitive map. Each entry must link to its appendix,
its security definition, its implementation, and every chapter that depends
upon it.

### Promising Formalizable Problems

#### `two_oracles_play_rock–paper–scissors`

Start with a commit-reveal protocol for simultaneous choice.

Analyze:

- hiding and binding;
- nonce generation;
- session binding;
- equivocation;
- selective abort;
- fairness;
- repeated play;
- and what happens when one party refuses to reveal.

This is an excellent first end-to-end chapter.

#### `tic_tac_toe_without_revealing_the_strategy`

Formalize a strategy as a committed policy over reachable game states.

Separate several possible claims:

1. proving knowledge of a strategy;
2. proving that a committed strategy is legal;
3. proving that it never loses;
4. answering selected moves without revealing the full policy;
5. preventing extraction through repeated queries.

Do not call the result zero knowledge unless a real zero-knowledge definition
and construction are provided.

#### `adversario_sibi_perfidiam_revelare`

Reframe as a family of problems:

- private comparison;
- private predicate evaluation;
- secure counterexample discovery;
- zero-knowledge proof that a hidden input violates a public invariant;
- or multi-party computation that returns only a warning bit.

Define exactly who holds the secret, who learns the output, and whether the
location or nature of the error is leaked.

#### `joint_secure_obstacle_avoidance`

Define the geometry and leakage.

Possible formulations include privately computing whether proposed trajectories
intersect, finding a jointly safe adjustment, or proving separation without
revealing full trajectories.

Clarify dimensions, uncertainty, timing, dynamics, and malicious input.

#### `secure_multiparty_optimization`

This may become a central general chapter.

Define:

- objective function;
- private variables;
- public constraints;
- leakage through the optimum;
- local versus global optimum;
- malicious inputs;
- verifiability;
- and whether the parties share the same objective.

Do not confuse secure computation of an objective with alignment on the
objective.

#### `law_without_referencering_the_law`

Potentially formalize a proof that a hidden event satisfies or violates a
publicly committed policy.

The chapter must also confront:

- due process;
- auditability;
- appeal;
- discriminatory hidden inputs;
- incorrect encodings of law;
- versioned legislation;
- authority over the circuit or predicate;
- and the danger of an unreviewable judgment whose reasons are concealed.

Privacy must not become immunity from accountability.

### Chapters That Are Primarily Research Questions

#### `addressing_goodharts_law`

Do not promise a single cryptographic solution.

Investigate whether cryptographic protocols can support:

- randomized evaluation;
- committed but rotating objectives;
- multi-metric evaluation;
- adversarial audits;
- challenge mechanisms;
- evidence of evaluator consistency;
- sunset clauses;
- objective versioning;
- and rewards for discovering flaws.

Treat this as cryptography plus mechanism design and governance.

#### `cryptographic_forgivness`

General proof that an adversarial machine deleted all copies of a file is not
available without strong assumptions.

Reframe toward:

- destruction of encryption keys;
- cryptographic erasure;
- threshold key deletion;
- forward-secure storage;
- hardware-backed attestation;
- mutually witnessed key destruction;
- and explicit residual risks from copies, backups, caches, and compromised
  devices.

The title may remain "Cryptographic Forgiveness", but the technical claim must
be narrower than universal proof of deletion.

#### `cryptographic_meta_mathematics`

Treat hidden axioms as an analogy until a precise formal object is identified.

A technically defensible direction may be:

> A party commits to a private policy, preference, constraint set, or witness,
> then proves in zero knowledge that a proposed action satisfies a public
> relation involving that committed private object.

Do not attribute this directly to Gödel's incompleteness theorems without a
rigorous connection.

### Institutional and Philosophical Chapters

The following are valuable, but should not be forced prematurely into
cryptographic protocol form:

- defining victory;
- conflict arbitrage;
- adversarial aftermath;
- aggregation disadvantage;
- momentum-zero trap;
- friendly fire;
- fighting dissipates strength;
- and the Helmsman's Game.

For each, first define:

1. the game or institutional failure;
2. the parties;
3. what information is asymmetric;
4. what commitment is absent;
5. what trusted power is dangerous;
6. what desirable transition is blocked;
7. and whether cryptography can actually change the result.

A chapter may remain a rigorous design essay. That is more honorable than a
false theorem.

## 17. Filename and LaTeX Hygiene

Prepare a rename map before changing paths.

Likely candidates include:

- `aprendix` to `appendix`;
- `figthing_dicipates_strength` to a reviewed English spelling;
- `law_without_referencering_the_law` to
  `law_without_referencing_the_law`;
- Unicode punctuation in filesystem names to ASCII equivalents;
- and spelling corrections in display titles.

Do not rename the Latin-titled chapter until its intended Latin meaning has
been reviewed.

Correct structural defects such as:

- `\chaper` instead of `\chapter`;
- Markdown tables in LaTeX files;
- code outside `verbatim`, `lstlisting`, or an equivalent environment;
- unresolved labels;
- placeholder section titles;
- malformed mathematics;
- undefined commands;
- and duplicated bibliography declarations.

Do not perform cosmetic mass editing before the manuscript compiles and the
research registers exist.

## 18. Research Sequence

Proceed in vertical slices rather than polishing every fragment simultaneously.

### Phase 0: Preservation and Audit

- inspect the repository;
- establish build commands;
- preserve uncommitted work;
- compare backups;
- compile manuscript and code;
- create the chapter matrix;
- create the claim and assumption ledgers;
- and identify immediate scientific red flags.

### Phase 1: Common Language

- write the provisional definition of adversarial cooperation;
- define the claim taxonomy;
- establish notation;
- build the primitive registry;
- and correct the hash and trust-establishment foundations.

### Phase 2: First Complete Protocol

Develop `Two Oracles Play Rock-Paper-Scissors` together with a reusable
commitment primitive.

Complete the chapter, C demo, tests, attack cases, transcript format, and
limitations before opening a larger construction.

### Phase 3: Finite Strategy and Mental Poker

- formalize the tic-tac-toe claims;
- repair and validate the poker demonstration;
- distinguish toy constructions from security-relevant ones;
- and add adversarial test harnesses.

### Phase 4: Private Joint Computation

Develop:

- secret-dependent error notification;
- obstacle avoidance;
- and secure multi-party optimization.

Use small executable models before making general claims.

### Phase 5: Institutional Chapters

Return to Goodhart, hidden law, conflict arbitrage, aftermath, momentum, and
aggregation after the common protocol language is mature.

These chapters should inherit precise concepts rather than inventing a new
meaning of security each time.

## 19. Definition of Done for a Technical Chapter

A chapter is not complete merely because it compiles.

It is provisionally complete when:

1. its problem and baseline are explicit;
2. its adversary and assumptions are explicit;
3. its protocol can be executed from the description;
4. its claims are recorded in the claim ledger;
5. every claim has an evidence status;
6. its C implementation corresponds to the text;
7. its honest execution tests pass;
8. meaningful adversarial tests exist;
9. its limitations are visible;
10. its citations are verified;
11. its contribution is classified without unsupported novelty;
12. and a reader can distinguish what is solved from what remains open.

## 20. Final Standard

Do not aim to make the manuscript sound certain.

Aim to make it impossible for a careful reader to confuse:

- hope with guarantee;
- privacy with justice;
- execution with proof;
- agreement with cooperation;
- secrecy with trust;
- victory with annihilation;
- and an open problem with a solved one.

The project earns authority not by pretending that hard questions are easy,
but by constructing the smallest honest protocols that move those questions
forward.
