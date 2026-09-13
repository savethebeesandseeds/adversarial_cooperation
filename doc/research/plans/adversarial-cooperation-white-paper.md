# Adversarial Cooperation: evolving white paper

Status: one-page condensation rebuilt and visually verified, 2026-09-13.
The document remains a research proposal in its text. All earlier edits remain
applied. The revision records are retained; the latest record at the end
supersedes earlier delivery paths and naming choices.

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

## White-paper filenames (2026-09-13)

The author requested a stable white-paper filename as the document develops,
avoiding confusion between `doc/research/` and `doc/research-proposal/`.
Use `doc/white-paper/` for its editable sources and retain the reader-facing
PDF directly in `doc/` as `Adversarial-Cooperation-White-Paper.pdf`.
The exact sequential moves are RN-022 through RN-024 in `../RENAME_MAP.md`.

Scope: rename the tracked folder, TeX entry and existing PDF; update the build
helper, navigation and current path guidance. Keep the manuscript, bibliography,
vendor distribution, existing generated objects and historical evidence intact.
The internal research-proposal designation remains appropriate to the current
stage and is unchanged. No cryptographic or editorial revision is inferred.

Baseline: clean Git worktree at `070f368`. Read the charter, current manuscript
and bibliography, its build helper and README, the root/doc/research navigation,
repository layout/rename records and the relevant proposal/organization plans.
Capture hashes before moving, validate resolved paths stay within this workspace,
and use Git renames. Check byte preservation, local links, active path references,
Bash syntax and the helper's path/argument routing without invoking TeX. The
author's no-compilation, no-Docker and no-Windows-installation instructions remain
in effect. No new commit or push is part of this naming request.

### Required work report

1. **Read:** the instructions, charter, manuscript, bibliography, build helper,
   navigation and relevant plans listed above. An independent path audit also
   checked for affected container, CI, code and web references; none were found.
2. **Changed:** the three Git renames RN-022 through RN-024; root, document,
   research and white-paper READMEs; repository layout and rename guides;
   the white-paper build helper's source/output paths; and this plan.
   `doc/research/` continues to hold the broader program's working records.
3. **Claims:** none introduced, strengthened, weakened or removed. Manuscript,
   bibliography, PDF and vendor bytes are unchanged; scientific text was not
   revised as part of the filesystem naming change.
4. **Assumptions:** `white-paper` is the stable filename for an evolving document.
   Its internal designation remains research proposal. No new cryptographic,
   hardware, dependency or deployment assumption was introduced.
5. **Commands/checks:** Git status/diff checks; pre-move SHA-256 inventory;
   three workspace-bounded `git mv` operations; immediate verification of all
   44 moved files; final preservation checks; resolution of 38 local Markdown
   links; old active-path search; Bash `-n` before and after; argument-recording
   checks of the build helper's default and custom output paths. In those checks
   a shell function replaced `latexmk`; no TeX process or compiler ran.
6. **Results:** all 44 files were unchanged immediately after moving. Following
   the deliberate README/build-helper edits, the remaining 42 files still
   match their original hashes, including all 36 IEEEtran distribution files.
   Links, Bash syntax and both output-path checks pass. The first routing check
   expected one extra recorded field; correcting that check's count resolved
   the failure without changing the helper. Old active file locations are absent.
7. **Limits:** no compilation, PDF regeneration, Docker operation or installation
   was performed. A real build from the new paths remains unverified under the
   author's standing instruction. This task establishes no additional protocol
   property; the manuscript's stated research limitations remain unchanged.
8. **Next smallest step:** resume editing this evolving document at the new
   source path. The technical next step remains specifying one concrete output
   release functionality and its device/communication assumptions. Commit and
   push require a new publication request for this naming change.

Local naming evidence: `.local/white-paper-naming-20260913/before.json` and
`checks.json`. Existing generated artifacts and historical maps are unchanged.
Preserved PDF SHA-256:
`e1a682d86137c4d22f7f3fbc801deb8a5f52cd2cde1e110b1641d86bb1a7922b`.

