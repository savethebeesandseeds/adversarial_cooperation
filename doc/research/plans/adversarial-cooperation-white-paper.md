# Adversarial Cooperation: IEEE-format research proposal

Status: research-proposal relabel and root-level delivery complete,
2026-09-12. All earlier edits remain applied. The historical filename and
revision records are retained; the latest record at the end supersedes the
original document designation and delivery paths.

## Request and acceptance criteria

Author request, 2026-09-11: prepare a short (1-3 pages) white paper for
professional cryptographers and prospective research funders, authored by
Santiago Restrepo, with contact@waajacu.com and https://waajacu.com/.
Explain cooperation between adversarial parties without a trusted authority,
including a research path to hardware synthesis. Preserve the ethical purpose,
use humble and precise language, and distinguish proposed research from results.

Deliver an IEEEtran conference-format PDF, editable LaTeX and bibliography,
rebuild instructions, and this evidence/work report. This is an independent
research white paper; no IEEE publication, endorsement, or acceptance is implied.
No budget, timetable, theorem, or novelty is invented. The author requested
removal of the Waajacu affiliation line on 2026-09-12; contact details remain.

## Scope and source boundary

- Add a standalone paper under document/white-paper/ and final artifacts under
  output/pdf/. Do not restructure or change the two books or their claims.
- Use the current TTT reference contract as the small, reproducible example;
  mention commit-reveal only if it helps explain abort limits.
- Private proofs, applied private joint decisions, and synthesis remain proposed.
- Verify a small primary-source bibliography, including prior mediator-replacement
  work and limitations. Classify the proposal as a research program using known
  constructions, with prospective compositions subject to review.
- Maintain exact distinctions between protocol guarantees, tested executable
  behavior, functional hardware equivalence, and physical leakage resistance.

## Required reading and initial observations

Read AGENTS.md, RESEARCH_CHARTER.md, README.md, Makefile.config, the two book
entries and chapter include maps, the short-book/companion plan, active TTT
reference/proof plans, relevant claim/assumption records, bibliography, C/test
Makefiles, setup.sh and container/tasks.sh. Independent reviewers read the
TTT, RPS, obstacle-avoidance and AST/HLS sources, relevant C/test sources and
available .bak counterparts. Backups retain author intuitions; none is obsolete
or removed by this work.

Initial Git worktree is clean. Ordinary host git status fails because the
Windows sandbox identity differs from the existing repository owner. A bounded
git -c safe.directory=C:/Work/applied_cryptography/adversarial_cooperation status
diagnostic succeeds; no ownership, identity, Git config, history, or metadata
change is made.

The documented existing container adversarial-cooperation-dev was inspected
and matches README configuration. It was stopped initially and is reused.
Immutable ID: 087f758ebdc8c512223a1414e871d84b3850a6491483b390900b12b2dc661738.
No container, volume, image, port, mount, or dependency setup is created/replaced.
IEEEtran is absent from the installed TeX distribution; use an unmodified,
locally vendored CTAN distribution with license/provenance, without changing
the project's runtime dependency setup.

## Execution sequence

1. Run the repository verification task as a baseline, with a unique artifact ID.
2. Verify sources and draft an approximately two-page paper; allow up to three
   pages if precision requires it. Record assumptions and prospective claims.
3. Independently review scientific claims and funding clarity; resolve findings.
4. Compile using IEEEtran with normal conference typography and bibliography.
5. Check warnings, citation resolution, page count, embedded fonts, author/contact
   metadata and links; render and inspect every final page.
6. Package source and rebuild instructions for offline use. Record final hashes,
   commands, results, limitations and the next bounded research step here.

## Validation and discoveries

- Baseline passed: docker exec --env AC_RUN_ID=white-paper-baseline-20260911
  adversarial-cooperation-dev /bin/bash container/tasks.sh verify.
  The write-once record is
  .container-output/white-paper-baseline-20260911/verification.txt.
