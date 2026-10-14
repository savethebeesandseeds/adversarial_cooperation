# Execution Plan: Clarify Manuscript Development Status

## Status

Wording changes and static review complete, 2026-10-14. Book compilation and
visual verification remain blocked by the existing container's port conflict.

## Problem and author direction

On 2026-10-14 the author clarified that most chapters have not been revisited
and consist mainly of chapter names and small notes. The author requested
wording refinements so the documentation reflects that state. An assembled
edition, expanded prose, or a chapter count can otherwise suggest more author
development than has taken place.

## Scope and preservation

- Update the root, document, and research READMEs; current charter, roadmap,
  edition map, and chapter matrix; and the two front-matter reader notes.
- Describe most entries as titles and short notes awaiting the author's
  return. Distinguish editorial assembly, the author's development of an idea,
  and evidence for a specific technical result.
- Replace ambiguous current inclusion labels with assembly-only wording.
- Preserve chapter bodies, titles, order, code, bibliography, backups,
  historical plans, and the white-paper source/PDF.
- No per-chapter author-review classification is inferred from the general
  clarification. No technical claim, assumption, or evidence status changes.
- No commit, push, dependency installation, or container replacement.

## Baseline and environment

- Clean `master` at `f50ed46` before this task; bounded Git diagnostics use
  per-command `safe.directory` for the separate Windows sandbox identity.
- Read the governing instructions, charter, active status documents, both
  canonical entry/include graphs, reader notes, original edition/assembly
  plans, build helpers, C test/build definitions, and bibliography conventions.
- Inspected the existing managed container `adversarial-cooperation-dev`, ID
  `087f758ebdc8c512223a1414e871d84b3850a6491483b390900b12b2dc661738`.
  Its documented image, user, labels, mounts, loopback port and lack of device
  access match. It was stopped.
- Attempted to start that container for baseline book builds. Docker rejected
  the start because running container `non-verba-dev` already holds
  `127.0.0.1:4173`. The project container remains stopped. No other container
  was stopped or reconfigured. No host toolchain fallback is permitted.

## Checks and progress

- [x] Confirm current source/status wording and author clarification.
- [x] Inspect the authoritative build environment; record blocked baseline.
- [x] Apply the bounded wording changes.
- [x] Review scope, consistency, local links and Git whitespace.
- [x] Record why baseline/final compilation and visual checks could not run.
- [x] Record final files, claim/assumption boundary, results and next step.

## Work report

1. **Files read:** `AGENTS.md`; the root, document, research, and container
   READMEs; charter, roadmap, chapter matrix, edition map; both canonical LaTeX
   entry points and reader notes; `short/layout.tex`, `utils/text_utils.tex`,
   `doc/references.bib`; the book/compiler helpers and C/test Makefiles; and
   the original edition/assembly plans. Existing claim/assumption registers,
   selected short chapters and verification records were read in the preceding
   status review. Historical plans remain historical records.
2. **Files changed:** `README.md`, `doc/README.md`, `doc/research/README.md`,
   `RESEARCH_CHARTER.md`, `ROADMAP.md`, `EDITION_MAP.md`, `CHAPTER_MATRIX.md`
   under `doc/research/`; `doc/content/research_draft_status.tex`;
   `doc/short/reading-note.tex`; and this plan. Replaced all 27 matrix inclusion
   labels with "assembled companion"; retained their technical descriptions.
3. **Claims:** clarified the author's account that most chapters remain names
   and small notes not yet revisited. Narrowed editorial-completion language
   to source coverage. No scientific claim, implementation result, guarantee,
   novelty classification, or recorded technical maturity was changed.
4. **Assumptions:** no technical or operational assumption added. The author's
   general clarification is not used to infer a review status for any specific
   chapter or require new approval for routine work.
5. **Commands/checks:** bounded `git status`, `git diff --stat`,
   `git diff --check`, and `git show HEAD:<reader-note>`; `rg` and PowerShell
   file inspection; Docker inspect/start and read-only port-holder inspection.
   Static checks resolved all 83 local Markdown links, confirmed unchanged
   LaTeX command sequences and balanced braces in both edited reader notes,
   and found no whitespace errors. No C or browser execution tests were
   needed for this prose-only scope. No project build ran on Windows.
6. **Results/failures:** wording and static checks pass. Independent review
   confirmed all 27 matrix rows preserve their technical content and prompted
   removal of wording that could imply a new author-participation prerequisite.
   Baseline and final book compilation could not run: `docker start` failed
   because `non-verba-dev` holds the documented loopback port 4173. The
   project's existing container remains stopped; no container was replaced,
   stopped, or reconfigured to resolve the conflict. No PDFs were regenerated.
7. **Limits:** source-level checks do not establish PDF pagination or visual
   correctness. Technical limitations remain as previously recorded, including
   the absence of a private Tic-Tac-Toe proof. Most chapter ideas still await
   the author's development; this wording task supplies no missing argument.
8. **Next smallest step:** when the project's existing container can start,
   build `short_book` and `research_companion` with the documented book helper
   and inspect the changed reader-note pages. The port conflict needs separate
   coordination; this task does not authorize interrupting Non-verba.

## Acceptance boundary

Readers and future status reviews should understand that most chapters remain
names and small notes the author has not yet revisited. Editorial assembly
does not settle their intended meaning. Specific developed technical work
keeps its existing evidence and limitations. A failed environment start is
not a successful manuscript build.