## Conservative scope revision (2026-09-13)

The author requests five bounded revisions: simplify Section I's privacy
paragraph; center Section II on two parties with their own inspectable protocol
implementations in equivalent hardware, leaving concrete models to later
protocol work; shorten Tic-Tac-Toe to an example and identify elemental-game
composition as an open research question; present C/circuits/RTL/FPGA work as
an intended research direction; and aim for a complete inspectable demonstrator
for each selected problem without claiming unestablished proofs or composition.

Preserve the five sections, professional tone, author details, existing
citations and prior chess/poker motivation. Keep existing uncommitted naming
changes. Read the current manuscript, bibliography, charter, plan, source README
and build helper; independently check the TTT reference contract and C sources
against the compressed implementation description. Snapshot the current source,
README and PDF in `.local/white-paper-revision-20260913/before/` before editing.

Revise the requested paragraphs, review scientific scope independently, and
check LaTeX structure, citation keys and the diff. The earlier Docker/build
restriction remains in force pending the author's response to a clarification;
no TeX engine was found on the current Windows PATH. Complete the text work
while that PDF-build question is pending. Do not install software or rebuild
C objects. If the PDF remains pending, identify its older content explicitly.

### Required work report

1. **Read:** current source, bibliography, charter, proposal plan, build helper,
   navigation/README, TTT source documentation and canonical reference contract.
   An independent reviewer inspected the associated TTT C source and headers
   and compared the complete revised manuscript with the preserved original.
2. **Changed:** the requested Section I privacy paragraph, Section II scope,
   Section III example, Section IV research direction and Section V demonstrator
   aim. Updated the white-paper README and root/doc navigation to disclose that
   the source is newer than the PDF, and maintained this plan. Earlier uncommitted
   filename changes remain intact; no code, bibliography or vendor files changed.
3. **Claims:** removed proposal-level commitments to authenticated channels,
   static single-party corruption, an ideal-functionality formalism and a selected
   output-release construction. Kept privacy and correctness as requirements for
   later protocols and withdrawal protection as a research objective. Removed the
   TTT displayed relation and chronology discussion while retaining the same-policy
   requirement and present implementation limits. Introduced elemental-game
   composition explicitly as an open research question. Hardware realization,
   translation correctness and complete demonstrators are intended work, not
   established proofs or general composability results.
4. **Assumptions:** the intended physical arrangement is two parties with their
   own inspectable implementations in equivalent hardware. Equivalence is to be
   assessed against the common specification and each party's role; inspecting
   one's own implementation does not establish peer compliance. Exact adversary,
   technical and physical assumptions are to be justified for each protocol.
5. **Commands/checks:** Git scope/diff checks, before-file preservation, a Python
   source comparison checking all five section titles, unchanged opening/author/
   abstract/chess text and final contact/ethical paragraph, matching citation keys,
   balanced LaTeX braces and environments, required content, section word counts
   and unchanged PDF bytes. Independent editorial and cryptographic review found
   no substantive issues. No compiler, Docker action or installation was run.
6. **Results:** source checks pass; the same five bibliography entries remain
   cited. The TTT section decreased from 317 to 155 whitespace-delimited words;
   Section II decreased from 307 to 195. All five author requests are addressed
   in the source. The existing PDF retains its earlier content and is explicitly
   marked as such in navigation and source documentation.
7. **Limits:** no local TeX engine was found on PATH. The earlier instruction to
   avoid Docker/recompilation remains in effect pending the author's answer to
   the build clarification. Source checks do not establish successful typesetting
   or visual layout. No cryptographic proof, hardware result or composition
   guarantee is established by this editorial revision.
