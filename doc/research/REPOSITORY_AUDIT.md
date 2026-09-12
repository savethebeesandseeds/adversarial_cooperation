# Phase 0 Repository Audit

## Audit Metadata

- Date: 2026-07-17
- Real repository: `C:\Work\applied_cryptography\adversarial_cooperation`
- Branch: `main`
- Baseline commit: `2824992ee2802b0b72f67054f63189628eb15103`
- Scope: preservation, live structure, builds, backups, claims, assumptions,
  scientific risk, and first-slice planning only
- Literature status: no literature review performed; all novelty classifications
  remain unassessed

## Executive Result

There is a real research program here, but the current repository is not yet a
book-wide technical baseline.

1. The main LaTeX document reaches **one of 27** content sources: Trust
   Establishment. The Introduction is commented out; the other 25 content
   sources are entirely orphaned.
2. The active manuscript does **not** build successfully from the documented
   command. After correcting the shell/path and installing the undeclared
   `algorithm2e` package, it still fails on output-directory creation, an
   unsupported Unicode symbol, a missing bibliography, and two undefined
   references. It can emit a partial PDF despite returning failure.
3. Poker is the strongest existing technical material and the only C companion.
   Its honest, centralized two-player program builds cleanly and survives smoke
   and sanitizer runs, but one process owns both parties' keys and hands. It is
   an educational algorithm animation, not evidence of privacy between
   distrustful peers.
4. There are no tests, fuzz targets, CI workflows, formal security games,
   complete proofs, `.bib` files, or `\cite` commands.
5. Ten tracked manuscript backup pairs and three ignored scholarship pairs were
   compared. Two manuscript backups and one ignored scholarship backup contain
   material that must be preserved.
6. The smallest credible first implementation remains a reusable commitment
   module plus Two Oracles Play Rock-Paper-Scissors, including selective-abort
   behavior as an explicit limitation.

No chapter, C source, backup, build script, or filename was changed during this
audit. The only intended writes are governance and research-register Markdown
files.

Shared build attempts temporarily modified three tracked generated log/database
files and created four untracked files under `document/.temp`. After capturing
their failure evidence, Phase 0 restored the three generated tracked files to
their exact `HEAD` blobs and removed only those four audit-created temporary
files. Final status contains no build-generated audit noise.

## Preservation Baseline

### Real checkout versus visible workspace

The user-specified checkout is authoritative and valid:

```text
C:\Work\applied_cryptography\adversarial_cooperation
  Git root: C:/Work/applied_cryptography/adversarial_cooperation
  branch: main
  HEAD: 2824992ee2802b0b72f67054f63189628eb15103
```

The visible Codex workspace is not a mirror of the project:

```text
C:\Users\santi\OneDrive\Documents\Adversarial-cooperation
  contains only .git
  unborn branch: master
  no valid HEAD
```

All audit work therefore occurred in `C:\Work\...`.

### Pre-existing worktree change

Before any audit write, Git reported exactly:

```text
 D idyicyanere-linux-x64.vsix
```

The tracked file is 2,180,444 bytes in `HEAD`. The audit did not restore,
stage, finalize, or otherwise alter that deletion.

### Ignored work

`.gitignore` contains only `scholarship/*`. The ignored scholarship tree is
real author work, not disposable build output. It contains three `.tex`/backup
pairs; Git cannot recover them if they are damaged or removed. In particular,
`scholarship/general_letter/chinese_v.tex.bak:36` preserves the correct full-
width closing parenthesis where the current file contains replacement
characters.

## Repository Inventory

Baseline Git tracks 120 files. The important live areas are:

| Area | Live contents | Audit result |
|---|---|---|
| Root | README, Bash helpers, shared Make configuration, license | Build documentation and script modes are inconsistent with a clean Linux clone |
| `document/` | Main book, utilities, 27 non-backup content `.tex` files, 10 tracked backups | Only Trust Establishment is included; most structural defects are dormant |
| `src/` | One C source, one Makefile, README, tracked poker binary | Poker only; no reusable primitive or protocol library |
| `demostrations/` | Makefile, README, one 101-line poker note, many empty mirrors, tracked outputs | Poker note is orphaned and duplicates the 172-line book chapter |
| `scholarship/` | Ignored LaTeX applications and backups | Out of book scope but preservation-sensitive |
| `research/` | Added by Phase 0 | Governance, ledgers, audit, roadmap, and execution plan |

