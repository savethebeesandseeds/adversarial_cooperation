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

The supported environment requires only Docker Engine with Docker Compose on
the host. The checked-in `setup.sh` installs Node.js, Emscripten, a native C
compiler, and the LaTeX packages needed to compile the book for the web and
preview modes. The broader verify mode additionally installs the existing
libsodium development backend. There are no JavaScript package dependencies
and no host toolchain installation. The
Compose build commands are shared; only artifact-ownership variables differ on
POSIX hosts. Command portability does not claim bit-identical output across
processor architectures. Each run records the selected platform and
architecture.

Before `web` or `verify`, create the ignored `.container-output/` directory
once after checkout. Compose deliberately refuses to create that missing host
bind path. Preview uses isolated container storage and does not need it.
On POSIX hosts, export `AC_UID=$(id -u)` and `AC_GID=$(id -g)` so the
unprivileged build can create its run directory without changing host
ownership. Docker Desktop hosts may use the Compose defaults.

From the repository root:

```text
docker compose run --rm -e AC_RUN_ID=web-local toolchain web
docker compose up preview
```

Open `http://127.0.0.1:4173`. Compose publishes no non-loopback preview port.
The preview service performs its own build in ephemeral container storage and
publishes no host artifact. Stop and start it again without choosing or
clearing a run ID. The `web` and `verify` operations are different: they copy
complete results to `.container-output/<run-id>/` and fail closed if that run
already exists. All URLs in `web-dist/` are relative, so the same directory
works at a GitHub Pages project subpath.

`docker compose run --rm toolchain verify` performs the broader manuscript and
native-companion verification. The source allowlist is mounted read-only;
`.container-output/` is the only writable project bind mount. The provisioning,
package, and shared-kernel limitations are recorded in
[`container/SECURITY.md`](../container/SECURITY.md).

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

The Pages workflow runs exactly `docker compose run --rm toolchain web` and
uploads only `.container-output/pages-<run-id>-<attempt>/web-dist`. Repository
build tools, tests, container privileges, and repository permissions are not
deployed as server capabilities. The artifact contains the compiled PDF,
reader assets, and registered WebAssembly demonstrations. It does not contain
`book.json` or a second runtime copy of the LaTeX manuscript. GitHub Pages
serves those static files and receives no repository write capability.

## Layout

```text
web/
  src/        reader shell, reusable demo contract/registry, adapters, workers
  tools/      static build, WebAssembly build, tests, and local server
  wasm/       thin C-to-browser bridge
  tests/      native bridge agreement test
  dist/       generated static site (ignored)
```