8. **Next smallest step:** when the build restriction is clarified and the
   existing environment is available, rebuild only this proposal with its current
   helper, check citations/fonts/layout on every page, and replace the reader-facing
   PDF after review. No C objects, package installs, new container or source ZIP
   are needed. No commit or push was performed for this revision.

Evidence: `.local/white-paper-revision-20260913/source-checks.json` and `before/`.

## Authorized PDF rebuild and standing instruction (2026-09-13)

The author clarified: "yes, on changes to the pdf always rebuild it".
This authorizes using the existing project container for proposal rebuilds and
supersedes the cleanup-specific restriction for this purpose. Every subsequent
proposal revision includes rebuilding, rendering, inspecting and updating its
reader-facing PDF. This instruction is recorded in AGENTS.md and the document
READMEs so it persists beyond this task.

### Required work report

1. **Read:** current manuscript/build helper, source/root/doc READMEs, this plan,
   container README and setup definition, PDF skill and installed runtime paths;
   inspected the existing container and named volume before reuse.
2. **Changed:** rebuilt `doc/Adversarial-Cooperation-White-Paper.pdf`; removed
   stale PDF-pending notices; recorded the standing rebuild instruction in
   AGENTS.md, document guides and this report. Manuscript text, bibliography,
   C code, vendor distribution and existing uncommitted renames are preserved.
3. **Claims:** no scientific claims introduced or altered by this rebuild.
   The PDF now represents the five source revisions documented above.
4. **Assumptions:** the author's PDF-rebuild authorization is standing.
   Reused the inspected `adversarial-cooperation-dev` container, immutable ID
   `087f758ebdc8c512223a1414e871d84b3850a6491483b390900b12b2dc661738`, with its
   existing base image, configuration, workspace/setup mounts and home volume.
5. **Commands/checks:** existing `doc/white-paper/build.sh` through `docker exec`
   with output in `.local/white-paper-revision-20260913/build`; latexmk/pdfLaTeX/
   BibTeX; final-log checks; Poppler rendering and visual inspection of both
   pages; pypdf/pdfplumber metadata, font embedding, hyperlinks and page bounds;
   independent full-text comparison; copied the reviewed PDF and checked hashes;
   Git whitespace and navigation checks; final container identity/config check.
6. **Results:** two US Letter pages, five resolved references, four embedded
   Type 1 fonts, and seven distinct email/site/DOI links. No undefined references,
   overfull boxes, clipping, overlap or broken glyphs. The existing underfull
   paragraph notice (badness 1810) remains visually acceptable. `pdffonts` was
   absent from PATH; font descriptors were verified directly with pypdf instead.
   Docker returned mounts in a different array order; comparison by destination
   confirmed identical mount contents. The container was healthy and idle before
   being returned to its original stopped state with unchanged identity/config.
7. **Limits:** typesetting and content checks establish document fidelity,
   not cryptographic proofs, hardware results or composition guarantees.
   No remaining document failure; no Windows installations, C object rebuilds,
   container creation, volume changes, source ZIP, commit or push.
8. **Next smallest step:** review the updated PDF. Future source revisions
   automatically include the same rebuild and visual-check workflow.

Final PDF: 67,614 bytes; SHA-256
`44f434b9852df9e262a1c69253fb89510b0fac64d1443337547966066f1fb3da`.
QA evidence: `.local/white-paper-revision-20260913/pdf-checks.json`, rendered
pages, final TeX log and before/after container inspection records.

## Abstract wording correction (2026-09-13)

1. **Read:** current abstract, build helper, instructions, latest report and
   existing container configuration.
2. **Changed:** "investigates cryptographic protocols for classical" becomes
   "investigates cryptographic solutions to classical" in the abstract;
   rebuilt the reader-facing PDF and added this record.
3. **Claims:** clarified the object of investigation; no solution, proof or
   result is newly claimed. All other manuscript text is unchanged.
