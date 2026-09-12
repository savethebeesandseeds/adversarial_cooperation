# Trust Establishment Model Repair

## Status

Completed on 2026-07-21. This was a bounded, manuscript-only repair of the
Trust Establishment foundation chapter. A checked item records evidence
actually obtained.

## Intended Outcome

Replace the current mixture of entropy generation, key derivation, peer
authentication, correlated-source agreement, and public coordination with a
precise map of distinct tasks. Preserve the author's original intuitions as
authorial or open-research material without presenting an unfinished entropy
sketch as a protocol.

This slice introduces no C implementation, no new cryptographic primitive, and
no runtime dependency. Publications and standards are bibliography entries,
not software libraries.

## Progress

- [x] Authoritative status, HEAD, source hash, and affected-file hashes recorded.
- [x] Live chapter and older Git history inspected; no Trust `.bak` exists.
- [x] Current full-book baseline build executed in a disposable LaTeX container.
- [x] Minimal primary/official source gate completed.
- [x] Corrected model and non-claims frozen.
- [x] Chapter, bibliography ownership, and standalone target repaired in staging.
- [x] Research registers updated without rewriting Phase 0 history.
- [x] Standalone and main-book Trust PDFs compile with clean final logs.
- [x] Every final Trust page rendered and visually inspected.
- [x] Existing Hash, RPS, and Poker section targets regress successfully.
- [x] Independent adversarial review has no unresolved P0/P1/P2 finding.
- [x] Guarded explicit copyback and final status verification complete.

## Preservation Baseline

The authoritative checkout is:

`C:\Work\applied_cryptography\adversarial_cooperation`

It is on `main` at
`2824992ee2802b0b72f67054f63189628eb15103`. The completed Phase 0, RPS,
and Hash work remains uncommitted, and the older deletion of
`idyicyanere-linux-x64.vsix` remains user-owned. Do not restore, discard,
stage, commit, or modify unrelated work.

The live Trust source was tracked and clean at task start, with SHA-256:

`a38c4593d3d29fcc99d61fba72138b8fbe5be0ab04ca45bb5d111ec712186860`

No Trust `.bak` exists. Do not invent one. An older revision, `91fc7b6`,
contains authorial ideas absent from the live file: the adversary/enemy
distinction, active interposition, challenge steps, denial of service, and the
line "Solve this, for it is a grail in cryptography." Preserve those ideas as
authorial intuition or open questions while stating that a challenge proves
possession only relative to an already bound credential.

Before copyback, every pre-existing destination must match its recorded
SHA-256 baseline. The new plan destination must remain absent. Copy only the
explicit affected-file list, verify every copied file byte-for-byte, and leave
all generated output disposable.

## Scope

### New artifact

- `research/plans/trust-establishment-model-repair.md`

### Existing artifacts that may change

- `document/content/trust_establishment/trust_establishment.tex`
- `document/references.bib`
- `document/adversarial_cooperation.tex`
- `book`
- `README.md`
- `research/BIBLIOGRAPHY_QUEUE.md`
- `research/DECISIONS.md`
- `research/CHAPTER_MATRIX.md`
- `research/CLAIM_LEDGER.md`
- `research/ASSUMPTION_LEDGER.md`
- `research/OPEN_PROBLEMS.md`
- `research/ROADMAP.md`

### Explicit non-goals

- no new key-establishment protocol, theorem, C code, test framework, or
  runtime library;
- no implementation of TLS, PAKE, PKI, TOFU, a randomness extractor, or an
  entropy source;
- no selection of one universal identity or credential system;
- no proof that the covariance/eigenvalue sketch produces cryptographic
  entropy;
- no full include-graph expansion, global LaTeX cleanup, macro cleanup,
  filename migration, backup creation, or generated-artifact policy change;
- no changes to the primitive registry because this slice approves no new
  primitive; and
- no novelty or production claim; no new or general information-theoretic
  guarantee beyond the cited conditional theorem; and no post-quantum,
  availability, fairness, or composability claim.

## Baseline Build Evidence

The current full manuscript was copied into a disposable Debian/TeX Live 2022
container and compiled with `latexmk 4.79`, after installing the already
documented `texlive-science` build package inside that disposable container.
The build failed at Trust line 94 on raw Unicode `U+2248`. Before failure it
also reported two undefined references (`step:model` and `alg:pkd`) and an
overfull box caused by Markdown table syntax. This is the preservation
baseline, not evidence against the repaired text.