- All 72 C test groups passed (7 commitment, 6 RPS, 14 adversarial RPS,
  6 hash, 13 TTT, 13 Boolean circuit, 3 TTT Core, 10 TTT Bind), as did the
  corresponding AddressSanitizer/UndefinedBehaviorSanitizer suite.
- Both canonical books, static-site tests, the six-group native browser bridge,
  and all three native/WebAssembly demo comparisons passed.
- Toolchain: GCC 14.2.0, GNU Make 4.4.1, libsodium 1.0.18, latexmk 4.86,
  pdfTeX 1.40.26 (TeX Live 2025/dev/Debian), Emscripten 3.1.69.
- The PDF skill's artifact-start marker ran successfully once before authoring.
- Used the actual IEEEtran 1.8b conference class and IEEEtran.bst 1.14 from
  CTAN, not a visual approximation. Full upstream source was retained;
  a byte comparison against the ZIP verified all 36 upstream files unchanged.
- The new paper is standalone; the books, root build system, runtime packages,
  site exports, C source and test files are unchanged.
- Three independent agent reviews covered repository/scientific consistency,
  primary-source/literature and hardware claims, and editorial/funding clarity.
  These are drafting reviews, not external peer review or formal proofs.
- Incorporated reviewers' substantive corrections: commitment/proof co-design,
  bounded rather than inexpensive testbed, an explicit bridge from the game
  to private decision-rule verification, and a precise classical coin-flipping
  limit. No unresolved blocking finding remains.
- Paper compilation passed with six resolved citations, no LaTeX/package
  warnings, no undefined references and no overfull boxes. One underfull-box
  diagnostic (badness 1810) remains in the related-work paragraph; visual
  inspection found acceptable ordinary justified spacing, with no overflow.
- Rendered both final pages with Poppler and inspected them. US Letter,
  normal 10-point body text, balanced final columns, no clipping or broken
  glyphs. All seven fonts are embedded Type 1. Verified PDF author/title,
  mailto link, website link, and all six DOI links directly in the PDF objects.
- No pdffonts executable is on PATH; embedded-font validation used pypdf's
  font descriptors instead. This is a completed alternative check, not an
  outstanding dependency failure.
- Final PDF and source ZIP are byte-checked; all 43 archive members match their
  source files. A rebuild from the independently extracted source archive is
  the final packaging check.

## Literature review and contribution classification

Verified primary bibliographic metadata and relevant claim boundaries on
2026-09-11. This is a focused foundational review, not an exhaustive novelty
search. Accordingly, the paper claims no novelty. Its present contribution is
an applied research agenda using known constructions, with prospective
compositions and applications to be evaluated individually.

| Source | Verified identifier | Role and limitation |
| --- | --- | --- |
| Naor, Journal of Cryptology 4(2), 151-158 (1991) | https://doi.org/10.1007/BF00196774 | Established commitment machinery; not a proof for the repository's hash profile. |
| Goldwasser, Micali, Rackoff, SIAM J. Comput. 18(1), 186-208 (1989) | https://doi.org/10.1137/0218012 | Zero-knowledge foundations; does not make a public checker private. |
| Goldreich, Micali, Wigderson, STOC 1987, 218-229 | https://doi.org/10.1145/28395.28420 | MPC foundation; honest-majority results are not generalized to the proposed two-party threshold. |
| Dodis, Halevi, Rabin, CRYPTO 2000, LNCS 1880, 112-130 | https://doi.org/10.1007/3-540-44598-6_7 | Prior cryptographic mediator replacement in strategic games under its computational/game-theoretic assumptions. |
| Cleve, STOC 1986, 364-369 | https://doi.org/10.1145/12130.12168 | Classical polynomial-round two-party coin flipping cannot guarantee negligible bias against one faulty party; not a universal impossibility for every functionality. |
| Herklotz, Pollard, Ramanathan, Wickerson, PACMPL 5(OOPSLA), Art. 117 (2021) | https://doi.org/10.1145/3485494 | Behavior-preserving HLS is prior work; functional semantics does not establish physical leakage/fault resistance. |

