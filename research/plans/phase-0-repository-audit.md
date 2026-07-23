# Phase 0 Repository Audit Execution Plan

## Objective

Establish a preservation-first baseline for the manuscript and C companion,
record the repository's actual build and research state, and create the Phase 0
registers without rewriting chapters, implementing cryptography, deleting
backups, or renaming files.

## Scope

This plan covers repository status, layout, LaTeX inclusion and bibliography,
C targets and dependencies, baseline builds, `.tex`/`.bak` comparisons, chapter
classification, major claims and assumptions, open problems, rename candidates,
and the staged roadmap.

The only planned writes are governance and research-audit Markdown files.
Existing source, manuscript, generated output, backups, and the user's deleted
`idyicyanere-linux-x64.vsix` remain untouched.

## Work Sequence

- [x] Verify the real repository root and record the initial worktree state.
- [x] Install and read `AGENTS.md` and `research/RESEARCH_CHARTER.md` supplied by
  the project brief.
- [x] Inventory LaTeX entry points, includes, bibliography, C sources, targets,
  tests, generated files, and dependencies.
- [x] Read all chapter sources and the C companion.
- [x] Run the discoverable manuscript and C build paths and record exact output.
- [x] Compare every `.tex` file having a `.bak` counterpart and record unique
  content.
- [x] Create and populate the seven required Phase 0 audit/register files.
- [x] Re-run relevant builds and inspect the final diff for scope and accuracy.

## Baseline

- Real checkout: `C:\Work\applied_cryptography\adversarial_cooperation`
- Branch: `main`
- Baseline commit: `2824992ee2802b0b72f67054f63189628eb15103`
- Pre-existing user change: tracked file `idyicyanere-linux-x64.vsix` is deleted.
- OneDrive path `C:\Users\santi\OneDrive\Documents\Adversarial-cooperation` is
  an unrelated unborn Git worktree containing only `.git`.

## Discoveries and Decisions

- The repository initially contained neither `AGENTS.md` nor a `research/`
  directory; both governance files were therefore added from the supplied
  project brief before deeper work.
- The root README describes a Debian 11 container workflow. Build inspection
  will prefer that documented environment when host tools are unavailable.
- No source correction will be made merely to obtain a green build; failures
  belong in the audit unless an audit artifact itself is defective.
- The main document reaches only Trust Establishment; 26 of 27 content sources
  are orphaned, and no bibliography file or citation command exists.
- The documented manuscript command has shell and path defects. With the
  undeclared `texlive-science` dependency supplied, the full build still exits
  1 on output-tree creation, Unicode, bibliography, reference, and Markdown
  table defects. The isolated poker section succeeds in the corrected
  throwaway environment.
- Fresh C11, `-Werror`, and ASan/UBSan builds succeed in a read-only-mounted
  Debian 11 container with external outputs. These results establish honest
  implementation hygiene only; the program remains a centralized two-player
  educational animation.
- Two tracked manuscript backups preserve substantive material: “Gödel's
  unprovable truths” and several concrete poker questions. An ignored Chinese
  scholarship backup also preserves text corrupted in the live ignored file.
- Build attempts temporarily touched generated files. Their evidence was
  recorded, the three tracked generated files were restored to their exact
  baseline blobs, and only the four audit-created untracked temporary files
  were removed.

## Verification Log

Commands, exact results, warnings, failures, and generated paths will be
summarized in `research/REPOSITORY_AUDIT.md`. This plan will be updated as the
audit progresses.

## Completion Criteria

Phase 0 is complete when all requested registers exist and agree with live
repository evidence, every discoverable build path has a recorded result,
every backup pair has a disposition, the final diff contains no unintended
source changes, and the next implementation slice is precise but unimplemented.

All completion criteria are satisfied. The remaining worktree consists of the
pre-existing VSIX deletion plus the ten intended governance/audit Markdown
files.
