# Folder and recovery guide

## Current layout

```text
doc/
  content/, short/, utils/       book sources
  white-paper/                  editable evolving white paper
  research/                     charter, registers, plans, earlier writing
  tools/                        book and LaTeX build helpers
  adversarial-cooperation-white-paper.pdf
code/
  src/                          C implementations
  include/                      public headers
  tests/
    vectors/                    reproducible reference inputs
  Makefile                      demonstration entry points
  Makefile.config               shared C build configuration
web/                            browser reader and demonstrations
.local/
  container/                    versioned environment instructions and tasks
  build/, scratch/              ignored generated output
  evidence/, archive/           ignored preserved local artifacts
  personal/                     ignored private material
```

Tests and vectors belong together: tests are executable checks; vectors are
fixed inputs and expected results used to reproduce them. Both now live inside
`code/tests/`.

`doc/white-paper/` holds the short document and its build instructions;
`doc/research/` holds the broader program's charter, registers and plans. The
white paper is evolving; its current text presents a research proposal.

Root files are limited to the README, license, agent instructions, Git rules,
and `setup.sh`. The saved Docker container directly mounts that exact setup
file; it remains byte-identical so a filesystem cleanup does not invalidate the
existing bind. The full container recipe is in `.local/container/README.md`.

`.local/container/` is versioned. Its sibling folders are ignored and stay on
this computer; they are absent from a repository clone. `web/dist/` remains the
web builder's constrained, ignored output destination. No private material or
local build evidence is added to Git by this organization.

## Preserved material

- `.local/evidence/<run-id>/` retains every previous verification run identity.
- `.local/archive/session-work/` retains earlier QA scripts, reports, rendered
  pages, source snapshots and previous proposal outputs.
- `.local/archive/generated/` retains historical objects, binaries and PDFs.
- `.local/archive/demonstration-placeholders/` retains the former empty mirrors.
- `.local/archive/proposal/` retains the redundant source ZIP.
- `doc/research/archive/manuscript/` retains all ten unique manuscript backups
  and the older standalone poker note in Git, with original-path mapping.

Existing objects and PDFs were moved intact. Nothing was compiled or installed
for the cleanup after the author's instruction. Historical binaries retain
that status; they are not presented as a new build of the reorganized sources.

## Earlier paths

| Previous path | Current location |
|---|---|
| `document/` | `doc/` |
| `research/` | `doc/research/` |
| `src/`, `include/` | `code/src/`, `code/include/` |
| `tests/`, `test-vectors/` | `code/tests/`, `code/tests/vectors/` |
| Demonstration Makefile and guide | `code/` |
| `Makefile.config` | `code/Makefile.config` |
| `book`, `compile_latex.sh` | `doc/tools/book.sh`, `doc/tools/compile_latex.sh` |
| `container/` | `.local/container/` |
| Root proposal PDF; `doc/Adversarial-Cooperation-Research-Proposal.pdf` | `doc/adversarial-cooperation-white-paper.pdf` |
| `doc/research-proposal/` | `doc/white-paper/` |
| `.temp/` | `.local/archive/session-work/` |
| `.container-output/` | `.local/evidence/` |
| `temp/`, `document/temp/` | `.local/archive/generated/root-temp/`, `document-temp/` |

[organization-moves.json](organization-moves.json) records the first artifact
cleanup; [structure-moves.json](structure-moves.json) records the author's
subsequent explicit layout correction. The later white-paper filename changes
are RN-022 through RN-025 in [RENAME_MAP.md](RENAME_MAP.md). Historical plans
and archive source contents retain their original paths. Apply these records
in order when locating an older reference. Active README/build instructions
use the current paths.

The first relocation verified 1,152 files. The second verified all 197 moved
files before active path references were edited. Local inventories and reports
are under `.local/organization-20260912/`.

Recheck preserved archives without compiling or modifying anything:

```powershell
python doc/research/tools/check_repository_layout.py --inventory .local/organization-20260912/before.json --moves .local/organization-20260912/archive-moves.json
```

Use an existing Python installation. This stdlib checker compares file lengths
and hashes against supplied records; it does not authenticate their provenance.
The current archive map excludes the two active demonstration entry files that
were intentionally edited during the second pass.

## Future housekeeping

Keep documents in `doc/`, implementation and tests in `code/`, browser code in
`web/`, and operational tooling in `.local/container/`. Use the documented
build outputs and unique evidence run IDs. Preserve unique drafts with their
original-path mapping. Avoid another root temporary directory or a release ZIP
that duplicates the source folder.
