# Dedicated Container Boundary

This document states what the project container is designed to isolate, what
authority it still has, and what evidence it records. It deliberately does not
call the environment simply "secure." A container is one control within a
larger trusted system; it is not a proof that source code or dependencies are
benign.

## Protected boundary

The operational goal is to build this repository without modifying its source
checkout, entering another project's container, or acquiring broad host
authority. The project protects against common accidental contamination by
giving the container:

- a Docker Official Debian stable-slim userspace pinned by image digest;
- only an enumerated set of project inputs, mounted read-only beneath
  `/source`;
- one project-owned writable artifact mount at `/artifacts`, backed by the
  host's `.container-output/` directory;
- no Docker or container-runtime socket;
- no explicitly mapped host devices, privileged mode, or mounts belonging to
  another project;
- no `.git` directory or deliberately passed or mounted SSH agent, cloud
  credential, package-registry token, or other secret; and
- a loopback-only host port for the optional static preview.

The container is not authorized to repair, clean, rename, commit, or otherwise
write the canonical source. It copies the read-only inputs to an ephemeral
working tree inside the disposable container. Its host-visible project writes
are confined to explicit build results and audit records under `/artifacts`;
Docker still maintains its own root filesystem and runtime metadata elsewhere.

The read-only source allowlist is declared as individual mounts in
`compose.yaml`. It contains the build entry points and these project trees:

```text
Makefile.config
book
compile_latex.sh
demostrations/
document/
include/
src/
test-vectors/
tests/
web/
```

No other canonical source trees or files are exposed. In particular, mounting
the repository root as a whole would violate this boundary because it would
expose Git metadata and unrelated working files. `setup.sh` and `compose.yaml`
are separate provisioning and orchestration inputs, mounted read-only at
`/bootstrap/setup.sh` and `/bootstrap/compose.yaml`; they are not copied into
the ephemeral manuscript work tree.

## Two phases, two kinds of authority

### 1. Provisioning

Every fresh container starts from the digest-pinned Debian base and runs the
repository's `setup.sh`. There is no Dockerfile or private prebuilt project
image. The provisioning phase:

- runs as root because APT must modify the container root filesystem;
- uses network access to Debian's configured `main`, updates, and security
  archives;
- requires the pinned base image's sole deb822 source file to match its exact
  recorded SHA-256, so added fields or stanzas are rejected;
- accepts only repositories whose Release metadata is authenticated by APT's
  configured Debian archive keys;
- explicitly refuses insecure and unauthenticated APT modes;
- installs an explicit top-level package request with
  `--no-install-recommends`, while also disabling recommended and suggested
  dependencies; and
- cleans downloaded package indexes after APT completes. The installed package
  database is recorded before the privilege transition.

APT's authentication model verifies signed repository metadata and the hashes
that metadata assigns to packages. It does not mean that every package file is
individually signed, audited by this project, free of vulnerabilities, or safe
against malicious input. Disabling recommends and suggests reduces the amount
of installed code; it does not make the packages that remain intrinsically
safe. Mandatory transitive dependencies and packages already present in the
base image remain part of the installed closure and are included in the
version record.

The exact top-level profiles are deliberately short and visible:

```text
web / serve:
  build-essential ca-certificates emscripten latexmk make nodejs python3
  texlive-fonts-recommended texlive-latex-base texlive-latex-extra
  texlive-latex-recommended texlive-science util-linux

verify additions:
  libsodium-dev pkg-config
```

This list describes direct requests, not the larger APT-resolved installed
closure.

The base image digest is immutable, but the signed Debian package repositories
can publish newer stable updates. Consequently this is an auditable build, not
a bit-for-bit frozen environment. Each run records the exact package versions,
tool versions, base reference, selected platform and architecture, setup hash,
and produced artifact hashes.

### 2. Build, verification, and preview

Compose drops all capabilities and restores only `CHOWN`, `DAC_OVERRIDE`,
`FOWNER`, `FSETID`, `SETFCAP`, `SETGID`, `SETPCAP`, and `SETUID` for the root
provisioning phase. Those capabilities allow package setup, creation of the
ephemeral work tree, and the privilege transition; they are not retained by
the build phase.

After provisioning, `setup.sh` copies the allowlisted input into ephemeral
container storage and re-executes the selected operation as the numeric
`AC_UID:AC_GID` (default `65532:65532`). That phase is intended to have all of
the following properties:

- UID is nonzero;
- Linux capabilities are empty;
- no-new-privileges is active;
- source mounts remain read-only;
- generated files are confined to ephemeral storage and `/artifacts`; and
- preview serving binds inside the container and is published by Compose only
  on host address `127.0.0.1`.

These properties constrain ordinary processes. They do not defend against a
kernel vulnerability, a compromised Docker daemon, a malicious host
administrator, or deliberately hostile build source.

