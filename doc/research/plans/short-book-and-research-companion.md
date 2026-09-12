# Execution Plan: Short Book and Research Companion

## Status

Complete, 2026-09-05. The author requests a short, contemplative book
containing every chapter, with detailed reasoning, algorithms, and executable
demonstrations in a separate expansion. Collaboration on prose is authorized;
authorship attribution is not a gate for this work.

## Problem and intended result

The current 112-page inclusive draft combines brief ethical notes with long
technical developments. Separate the reading experience into two linked
editions, preserving all 22 chapters and five appendices in the same order:

1. A short reading edition: one central insight per chapter, plain language,
   restrained contemplative prose, and a reference to the matching companion.
2. A research companion: the existing expanded sources, algorithms, citations,
   implementation mappings, and explicit open questions. A short source does
   not acquire a rigorous expansion merely by changing the volume's title.

The short book is an editorial first draft, not a translation or imitation of
any Tao scripture. No theological attribution or new technical guarantee is
introduced. Defining Victory retains its central sentence. Brevity must not
turn a conditional result into an unconditional promise.

## Baseline and preservation

- Clean `master` at `6b0483d` before work. Root and `.git` owner are
  `waajacu\\santi`; bounded Git diagnostics use per-command safe.directory
  because the sandbox account differs. No Git configuration changes.
- September 1 evidence: 112-page PDF; all 145 recorded build inputs match the
  current workspace; 72 native tests and the same sanitizer tests recorded as
  passing. A fresh baseline web/manuscript build will precede source edits.
- Existing chapter bodies and all ten manuscript `.bak` files are preserved.
  Their differences were inspected during the preceding repository review;
  backup-only questions remain in the research corpus.
- Existing `document/adversarial_cooperation.tex` remains the expanded entry.
  Its title and reader guide become Research Companion. No source rename.
- New short entry: `document/adversarial_cooperation_short.tex`; its chapter
  sources live under `document/short/`, one source for every companion source.
- Existing public PDF URL `book/Adversarial-Cooperation.pdf` continues to
  identify the expanded volume. New `Adversarial-Cooperation-Short.pdf` is the
  short book. The browser reader offers both and starts with the short book.

## File scope

- New short LaTeX entry, layout, front matter, and 27 short chapter sources.
- Existing expanded entry and draft reading guide: framing changes only.
- `research/EDITION_MAP.md`: complete chapter/source/implementation mapping.
- This plan, charter edition guidance, chapter-matrix reading key, roadmap,
  README, and web README: explain the two editions without rewriting history.
- Build helper, website PDF assembly/reader/tests, and container artifact
  export: build, expose, and verify both editions using existing dependencies.
- No C/API/primitive changes, new proof backend, external deployment, commit,
  source deletion, or dependency installation.

## Work sequence and checks

- [x] Read governing charter, entry point, relevant plans and source/backup
  review, bibliography/build layout, recorded verification, and runtime config.
- [x] Run fresh baseline through the existing reusable container.
- [x] Write short chapter drafts and preserve chapter/appendix correspondence.
- [x] Frame the expanded volume as the research companion; document the map.
- [x] Update build, reader selection, and evidence export for two PDFs.
- [x] Build both editions; run existing browser/bridge checks appropriate to
  changed build and UI code; inspect links, coverage, citations, and logs.
- [x] Render and visually inspect final short pages and changed companion
  pages; check that unchanged companion body text is preserved.
- [x] Review short prose for unsupported claims and fidelity to source ideas.
- [x] Record outcomes, limitations, commands, and next smallest step.

## Environment and scientific boundary

Reuse only inspected container `adversarial-cooperation-dev`, immutable ID
`087f758ebdc8c512223a1414e871d84b3850a6491483b390900b12b2dc661738`,
with the pinned Debian image and mounts documented in README. Starting the
existing environment for necessary compilation is within the requested task.
Do not create or replace containers or volumes. Build in the task runner's
isolated snapshots and export under a new `.container-output` run ID.

No cryptographic claim is strengthened. Current strategy verification is
disclosed, RPS permits selective abort, and poker remains a centralized toy.
The short edition points readers to the companion's exact models and evidence;
its ethical questions do not establish fairness, peace, or institutional justice.

## Verification record

Final bundle: .container-output/two-editions-final-20260905b/.

- The short PDF is 31 A5 pages: cover, reading note, two contents pages, and
  one page for each of 22 chapters and five appendices.
- The Research Companion is 112 pages. Every original chapter body and all
  backups are untouched. Extracted text for all 104 body pages from Part I
  through the bibliography matches the fresh baseline exactly.
- All 176 final recorded source inputs match the current workspace by SHA-256.
  All 30 exported artifact hashes verify. The separately exported PDFs match
  the copies in the website byte for byte.
- All 27 short-to-companion PDF links are actual GoToR actions with a matching
  chapter.N or appendix.A-E destination in the companion.
- Every short chapter has the matching title, order, number, appendix boundary,
  limitation, and companion reference. There are no blank short-book pages.
- Stabilized logs for both PDFs have no Warning, Overfull, Underfull, or TeX
  error matches. Initial title-page duplicate anchors and a reader-note
  overfull line were corrected.
