## License
This work is licensed under a Creative Commons Attribution-NonCommercial 4.0 International License.

![Creative Commons License](https://i.creativecommons.org/l/by-nc/4.0/88x31.png)

For more details, see the [LICENSE](LICENSE) file in this repository.

## Description

This is a work in progress.

## Reusable development container

The supported environment is one reusable Linux container named
`adversarial-cooperation-dev`. There is no Dockerfile, Compose file, or host
launcher. The repository is mounted read-write at `/workspace`; this is useful
for development but also gives container processes authority to modify source,
uncommitted work, and `.git`. See
[`container/SECURITY.md`](container/SECURITY.md) for the exact boundary.

Before initial creation, inspect both names. If either command succeeds, do not
replace or relabel the object; inspect it and reuse it only if it matches the
documented configuration:

```powershell
docker container inspect adversarial-cooperation-dev
docker volume inspect adversarial-cooperation-dev-home
```

The following PowerShell commands are the exact commands used to create the
verified local environment from the repository root. The first command creates
the labeled persistent home volume. The second creates the non-root, idle
container without deleting it automatically:

```powershell
docker volume create `
  --label io.adversarial-cooperation.owner=adversarial-cooperation `
  --label io.adversarial-cooperation.role=development-home `
  --label io.adversarial-cooperation.config=single-dev-v1 `
  adversarial-cooperation-dev-home

docker run --detach `
  --name adversarial-cooperation-dev `
  --hostname adversarial-cooperation-dev `
  --platform linux/amd64 `
  --pull=missing `
  --label io.adversarial-cooperation.owner=adversarial-cooperation `
  --label io.adversarial-cooperation.role=development `
  --label io.adversarial-cooperation.managed-by=repository-readme `
  --label io.adversarial-cooperation.config=single-dev-v1 `
  --restart=no `
  --stop-timeout=10 `
  --init `
  --user 65532:65532 `
  --workdir /workspace `
  --env HOME=/home/ac `
  --env USER=ac `
  --env LOGNAME=ac `
  --env AC_CONTAINER_CONTEXT=adversarial-cooperation-dev-v1 `
  --env AC_BASE_IMAGE=debian:13.6-slim@sha256:020c0d20b9880058cbe785a9db107156c3c75c2ac944a6aa7ab59f2add76a7bd `
  --env AC_UID=65532 `
  --env AC_GID=65532 `
  --security-opt no-new-privileges=true `
  --cap-drop ALL `
  --cap-add CHOWN `
  --cap-add DAC_OVERRIDE `
  --cap-add FOWNER `
  --cap-add FSETID `
  --cap-add SETFCAP `
  --cap-add SETGID `
  --cap-add SETPCAP `
  --cap-add SETUID `
  --pids-limit 1024 `
  --tmpfs '/tmp:rw,nosuid,nodev,exec,mode=1777,size=4g' `
  --tmpfs '/run:rw,nosuid,nodev,noexec,mode=755,size=16m' `
  --mount "type=bind,source=$((Get-Location).Path),target=/workspace" `
  --mount "type=bind,source=$((Resolve-Path -LiteralPath .\setup.sh).Path),target=/bootstrap/setup.sh,readonly" `
  --mount 'type=volume,source=adversarial-cooperation-dev-home,target=/home/ac,volume-nocopy' `
  --publish 127.0.0.1:4173:4173/tcp `
  --health-cmd 'test -r /var/lib/adversarial-cooperation/setup-complete && command -v gcc >/dev/null && command -v make >/dev/null && command -v node >/dev/null && command -v emcc >/dev/null && command -v latexmk >/dev/null && pkg-config --exists libsodium' `
  --health-interval=30s `
  --health-timeout=5s `
  --health-retries=3 `
  --health-start-period=30m `
  --entrypoint /bin/sleep `
  'debian:13.6-slim@sha256:020c0d20b9880058cbe785a9db107156c3c75c2ac944a6aa7ab59f2add76a7bd' `
  infinity
```

Provision the declared dependencies once, separately from container lifecycle
and project work:

```powershell
docker exec --user 0:0 --workdir / adversarial-cooperation-dev /bin/bash /bootstrap/setup.sh
```

After creation and setup, inspect the container and volume, then complete the
short verification checklist in
[`container/SECURITY.md`](container/SECURITY.md#verification-checklist) before
treating the environment as reproduced.

Docker health means that one setup run completed and the core tools remain
available. After `setup.sh` changes, rerun the root setup command; project tasks
also compare the recorded setup hash and refuse a stale environment.

Normal use reuses the same container, including when it is stopped:

```powershell
docker start adversarial-cooperation-dev
docker exec -it adversarial-cooperation-dev /bin/bash
docker stop adversarial-cooperation-dev
```

Do not run `docker run` again for normal use, and do not delete the container or
named volume as a recovery shortcut. `setup.sh` installs the complete declared
dependency set with recommended and suggested packages disabled; it performs no
build, test, preview, publication, or Docker lifecycle operation. Project work
runs through the separate in-container task surface:

```bash
AC_RUN_ID=verify-unique bash container/tasks.sh verify
AC_RUN_ID=web-unique bash container/tasks.sh web
bash container/tasks.sh serve
```

`web` and `verify` publish write-once evidence beneath
`.container-output/<run-id>/`; choose a new run ID each time. Preview is
available at `http://127.0.0.1:4173` while `serve` is running; `serve` runs the
same web build and tests but does not publish an evidence bundle. The base and
architecture are pinned, but authenticated Debian repositories can advance, so
recreation is inspectable rather than bit-for-bit frozen.

## Compile book

The in-container verification task compiles the corrected full-book entry point
and exports its artifacts without requiring LaTeX on the host:

```bash
AC_RUN_ID=verify-unique bash container/tasks.sh verify
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
AC_RUN_ID=web-unique bash container/tasks.sh web
bash container/tasks.sh serve
```

The generated site is exported to `.container-output/<run-id>/web-dist/` and
uses only relative URLs, so the same artifact can be served locally or below a
GitHub Pages repository path. Each task snapshots the current declared inputs
to a fresh work directory in the container's `/tmp` tmpfs. This preserves
pre-existing generated checkout data while still letting you edit `/workspace`
directly; scratch remains until the container stops or restarts. `serve` builds
and tests from that snapshot without publishing an evidence bundle, then
listens on container port 4173, which the documented container publishes only
through `127.0.0.1`. See `web/README.md` for the reusable demonstration contract
and build boundary.

## Compile scripts

The repository retains these lower-level `book` helper targets. They are build
implementation entry points inside the mounted workspace, not instructions to
install or run the toolchain directly on the host:

```bash
bash book -list
bash book -pdf section_poker
bash book -pdf section_rps
bash book -pdf section_hash
bash book -pdf section_trust
bash book -pdf section_ttt
```

Within the mounted working tree, the helper outputs are:

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