## Network boundary

Root provisioning requires outbound network access to Debian mirrors. The
later non-root build phase retains ordinary outbound network reachability. A
single running container cannot have its Compose network removed after APT
without either a second built image or additional host-side orchestration; both
would obscure the simple `setup.sh` boundary chosen for this project.

Compose deliberately passes no host credentials, and the build has read-only
source access, which limits what such connectivity can expose. Secrets already
present inside an allowlisted source file would still be visible and must be
prevented by repository hygiene. These measures do not make the build offline.
A future offline or hermetic mode would require a separately reviewed package
snapshot and cache design.

## Shared-kernel and supply-chain limitations

Docker containers share the host kernel. The trusted computing base therefore
includes:

- the host hardware, operating system, and kernel;
- the Docker daemon and Compose implementation;
- the digest-pinned Debian base image;
- Debian archive keys, repositories, packages, and mirrors;
- GitHub Actions and GitHub Pages when continuous deployment is used; and
- the project sources and build scripts themselves.

This environment is not a sandbox for arbitrary untrusted code. It does not
establish reproducible-build equality across runs, dependency freedom,
cryptographic security of a protocol, freedom from compiler defects, or
absence of compromised upstream software. Package and artifact manifests make
changes inspectable; they do not make those claims true.

## Supported commands

From the repository root on any host with Docker Engine and Docker Compose:

```bash
# Full manuscript and native-companion verification
docker compose run --rm -e AC_RUN_ID=verify-local toolchain verify

# Static book and WebAssembly build plus agreement tests
docker compose run --rm -e AC_RUN_ID=web-local toolchain web

# Build and serve a fresh loopback-only preview artifact
docker compose up preview
```

The ignored `.container-output/` host directory must exist before `web` or
`verify`. Compose deliberately refuses to create a missing toolchain bind
source. Preview does not mount that directory.

Set `AC_RUN_ID` to a simple unique identifier when several `web` or `verify`
artifact sets must coexist. The default is `local`; GitHub Pages uses
`pages-<run-id>-<attempt>`. Those artifact destinations are write-once and
appear only after an atomic rename on the artifact filesystem: an existing
run-ID directory causes a fail-closed error instead of an overwrite. Preview
instead builds and serves from disposable container storage. It publishes no
artifact run and therefore needs no preview run ID. Its health check allows a
30-minute cold-provisioning window for the Dockerfile-free TeX and Emscripten
installation, but a successful probe marks it healthy immediately.
On POSIX hosts, `AC_UID` and `AC_GID` must be set to the numeric owner of the
host artifact directory (normally `$(id -u)` and `$(id -g)`). A conventional
user-owned mode-0755 directory is not writable by the default `65532:65532`, so
the build intentionally fails rather than changing host ownership. The Pages
workflow sets the runner IDs explicitly. Docker Desktop hosts that do not
expose ordinary POSIX ownership may use the conservative Compose defaults.

Do not run `setup.sh` directly on the host. It performs system package
installation and validates that it is running inside the intended container
boundary.

The commands are platform-neutral at the interface. Unless `compose.yaml`
selects one architecture explicitly, a multi-architecture image reference may
resolve to different child images on different hosts. The environment record
makes that choice visible; it does not establish identical output across
architectures.

## GitHub Pages boundary

The Pages workflow runs:

```text
docker compose run --rm toolchain web
```

and uploads only:

```text
.container-output/pages-<run-id>-<attempt>/web-dist
```

That directory is a static deployment artifact containing the compiled book
PDF, HTML, CSS, JavaScript, and registered WebAssembly demonstrations. It
contains no serialized LaTeX manuscript, source map, running container, Docker
socket, build compiler, server-side write endpoint, or host filesystem
permission. The published reader requests no local-checkout directory grant
and contains no client-side manuscript write path.

## Audit checklist

Before treating a run as verification evidence, check that:

1. `docker compose config` resolves the expected digest-pinned Debian image.
2. Only containers under the chosen Adversarial Cooperation Compose project
   change state.
3. The build report records a nonzero UID, empty capability sets, and
   no-new-privileges.
4. Every canonical source mount reports read-only, the write probe against
   `/source/web` fails, and `.git` is absent.
5. Among project bind mounts, the only writable host path is
   `.container-output/`. The setup publishes to one write-once run directory,
   but the mount boundary itself includes sibling runs; do not execute hostile
   build source on that basis.
6. Installed package and tool versions, plus platform and architecture
   identifiers, are present in the environment record.
7. Artifact hashes match the files that are reviewed or deployed.
8. Preview publication resolves only to `127.0.0.1:4173`.

Passing this checklist establishes only the named operational observations for
that run. It does not turn a successful build or test into a scientific or
cryptographic proof.