Git tracks **36 build-derived paths** matched by output directories/extensions:
35 obvious compiled artifacts (PDFs, binaries, object, aux/log/fls/fdb/toc/out
files) plus `.temp/book_sources/section_poker.tex`, a generated wrapper. Outputs
are spread among `.temp/`, `temp/`, `document/temp/`, and
`demostrations/.build/`. The ignore file protects none of these paths.

Eight intended appendix directories are empty and therefore absent from Git:

- `abstract_zero_knowledge_proofs`
- `block_chains`
- `complexity_theoretic_security`
- `digital_signatures`
- `fully_homomorphic_encryption`
- `information_theoretic_security`
- `security_models`
- `zero_knowledge_proofs`

## LaTeX Entry Point and Include Graph

The complete active project-local graph is:

```text
document/adversarial_cooperation.tex
├── \input{utils/text_utils.tex}                         line 4
├── \input{utils/math_utils.tex}                         line 5
├── % \include{content/introduction}                     line 19 (disabled)
└── \include{content/trust_establishment/
             trust_establishment}                       line 20
    ├── \bibliographystyle{plain}                        line 97
    └── \bibliography{references}                        line 98 (missing target)
```

No other non-backup content source contains an active `\input` or `\include`.
The main file directly adds only a placeholder appendix at lines 24-27.

Consequences:

- 26 of 27 content sources are absent from the built book.
- Only five content files even contain a valid `\chapter` command:
  Introduction, Trust Establishment, Poker, Tic-Tac-Toe, and Hash Functions.
- The apparently narrow current build hides most LaTeX errors rather than
  demonstrating manuscript-wide integrity.
- The separate poker demonstration note is not included by any document.

## Bibliography and Reference State

- Repository `.bib` files: **0**.
- Manuscript `\cite...{}` calls: **0**.
- Bibliography declarations: one, inside Trust Establishment.
- Missing target: `references.bib`.
- Undefined labels referenced at `trust_establishment.tex:61`:
  `step:model` and `alg:pkd`. The actual algorithm label is `alg:emp-max`.

Primary-source work is required for Shannon, Maurer/Wolf, Goodhart and related
effects, Gödel/CH/large cardinals, poverty-trap terminology, hash definitions
and MD5, mental-poker/verifiable-shuffle prior art, commitments, and exact
ISO/IEC parts/editions.

## Build and Test Audit

### Host and declared environment

The Windows host exposes Docker 29.6.1 but no native `make`, GCC/Clang,
`latexmk`, `pdflatex`, or BibTeX. The repository documents a Debian 11
container workflow.

Declared dependencies are split and incomplete:

- README LaTeX list: `texlive`, `texlive-latex-extra`,
  `texlive-lang-spanish`, `latexmk`, `aspell`, `aspell-en`, `coreutils`.
- `setup.sh` C list: `build-essential`, `make`, `pkg-config`, `libssl-dev`,
  `libsodium-dev`.
- Make configuration: C11, `-Wall -Wextra -Wpedantic -O2`, and `-lsodium`.
- Actual additional LaTeX requirement: `algorithm2e.sty`, supplied by Debian's
  `texlive-science`, which the README does not install.

`book`, `compile_latex.sh`, and `setup.sh` are all tracked with Git mode
`100644`; nevertheless README documents `./book`. A clean Linux clone cannot
assume direct execution permission.

### Exact manuscript command results

The documented root workflow is:

```bash
cd /src
sh compile_latex.sh adversarial_cooperation.tex
```

The audit added `-s` only to suppress interactive spell-checking and ran the
otherwise equivalent command:

```bash
sh compile_latex.sh -s adversarial_cooperation.tex
```

It fails before TeX because `compile_latex.sh` uses Bash-specific
`set -o pipefail` while Debian `/bin/sh` is `dash`. Running it with Bash exposes
the second defect: there is no root-level `adversarial_cooperation.tex`.

The corrected non-interactive command was then exercised:

```bash
bash compile_latex.sh -s document/adversarial_cooperation.tex
```

Results by environment/stage:

| Environment / attempt | Result |
|---|---|
| Existing TeX Live 2022 image | Exit nonzero: `algorithm2e.sty` not found; no PDF from that run |
| Throwaway `documents-latex:bookworm` plus ephemeral `texlive-science`, first attempt | Exit nonzero: output tree `.temp/content/trust_establishment` was not created; latexmk requested a retry |
| Same throwaway environment with nested output path precreated | Exit 1; emitted a **partial 9-page, 191,479-byte PDF**, but failed on Unicode `≈` at trust line 94, missing `references.bib`, undefined labels at line 61, and an overfull Markdown-table box at lines 79-84 |

Tool versions in the throwaway LaTeX check:

- Debian bookworm image
- TeX Live 2022
- pdfTeX 1.40.24
- latexmk 4.79

An emitted PDF is therefore not a successful manuscript build.

### Section helper

```bash
./book -list
./book -pdf section_poker
```

- `-list` succeeds and exposes exactly one target:
  `section_poker -> demostrations/poker_without_revealing_the_cards/poker_without_revealing_the_cards.tex`.
- Without `texlive-science`, the section build fails on `algorithm2e.sty` and
  leaves the pre-existing tracked three-page PDF untouched.
- In a throwaway copy with `texlive-science`, the section build succeeds:
  exit 0, three pages, 118,000 bytes.

This target builds the duplicate demonstration note, not the 172-line book
chapter.

### Fresh C builds

All 18 pre-existing local Docker images and eight running containers were
checked. Some had GCC/Make, but none provided `/usr/include/sodium.h` or a
libsodium development `pkg-config` entry. A fresh compile therefore used a
throwaway `debian:11` container (digest
`sha256:6cb68b1be980a0e5b19be25582b34b5cf9cb466d52d08ab4354b79051f2cd298`)
with the repository mounted read-only and these ephemeral packages:

```bash
apt-get update
apt-get install -y --no-install-recommends \
  build-essential make pkg-config libsodium-dev
```

Build outputs were redirected to `/tmp`, not written into the repository.

```bash
make -C demostrations demo_poker \
  TEMP_DIR=/tmp/ac-default-temp \
  BUILD_DIR=/tmp/ac-default-build \
  POKER_ARGS='--suits ascii'

make -C demostrations build_poker \
  TEMP_DIR=/tmp/ac-strict-temp \
  BUILD_DIR=/tmp/ac-strict-build \
  CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'

/tmp/ac-strict-build/poker_without_revealing_the_cards/poker_demo \
  --suits ascii

make -C demostrations build_poker \
  TEMP_DIR=/tmp/ac-san-temp \
  BUILD_DIR=/tmp/ac-san-build \
  CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer' \
  LDFLAGS='-fsanitize=address,undefined'

ASAN_OPTIONS='detect_leaks=1:halt_on_error=1' \
UBSAN_OPTIONS='halt_on_error=1' \
  /tmp/ac-san-build/poker_without_revealing_the_cards/poker_demo \
  --suits ascii
```

Environment:

- Debian 11
- GCC 10.2.1
- GNU Make 4.3
- libsodium-dev 1.0.18-1+deb11u1

Results:

- Default fresh compile: success, no compiler warnings, demo exit 0.
- `-Werror` fresh compile: success, no compiler warnings, demo exit 0.
- AddressSanitizer + UndefinedBehaviorSanitizer: build and run exit 0, no
  diagnostics.
- One hundred randomized smoke runs of the available current demo: 100/100
  exit 0.
- `--help`: exit 0.
- ASCII and Unicode suit modes: exit 0.
- Unknown argument, missing `--suits` value, invalid `--suits` value, and
  invalid environment override: exit 1 as expected.

There is no repository `test`, sanitizer, fuzz, or CI target. The sanitizer and
smoke checks above are audit probes, not a substitute for protocol tests.

### Hash example verification

Live MD5 computation produced:

| Input | Actual MD5 | Manuscript status |
|---|---|---|
| `Hello, World!` | `65a8e27d8879283831b664bd8b7f0ad4` | Correct |
| `ello, World!` | `2f695452b98fa1028dc11e1e60f17614` | Manuscript incorrectly prints the empty-string digest |
| empty string | `d41d8cd98f00b204e9800998ecf8427e` | This is the value wrongly assigned to the prior row |