4. **Assumptions:** none changed; standing PDF-rebuild authorization applies.
5. **Checks:** exact source replacement comparison; existing latexmk build;
   Poppler rendering and inspection of both pages; embedded-font and final-log
   checks; independent before/after PDF text comparison; delivery hash check;
   container idle/state/configuration verification; Git whitespace check.
6. **Results:** two pages with exactly the requested wording change in extracted
   text; no new layout or reference issue. The existing underfull paragraph
   notice remains visually acceptable. Container restored to its original
   stopped state with unchanged identity, configuration and mounts.
7. **Limits:** no scientific result established; no code build, installation,
   container recreation, commit or push performed.
8. **Next smallest step:** review the updated abstract in the PDF.

Evidence and before snapshots:
`.local/white-paper-revision-20260913/abstract-solutions/`.
Updated PDF: 67,608 bytes; SHA-256
`18c32fcbcae5640367cfeb2fe1efa565b85271726c6306ebabc68d0f22efe671`.

## Author's corrected opening (2026-09-13)

1. **Read:** current abstract, build helper, latest work record and inspected
   existing container configuration.
2. **Changed:** the opening now reads "Adversarial Cooperation derives solutions
   to classical and modern problems in game theory using cryptographic protocols
   without a trusted referee, and investigates hardware for their execution."
   Rebuilt the PDF and added this record. All remaining manuscript text and
   final layout commands are unchanged.
3. **Claims:** adopted the author's requested description of the program as
   deriving solutions using protocols. This edit adds no particular construction,
   theorem or proof; the existing research-status qualifications remain.
4. **Assumptions:** no technical assumptions changed; PDF rebuilding remains
   authorized under the standing instruction.
5. **Checks:** source comparison; existing latexmk build; Poppler renders and
   inspection of both pages; PDF text/font/size/log checks and delivery hash;
   independent review of the requested opening; container restoration and Git
   whitespace checks.
6. **Results:** two pages with the requested wording, embedded fonts, resolved
   citations and no overfull boxes. The first expanded hardware clause caused
   a column-balancing notice; concise equivalent wording resolved it. Trial
   layout adjustments were reverted. The known underfull paragraph notice is
   visually acceptable. Container restored stopped with unchanged configuration.
7. **Limits:** no scientific result established by this edit; no code build,
   installation, container recreation, commit or push.
8. **Next smallest step:** review the corrected opening in the updated PDF.

Evidence: `.local/white-paper-revision-20260913/abstract-derives/`.
PDF: 67,616 bytes; SHA-256
`1a6007786b2e446b6b31be9a3d4aab3b7274c8b67c861f501c8ee53bc33f0ffa`.

## One-page condensation (2026-09-13)

The author first removed the Scope and Tic-Tac-Toe sections, then requested
that the remaining document fit on exactly one page. Preserve the three
surviving sections, author/contact details, central cooperation question,
poker/chess intuition, inspectable hardware direction and request for support.
Condense prose while retaining IEEEtran's normal typography and geometry.

1. **Read:** current author-edited source, build helper, prior work record,
   instructions and existing container configuration. An independent reviewer
   compared the condensed draft with the preserved pre-edit source.
2. **Changed:** condensed abstract and three surviving sections; moved the
   existing column-balancing command for a one-page document; rebuilt the PDF;
   updated source README status and this record. No deleted section was restored.
3. **Claims:** preserved privacy under explicit assumptions, research status of
   hardware correspondence, and no proof/novelty/general-composability claim.
   The institutional exposure statement is now explicitly an author concern
   rather than an unsupported general finding. No research result was added.
4. **Assumptions:** no new technical assumptions or toolchain commitments.
   IEEEtran class, title/author metadata, 10-point type, Letter size and margins
   are unchanged. Existing Docker environment and PDF-rebuild authorization apply.
5. **Checks:** independent meaning review; latexmk build; one-page count; Poppler
   render and visual inspection; unchanged preamble, sections and citation-key
   checks; embedded fonts, seven URI links and page bounds; delivery hash and
   Git whitespace checks. No code build or installation.