## Minimal Source Gate

This chapter is a literature-backed taxonomy and makes no novelty claim. Use
only the following narrow support:

1. NIST SP 800-90B, *Recommendation for the Entropy Sources Used for Random
   Bit Generation*, DOI `10.6028/NIST.SP.800-90B`, for noise-source models,
   conservative min-entropy estimation, conditioning limits, and health tests.
   It does not authenticate a peer, validate the chapter's covariance sketch,
   or prove that a source is metaphysically "true random."
2. RFC 9846, *The Transport Layer Security (TLS) Protocol Version 1.3*, DOI
   `10.17487/RFC9846`, as a current official example that key exchange,
   authentication, key confirmation, transcript integrity, and external
   credential binding are distinct. It is not this project's protocol and
   does not establish moral identity or trust.
3. Ueli Maurer, *Secret Key Agreement by Public Discussion from Common
   Information*, IEEE Transactions on Information Theory 39(3), 733--742
   (1993), DOI `10.1109/18.256484`, for the exact `X,Y,Z` authenticated-public-
   discussion model and its upper bound. It does not rule out computational
   key agreement and it assumes active modification/injection is detected.
4. The official NIST Interoperable Randomness Beacons project page for public
   common randomness and its explicit warning not to use beacon outputs as
   secret cryptographic keys. It does not authenticate peers or create a
   private shared secret.

Do not add Diffie--Hellman history, Bellare--Rogaway, Shannon, Maurer/Wolf
surveys, NIST SP 800-56A, or HKDF merely to enlarge the bibliography. Reopen a
source only when an active sentence requires it.

## Scientific Contract

Use two intended parties, Alice and Bob, a network adversary Mallory, a setup
resource `Sigma`, a public transcript `T`, and outputs `K_A`, `K_B`, or abort.
Do not imply that these symbols alone define a protocol.

Separate at least these tasks:

1. local secret generation;
2. public coordination or public common randomness;
3. unauthenticated computational key exchange;
4. identity or credential binding;
5. authenticated key establishment and key confirmation; and
6. information-theoretic agreement from correlated observations.

For each task, identify its initial resource, output, secrecy from an observer,
peer/identity guarantee, and principal failure. Split channel confidentiality,
origin authentication, integrity, freshness, delivery, and ordering rather
than treating "channel security" as one property.

### Local entropy

Preserve the author's desire for participant-controlled sources, but present a
defensible pipeline: characterize the source, digitize, conservatively bound
min-entropy, condition/extract within stated preconditions, derive
purpose-specific keys, health-test, protect state, rotate, and fail closed.
Covariance and visible variability are not min-entropy bounds. Deterministic
conditioning cannot manufacture entropy absent from its input. A local secret
does not identify a remote peer.

Retain the old `Lambda`, covariance-eigenvalue, quantization, and participant-
preference ideas only in an explicit Open Research Note. Ask whether covariance
can aid diagnostics and how source autonomy can coexist with objective security
bounds. Remove the current protocol/algorithm status and broken references.

### Authentication and identity

Give a concrete man-in-the-middle trace: Mallory substitutes key material and
establishes one session with Alice and another with Bob. Mallory knows both
resulting keys. A separate passive observer may find them unpredictable, but
that property binds neither key to the intended peer.

Explain that a challenge/response step can show possession of a credential or
secret only after the system has an authentic rule binding that credential to
the intended peer. Setup choices such as a pre-shared secret, authenticated
public key, certificate, pin, TOFU, password ceremony, human fingerprint, or
physical meeting are distinct resources with distinct failures. This chapter
selects none universally.

### Correlated observations

For the cited theorem, fix a finite-alphabet joint distribution `P_XYZ` and
give the parties `N` independent repetitions `X^N`, `Y^N`, and `Z^N`. Let
`T_N` be the public discussion transcript. The asymptotic rate counts secret-
key bits per repetition under vanishing disagreement probability and vanishing
normalized leakage given `(Z^N,T_N)`. Positive `I(X;Y)` alone is insufficient:
let `H(X)>0` and `X=Y=Z`, so `I(X;Y)=H(X)>0` while Eve knows the same value. In
Maurer's exact repeated-source model,

`S(X;Y || Z) <= min{ I(X;Y), I(X;Y | Z) }`.