The two large purported MD5 collision blocks at hash lines 47-48 are not a
collision as written. After whitespace removal and hex decoding, their digests
are respectively `166a4f371787a68401d88b1241e0614d` and
`ecab8d70d60cb15ab6a2732d93ea3318`.

## Backup Comparison

A full filesystem scan found 13 live/backup pairs.

### Tracked manuscript pairs

| Pair | Result |
|---|---|
| `adversarial_aftermatch` | Backup has no unique idea; current fixes spelling and adds two notes |
| `adversario_sibi_perfidiam_revelare` | Spelling-only change |
| `aprendix/hash_functions` | Spelling/style-only change |
| `aprendix/non_repudiation` | Spelling-only change |
| `conflict_arbitrage` | Spelling-only change |
| `cryptographic_meta_mathematics` | **Substantive:** backup line 5 says “Godel's unprovable truths”; current says “improvable truths” |
| `interactive_secure_stag_hunt` | Spelling-only change |
| `joint_secure_obstacle_avoidance` | Spelling/style-only; both retain `\chaper{}` |
| `poker_without_revealing_the_cards` | **Substantive major rewrite:** backup preserves unique research questions |
| `tic_tac_toe_without_revealing_the_strategy` | Spelling-only change |

Poker backup material retained in the open-problem register:

- dynamic join/decline (`.bak:10`);
- full-deck completeness comparison (`.bak:13`);
- withholding cards even from owners to resist association, and the resulting
  impossibility of meaningful betting (`.bak:14-18`);
- assigning values to commitments and homomorphic/proof-based winner evaluation
  (`.bak:22-24`);
- generalization to other card games (`.bak:28`).

### Ignored scholarship pairs

| Pair | Result |
|---|---|
| `scholarship/beihang/research_proposal` | Spelling-only change |
| `scholarship/general_letter/applying_for_scholarship` | Mostly spelling/terminology; backup has a stylistically cleaner sentence boundary |
| `scholarship/general_letter/chinese_v` | **Preserve backup:** correct `）` at line 36 is corrupted as two `U+FFFD` replacement characters in current |

No backup was removed or modified.

## Defect and Research-State Classification

### Build defects

- Documented shell is wrong for a Bash script.
- Documented main `.tex` path does not exist.
- Scripts are not executable in Git despite `./book` documentation.
- `texlive-science`/`algorithm2e` dependency is undeclared.
- Build helper does not precreate nested include-output directories.
- Missing bibliography and unsupported Unicode prevent main success.
- Tracked generated outputs make failed builds modify logs while leaving an old
  PDF looking authoritative.
- No clean-container definition or pinned dependency versions exist.

### Active LaTeX defects

- Book is in `draft` mode.
- Trust line 61 has two undefined references.
- `references.bib` is missing.
- Trust lines 79-84 contain a Markdown table rather than LaTeX.
- Trust line 94 uses unsupported `≈` under the current pdfLaTeX setup.
- Bibliography ownership is inside a chapter.
- `\section{Topic}` and disconnected protocol notation remain placeholders.
- Utility macros contain duplicated packages and fragile/unmatched math
  construction patterns.

### Dormant LaTeX defects

- `\chaper` typo and unescaped coordinate subscripts in obstacle avoidance.
- Raw `#FIXME` in Non-Repudiation.
- Markdown fences/lists in Goodhart, Aftermath, Victory, and Meta-Mathematics.
- Raw shell commands outside a code environment in Hash Functions.
- Four generic `Section Title` headings in Introduction.
- CJK title text with no CJK-capable documented engine/setup.
- Unicode punctuation in the RPS filesystem path.
- Many sources have no chapter/section structure and one is empty.

### Implementation defects and limits

The C program does establish a narrow set of honest-execution observations:

- a 52-card array is permuted;
- exponent layers round-trip correctly in the fixed small group;
- nine honest decrypted draws are valid and non-duplicated;
- in-memory draw hashes and hand commitments recompute;
- argument validation and suit rendering behave consistently;
- current code triggers no compiler/sanitizer diagnostic in the audited run.

It does **not** establish the chapter's distributed security properties:

1. One process holds Alice's and Bob's secret exponents, decrypts both hands,
   and prints both hands and both nonces. No participant privacy boundary exists.
2. There are no protocol messages, parsers, participant-local state machines,
   authenticated channels, transcript consensus, session IDs, or abort states.
