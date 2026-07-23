# Adversarial Cooperation: Codex Instructions

## Mission

This repository develops a book and accompanying C implementations about
adversarial cooperation:

> How can mutually distrustful, strategically aware parties cooperate on a
> jointly valuable outcome while retaining legitimate secrets, limiting the
> trust they must place in one another, and detecting or containing deviation?

Treat this as a research program, not merely a documentation project.

The objective is not to claim that cryptography makes people good or guarantees
peace. The objective is to identify precise cooperation problems in which
cryptography changes what parties can safely know, prove, compute, commit to,
or jointly execute.

A protocol is a constitution for a small universe. State exactly what it
permits, what it prevents, what it assumes, and how that universe can end.

## Required Reading

Before any substantial change, read:

1. `research/RESEARCH_CHARTER.md`
2. The relevant chapter source and any associated C implementation.
3. The active execution plan under `research/plans/`, if one exists.
4. Existing build files, tests, bibliography, and repository conventions.

For work affecting multiple chapters, protocol architecture, public C APIs, or
cryptographic claims, create or update a self-contained execution plan in:

`research/plans/<descriptive-name>.md`

Keep the plan current while working. Record discoveries, decisions, tests, and
remaining uncertainties in it.

## Non-Negotiable Research Rules

1. Never use the word "secure" without identifying:
   - the protected property,
   - the adversary model,
   - the setup and channel assumptions,
   - the corruption or collusion threshold,
   - and the type of guarantee.

2. Distinguish explicitly among:
   - perfect or information-theoretic security,
   - statistical security,
   - computational security,
   - heuristic resistance,
   - and an educational demonstration.

3. Never claim novelty until a literature review has been performed and
   recorded. Classify each contribution as:
   - known construction,
   - known construction applied to a new problem,
   - new composition,
   - new definition,
   - conjecture,
   - or potentially novel protocol.

4. Never convert a philosophical intuition into a technical claim merely by
   making the prose more formal.

5. Never fabricate a theorem, proof, citation, experiment, benchmark, security
   property, or implementation result.

6. When evidence is absent, write one of:
   - `Conjecture`
   - `Open problem`
   - `Design intuition`
   - `Proof sketch`
   - `Citation needed`
   - `Not established by the current implementation`

7. Attempt to break every construction before improving its presentation.
   Search for counterexamples, collusion, aborts, equivocation, replay,
   extraction, coercion, denial of service, side channels, and malformed input.

8. Preserve the author's original insight. Correct language and structure, but
   do not silently replace an ambiguous idea with a different one. When the
   intended meaning cannot be recovered, retain the fragment and record the
   ambiguity.

9. Do not erase the poetic or ethical voice. Place it in clearly marked
   epigraphs, remarks, motivations, or `Author's intuition` passages. Poetry may
   ask questions that mathematics cannot yet answer, but it must not wear
   theorem clothing.

10. Peace is the ethical purpose of the project, not a cryptographic property
    that a proof can establish.

## Repository Safety

Before modifying files:

1. Inspect repository status and uncommitted changes.
2. Discover the real build system and source layout.
3. Identify the main LaTeX entry point and all included chapter files.
4. Identify C targets, tests, generated files, and external dependencies.
5. Compare `.tex` files with corresponding `.bak` files before treating either
   as obsolete.

Do not:

- discard uncommitted work,
- delete `.bak` files before reviewing their unique content,
- perform mass renames without a documented rename map,
- initialize or rewrite version-control history,
- push changes,
- add production dependencies silently,
- or run destructive commands.

Commit only when the user requests it or an approved execution plan explicitly
authorizes it.

Use `git mv` for approved renames. Prefer ASCII filesystem names while retaining
the intended Unicode, Latin, Spanish, or other display title inside the book.

## Work Style

Make small, reviewable changes.

For each task:

1. State the exact problem.
2. Identify the files and claims affected.
3. Establish a baseline by building or running tests.
4. Make the smallest coherent change.
5. Build and test again.
6. Report what is now established and what remains uncertain.

Resolve reversible engineering details autonomously and record the decision.
Ask for author judgment only when a choice would alter the meaning, moral
position, research claim, or public structure of the book.

Do not rewrite an entire chapter when a narrower correction is sufficient.

## C Implementation Rules

The C code is an executable companion to the book. It must teach honestly.

- Follow the repository's established C standard. If none exists, propose C17
  and record that choice before reorganizing code.
- Do not use `rand()` for cryptographic randomness.
- Do not implement production cryptography from scratch.
- Use a reviewed cryptographic library for security-relevant operations.
- Place pedagogical implementations in an explicitly named `toy` or
  `educational` area.
- Prefix toy APIs and demonstrations clearly.
- Print an unmistakable warning when a demonstration is not production-safe.
- Separate protocol state machines from cryptographic backends and transport.
- Use canonical, length-delimited serialization.
- Include protocol versioning and domain separation.
- Never construct commitments by ambiguous string concatenation.
- Do not treat a bare hash of a low-entropy secret as hiding.
- Check all lengths, return values, allocations, arithmetic, and parser states.
- Avoid undefined behavior.
- Do not log secrets unless the program is an explicitly labeled teaching demo.
- Use deterministic injected randomness for tests and operating-system or
  library randomness for normal execution.
- Use library-supported constant-time comparison and secret clearing where
  relevant.
- Include negative tests, malformed transcripts, replay attempts, corrupted
  commitments, premature reveals, and abort paths.
- Run available compiler warnings, sanitizers, unit tests, and fuzz targets.

A working demo proves that the demo executes. It does not, by itself, prove the
security of the protocol.

## Manuscript Rules

Every mature protocol chapter must identify:

- the cooperation problem,
- players and roles,
- public and private inputs,
- outputs,
- classical or trusted-intermediary baseline,
- threat model,
- setup assumptions,
- ideal functionality or exact goal,
- protocol construction,
- transcript and state transitions,
- security claims,
- proof or argument status,
- implementation mapping,
- tests or experiments,
- failure modes,
- abuse analysis,
- limitations,
- and open problems.

Use valid LaTeX rather than Markdown syntax inside `.tex` files.

Compile after structural edits. Fix broken labels, references, environments,
tables, code blocks, bibliography entries, and spelling in touched regions.

Do not allow LaTeX success to substitute for scientific correctness.

## Required Work Report

At the end of every task, report:

1. Files read.
2. Files changed.
3. Claims introduced, strengthened, weakened, or removed.
4. Assumptions introduced or made explicit.
5. Commands and tests run.
6. Results and remaining failures.
7. Known security or research limitations.
8. The next smallest coherent step.

Do not merely report that the task is complete.