Author copies consulted by the literature reviewer included
https://www.iacr.org/archive/crypto2000/18800113/18800113.pdf and
https://johnwickerson.github.io/papers/vericert_oopsla21.pdf.
The Waajacu site and its project link were checked for public context. The
paper uses the user's requested name, email and website directly. It does
not claim that the current local artifacts have been uploaded or published.

## Current claim boundary

No existing scientific claim is strengthened. Proposed work targets computational
privacy and correctness for explicitly selected relations and adversary/setup
models, allowing abort where needed. No general fairness, guaranteed delivery,
input truthfulness, production readiness, private TTT proof, hardware leakage
resistance, or peace guarantee is established by this paper.

## Required work report

### 1. Files read

AGENTS.md; research/RESEARCH_CHARTER.md; README.md; .gitignore;
Makefile.config; document/adversarial_cooperation.tex and
document/adversarial_cooperation_short.tex; document/references.bib;
research/ROADMAP.md, DECISIONS.md, CLAIM_LEDGER.md and relevant assumption
records; short-book/companion, TTT canonical-reference, private-proof and
binding-route plans. Relevant full/short chapters: introduction, defining
victory, RPS, TTT, private optimization, obstacle avoidance and AST/HLS.
Reviewers compared applicable .tex/.bak pairs and checked the C protocol,
commitment, TTT and circuit implementations, interfaces and adversarial tests.
Build reading included setup.sh, container/tasks.sh, container configuration,
src/README.md, source/tests/demonstrations Makefiles and book-entry conventions.
Also read IEEEtran notices, class/bibliography versions, original-paper metadata,
baseline manifests, and final LaTeX/PDF evidence.

### 2. Files changed

Only new paths were added:

- document/white-paper/adversarial-cooperation-white-paper.tex
- document/white-paper/references.bib
- document/white-paper/build.sh, README.md and .gitignore
- document/white-paper/vendor/PROVENANCE.md and the 36 unmodified upstream files
- output/pdf/Adversarial-Cooperation-White-Paper.pdf
- output/pdf/Adversarial-Cooperation-White-Paper-Source.zip
- research/plans/adversarial-cooperation-white-paper.md (this plan/report)

Generated scratch and QA records are in ignored .temp/white-paper/; baseline
artifacts are in ignored .container-output/white-paper-baseline-20260911/.
No existing tracked file, backup, cryptographic dependency, public C API,
book chapter, Git config, ownership, commit or remote was changed.

### 3. Claims introduced, strengthened, weakened, or removed

Introduced an explicitly proposed research agenda and three funding milestones,
a schematic shared-policy relation inherited from the existing TTT contract,
and a bounded account of current educational implementation status. Introduced
no theorem, new construction result, benchmark, external review claim or novelty
claim. No existing scientific claim was strengthened, weakened or removed.
The hardware path is an agenda, not a working synthesis pipeline.

### 4. Assumptions made explicit

The proposed first model has two parties, at most one static corruption by a
probabilistic polynomial-time adversary, authenticated communication and
permission to abort. Channel secrecy, common parameters and their provenance
are construction-dependent and still to be selected. Computational privacy
and correctness are prospective targets. Truthful input, beneficial incentives,
fairness, guaranteed output, physical device integrity and chronology do not
follow automatically. The relation's semantic and commitment checks share
the same canonical policy witness. Software/hardware translations need explicit
behavioral contracts; physical leakage and faults require separate treatment.

### 5. Commands and tests run

- Bounded git status, rev-parse and diff --check diagnostics.
- Inspect/start the existing project container; check installed tools and setup.
- The repository verify command recorded above.
- Download/extract the unmodified CTAN IEEEtran distribution and compare hashes.
- docker exec adversarial-cooperation-dev /bin/bash
  document/white-paper/build.sh /workspace/.temp/white-paper/build
- pdfinfo; Poppler pdftoppm rendering; pypdf/pdfplumber structural, link,
  metadata, font, bounds and extraction checks; two-page visual inspection.
- ZIP CRC and byte comparisons, followed by an extracted-source rebuild.