3. The fixed prime `104729` is a 17-bit educational group and provides no
   meaningful production confidentiality.
4. Deck “commitments” are a custom 64-bit FNV-style checksum, not a
   cryptographic binding commitment.
5. No participant proves that its shuffle is a permutation, its encryption is
   valid, or it did not replace/mark/duplicate cards.
6. Integrity checks cover only nine honest plaintext draws, not the full deck
   or malicious omission/substitution.
7. Hand commitments lack protocol, version, session, player, and role domain
   binding, and have no negative/replay tests.
8. The program prints “Demo scope: secrecy, integrity, and commitments” without
   an unmistakable production-safety warning; this can overstate the evidence.

The per-draw BLAKE2b value is deterministic over a public label and a ciphertext
in the range 1–104,728. An audit probe enumerated that domain with:

```python
hashlib.blake2b(label + struct.pack('<Q', value), digest_size=32)
```

It recovered one live encrypted value (`102016`) after 102,016 trials in
0.066455 seconds. This demonstrates that the draw hash does **not** hide the
17-bit ciphertext. It does not by itself remove the secret exponent layers or
recover the plaintext card; the finding must not be overstated.

### Incorrect or overbroad cryptographic claims

Highest-impact examples are recorded verbatim in `CLAIM_LEDGER.md`:

- blanket information-theoretic and quantum-unbreakable claims in Introduction;
- local randomness as the sole trust assumption and “only signal loss” risk;
- “no harm” from secret-key disclosure;
- secrecy-versus-coordination as a general dichotomy;
- bare hashes as hiding, anonymity, authentication, or signatures;
- a hash of a hidden legal reason as sufficient protection/proof;
- universal proof that every copy of a file was deleted;
- poker privacy, binding, shuffle, and transcript language stronger than the
  centralized honest demo establishes;
- zero knowledge treated as a generic proof of truth, strategy, law, or
  behavior without a defined relation.

### Philosophical claims and design principles

These should be preserved, clearly labeled, and later connected to operational
questions rather than removed:

- “Victory is not the defeat of the adversaries.”
- “Never make victory perpetual.”
- hard problems require humble solutions;
- systems should optimize for succession, harmony, and change;
- conflict may reveal strength;
- aggregation may destroy separable value;
- unity may change after a common adversary disappears.

### Conjectures and open research questions

- evolving objectives or strange attractors as a response to Goodhart;
- hidden-policy/“hidden axiom” compliance proofs;
- secret-dependent error notification;
- private obstacle avoidance;
- private optimization without objective alignment;
- non-revealing strategy certification with bounded query leakage;
- hidden-law compliance compatible with due process;
- cryptographic erasure/forgiveness under realistic device assumptions;
- aftermath, friendly-fire, momentum, and conflict-arbitrage mechanisms.

### Empty or near-empty chapters

- Byzantine Generals Problem: empty file.
- Non-Repudiation: FIXME only.
- Primitives, Secure Multi-Party Optimization, Two Oracles, AST/HLS, and
  Cryptographic Forgiveness: one-line notes.
- Several institutional chapters are valuable but currently only one to six
  lines of intuition.

### Executable companions

Poker is the sole C companion. There is no implementation for any primitive or
other chapter. The current poker note and book chapter duplicate one another,
creating claim drift.

## Ten Most Consequential Scientific Risks

1. The only active chapter claims local randomness is the sole trust assumption
   and leaves only signal loss, while supplying no identity/authentication
   mechanism.
2. Introduction announces universal information-theoretic/post-quantum
   guarantees without a construction, adversary model, or proof.
3. Hash Functions teaches incorrect definitions/uses and demonstrably wrong MD5
   examples.
4. Poker's “secure shuffle/deal” vocabulary can be read more strongly than its
   centralized, cooperative toy program permits.
5. Almost every protocol lacks parties, inputs, leakage, setup, channels,
   corruption threshold, abort behavior, and exact desired property.
6. Zero knowledge is repeatedly used as a universal truth/compliance mechanism
   without an efficiently decidable relation or setup.
7. There is no bibliography or recorded literature review, so no contribution
   can responsibly be called novel.
8. Universal deletion proof ignores copies, backups, caches, compromised
   devices, coercion, and hardware boundaries.
9. MPC, optimization, and rotating metrics risk being conflated with alignment,
   fairness, justice, or truthful input.
