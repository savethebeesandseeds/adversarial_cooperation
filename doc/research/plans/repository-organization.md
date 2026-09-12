# Repository organization

Status: author-directed doc/code/web/.local consolidation complete, 2026-09-12.
Preservation, static checks and the existing non-compiling browser contract
test pass. Compilation remains explicitly excluded. The author has now
explicitly requested commit and push; the final Git result is reported in the
task. The latest report below supersedes the first-pass layout choice.
Author authorization: 2026-09-12, clean and order this repository after identifying
duplicate temporary directories and the redundant proposal ZIP.

## Objective and boundaries

Make the checkout and published repository easy to navigate, keep generated
material separate from sources, preserve unique writing and research evidence,
and ensure the established builds continue to work. This is a filesystem and
build-path change; it introduces no cryptographic claim, protocol, or dependency.
The proposal PDF stays at the repository root as the author requested.
The existing Docker container, data, configuration and lifecycle contract remain.

Initial Git status is clean at `0d71cb105abcc1a3ac537ac165926fda86a9cf37`
on `master`, matching the last verified `origin/master`. Native Git mutations
run as the workspace owner; no history, identity or global config changes.

## Findings and chosen layout

- `.temp` holds 651 files, including QA scripts, reports, source snapshots and
  earlier PDFs. Preserve it as `.local/archive/session-work/`.
- `.container-output` holds 374 files of write-once build/provenance evidence.
  Move it to `.local/evidence/` and update the task runner and CI artifact paths.
- 35 historical generated files are still tracked: 2 beneath `.temp`, 14 in
  `temp`, 12 in `document/temp`, and 7 in `demostrations/.build`. Preserve their
  bytes locally, then remove generated files from the Git index. Preserve the
  old source-demo executable in the same way if its build mapping is confirmed.
- New native/book builds use `.local/build/` and scratch uses `.local/scratch/`.
  The web builder retains its deliberately constrained `web/dist/` output;
  its LaTeX intermediates move to the same `.local/build/pdf/` as other books.
  The web-output safety boundary is unchanged. Both outputs remain ignored.
- Correct the root `demostrations/` directory to `demonstrations/`; update all
  active command/source references. Preserve older audit records verbatim and
  give readers a current path map. Deeper chapter names and scientific prose
  are not part of this migration.
- Preserve the 18 placeholder-only demonstration directory trees (30 tracked
  `.gitkeep` files) locally. The active demonstrations folder will contain its
  Makefile and guide; implemented C remains under `src/`. Preserve the older
  poker note in the tracked manuscript archive and build its canonical chapter.
- All ten `.tex.bak` files differ from current sources and preserve author's
  wording or questions. Move them byte-for-byte into a tracked manuscript
  archive with original-path mapping; do not merge or discard their content.
- The root proposal ZIP duplicates checked-in files. Preserve a local archival
  copy, stop tracking the ZIP, and link the editable source directory directly.
- Retain ignored scholarship material locally, clearly separated from public
  sources. No private material may be added to Git.
- Keep the exact raw Docker creation commands in the root README, as required
  by the author's existing container decision; improve the opening navigation.

## Execution and verification

1. Record a complete file/hash inventory, tracked status, and symlink checks
   before moves. Read the existing charter, manuscript/build entries, active
   container/edition plans, bibliography conventions and rename register.
2. Preserve the existing successful verification evidence. Docker startup is
   unavailable; the author explicitly directed continuing without Docker,
   recompilation, or installing anything on Windows. Move existing objects
   and PDFs intact. No fresh baseline build or post-change compilation will run.
3. Record an exact rename map before moves; validate all resolved sources and
   destinations remain inside the workspace and refuse existing destinations.
   Use `git mv` for tracked renames and native PowerShell moves for untracked
   local material. No recursive deletion, Git clean, Docker cleanup, or source
   deletion is authorized by this plan.
4. Update build defaults, active links/commands, ignores, CI and navigation.
   Keep unique manuscript backups, vendor bytes, historical reports and local
   scholarship documents intact. Distinguish current paths from historical ones.
5. Run read-only repository-layout and static reference checks. Verify move
   hashes, backup hashes, root PDF hash,
   active links, absence of tracked generated outputs and new temporary roots,
   and the final Git diff. Existing compilation/test evidence is preserved;
   build-path execution after the move remains unverified by author direction.
6. Record files read/changed, claims/assumptions, commands/results, remaining
   limitations and the next step. No container was successfully started or
   used for this task. Report publishing separately from local cleanup.

## Progress

- Independent audits examined manuscript backups, build/CI path dependencies,
  tracked generated outputs, ignored scratch/evidence, and root navigation.