No full C rerun was needed after the baseline because this isolated document
adds no code, test, source dependency or existing build changes. The final
paper's own build, references and packaging were checked after its last edit.

### 6. Results and remaining failures

The PDF is two pages and all scientific drafting-review findings were resolved.
The baseline and final paper checks passed. The sole remaining LaTeX diagnostic
is the visually inspected harmless underfull paragraph noted above. Private
proofs and hardware results remain absent by design, rather than failed promised
deliverables of this writing task. Final packaging completion is recorded below.

### 7. Known limitations

No proof of privacy, knowledge extraction, fairness, delivery, all-policy circuit
equivalence, production suitability or physical leakage resistance is newly
established. The application beyond the finite-game example still needs a
precise functionality. This is IEEE conference formatting, not a submitted or
accepted IEEE paper. The focused bibliography does not settle novelty. The
source package includes the paper and formatter, not the whole cryptography
repository or a complete offline operating-system/TeX installation.

### 8. Next smallest coherent step

Freeze and independently review the complete committed-policy relation and
jointly select its commitment profile and proof representation, using existing
cost evidence. Produce one measured private-proof demonstrator before expanding
to a consequential application or a hardware claim.

## Original artifacts (2026-09-11)

- PDF SHA-256:
  3f35ce7920ce6ff0b4ec749dfe02fbb1772fbfeae5dc42f3228e2f5e784fa4e3
- Source ZIP SHA-256:
  57fabc347811938598d93c4bafe2901f465a597f3e1d49d93fafb238c3118d5a
- Source ZIP: 43 files, 1,839,940 bytes. Final PDF: 99,992 bytes.
- QA details: .temp/white-paper/qa-report.json.

### Final packaging and environment verification

The extracted 43-file archive rebuilt successfully with the installed toolchain
and no network fetch. Its two pages have identical extracted text and identical
page drawing streams to the delivered PDF; differing build-time metadata is
expected. All references remain resolved. Final git diff --check passed, and
status shows only the three new scopes listed above.

After verifying that the container held only docker-init and its documented
sleep process, it was returned to its originally stopped state. Its immutable
ID, pinned image and existing mounts/volume were rechecked and preserved. No
container, image, volume, user work, or Git data was deleted. No work remains
for the requested white paper; future research is described separately above.

## Author revision, 2026-09-12

### Requested changes and decisions

All six PDF comments are in scope: remove the subtitle and affiliation line;
simplify the abstract around referee-free cryptographic protocols for classical
and modern game-theoretic problems and participants' hardware; replace the
high-level-synthesis keyword with hardware; turn the final section into an
explicit request for funding, collaborators, computing and hardware resources;
and center the introduction on strategic privacy with the author's poker/chess
images. The chess image is retained as Author's intuition with its functionality
explicitly open. The poker question preserves winning-hand verification without
disclosure from the original chapter backup, rather than substituting ordinary
private dealing. No chapter or C implementation changes are in scope.

The aspiration to share no information is made technically precise as no
additional private disclosure beyond agreed outputs/public information under
the chosen model. This does not promise zero communication or preservation of
every strategic advantage. Outputs, repeated queries, costs and aborts matter.

### Baseline and preservation

The pre-revision PDF, source ZIP, paper source and plan are preserved in
.temp/white-paper/revision-20260912/before/. The prior PDF QA passed again.
Git status shows only the original white-paper additions; existing tracked
files are unchanged. The existing container was inspected: same immutable ID,
image, command, mounts, home volume, loopback port and no-restart configuration,
initially stopped. No new container or dependency is needed.

### Validation and work report

1. **Files read:** current paper, README and build helper; charter and existing
   plan; poker chapter, its original .bak and short counterpart; reviewer read
   targeted poker C source; original PDF QA, packing helper, container config.
2. **Files changed:** white-paper .tex and README; output PDF and source ZIP;
   this execution plan. Scratch QA/packaging scripts were adapted in .temp/.
   Bibliography, vendor, books, C sources and build helper remain unchanged.