Consequently, independent `X` and `Y` give zero information-theoretic secret-
key rate in that model. State explicitly that this does not rule out
computational key agreement. Reconciliation leakage must be counted before
claiming extractable secrecy.

### Public coordination

A public beacon, public market value, or deterministic optimum may coordinate
public behavior but is not automatically secret. Retain the Dijkstra example
only under identical public input, algorithm, version, and deterministic
tie-breaking. Keep the market-as-secret-source idea as an open empirical
hypothesis requiring defined `X,Y,Z,T`.

## Mandatory Counterexamples and Non-claims

The chapter must cover:

- active MITM and credential substitution;
- TOFU first-contact capture;
- replay without freshness/context;
- source manipulation and quantizer bias;
- endpoint compromise and secret-key exposure;
- nondegenerate `H(X)>0`, `X=Y=Z` public correlation;
- reconciliation leakage;
- a public beacon known to Eve;
- public-optimum divergence from different state or tie-breaking; and
- denial of service by message dropping, including the absence of reliable
  suppression detection without an explicit timing or failure-detector model.

It must say that cryptography does not establish interpersonal or moral trust,
truth, authorization policy, availability, honest endpoints, just identity
governance, or willingness to cooperate. A credential authenticates only
relative to its setup and binding policy.

## Manuscript Structure

1. Abstract and scope
2. Author's Intuition
3. Cooperation problem and model
4. Six different tasks
5. Local entropy is not remote identity
6. Open Research Note on the original entropy model
7. Active interposition and credential binding
8. Setup choices and their distinct failures
9. Correlated observations with Eve and public discussion
10. Public coordination without secrecy
11. Attacks and counterexamples
12. What Cryptography Does Not Solve
13. Limitations, open problems, exercises, and references

## Register Contract

- Preserve `AC-TRUST-01` through `AC-TRUST-11` verbatim as former-draft
  history and record dispositions; do not silently replace them.
- Add active claim rows for local-entropy/identity separation, setup-dependent
  authentication, the precise `X,Y,Z,T` source-model boundary, public beacon
  non-secrecy, and deterministic public coordination.
- Preserve `AS-TRUST-01` through `AS-TRUST-12` while marking which risks were
  resolved in prose and which remain open. Add explicit source, endpoint,
  active-network, setup-binding, credential-protection, freshness, Eve-side-
  information, authenticated-public-discussion, and common-state assumptions.
- Update `OP-F04`, `OP-F05`, and `OP-F06` only to the extent actually achieved.
- Record bibliography support and non-support boundaries, the manuscript-only
  decision, the preserved open entropy model, and the no-mutual-information-
  shortcut decision.
- Reclassify the chapter matrix row as a foundation/model chapter with no C
  companion and no novelty claim.
- Do not modify `research/REPOSITORY_AUDIT.md`; it is historical evidence.

## Build and Verification

Use the prepared disposable LaTeX container and disposable output directories.
Required checks after the final text is frozen:

```text
bash book -pdf section_trust
latexmk -pdf document/adversarial_cooperation.tex
bash book -pdf section_hash
bash book -pdf section_rps
bash book -pdf section_poker
```

For Trust standalone and main-book outputs:

- require fresh nonempty PDFs and exit zero;
- fail on LaTeX errors, undefined citations/references, missing glyphs, or
  overfull boxes originating in touched files;
- inspect the final log after BibTeX has stabilized;
- render every page with `pdftoppm` and inspect typography, tables, equations,
  bibliography, headers, footers, and transitions;
- re-render after every meaningful manuscript/layout change; and
- remove disposable repository staging and verification artifacts after guarded
  copyback.

Regression section targets must remain buildable. Pre-existing RPS underfull
notices may be recorded but must not be attributed to this slice.

## Verification Record

Verification used the disposable `documents-latex:bookworm` container with
LaTeXmk 4.79 and TeX Live 2022. No package was added to the repository and no C
source was changed.

- `bash book -pdf section_trust` produced a nonempty 12-page PDF (197,238
  bytes). Its stabilized log contains no matched LaTeX error, warning,
  overfull/underfull box, undefined reference/citation, duplicate destination,
  or missing-character notice.
- the main `adversarial_cooperation.tex` entry point produced a nonempty
  19-page PDF (212,636 bytes) after the output directory mirrored the existing
  `\include` auxiliary path. Its stabilized log passes the same clean scan.
