# Execution Plan: Simple Reader and Reusable WebAssembly Demos

## Status

In progress (2026-08-25).

## Objective

Replace the browser manuscript-review system with the smallest honest public
website for the book:

1. **Read** presents the PDF compiled from the canonical LaTeX manuscript.
2. **Demos** lists only registered, working WebAssembly companions.
3. **About** explains that LaTeX is the manuscript source and C is the
   executable source.

The first reusable demo adapter wraps the existing disclosed-policy
Tic-Tac-Toe checker. Future chapters must be able to register another demo
without adding another navigation system or another result renderer.

## Public Demo Contract

Every registered demo supplies:

- a stable demo identifier;
- the exact chapter source path it accompanies;
- a title, summary, and educational limitation;
- one or more named usage examples with descriptions and expected outcomes;
- an asynchronous runner with explicit readiness and disposal;
- and a normalized result containing a verdict, summary, metrics, optional
  replay steps, and a statement of what the run establishes.

The public shell owns navigation and presentation. A demo module owns only the
translation between its authoritative WebAssembly worker and that contract.

## Removed Product Boundary

The public website will not contain:

- local-folder access or manuscript writes;
- comments or change indications;
- raw-LaTeX editing or a raw-document view;
- reader-facing or serialized source paths, hashes, byte ranges, line ranges,
  or block counts;
- selectable/focusable source-map blocks;
- an unavailable Demo tab on chapters with no companion;
- or a serialized second copy of the manuscript in `book.json`.

Ordinary browser selection remains ordinary selection. Scientific work stays
in the canonical LaTeX and C files.

The small unbundled demo registration module may retain its canonical chapter
path as maintenance metadata required by the demo contract. The shell never
renders that path, fetches source through it, or turns it into a source map.

## Exact File Scope

Modify:

1. `compose.yaml`
2. `setup.sh`
3. `README.md`
4. `web/README.md`
5. `web/package.json`
6. `web/src/index.html`
7. `web/src/styles.css`
8. `web/src/app.mjs`
9. `web/src/ttt-worker.mjs`
10. `web/tools/build-site.mjs`
11. `web/tools/build-wasm.mjs`
12. `web/tools/serve.mjs`
13. `web/tools/test-site.mjs`
14. `web/tools/test-static-ui.mjs`
15. `web/tools/test-wasm.mjs`
16. `.github/workflows/pages.yml`
17. `research/DECISIONS.md`
18. `container/SECURITY.md`

Create:

19. `research/plans/simple-reader-and-reusable-demos.md`
20. `web/src/demo-contract.mjs`
21. `web/src/demo-registry.mjs`
22. `web/src/demos/ttt-demo.mjs`
23. `web/tools/test-demo-contract.mjs`

Delete after confirming no remaining imports:

24. `web/src/latex.mjs`
25. `web/src/render.mjs`
26. `web/src/views.mjs`
27. `web/tests/rendering-fixtures.mjs`
28. `web/tools/test-render.mjs`
29. `web/tools/test-views.mjs`
30. `web/tools/test-change-indications.mjs`

No manuscript chapter, bibliography, backup, C protocol implementation,
cryptographic primitive, test vector, or dependency lockfile is in scope.

## Authoritative Preimages

Guarded copyback must stop if any existing scoped file differs from these
SHA-256 values recorded before staging:

```text
6e841135c90a1778dab04a3a093775de73e8e42a3e707f13559571f1bdb1f40b  compose.yaml
25e7cd806abbedcff295317dcbed107cfdd7f4b80a9c99725aa24fbc947bc220  setup.sh
69406ce93ce964190b17ce388d677e1a2d7e13484f43b75c041d142e45b76833  README.md
d4d40019caf753a22340405c273dbc5acdd6b59118034f2b1f3b5e4778ccc8f1  web/README.md
5ba5ec51e976b12e97eaf77b8d4bd682c70f64a5c37544d701146cc00aedfc73  web/package.json
91d16ee8a50f77bf9ac69e89a5be40d9252bc3ab7e5dbb8611af4a17b8b2c1de  web/src/index.html
c454b4d96915c0a066dad5f98f8fc182700881700d22ac82af2a86dbf1075206  web/src/styles.css
5db8e99c3c064d170bfb6af9bd46e467b44006277ce8b686c6fcb9a6aa5292cb  web/src/app.mjs
9d6b06e380bb229bb127518de80309a554ab3facfa39011224618900a3deefae  web/src/ttt-worker.mjs
af88f15768f562410af5d8160938c21553731a44f9d507c247f4d1131e2930dd  web/src/latex.mjs
7402cc428b3c65d88b6b2c71ca518224f3d7ef7cdd2e429e079ca39b17e14f4d  web/src/render.mjs
8450991a9e7355c51f9974fcf5e3be8e39a3786d14034f1921a0822d3a5efa5d  web/src/views.mjs
d8af8e960028984f5b5889ea1788f49aeaa3134650f880bcea362bb0c75237e1  web/tools/build-site.mjs
ea29fc99c6eabe01edd7a536ddca07f9080dd81b4ae9de91e5e251d285c9ee27  web/tools/build-wasm.mjs
af4eb1e93f9461ca78c1e9669033df566970c5a4a0d84965be85d8c795054689  web/tools/serve.mjs
dc6ce1dd85efe9c20fbceb824a93e65918fbb1605eccb9cf8f677bfc714d8455  web/tools/test-site.mjs
c74a743c2e257fda1e18783da892f0d113181ab90fe9161fd744b54c0292a041  web/tools/test-static-ui.mjs
8683a78dbaa054ad44a6f2a8cb8d527bc7144d76923dc225b0c567aa9abd9eb5  web/tools/test-wasm.mjs
5b8b6f8ca5a1fa1d4eadf2a3b8ba1e3dc342d40122b7c56e7b0d22dd62410374  web/tools/test-render.mjs
f3dda580717d9df7c1f49ff56e166699effea7af62444ebf7d50a1d3cf09b8fa  web/tools/test-views.mjs
be6baaa2a87f3533e205321f40fa33c9446ba9aeb6e7c064be5d19b59f1032ed  web/tools/test-change-indications.mjs
6b4d1956f1a40a29f56e168fd55a7cafc2776a913176d0525cd95611b82c29ce  web/tests/rendering-fixtures.mjs
975dc0b86a131af1ccd46115e79d05b9e5d35c2f2c8e08cfae8b55432b3d98cb  .github/workflows/pages.yml
6837dc583819964f225d890c7149803f99052341b88d6e6ca44f57e9d453ff07  research/DECISIONS.md
c12d2d16b771053862d72c1ea367a6cc40630d457541cddf4a25da3696f97a52  container/SECURITY.md
```

All five create targets were absent at the canonical destination.

## Build and Runtime Standard

- A complete web build compiles the manuscript PDF, assembles the static shell,
  compiles every registered WebAssembly module, tests the complete staged
  output, and only then replaces `web/dist`.
- Component builders may write only into an explicitly supplied staging path.
- `serve` uses an ephemeral work tree and does not publish to a fixed artifact
  run ID.
- `web` and `verify` retain write-once artifact publication.
- Preview remains bound to `127.0.0.1:4173` and gains a readiness health check.
- The container keeps read-only canonical source mounts, a non-root build
  phase, empty capability sets, and no Docker socket.

## Acceptance Criteria

- the public UI contains only Read, Demos, and About;
- Read embeds the newly compiled PDF and provides a direct PDF link;
- the PDF begins with a valid PDF header and contains the complete current
  manuscript, including front matter and bibliography produced by LaTeX;
- the site publishes no serialized manuscript text or source-map metadata;
- no editing, commenting, change-indication, folder-picker, or raw-LaTeX UI or
  runtime API remains;
- only working demos appear in the gallery;
- Tic-Tac-Toe exposes three explained examples and all three execute through
  the existing C/WebAssembly worker;
- the reusable contract rejects malformed or duplicate demo definitions;
- native and WebAssembly fixtures agree;
- two fresh preview starts can succeed without an artifact-name collision;
- graceful preview termination does not produce the former `tini` signal error;
- the complete build is assembled and published atomically;
- GitHub Pages receives only the PDF, static shell, and registered demo assets;
- desktop and narrow browser smoke checks pass without console errors or page
  overflow; and
- no out-of-scope canonical file changes during guarded copyback.

## Claim Boundary

This slice changes presentation and build lifecycle only. It introduces,
strengthens, weakens, or removes no cryptographic, mathematical, historical, or
novelty claim. A demo result remains an implementation observation under its
displayed fixed model; the PDF remains the manuscript authority.

## Work Sequence

1. Establish staged syntax, PDF, C/WebAssembly, and artifact baselines.
2. Implement the reusable demo definition and registry validation.
3. Replace the manuscript-review interface with Read, Demos, and About.
4. Replace the source-map build with staged PDF/static/Wasm assembly.
5. Make preview ephemeral and artifact publication build-only.
6. Remove obsolete renderer, mapper, view, and change-indication modules/tests.
7. Run static, contract, native/Wasm, PDF, container, lifecycle, and browser
   verification.