- No implementation or scientific content changes have been made at plan creation.
- Complete pre-move checksum inventory: `.local/organization-20260912/before.json`
  (1,365 files, 91,901,110 bytes). Exact 38-entry move map recorded before
  execution in `research/organization-moves.json`. The map is ordered so more
  specific source moves occur before their containing directory is moved.
- Existing completed build bundles were independently checksum-verified:
  347 files across 13 bundles, no mismatches. Unique incomplete-run evidence,
  distinct historical PDFs and all local scholarship material will be retained.
- Docker startup failed; pending status commands were canceled. The author
  explicitly directed no compilation or Windows installations. No packages,
  containers, volumes, images, or Docker settings were created or replaced.

## Required work report

1. **Files read:** project instructions and research charter; root/source/demo/
   proposal/web READMEs; both manuscript entries and their include conventions;
   utilities, canonical poker/hash command examples, bibliography conventions;
   all ten manuscript backup differences and the former poker note; existing
   audit, container and edition plans; rename map, Makefiles, book/LaTeX helpers,
   task runner, Pages workflow and web build/test tools. Independent audits
   examined the full temporary/output trees and validated earlier manifests.
2. **Files changed:** 38 mapped source/directory moves; root and folder navigation,
   ignores and line-ending attributes; shared output defaults and active command
   paths; container evidence/export and CI upload paths; web PDF paths and their
   existing test expectations. Added the current folder guide, research/archive
   indexes, exact JSON move map, this report, and a stdlib preservation verifier.
   All 67 formerly tracked generated/placeholder/ZIP files remain physically
   present in ignored local archives. Eleven earlier manuscript files remain
   tracked in `research/archive/manuscript/`. The root proposal PDF stays put.
3. **Claims:** no cryptographic claim introduced, strengthened or weakened. C
   implementations/interfaces, proposal prose and PDF, vendor distribution,
   scientific chapter prose and reference-vector values are unchanged. The
   standalone poker build now points to the already-canonical chapter rather
   than the preserved older duplicate. This removes a duplicate build input;
   it does not establish any new property.
4. **Assumptions:** `.local/` is machine-local and ignored; it will not appear
   in a Git clone. Historical objects/PDFs retain their original provenance and
   are not represented as newly compiled artifacts. Existing build environment,
   protocol assumptions and web output safeguards remain; no dependency added.
5. **Commands/checks:** bounded Git status/diffs and tracked-file inventories;
   SHA-256 pre-move inventory; validated native `git mv`/PowerShell moves and
   `git rm --cached` for preserved local files; preservation verifier against
   the complete map; Markdown link resolution; old active path and ignore
   checks; comparisons against HEAD for C/interfaces, proposal and all 36
   vendor files; exact command-only comparison for the two chapters and vector
   instructions. Existing Git Bash `-n` checked four scripts and bundled Node
   `--check` checked two modules. No compiler, package installation, object
   rebuild, PDF regeneration or runtime test was run.
6. **Results:** all 1,152 affected files match their original hashes and sizes;
   all 38 old sources are absent; no collisions or missing files. All 45 checked
   local Markdown links resolve. No old temporary roots, generated objects or
   private/local material remain in the Git index. Independent tooling review
   found the new path chain consistent; syntax and whitespace checks pass.
   The read-only verifier passed 17 isolated boundary fixtures, including
   Windows junction rejection. Its records are supplied evidence, not an
   authenticated provenance system. No preservation/static-check failure remains.
7. **Limits:** Docker did not become available, and the author explicitly
   directed proceeding without recompilation or Windows installations. Runtime
   build behavior at the new paths has therefore not been demonstrated. Earlier
   successful test evidence is preserved; it is not substituted for a new run.
   Historical QA scripts retain original paths and may need explicit path
   adaptation for reuse. Existing local/archive material was organized, not
   destroyed or silently deduplicated.
8. **Next smallest step:** review/commit the organized tree. When the existing
   build environment is available and a build is authorized, run the established
   full verification with a fresh run ID and the standalone section/proposal
   targets. Git publication has not been performed as part of this local cleanup.

Evidence: `.local/organization-20260912/before.json` and `static-checks.json`.
The move verifier command is documented in `research/REPOSITORY_LAYOUT.md`.
Preserved proposal PDF SHA-256:
`e1a682d86137c4d22f7f3fbc801deb8a5f52cd2cde1e110b1641d86bb1a7922b`.

## Author's explicit structure correction

The author clarified that the desired roots are `doc/`, `code/`, and `web/`,
with tests and vectors together under `code/tests/`, container tooling inside
`.local/`, and fewer root files. This supersedes the preceding layout choice.

The exact second-pass map is `structure-moves.json` beside the original map.
It moves manuscripts into doc, research into doc/research, C source/headers and
test data into code, demonstration Makefile/README to the code root, shared make
configuration into code, and book/LaTeX helpers into doc/tools. The proposal PDF
also moves into doc and remains prominent in the root README.