3. **Claims:** no theorem, novelty or implementation claim added. Introduction
   now centers on strategic privacy, with poker winner verification as an open
   question and chess as an explicitly undefined author's intuition. The generic
   joint-feasibility example was removed from this short paper. The three-step
   funding outline was replaced with a direct request for specific resources.
4. **Assumptions:** the existing computational, corruption, authentication and
   abort model remains. The revised privacy aspiration explicitly allows agreed
   outputs/public information; privacy alone does not guarantee unchanged
   strategic advantage. No budget, promised funder or funding commitment added.
5. **Commands/checks:** bounded git status/diff --check; initial PDF QA; inspect
   and start the existing container; run document/white-paper/build.sh with output
   in .temp/white-paper/revision-20260912/build; pdfinfo; Poppler rendering;
   pypdf/pdfplumber checks; archive CRC and member byte comparison; archive rebuild.
6. **Results:** independent scientific drafting review found no substantive issue.
   Two pages; all seven fonts embedded Type 1; six references resolved; email,
   website and DOI links correct. Removed subtitle and affiliation verified absent;
   hardware keyword and new support request present. Both rendered pages inspected
   with no clipping, overlaps or broken glyphs. One unchanged underfull paragraph
   diagnostic (badness 1810) remains visually acceptable. No unresolved LaTeX
   references, overfull boxes, or build errors. The initial literal extraction
   assertion for the small-cap heading was corrected to normalize whitespace;
   the heading itself rendered correctly throughout.
7. **Limitations:** poker winner verification, a formal meaning for the chess
   intuition, private-policy proofs and hardware remain research objectives.
   No cryptographic or physical-device guarantee is established by this edit.
8. **Next smallest step:** author review of the clarified research framing;
   the next technical step remains selecting the complete commitment/proof
   relation and building one measured private-proof demonstrator.

The existing 72-group software baseline was not rerun: no implementation or
software dependency changed. The affected artifact was rebuilt and checked.

Current PDF SHA-256:
1331d8d662f6d4a45bf062fbb39440f2add3db9c2ed5f73d47aef9a407ddb9bc

Current PDF size: 98,447 bytes. Detailed QA:
.temp/white-paper/revision-20260912/qa-report.json.

Current source ZIP SHA-256:
d011939dea502a47c5dcd5b1a8c06f88d45ecdac7f01f970785a944c6b696875

The refreshed ZIP has 43 files and is 1,838,857 bytes. Its independent rebuild
passed; extracted text and page drawing streams exactly match the delivered
two-page PDF. Final diff --check passed. The existing container was confirmed
idle and returned to its original stopped state with the same immutable ID.
All six comments and the affected artifact checks are complete.

## Chess clarification, 2026-09-12

The author clarified that public play can expose strategic information, so
parties with algorithmically defined play may prefer to determine the matchup
outcome without playing publicly. Replace the formerly undefined intuition
with this proposed private computation, preserving the exact chess image.
The bounded formulation uses fixed deterministic move-selection algorithms,
an agreed initial position and rules, explicit computation bounds and
termination, and an outcome of win/loss/draw. Algorithms and move trace are
intended to remain private under the paper's stated model. Internal simulation
may still be necessary. No shortcut, optimal-play claim, actual private chess
implementation or new proof is asserted. Randomized strategies would require
an explicit treatment of their randomness; that extension is not specified here.

Files in scope: paper .tex, accompanying README, final PDF/source ZIP, and
this plan. Books, bibliography and C code are unchanged. Previous PDF, ZIP and
source are preserved in .temp/white-paper/chess-clarification-20260912/before/.
Existing paper/build/QA/packing sources and current plan were read; the charter
and scientific boundaries read earlier in this task remain applicable.

### Work report and validation

- **Read:** current paper, README, execution plan, established build and QA/
  packaging helpers, original PDF evidence and inspected container configuration.
  The prior charter, chapter/C and bibliography review remains applicable.
- **Changed:** the chess paragraph in the paper, its README description, this
  plan, final PDF and source ZIP. Scratch helpers were copied/adapted for this
  revision. No book, bibliography, C implementation or build helper changed.