8. Recheck every canonical preimage, then copy only the scoped paths and
   verify destination hashes.

## Completion Record

Completed on 2026-08-25.

### Result

The browser edition is now one reusable, read-only shell with exactly three
public routes: Read, Demos, and About. Read embeds the complete PDF compiled
from the canonical LaTeX manuscript. Demos lists only registered working
WebAssembly companions. The first registration is the disclosed-policy
Tic-Tac-Toe checker with three explained examples. About states the LaTeX/C
authority boundary. The former editor, comment, selection, source-map,
raw-LaTeX, and serialized-manuscript surfaces were removed.

The shared renderer contains no Tic-Tac-Toe-specific labels. A validated demo
definition, collision-checked registry, asynchronous runner lifecycle, and
presentation-neutral result record form the reusable chapter-demo standard.
Adding a companion requires both runtime registration and a matching compiler
entry; the build fails if those registries drift.

Preview now builds only in disposable container storage. It publishes no host
artifact and survived two stop/start cycles without an artifact collision,
port collision, `tini` signal error, or stale work tree. Complete `web` and
`verify` results remain write-once and are copied to a hidden sibling on the
artifact filesystem before one atomic rename makes the run visible.

### Verification Evidence

The complete isolated command was:

```text
docker compose -p adversarial-cooperation-stage run \
  --name ac-reader-verify-retry \
  -e AC_RUN_ID=reader-standard-20260825b toolchain verify
```

It passed:

- every current native C demonstration build;
- the complete native C test suite;
- AddressSanitizer and UndefinedBehaviorSanitizer runs;
- the complete LaTeX manuscript and bibliography build;
- static publication-boundary and source-leak checks;
- demo-definition, immutability, duplicate, registry, and lifecycle tests;
- the Read/Demos/About shell checks;
- all three native/WebAssembly Tic-Tac-Toe agreement fixtures; and
- write-once, same-filesystem atomic artifact publication.

The verified run is
`.container-output/reader-standard-20260825b/`. Its `SHA256SUMS` file was
independently rechecked. The published site contains only the PDF, static
reader assets, and one registered WebAssembly module. The PDF is 638,991
bytes, has 112 pages, contains front matter through bibliography, has no
embedded JavaScript, and exposes no forms. All 112 pages were rendered and
inspected in four contact sheets; representative title, Tic-Tac-Toe,
mathematical, and bibliography pages were also inspected at full rendered
resolution. Text extraction covered every page.

The dedicated preview served the complete 638,991-byte PDF at HTTP 200. HTML,
PDF, WebAssembly, and JavaScript responses had the expected MIME types. HTML
and executable assets used `frame-ancestors 'none'`; the same-origin embedded
PDF alone used `frame-ancestors 'self'`. The preview was restarted twice and
returned healthy after each fresh ephemeral rebuild.

The in-app browser-control surface refused the loopback URL under its browser
URL security policy. That policy was not bypassed with another automation
surface. Consequently, automated desktop/narrow screenshots, console capture,
and clicks are not claimed. The server is left on `127.0.0.1:4173` for normal
human inspection; static responsive checks, HTTP checks, complete PDF visual
QA, and native/WebAssembly execution checks passed independently.

### Guarded Publication

Immediately before canonical publication, all 25 existing scoped files still
matched their frozen preimages and all five create targets were absent. The
source comparison showed exactly the 29 intended deltas: 17 modified files,
five additions including this plan, and seven obsolete web-only deletions;
the permitted `ttt-worker.mjs` remained byte-identical.

Windows rejected the first attempt to call `System.IO.File.Replace` with a
null backup path. Existing files remained unchanged, while the four new
runtime/test modules and seven approved deletions had already been applied.
That exact partial state was audited. The 17 verified temporary files were
then moved over their still-frozen destinations with the overwrite overload of
`System.IO.File.Move`. Every resulting destination hash matched staging, no
temporary file remained, and the only remaining staged/canonical difference
was this intentionally last completion record. This record was then published
as the fifth addition.

No manuscript, bibliography, backup, C implementation, cryptographic
primitive, test vector, lockfile, scientific claim, or novelty claim changed.
The next smallest vertical slice is to register the next already-working C
companion—preferably Two Oracles Play Rock-Paper-Scissors—through this same
contract, compiler registry, native/WebAssembly parity test, and displayed
claim boundary without changing the reader shell.