The existing root `setup.sh` stays byte-identical because the saved container
bind names that exact host file. Recreating or changing the container is outside
the current no-Docker instruction. All other container tooling moves into
`.local/container/`, which is explicitly kept in Git while sibling local
archives/evidence/private data remain ignored. The lengthy container recipe
moves out of the root README into the container folder. No installation,
compilation, Docker command, or container lifecycle change is part of this pass.

Capture a second pre-move checksum inventory, verify all second-pass moves
before editing relocated files, then review each path-only source change and
run static/non-compiling checks. Preserve the earlier cleanup and its evidence;
do not restart from or restore the previous Git commit.

### Final structure and work report

1. **Read:** current instructions, prior cleanup plan/evidence, Makefiles,
   document helpers, container contract/task runner, Pages workflow, web build
   and contract/test modules, research registers, canonical chapter path
   references, vector guides and navigation files. Work started from the
   existing uncommitted cleanup, preserving every staged move and local archive.
2. **Changed:** the twelve moves in `structure-moves.json`; code Makefile/config,
   document helper paths, container task/source/export references, Pages task
   path, web compiler/source/header/chapter paths and contract fixtures, active
   chapter/register references, root AGENTS paths, folder READMEs and guides.
   Moved the full environment recipe into `.local/container/README.md`; reduced
   the root README to navigation. Updated the preservation tool's default root
   after relocation. Root setup.sh remains byte-identical for the saved mount.
3. **Claims:** no scientific claims or vector values changed. The 29 relocated
   C source/header/test files match their second-pass pre-move hashes. Proposal
   manuscript/PDF and all 36 vendored IEEEtran files are unchanged. Web bridge
   source changes only adapt a relocated header include and source-path comment.
4. **Assumptions:** only `.local/container/` is versioned under `.local`; all
   sibling evidence, archives and private material remain ignored. Existing
   setup and Docker lifecycle contracts are preserved. The new source layout
   is doc/code/web, with build/evidence destinations under .local and the web
   output safety boundary retained. No dependency or cryptographic assumption
   added.
5. **Commands/checks:** pre-move inventory of 232 current files; twelve validated
   native Git moves; immediate verification of all 197 affected files before
   active path edits; current archive verification of 1,150 unchanged files;
   Git whitespace/index/ignore checks; 103 Markdown links; all seven book target
   paths and current chapter code references; four Bash syntax checks; seven
   JavaScript syntax checks; existing `web/tools/test-demo-contract.mjs` test;
   a read-only `doc/tools/book.sh -list`. No compiler, Docker action, environment
   installation, PDF regeneration, or build-dependent test ran in this pass.
6. **Results:** all relocation checks passed. All checked current links and
   references exist, and the existing browser contract test passes. C code,
   setup, proposal and vendor hashes match. Only container tooling is tracked
   under .local. The root now has three public content folders (doc/code/web),
   the hidden tooling/Git folders, and six files: README.md, LICENSE, AGENTS.md,
   .gitignore, .gitattributes, and setup.sh. The first attempt to list book
   targets lacked Git Bash utilities on PATH; retrying with its existing
   /usr/bin and /bin for that single process succeeded. No installation or
   persistent PATH change was made. No static or preservation failure remains.
7. **Limits:** runtime compilation at the new paths remains unverified by the
   author's explicit instruction. Existing generated objects and historical
   reports were moved intact, not presented as newly rebuilt results. Completed
   historical plans and archive contents retain original paths; the two move
   maps document their relocation chain.
8. **Next smallest step:** review and commit this final layout. When authorized
   and the existing container is available, run the documented full verification
   and standalone document targets. No Git commit or push was performed for
   the cleanup without a new publication request.

Current checks: `.local/organization-20260912/structure-checks.json`.
Current preservation command uses the original `before.json` and the local
composed `archive-moves.json`. The two active demonstration entry files are
excluded from that archive-only check because their command paths were edited;
their moves were verified before editing. The second-pass inventory is
`.local/organization-20260912/second-pass-before.json`.

### Authorized publication

The author requested "commit and push" after reviewing the final structure.
The destination is the existing `origin/master` at
`https://github.com/savethebeesandseeds/adversarial_cooperation.git`.
The commit includes the reorganized public sources, document PDF, preserved
manuscript snapshots, navigation, path updates and versioned container tooling.
Local archive, evidence, scratch and private files remain excluded.

Publication preparation read the current Git scope, ignore rules, final report,
shared configuration and CI path. It adds no scientific claims, assumptions,
dependencies, installations, compilation or Docker operations. Prior static
and preservation checks remain applicable. Git staging/content checks and the
final commit/push verification are reported in the task; no force push or
history rewrite is authorized or needed.