- **Claim change:** replaced an undefined author intuition with a proposed
  computation of the outcome for two submitted algorithms. No theorem, novelty,
  efficiency, general chess-solving or implementation result was introduced.
- **Assumptions:** fixed deterministic algorithms, agreed initial state, draw/
  legality rules and handling of computation-limit violations. Output is on
  completion, preserving the existing abort boundary; hiding is relative to
  what the outcome implies. The model still needs a complete formal specification.
- **Review:** an independent agent checked determinism, termination, draws,
  leakage and abort; its suggestions on illegal moves, execution limits and
  output on completion were incorporated. This is a drafting review, not a proof.
- **Commands/tests:** bounded git status/diff --check; inspect/start existing
  container; document/white-paper/build.sh targeting the revision build directory;
  pdftoppm; pypdf/pdfplumber assertions; visual inspection of both pages; ZIP CRC
  and byte comparison of all 43 members. No C rerun or further archive rebuild
  was needed for this paragraph-only edit; the build/packaging infrastructure was
  already verified and the refreshed archive exactly contains the checked files.
- **Results:** two pages, six resolved citations, seven embedded Type 1 fonts,
  correct author/title and all links. No clipping, overlap, overfull box or
  unresolved reference. One existing, visually acceptable underfull paragraph
  notice remains (badness 1810). A text assertion initially encountered the
  geometric extractor interleaving columns; source-order PDF extraction confirms
  the correct complete passage. No unresolved task failure remains.
- **Research limits/next step:** privately evaluating this bounded matchup is
  still proposed; internal simulation may be necessary and no cryptographic
  realization has been proved or implemented here. The next smallest chess
  research step is to specify the algorithm interface, outcome/limit rules and
  leakage precisely, before choosing an implementation. The existing TTT
  demonstrator remains the current technical foundation.

The prior stopped container was inspected, reused, confirmed idle and stopped
again, preserving immutable ID 087f758ebdc8c512223a1414e871d84b3850a6491483b390900b12b2dc661738.

Latest PDF SHA-256:
43503bddaca00f285e612695a615d3990a5c9018b998bc0fb96da443e5d2c00d

Latest source ZIP SHA-256:
c420b0fa42c0c4cbb78e41ed67bcf73142f58adce522bc0434f400f369615b0f

Source ZIP: 43 files, 1,839,471 bytes. Detailed QA is preserved in
.temp/white-paper/chess-clarification-20260912/qa-report.json.

## Participant hardware and output release, 2026-09-12

The author requests replacing the referee comparison in Section II with a
requirement that each participant independently conclude the algorithm is
acceptable and fabricate its own trusted hardware. The author also clarifies
that early result disclosure followed by refusal is precisely the behavior the
research aims to avoid. Treat it as an objective to construct and demonstrate,
rather than the organizing message of a limitations paragraph.

Implementation of the editorial request: name privacy/correctness and their
evidence as the acceptance criteria; define local hardware trust in terms of
execution and protection of local secrets; require explicit justification for
reliance on peer hardware; replace permission to abort with an adversary able
to stop/delay communication; and state coordinated result-release protection
as a research objective. Align Section IV with participant fabrication and the
need to connect inspected designs to fabricated devices. No new protocol or
hardware guarantee is asserted.

The specific Cleve impossibility paragraph is removed from this short paper,
not contradicted. Its exact prior-art result and scope remain recorded in the
literature table above and in the unused bibliography entry. Independent agent
review recommended this scope-preserving move. The article now cites five
sources. No implication that local device fabrication alone overcomes fair-
release limits is introduced.

Prior output PDF, source ZIP and .tex are preserved in
.temp/white-paper/participant-hardware-20260912/before/. Same uncommitted scope
and existing stopped container were inspected.

### Required work report

1. **Read:** current white-paper source, README, active plan, established QA and
   packaging helpers, and existing container configuration. Prior charter,
   chapter/C and bibliography evidence remains applicable to unchanged material.