10. The include graph hides 26 of 27 content sources and most structural errors;
    a narrow or partial PDF is not evidence that the book compiles.

## Strongest Existing Material

1. **Poker's decomposition and limitations.** It names a trusted-dealer
   baseline, four properties, three assumptions, four stages, an implementation
   mapping, and major non-goals. Its claims need narrowing, but its architecture
   is the book's best current technical anchor.
2. **The adversary/enemy distinction.** It can become a useful definition once
   expressed as behavior or preferences rather than moral essence.
3. **Secret-dependent error notification.** It is compact enough to become a
   precise private-predicate example.
4. **Hidden-event/public-rule verification.** It is a promising ZK/MPC question
   if policy authority, real-world facts, audit, appeal, and due process remain
   first-class requirements.
5. **The ethical voice.** “Never make victory perpetual,” objective fallibility,
   succession, and flaw-discovery incentives are distinctive design principles.
6. **Two Oracles.** Although currently one sentence, its bounded state makes it
   the best first complete prose/code/test vertical slice.

## Precise Next Implementation Scope

The next plan should implement **only** a reusable commitment primitive and a
two-party Rock-Paper-Scissors commit-reveal protocol.

It should include:

- C11 + libsodium commitment API;
- domain/version/session/role binding and canonical encoding;
- OS randomness plus deterministic test injection;
- participant-local RPS states and versioned messages;
- commit/commit/reveal/reveal transcript;
- explicit invalid, duplicate, replay, reorder, premature-reveal, timeout, and
  selective-abort states;
- all nine honest move pairs and negative tests;
- sanitizer/warning builds;
- a mature chapter with assumptions, security-argument status, limitations,
  citations, and “What Cryptography Does Not Solve.”

It should explicitly exclude networking, zero knowledge, deposits/penalties,
fair-output guarantees, poker refactoring, mass renames, and production claims.
`ROADMAP.md` contains the full acceptance criteria.

## Files Read

The audit read and/or mechanically compared:

- root `.gitignore`, README, `book`, `compile_latex.sh`, `setup.sh`,
  `Makefile.config`, Git status/history/modes, and generated logs;
- `document/adversarial_cooperation.tex` and both utility files;
- all 27 non-backup sources under `document/content/`;
- all 10 tracked content backups;
- all three ignored scholarship live/backup pairs;
- both Makefiles and READMEs under `src/` and `demostrations/`;
- the sole C source and both tracked poker binaries/output metadata;
- the 101-line poker demonstration note;
- governance and research artifacts created during Phase 0.

## Files Created or Changed by Phase 0

Created:

- `AGENTS.md`
- `research/RESEARCH_CHARTER.md`
- `research/REPOSITORY_AUDIT.md`
- `research/CHAPTER_MATRIX.md`
- `research/CLAIM_LEDGER.md`
- `research/ASSUMPTION_LEDGER.md`
- `research/OPEN_PROBLEMS.md`
- `research/RENAME_MAP.md`
- `research/ROADMAP.md`
- `research/plans/phase-0-repository-audit.md`

Not changed:

- manuscript sources;
- C sources;
- build scripts and Makefiles;
- backups;
- filenames/directories;
- the user's pre-existing VSIX deletion.

## Claim and Assumption Change Report

- Manuscript claims introduced, strengthened, weakened, or removed: **none**.
- Protocol assumptions introduced: **none**.
- Existing claims were copied/classified in the claim ledger.
- Existing explicit and necessary implicit assumptions were exposed in the
  assumption ledger.
- Governance language was added to prevent future work from silently promoting
  intuition into theorem or demo output into security evidence.

## Current Blockers

These block a credible next implementation, not completion of Phase 0:

1. clean build/test spine and generated-output policy;
2. explicit commitment/RPS execution plan;
3. author decision on “unprovable” versus “improvable” and the VSIX deletion;
4. preservation decision for ignored scholarship work;
5. primary-source bibliography work before novelty/security claims;
6. exact threat models before expanding poker or institutional chapters.

## Next Smallest Coherent Step

Review this audit and approve the scope of
`research/plans/commitment-and-rps-vertical-slice.md`. That plan—not code—should
be the next artifact. It can settle the message format, guarantee type, tests,
build outputs, and selective-abort semantics before implementation begins.
