## License
This work is licensed under a Creative Commons Attribution-NonCommercial 4.0 International License.

![Creative Commons License](https://i.creativecommons.org/l/by-nc/4.0/88x31.png)

For more details, see the [LICENSE](LICENSE) file in this repository.

## Description

This is a work in progress.

## Dedicated build environment

The supported build path is the project's own disposable Debian container. It
uses `compose.yaml` and the repository's visible `setup.sh`; there is no
Dockerfile and no dependency installation on the host. Before a `web` or
`verify` publication, create the ignored `.container-output/` directory once
after checkout; Compose deliberately refuses to create a missing host bind
path. Preview does not use that host directory. Docker Compose then provides
the same command interface on Windows, Linux, and macOS:

On POSIX hosts, export `AC_UID=$(id -u)` and `AC_GID=$(id -g)` first so the
non-root build identity owns its artifacts. GitHub Actions does this
explicitly; Docker Desktop hosts may use the conservative Compose defaults.

```bash
docker compose run --rm -e AC_RUN_ID=verify-local toolchain verify
docker compose run --rm -e AC_RUN_ID=web-local toolchain web
docker compose up preview
```

Command portability does not imply bit-identical cross-architecture output.
The selected container platform and machine architecture are recorded with the
environment evidence for each run.

`preview` performs its own build in disposable container storage and does not
publish or overwrite an artifact run. The preview is available only at
`http://127.0.0.1:4173`. The `web` and `verify` operations publish reviewed
results beneath `.container-output/<run-id>/` and refuse to overwrite an
existing run. Among project bind mounts, `.container-output/` is the only
writable host path; repository inputs are mounted read-only.
Do not run `setup.sh` directly on the host: it is a provisioning entry point
for the constrained container described by `compose.yaml`.

The provisioning phase runs as root and needs network access to authenticated
Debian repositories. Compilation then runs as a numeric non-root user with no
Linux capabilities and no-new-privileges. The container receives no Docker
socket, deliberately passed host credentials, Git metadata, explicitly mapped
host devices, or mounts belonging to another project. See
[`container/SECURITY.md`](container/SECURITY.md) for the exact boundary and its
limitations.

This arrangement is inspectable, but it is not a claim that containers can
safely execute hostile code. Docker shares the host kernel, the Docker daemon
and host remain trusted, and signed repository metadata does not prove that a
package is free of vulnerabilities. Each run therefore records the package and
tool versions actually used together with artifact hashes.

The C companions use the C11 standard library plus one cryptographic backend,
libsodium. The protocol and state-machine code do not depend on any other
third-party C library. Node.js, Emscripten, LaTeX, and related Debian packages
are build tools only. `setup.sh` contains the complete top-level package
request and disables recommended and suggested packages. APT still selects
mandatory dependencies, and the resulting installed closure is recorded for
each run.

## Compile book

The dedicated verification operation compiles the corrected full-book entry
point and exports its artifacts without requiring LaTeX on the host:

```bash
docker compose run --rm toolchain verify
```

The main manuscript is now the inclusive **Complete Research Draft**. Its five
parts expose all **27 live content sources**: 22 chapters and research notes,
followed by five appendices. Early fragments and placeholders carry visible
research-status notices alongside the more developed Trust Establishment,
Rock--Paper--Scissors, Tic-Tac-Toe, Poker, and Hash Functions material.

Presence in the Complete Research Draft means that an idea is preserved and
visible. It does **not** mean that the chapter is mature, that its claims are
validated, that its protocol is secure, or that its contribution is novel.
Build and test evidence remains scoped to the exact chapters and executable
companions named in their verification records.

## Browser reader and demonstrations

The static website has three deliberately small areas:

- **Read** embeds the PDF compiled from the canonical LaTeX manuscript and
  provides a direct PDF link.
- **Demos** lists only executable companions that have been explicitly
  registered and built. The first is the disclosed-policy Tic-Tac-Toe checker,
  compiled from the existing C implementation to WebAssembly.
- **About** states the source and evidence boundaries in plain language.

The site does not parse or serialize the manuscript into a second browser
document. It publishes no `book.json`, source map, raw-LaTeX view, comment
system, or manuscript-writing interface. Selecting text in the PDF is ordinary
browser/PDF-viewer selection; changes still belong in the canonical LaTeX
source through the normal research workflow.

```bash
docker compose run --rm toolchain web
docker compose up preview
```

The generated site is exported to
`.container-output/<run-id>/web-dist/` and uses only relative URLs, so the same
artifact can be served locally or below a GitHub Pages repository path. The
preview is rebuilt in ephemeral container storage and served on
`http://127.0.0.1:4173`; it does not consume a write-once artifact name. See
`web/README.md` for the reusable demonstration contract and build boundary.

## Compile scripts

The repository retains these lower-level `book` helper targets. They are build
implementation entry points inside the ephemeral workspace, not instructions
to install or run the toolchain directly on the host:

```bash
bash book -list
bash book -pdf section_poker
bash book -pdf section_rps
bash book -pdf section_hash
bash book -pdf section_trust
bash book -pdf section_ttt
```

Within that ephemeral working tree, the helper outputs are:

- `demostrations/.build/pdf/section_poker.pdf`
- `demostrations/.build/pdf/section_rps.pdf`
- `demostrations/.build/pdf/section_hash.pdf`
- `demostrations/.build/pdf/section_trust.pdf`
- `demostrations/.build/pdf/section_ttt.pdf`

## C demonstrations
The repository now includes:

- `src/`: C source code for game demonstrations.
- `demostrations/`: `.tex` notes mirroring `document/content` structure.

The `verify` container operation builds and tests the implemented examples.
Their lower-level target inventory is:

```bash
make -C demostrations demo_poker
make -C demostrations demo_rps RPS_ARGS='--alice rock --bob scissors'
make -C demostrations demo_rps RPS_ARGS='--alice rock --bob scissors --selective-abort'
make -C demostrations demo_hash
make -C demostrations demo_ttt
make -C demostrations demo_ttt_bind
make -C demostrations demo_ttt_bind TTT_BIND_ARGS='--reference-vector'
make -C tests test
```

The hash companion calls the existing libsodium BLAKE2b backend through one
fixed-output wrapper. It does not add a library, implement a hash algorithm, or
provide password hashing, authentication, signatures, encryption, or a
production API.

The Tic-Tac-Toe companion is the book's canonical separation of three layers:
the public finite-game predicate (`Core`), an exact disclosed-opening relation
(`Bind`), and a future private proof layer that is not yet implemented. The
recursive checker, fixed DAG, and Boolean Core use plain C11 and disclose the
policy. The separate Bind driver reuses the repository's sole cryptographic
backend, libsodium, and can replay a deterministic valid reference policy with
`--reference-vector`. Neither path implements zero knowledge, proof of
knowledge, private evaluation, authentication, fairness, or anti-cloning.