2. **Changed:** paper Sections II and IV, README, this plan, final PDF and source
   ZIP. Section II's heading now says Requirements; its model paragraph was
   tightened without altering the adversary to avoid splitting a word at the
   page boundary. Scratch validation helpers were adapted. No C, book, vendor,
   bibliography file, dependency or build-helper change.
3. **Claims:** participant assessment/fabrication and coordinated release are
   proposed requirements/objectives. The former limitations framing and referee
   comparison were removed as requested. No claim that these goals are already
   achieved or that independent fabrication guarantees peer-device behavior.
4. **Assumptions:** privacy and correctness evidence under the stated adversary
   model; local hardware execution/secret protection; fabrication and physical
   access; separately justified reliance on peer devices. Static one-party
   corruption and message stopping/delay remain explicit. Output-release
   assumptions/mechanism still need to be specified and demonstrated.
5. **Checks:** bounded git status/diff --check; inspect/start existing container;
   IEEE latexmk build through document/white-paper/build.sh; pdftoppm rendering;
   pypdf/pdfplumber metadata, citation, hyperlink, font, page-count and bounding
   checks; manual inspection of both final pages; ZIP CRC/member byte checks.
   The existing software baseline was not rerun because no implementation or
   build dependency changed. The unchanged archive build process was not rerun;
   all refreshed archive files exactly match the checked source and PDF.
6. **Results:** focused independent review found no substantive issue. Two
   pages, five resolved references, seven embedded Type 1 fonts, valid email,
   website and DOI links. No broken glyphs, clipping, overlaps, overfull boxes,
   or unresolved references. Existing underfull paragraph notice (badness 1810)
   remains visually acceptable. No task failure remains.
7. **Limitations:** intended participant-controlled hardware and protection
   against premature result access/withdrawal remain proposed research, with
   no new proof, implementation or physical assurance result.
8. **Next smallest step:** define one concrete release functionality and the
   exact device/communication assumptions on which its desired protection
   would depend, before designing or claiming an enforcing mechanism.

Latest PDF SHA-256:
2bee0c3af0b8cde5d9f3e1b409328d7f1cce0388b01965b8c66fdf1b9cf71ceb

Detailed QA: .temp/white-paper/participant-hardware-20260912/qa-report.json.

Latest source ZIP SHA-256:
85b446dc555a2e0bc252a4cb9a31b293b01875c166bfddd8de898bd8fb9d985d

The refreshed ZIP has 43 files and is 1,839,096 bytes. The existing container
was confirmed idle and returned to its originally stopped state with its same
immutable ID and preserved data. Both comments and all affected checks are done.

## Research-proposal designation and root delivery (2026-09-12)

The author requested that this beginning-stage research document be designated
a research proposal and that the PDF be moved out of output/pdf so it is easy
to find. Current delivery is:

- `Adversarial-Cooperation-Research-Proposal.pdf` at the repository root.
- `Adversarial-Cooperation-Research-Proposal-Source.zip` beside it.
- Editable source under `document/research-proposal/`, with the main file
  `adversarial-cooperation-research-proposal.tex`.

The root README links these artifacts before the license section. This record
supersedes the original delivery paths and designation above; earlier records
and this plan's historical filename are retained. Authorized moves RN-018
through RN-021 were documented before execution. All affected source/output
paths were untracked, so native PowerShell moves were used after resolving
and checking sources and destinations within the workspace. No existing
destination was overwritten during the moves; original artifact and source
copies remain in `.temp/white-paper/proposal-relocation-20260912/before/`.

### Required work report

1. **Read:** charter, current proposal source, source/root READMEs, build helper,
   rename map, this plan, QA/packaging helpers, and existing container details.
   Prior chapter/C/bibliography evidence remains applicable. Independent
   read-only review confirmed all active naming references and that the
   scientific prose already distinguishes proposed work from established results.
2. **Changed:** the source folder and main-file names, visible document label
   and PDF subject metadata, build-helper input filename, source/root READMEs,
   rename map, this plan, and the renamed/rebuilt root PDF and source ZIP.
   Scratch QA helpers were adapted. Vendor, bibliography, book and C content
   are unchanged. Historical provenance and baseline paths remain valid.