- Rendered and inspected every short page and the nine companion front pages.
  The final 27 body-page rasters match those reviewed before the contents
  tightening; all 31 final short and nine companion-front rasters match the
  reviewed final-layout candidate exactly. No clipping or overlap remains.
- In-app browser smoke test: default short book loads, companion selection
  loads the 112-page PDF, short selection loads the 31-page PDF, and direct
  links remain visible. A selected-button hover contrast defect was found
  visually, repaired, and rechecked against the final exported site.
- Both whole-book helper targets execute successfully in an isolated snapshot
  and emit their normal aliases plus the public cross-volume filenames.
- The existing container was reused with the same immutable ID and returned
  to its initial stopped state. No container or volume was created or removed.

### Commands and outcomes

- Baseline: AC_RUN_ID=short-book-baseline-20260905a bash container/tasks.sh web.
  Existing PDF, static reader, demo contract/UI, and six native/WASM bridge
  checks passed before manuscript/build edits.
- Final: AC_RUN_ID=two-editions-final-20260905b bash container/tasks.sh verify.
  All 72 native test groups and the same 72 under AddressSanitizer and
  UndefinedBehaviorSanitizer passed. Both PDFs, coverage, artifact identity,
  static reader, demo contract/UI, and all six native/WASM bridge checks pass.
- bash -n book container/tasks.sh; node --check for changed/new browser modules;
  git diff --check: passed. Git emits only its ordinary LF/CRLF conversion
  notices; no repository Git configuration was changed.
- bash book -pdf short_book and bash book -pdf research_companion, with
  BOOK_OUT_DIR set to a fresh isolated helper directory: passed.
- Python/pypdf: chapter destinations, per-page body preservation, PDF export
  identity, page counts, and all input/artifact SHA-256 checks: passed.
- Poppler pdftoppm at 95 DPI, image inspection, and raster identity comparisons:
  passed. QA records are under .temp/edition-qa/; final artifact evidence is
  in the write-once bundle above.

### Discoveries and decisions

- Hyperref treats a local file.pdf fragment as a GoToR destination, not as a
  browser URL parameter. The initial fragment nameddest=chapter.1 produced
  a nonexistent destination. The final fragment is chapter.1 directly, and
  the actual emitted PDF actions were independently checked.
- The short layout uses the already-installed mathpazo font package. No
  package or production dependency was added.
- The small table of contents was tightened to avoid a final page containing
  only the last appendix entry.
- Shortening was checked for semantic drift: the original oracle intuition
  remains separate from the narrower timing demonstration; current strategy
  verification is disclosed; secret minimization concerns protocol material;
  the private-warning note retains unresolved roles; public rules remain
  distinct from identifying the decisive rule.
- A temporary QA script was renamed because inspect.py shadowed Python's
  standard-library inspect module. Its subsequent checks passed; this was
  not a product or dependency defect.

## Required work report

### Files read

The governing charter and repository instructions; all 27 companion sources
and their available manuscript backups; main entry, reader guide, text/math
utilities and bibliography; chapter matrix and relevant claim/open-problem
records; complete-draft, canonical-TTT, private-proof, reader and container
plans; README and source/web guidance; source/test/demonstration Makefiles,
book and compile_latex.sh; container setup/tasks/security definitions; relevant
C interfaces, teaching warnings and hash implementation/tests; browser build,
reader, tests and server; baseline and final verification manifests/logs.

### Files changed

- Added document/adversarial_cooperation_short.tex and document/short/:
  layout, reading note, and 27 separate short chapter sources.
- Changed only title/front-matter framing in the existing companion entry
  and document/content/research_draft_status.tex.
- Added research/EDITION_MAP.md and this plan; updated edition guidance in
  RESEARCH_CHARTER.md, ROADMAP.md, CHAPTER_MATRIX.md and README.md.
- Updated book, container/tasks.sh, web/README.md, web build/site/UI tests,
  app/index/styles; added web/src/book-editions.mjs.
- No C source, public API, primitive, bibliography entry, existing chapter
  body, backup, setup dependency, or Git history changed.

### Claims and assumptions

Added brief editorial condensations, ethical interpretations and questions.
No existing technical claim is strengthened; no new theorem, proof, protocol,
experiment result beyond the enumerated execution checks, or novelty assertion
is introduced. The two editions share chapter order and reference numbers.
Cross-PDF links assume the two public filenames remain together and a reader
supports remote PDF links; printed chapter numbers provide a fallback.

### Results and remaining limitations

The separation and build/reader integration are complete and verified.
The short text is a first reading draft. Several companion chapters are still
research notes or placeholders. Private strategy proofs, RPS fairness against
selective abort, distributed poker and collusion resistance, and institutional
outcomes remain as bounded or open in the existing companion. Passing the
software tests does not prove cryptographic hardness or ethical objectives.

### Next smallest coherent step

Read and refine Defining Victory as a short chapter, then choose one concrete
companion problem with an explicit meaning of victory and a non-cryptographic
baseline. Develop its model, attacks, and demonstration as one bounded slice
while keeping the brief chapter readable.