6. **Results:** exactly one page with all three sections and five references.
   Manuscript whitespace-delimited word count fell from 829 to 484, including
   LaTeX commands. Four embedded Type 1 fonts; no overfull boxes, unresolved
   citations, clipping, overlap or broken glyphs. The underfull paragraph notice
   remains visually acceptable. Existing idle container returned to stopped.
7. **Limits:** condensation and typesetting establish no cryptographic or hardware
   result. Detailed models and evidence remain future research. No commit/push.
8. **Next smallest step:** review the one-page document; further edits include
   rebuilding and checking the PDF under the standing instruction.

Before snapshots and QA: `.local/white-paper-revision-20260913/one-page/`.
PDF: 62,323 bytes; SHA-256
`3962ac557b817171438028c4d7cb9e688e1b6d754507893bb62e25274be47f25`.

## Selected games section (2026-09-13)

The author selected exactly six cases for a new Section II: Rock-Paper-Scissors,
Tic-Tac-Toe, Poker, Chess, Stag Hunt, and Joint Optimization. Add one short
explanation per item, preserve the surrounding manuscript, and rebuild only
this white paper using the existing environment. Verify its rendered layout.

- Read the current manuscript, bibliography, build helper, source README,
  research charter, edition map/matrix, relevant game chapters, earlier chess
  clarification, and existing container instructions/configuration.
- Baseline: preserved current source and one-page PDF under
  `.local/white-paper-revision-20260913/games-section/`. The preceding revision
  already established a successful build; retain its typography and prose.
- Add the selected list as research targets, with the composition of elemental
  games/protocols explicitly an open question. Do not claim new protocols,
  implementations, proofs, novelty, or general composability. Existing
  protocol-specific model and assumption requirements still apply.
- Chess retains fixed deterministic algorithms, agreed rules and computation
  bounds. The Stag Hunt entry supplies a proposed coordination motivation;
  its book chapter remains an undefined fragment, not a formal construction.
- Independent review checked the six entries against chapter status and author
  intent. Clarify RPS as verification of a completed round and Poker privacy
  relative to what the agreed outcome implies.
- The author's follow-up removed Section I's intuition quotation and adjacent
  poker/chess explanation, and removed the abstract's classical/modern game
  theory framing. The examples now appear in Section II. Hardware and support
  prose, typography, bibliography and author/contact metadata are unchanged.
- Changed files: white-paper source, delivered PDF, source README status and
  this work record. No code, book chapter, dependency or container definition
  changed. No commit or push.
- Commands/checks: existing container `build.sh` through latexmk/pdfLaTeX and
  BibTeX; `pdfinfo`; `pdftoppm -r 120 -png`; full-page visual inspection;
  pypdf/pdfplumber checks of text, six entries, four sections, five references,
  Letter dimensions, embedded fonts, URI links and page bounds; source diff
  review and SHA-256 equality of reviewed and delivered PDF.
- Results: the initial addition produced two pages; the author's follow-up
  brings the final PDF to one page at unchanged IEEEtran 10pt and margins.
  Five embedded fonts and seven distinct URI links. No unresolved references,
  missing glyphs, clipping, overlap or overfull boxes. The preexisting underfull
  paragraph notice (badness 1810) remains visually acceptable. The initial
  text-extraction check needed whitespace normalization for IEEE small-cap
  headings; the corrected final check passed. Existing idle container restored
  stopped with unchanged identity, configuration and mounts.
- Research limits: these are selected aims, not achieved protocol, privacy,
  equilibrium, hardware or composition results. No new setup assumptions or
  implementation commitments introduced. Next smallest step: author review
  of the six descriptions in the rebuilt proposal.

Final PDF: 71,559 bytes; SHA-256
`817e4fbdec17765e941dfca5b85704b41451e159c3979d56f6a262abcb63e606`.