- every page of both final PDFs was rendered at 144 DPI and inspected. The
  inspection found and drove two repairs: an indivisible setup table that had
  crossed into the next section's running header was replaced by breakable
  prose, and visible PDF-link border boxes were hidden while links remained
  active. Final typography, equations, tables, citations, bibliography,
  headers, footers, and page transitions were accepted. Blank verso pages in
  the main output are normal `book`-class pagination.
- a final model-precision pass now states that the active interposer knows both
  substituted keys, defines the cited finite-alphabet repeated-source rate
  model, makes the `X=Y=Z` counterexample nondegenerate, and distinguishes
  message suppression from delay unless timing or failure detection is assumed.
  Both PDFs were rebuilt, rescanned, rerendered, and reinspected after these
  changes.
- `section_hash` (214,224 bytes) and `section_poker` (118,000 bytes) rebuilt
  with clean final logs. `section_rps` rebuilt successfully (245,232 bytes) and
  retained only its two previously recorded underfull-vbox notices (badness
  1442 and 4378).
- C tests and sanitizers were not rerun because this slice changes no C source,
  public API, primitive, compiler option, or runtime dependency.

## Guarded Copyback Record

Immediately before copyback, the authoritative checkout still had HEAD
`2824992ee2802b0b72f67054f63189628eb15103`; all twelve pre-existing
destinations matched their recorded baseline SHA-256 values; and this plan's
destination was absent. The full dirty status was captured and retained,
including the author's existing Phase 0, RPS, and Hash work and the pre-existing
deletion of `idyicyanere-linux-x64.vsix`.

Only the twelve listed existing artifacts were copied first. Each destination
was then compared byte-for-byte with staging. Their post-copy SHA-256 values
were:

- `book`: `bb29f5c6811d5303784f9af8051620d2f5149751c38ddd1c6e3691b78b85807a`
- `README.md`: `3aeca9e40227d906c31e836088798ba26eb5805cf301b85fcf766cf31a63dc89`
- `document/adversarial_cooperation.tex`: `5ea562cb9149a1a9b20226a89cb1d122747fafd6204c0b6ed3c4527dea7654be`
- `document/content/trust_establishment/trust_establishment.tex`: `e38d4e880ab5b6ec4bb0a1a793b1c6f524419cfc3dd82f1ddecd6e3a1166de6d`
- `document/references.bib`: `a785239e437d715e1178fd740454bb734a904af2689d66b3914da9a53dabf78f`
- `research/BIBLIOGRAPHY_QUEUE.md`: `6999606a9f3497308b012425ed04c8c1da97fd2247a2875768d999b39d1de9fa`
- `research/DECISIONS.md`: `c28380337b4de2f91ff7f8aaa38433afdc4d109c312673328a568eb8581b637b`
- `research/CHAPTER_MATRIX.md`: `812379f63eff7806e83b5d658c02ce8425b3de0e10c8f5f66a73d9edbe5e04f1`
- `research/CLAIM_LEDGER.md`: `bb78ca12641a9cc411289e58b4d0abb04915dbf1d5397b624ac2d83041388fb2`
- `research/ASSUMPTION_LEDGER.md`: `89c7441af4d6faf45d7753990c67b3923eea4dcbf9eedea8959b3fb0181a6e64`
- `research/OPEN_PROBLEMS.md`: `e172c7b661eb0a30db99bda0afc9a7a01e4d0d32bd702eaa62b1c7a63e535351`
- `research/ROADMAP.md`: `b2fe3fc023d065b0bbae98f2e0ff2f5546cc20c1968513e2c9ad7af79296e71b`

This completed plan is the thirteenth and final explicit artifact copied. No
backup, C source, test, generated PDF, repository metadata, or unrelated file
is part of copyback. Final verification requires all thirteen authoritative
files to match staging, all staged backups to remain equal to their live
counterparts, HEAD to remain unchanged, and the dirty status to contain only
the expected additions to the preserved pre-task state.

## Acceptance and Evidence Boundary

The slice is provisionally complete only when the active chapter cleanly
separates the six tasks, every security statement names its model/setup, all
old ideas remain traceable, citations support only matched claims, no new
runtime dependency or primitive exists, the final PDFs are visually clean, and
the registers agree with the manuscript.

A successful build proves document production only. It does not prove an
entropy source, authenticate a peer, establish a key, or validate a security
theorem beyond the cited model and reproduced derivation.
