# Execution Plan: Static Browser Book Studio and WebAssembly Demo

## Status

Complete (2026-08-01).

## Objective

Create a dependency-light static browser edition of *Adversarial Cooperation*
that can be deployed by GitHub Pages while preserving the repository's LaTeX
and C files as the only authored sources.

The first vertical slice must:

1. derive the complete table of contents and reader view from the canonical
   LaTeX include graph;
2. retain exact source-file and source-range metadata for every rendered block;
3. let a user who explicitly grants a local repository directory edit the
   corresponding raw LaTeX block with stale-write protection;
4. run the existing disclosed-policy Tic-Tac-Toe checker as WebAssembly through
   a thin C ABI, without reimplementing the protocol in JavaScript; and
5. build into a self-contained static directory suitable for GitHub Pages.

## Architectural Boundary

- `document/**/*.tex` remains the sole authored manuscript.
- `src/protocols/ttt.c` and its public headers remain the authoritative
  Tic-Tac-Toe implementation.
- Generated HTML, JSON, JavaScript glue, and WebAssembly are disposable build
  artifacts, not independent manuscript or protocol sources.
- The browser editor edits raw LaTeX blocks. It does not convert arbitrary HTML
  back into LaTeX.
- Static hosting cannot execute LaTeX. Browser saves therefore perform source
  hashing, range matching, and bounded syntax checks, but a later local or CI
  manuscript build remains the compilation authority.
- The first WebAssembly demo is the public, fully disclosed Tic-Tac-Toe checker.
  It is an educational execution of the fixed model, not zero knowledge,
  strategy privacy, authorship, or a cryptographic proof.
- No new cryptographic primitive or runtime cryptographic library is added.
  Emscripten is a pinned build tool only.

## Safety Contract

The static editor must:

1. request an explicit directory grant from the browser;
2. resolve only manifest-listed manuscript files beneath `document/content`;
3. reject stale whole-file or selected-block hashes;
4. preserve UTF-8 and the file's existing LF or CRLF convention, while
   refusing mixed or lone-CR files;
5. reject unbalanced braces/environments and new `\\input` or `\\include`
   commands in editable blocks;
6. preview the exact raw-LaTeX replacement before writing;
7. keep an in-memory reverse patch for immediate undo; and
8. never touch `.bak` files, the preamble, root include graph, bibliography, or
   macro-definition files.

## Exact File Scope

Modify:

1. `.gitignore`
2. `README.md`
3. `research/DECISIONS.md`

Create:

4. `research/plans/browser-book-studio-wasm.md`
5. `.github/workflows/pages.yml`
6. `web/README.md`
7. `web/package.json`
8. `web/tools/build-site.mjs`
9. `web/tools/build-wasm.mjs`
10. `web/tools/serve.mjs`
11. `web/tools/test-site.mjs`
12. `web/tools/test-wasm.mjs`
13. `web/src/index.html`
14. `web/src/styles.css`
15. `web/src/app.mjs`
16. `web/src/latex.mjs`
17. `web/src/ttt-worker.mjs`
18. `web/wasm/ttt_web.c`
19. `web/tests/test_ttt_web.c`

If implementation discovers that an additional file is necessary, this scope
must be updated before that file is changed.

## Authoritative Preimages

Guarded copyback must refuse any existing file whose SHA-256 changes from:

```text
3c974ea3f6ef55ff4c843dd1d05655d3f884e1010363ecea5814966feae17fcb  .gitignore
c14740f2ae73130241efddb67efcb502dae81b1b9b04ae5f9b29e189d9831872  README.md
40d0bcbbceca8220d949511898dfddc7df20f3fa4f7474b5554caf7ab145b707  research/DECISIONS.md
```

All create targets were absent at the authoritative destination when this plan
was opened.

## Work Sequence

1. Establish the authoritative source and native Tic-Tac-Toe test baseline.
2. Implement one token-preserving LaTeX block parser shared by the build and
   browser-local refresh path.
3. Generate the 27-source book manifest and static reader payload.
4. Add the guarded File System Access editing path and read-only fallback.
5. Add the thin Tic-Tac-Toe WebAssembly bridge and background worker.
6. Prove native and WebAssembly results agree on the recorded fixtures.
7. Add a static build, local preview command, and GitHub Pages workflow.
8. Validate content coverage, hashes, path safety, responsive behavior,
   accessibility, and static subpath loading.
9. Copy back only the enumerated paths after rechecking all preimages.

## Acceptance Criteria

- all 27 chapter/appendix sources appear once in book order;
- no authored prose exists beneath `web/`;
- generated blocks cover every source byte or expose it in an explicit raw
  source block;
- every editable block carries file revision, source range, and exact-byte
  hashes;
- stale local files cannot be overwritten;
- unsupported browsers remain useful in read-only mode;
- the WebAssembly module is compiled from the existing Tic-Tac-Toe C source;
- JavaScript only marshals inputs and renders returned results;
- native and WebAssembly fixture results agree;
- the static site works beneath a non-root GitHub Pages base path;
- the Pages workflow uploads only generated static artifacts;
- existing C and manuscript tests remain green; and
- no unrelated dirty-worktree file or backup is changed.

## Verification Record

- The static generator followed the canonical include graph and emitted five
  parts, all 27 live chapter/appendix sources exactly once, and 1,280
  contiguous lossless source blocks. Backups were excluded.
- Every generated chapter hash matched its source; UTF-16 offsets, UTF-8 byte
  offsets, line ranges, path confinement, and relative project-Pages assets
  passed the static test suite.
- The complete site built with the pinned
  `emscripten/emsdk:6.0.5` image at digest
  `sha256:76a44fff907397784decc435115d07fcb9587a4f1504977f39f3745e538e3a1e`.
  The generated bridge was 7,693 bytes of ES-module glue and a 4,618-byte
  WebAssembly module.
- The native bridge suite passed 6 tests, its ASan/UBSan run passed, and the
  WebAssembly reports matched the native fixtures exactly for reference X,
  reference O, and the deterministic losing naive-X policy.
- The repository's eight existing native suites passed 72 tests with zero
  failures in an isolated Debian build tree.
- The manuscript compiled successfully to 112 pages. The container did not
  contain `aspell`, so spell checking was unavailable; this did not affect the
  LaTeX build.
- The local static server returned HTTP 200 for the reader, generated book
  manifest, and WebAssembly module with the expected HTML, JSON, and
  `application/wasm` content types and its configured security headers.
- Browser checks covered the complete table of contents, chapter status,
  source selection, exact replacement preview, rejection of a new
  `\\input`, responsive layout, Tic-Tac-Toe lab loading, and graceful missing-
  WebAssembly behavior.
- A final safety review confirmed live-`.tex` manifest confinement, backup
  rejection, stale whole-file and exact-range checks, line-ending
  preservation, text-only DOM insertion, relative Pages URLs, worker loading,
  and keyboard access to the chapter-title source.
- The staged non-generated differences are exactly the 19 paths enumerated in
  this plan. No manuscript source, backup, cryptographic backend, dependency,
  commit, push, deployment, or unrelated dirty-worktree file was changed.
- Immediately before copyback, all three authoritative preimages matched and
  all 16 create targets remained absent. Only the 19 enumerated paths were
  copied, and every destination SHA-256 then matched its staged source.