3. **Claims:** no scientific claim introduced, strengthened, weakened or removed.
   A source comparison established that the only manuscript edits replace
   `Research white paper` with `Research proposal` in the label and metadata.
4. **Assumptions:** none added. The document remains a proposal for research;
   intended private proofs, output-release protection and hardware properties
   are not newly established by this editorial task.
5. **Checks:** bounded git status/diff --check; path and destination validation;
   IEEE latexmk build through the renamed helper; pdftoppm rendering and visual
   inspection of both pages; pypdf/pdfplumber page, metadata, link, font,
   content-bound and citation checks; byte comparison of all 36 upstream
   vendor files; ZIP CRC/member checks; independent build of extracted source;
   PDF text and page drawing-stream comparison; root README link resolution
   and absence of files at their former delivery paths. Existing software tests
   were not rerun because software and its build dependencies did not change.
6. **Results:** two US Letter pages, five resolved references, seven embedded
   Type 1 fonts, and all seven distinct email/site/DOI links retained. No
   unresolved references, overfull boxes, clipping, overlaps or broken glyphs.
   The existing underfull paragraph notice (badness 1810) remains visually
   acceptable. Extracted source rebuilt with identical text and page drawing
   streams. The refreshed ZIP contains 43 byte-checked members. The existing
   container was inspected, reused, confirmed idle, and returned to its
   originally stopped state with unchanged immutable ID. No task failure remains.
7. **Limitations:** no proof or implementation result follows from relabeling,
   formatting, rendering, or an offline source rebuild. The proposal's stated
   research limitations are unchanged.
8. **Next smallest step:** the root PDF is ready for the author's review and
   sharing. The technical next step remains specifying one concrete release
   functionality and its device/communication assumptions.

Current PDF SHA-256:
`e1a682d86137c4d22f7f3fbc801deb8a5f52cd2cde1e110b1641d86bb1a7922b`

Current source ZIP SHA-256:
`9fd676d67d27b6f1c96715ea97d1bd0a79748cb4a9ff7e08233b8d33a5f9864f`

PDF size: 98,200 bytes. ZIP size: 1,839,239 bytes. Detailed validation reports:
`.temp/white-paper/proposal-relocation-20260912/qa-report.json` and
`.temp/white-paper/proposal-relocation-20260912/relocation-check.json`.

## Authorized Git publication (2026-09-12)

The author explicitly requested "commit, and push." The inspected branch is
`master`, tracking `origin/master` at
`https://github.com/savethebeesandseeds/adversarial_cooperation.git`.
After fetching origin, the local and remote heads were aligned. The publication
scope is the root PDF and source archive, editable proposal directory, root
README, rename map, and this report; local scratch and build evidence stay ignored.

Pre-commit review read these files, checked the artifact hashes and README
links, and found one active statement that would become stale on publication.
The source README now describes the implementation baseline and delivered
sources without calling them uncommitted. Its copy in the ZIP was refreshed.
The rename map now records that paths were untracked at the time of their move.
A proposal-local `.gitattributes` preserves LF in the Bash build helper and
the exact vendored IEEEtran bytes across Windows checkouts; this is relevant
because the existing Git configuration has `core.autocrlf=true`.

No scientific claims or assumptions changed. The checked PDF, manuscript,
bibliography and build commands are unchanged; prior visual inspection and
independent archive rebuild remain applicable. The updated ZIP passes CRC and
member-byte checks and includes only the expected 44 files. Its current hash
is recorded below; the PDF hash above remains current. Git staging checks and
the resulting commit/push status are reported in the task. No implementation
tests or scientific results are added by publication. Next step: commit the
reviewed paths, push without force, and verify the remote head and worktree.

Publication source ZIP SHA-256:
`93406b1ad8c7c2bd101ee89d48174fa9f14742a21137f87177ce228103aba150`

Publication source ZIP size: 1,839,388 bytes.
