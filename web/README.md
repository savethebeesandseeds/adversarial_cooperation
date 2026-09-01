# Adversarial Cooperation Reader

This directory builds a small static reader and a gallery of working
WebAssembly demonstrations. Its source boundary is strict:

1. LaTeX beneath `document/` is the only authored manuscript source.
2. The compiled PDF is the browser's complete reading surface and typographic
   authority.
3. The existing C implementation is the computational source of each browser
   demonstration. JavaScript adapts inputs and results; it does not reproduce
   the protocol or game evaluator.

Nothing beneath `web/dist/` should be edited. It is a disposable, complete
build result.

## Build and preview

Use the one reusable development container documented in the root
[`README.md`](../README.md). Start it, enter its Linux shell, and run project
operations from `/workspace`; no Dockerfile, Compose service, host toolchain,
or JavaScript package download is required.

Inside the container:

```text
AC_RUN_ID=web-unique bash container/tasks.sh web
bash container/tasks.sh serve
```

`web` snapshots the declared source inputs into the container's `/tmp` tmpfs,
checks
the browser modules, compiles the canonical PDF and registered WebAssembly
demonstrations, assembles the static site there, and runs the existing static,
contract, UI, and native/WebAssembly agreement tests. It exports the verified
site to `.container-output/<run-id>/web-dist/`. `serve` performs the same build
and tests without publishing an evidence bundle, then listens inside the
container on port 4173. Open
`http://127.0.0.1:4173`; the raw-Docker configuration publishes the port only
on host loopback.

For the broader native build, test, sanitizer, manuscript, and browser check,
run:

```text
AC_RUN_ID=verify-unique bash container/tasks.sh verify
```

The repository is mounted read-write for direct editing, while automated build
scratch stays in a fresh directory in `/tmp` so stale or foreign-owned generated
checkout data is not changed. Scratch accumulates only for the current container
session and is discarded when the container stops or restarts. Published results
appear under `.container-output/<run-id>/`; the `web-dist/` artifact uses only
relative URLs and can be previewed locally or deployed below a GitHub Pages
project path. The source-authority, package, network, and shared-kernel
limitations are recorded in [`container/SECURITY.md`](../container/SECURITY.md).

## Read, Demos, and About

The public shell contains only these top-level views:

- **Read** embeds `book/Adversarial-Cooperation.pdf` and links to that same
  file for browsers whose embedded PDF viewer is unavailable.
- **Demos** shows only registered companions whose WebAssembly assets are part
  of the completed build.
- **About** explains that LaTeX and C remain authoritative.

The static artifact contains no serialized manuscript text, source ranges,
source hashes, raw LaTeX, local-folder access, change indications, comments, or
write API. The browser is a place to read and run examples, not a second
manuscript editor.

## Reusable demonstration contract

A browser companion registers one immutable definition through
`src/demo-contract.mjs`. The definition supplies:

- a stable demo ID and the canonical chapter source path it accompanies;
- a title, summary, and explicit educational limitation;
- named usage examples, each with a description and expected outcome; and
- `createRunner()`, which returns asynchronous `ready`, `run(exampleId)`, and
  `dispose()` operations.

Every run returns the same presentation-neutral result shape: a verdict,
summary, metrics, optional replay steps, and a statement of exactly what the
execution establishes. The registry rejects malformed definitions and
duplicate IDs. The shell owns navigation and rendering; a demo module owns
only the translation between this contract and its authoritative WebAssembly
worker.

To add a chapter companion, add its C-to-WebAssembly bridge and worker, create
one adapter beneath `src/demos/`, register the validated runtime definition in
`src/demo-registry.mjs`, register its compiler description in
`tools/build-wasm.mjs`'s `registeredWasmBuilds`, and add native/WebAssembly
agreement fixtures. The build rejects a runtime/build registry mismatch. Do
not add a chapter-specific navigation system or copy chapter prose into the
demo.

## Tic-Tac-Toe demonstration

The worker loads the Emscripten module generated from the repository's existing
`src/protocols/ttt.c` through the thin bridge in `wasm/ttt_web.c`. JavaScript
selects one of three fixed fixtures, invokes the bridge, and displays its scalar
report. It does not reproduce the game evaluator.

The gallery exposes three explained, fixed usage examples: the reference X
policy, the reference O policy, and a deliberately naive losing X policy. The
demonstration is a fully disclosed educational checker. It provides no strategy
privacy, zero knowledge, commitment, authorship, cryptographic proof, or
guarantee of future behavior. If WebAssembly is missing or blocked, the book
reader continues to work and reports the affected run as unavailable.

## Static deployment

The Pages workflow creates a uniquely named raw-Docker container on its
ephemeral runner, invokes the same dependency setup and `web` task, and uploads
only `.container-output/local/web-dist`. Repository build tools, tests, container privileges, and
repository permissions are not deployed as server capabilities. The artifact
contains the compiled PDF, reader assets, and registered WebAssembly
demonstrations. It does not contain `book.json` or a second runtime copy of the
LaTeX manuscript. GitHub Pages serves those static files and receives no
repository write capability.

## Layout

```text
web/
  src/        reader shell, reusable demo contract/registry, adapters, workers
  tools/      static build, WebAssembly build, tests, and local server
  wasm/       thin C-to-browser bridge
  tests/      native bridge agreement test
  dist/       generated static site (ignored)
```
