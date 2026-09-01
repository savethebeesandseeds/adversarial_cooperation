# Reusable Development Container Boundary

This document states the authority of the Adversarial Cooperation development
container and the evidence required before it is used. It deliberately does not
call the environment simply "secure." A container shares the host kernel and is
not a sandbox for hostile source code.

## Operational goal

The supported local environment is one named container,
`adversarial-cooperation-dev`. It exists so the author can start it, enter a
Linux shell, edit and build the mounted checkout, run tests, and stop it without
recreating or deleting it. The repository contains no Dockerfile or Compose
definition. The exact raw `docker run` command that was used and inspected is
recorded in the root README.

The intended runtime contract is:

- Docker Official `debian:13.6-slim` pinned by manifest digest;
- the complete repository mounted read-write at `/workspace`;
- the named `adversarial-cooperation-dev-home` volume mounted read-write at
  `/home/ac`;
- `setup.sh` mounted separately and read-only at `/bootstrap/setup.sh`;
- an inert long-running command, with `/workspace` as the working directory;
- container port 4173 published only at host address `127.0.0.1`;
- restart policy `no`, privileged mode disabled, and no Docker socket;
- no GPU or explicitly mapped host device;
- explicit project, role, and configuration-version labels; and
- ordinary project work as numeric UID/GID `65532:65532` with
  no-new-privileges.

Initial package provisioning is the exception to the ordinary identity. It is
invoked explicitly as root with a bounded capability allowlist so APT can
modify the container root filesystem and configure `/home/ac`. Normal start,
shell, build, test, manuscript, WebAssembly, and preview operations do not
require root.

## Read-write source authority

The author explicitly chose a read-write whole-repository bind for development.
Processes in the container can therefore read, modify, rename, or delete:

- canonical manuscript and implementation sources;
- uncommitted work;
- generated files; and
- `.git` metadata.

This is a deliberate usability-for-authority tradeoff. It supersedes the former
disposable container's read-only source allowlist and absent-Git boundary. Do
not use this environment to execute deliberately hostile source. Review Git
status before and after broad build or maintenance operations, and do not pass
host credentials into the container merely because the checkout is available.
A credential already stored inside the repository would still be visible.

The read-only `/bootstrap/setup.sh` path prevents the provisioning command from
accidentally modifying the file through that alias. It is not an integrity
boundary: the same host file remains reachable and writable as
`/workspace/setup.sh` through the author-selected read-write repository mount.
Provisioning therefore trusts the inspected repository source.

No Docker or container-runtime socket, unrelated host path, SSH agent, cloud
credential, package-registry token, GPU, or device is part of the documented
configuration. Their absence limits accidental reach; it does not compensate
for the authority of the read-write repository mount.

## Dependency setup

`setup.sh` is a dependency and environment configuration entry point only. It
must not create, start, stop, enter, rebuild, test, publish, serve, or delete a
container. It must not dispatch project operations. The host invokes it inside
the already-created container as:

```text
docker exec --user 0:0 adversarial-cooperation-dev /bin/bash /bootstrap/setup.sh
```

The script:

- refuses to run outside the expected container context;
- verifies the declared pinned base reference and absence of the Docker socket;
- validates the expected Debian APT source configuration;
- rejects insecure and unauthenticated APT modes;
- installs the explicit top-level package request with
  `--no-install-recommends` and recommended/suggested packages disabled;
- checks the package database and removes downloaded package archives;
- configures the non-root home; and
- writes `/var/lib/adversarial-cooperation/setup-complete` only after successful
  completion.

APT authenticates repository metadata and the package hashes named by that
metadata. This does not establish that every package is individually audited,
free of vulnerabilities, or safe against malicious input. The base manifest is
pinned, but authenticated Debian repositories can publish newer stable updates.
The environment is therefore functionally reconstructable and inspectable, not
bit-for-bit frozen.

Docker health records that setup completed and the core tools remain available;
it does not compare the marker with a later edit to `setup.sh`. Every project
task performs that hash comparison and refuses to run until dependency setup is
rerun.

Do not install unrecorded packages manually and then treat the writable
container layer as reproducible project configuration. Add a required
dependency to the reviewed `setup.sh` request and recreate only through an
explicitly authorized recovery operation.

## Project operations

Project operations live in `/workspace/container/tasks.sh`, separately from
dependency setup:

```text
AC_RUN_ID=web-unique bash container/tasks.sh web
AC_RUN_ID=verify-unique bash container/tasks.sh verify
bash container/tasks.sh serve
```

`web` checks browser modules, builds the complete PDF/static/WebAssembly site,
and runs its existing tests. `verify` additionally builds the native
companions, runs the complete native and sanitizer suites, and then runs
the same web build and tests. `serve` runs the web build and tests without
publishing an evidence bundle, then listens on container address
`0.0.0.0:4173`; Docker exposes that port only through host loopback.

Each command reads the declared inputs from the read-write checkout, copies them
to a fresh isolated run directory in the `/tmp` tmpfs, and builds there. Scratch
accumulates only for the current container session and is discarded when the
container stops or restarts. `web` and `verify` publish only verified results to
the write-once `.container-output/<run-id>/` path. This leaves pre-existing
generated checkout data untouched while preserving `/workspace` for direct
development. Success is implementation evidence for the inspected source and
environment; it does not prove any cryptographic property, protocol security
claim, or scientific novelty claim.

## Lifecycle and conflict handling

Normal use reuses the existing named container:

```text
docker start adversarial-cooperation-dev
docker exec -it adversarial-cooperation-dev /bin/bash
docker stop adversarial-cooperation-dev
```

Creation and recovery never imply deletion. Before creating anything, inspect
the exact container name and named home volume. If a matching managed container
exists, including in a stopped state, start and reuse it. If an object with the
same name has different labels, image, command, mounts, port, restart policy,
user, or device access, preserve it and report the conflict. Only a separately
authorized rebuild may replace an inspected managed container by immutable ID,
and it must preserve the named volume and host-mounted repository.

There is no normal `rm`, prune, recreate, or volume-reset step. A failed setup
leaves its incomplete state in the container layer. A failed task leaves its
scratch for diagnosis until the container stops or restarts; published evidence
is not overwritten.

## Trust and network limitations

The trusted computing base includes:

- the host hardware, operating system, and kernel;
- Docker Desktop, its daemon, and its storage;
- the digest-pinned Debian userspace;
- Debian archive keys, repositories, mirrors, and installed packages;
- the repository source and scripts; and
- GitHub Actions and GitHub Pages when continuous deployment is used.

Provisioning requires outbound access to Debian mirrors. The persistent
development container retains ordinary outbound network access after setup.
No claim of an offline, hermetic, or reproducible build follows from this
design. A compromised host, daemon, kernel, dependency, or repository process
can exceed the intended operational boundary.

## GitHub Pages boundary

The Pages workflow uses a uniquely named, labeled raw-Docker container on its
ephemeral runner. The checkout disables persisted Git credentials before it is
mounted read-write. The workflow mounts `setup.sh` read-only, performs root
dependency setup separately, and runs the `web` task as the runner's numeric
non-root UID/GID. It does not create a second local development service or
require Compose. The workflow uploads only
`.container-output/local/web-dist`.

The deployed directory contains the compiled book PDF, HTML, CSS, JavaScript,
and registered WebAssembly demonstrations. It contains no running container,
Docker socket, compiler, server-side write endpoint, serialized LaTeX
manuscript, or source map.

## Verification checklist

Before treating a recreated local environment as the documented development
container, verify:

1. Record the newly created immutable container ID and use that exact ID for
   the rest of the inspection. The configured image reference and resolved
   image ID must match the documented contract; a future authorized recreation
   necessarily receives a new container ID.
2. Management labels identify the expected project, development role, and
   configuration version.
3. The command, working directory, non-root user, restart policy, and hostname
   match the documented contract.
4. `/workspace` is the intended repository bind and is read-write;
   `/home/ac` is the expected named volume; `/bootstrap/setup.sh` is the intended
   read-only file bind; and no other host mount or Docker socket is present.
5. Port 4173 resolves only to `127.0.0.1:4173`, with no GPU or explicitly
   mapped device.
6. The setup-completion marker exists and the declared compiler, build,
   libsodium, Node.js, Emscripten, Python, and LaTeX tools are available.
7. An ordinary shell is non-root, has no-new-privileges, has empty inherited,
   permitted, effective, and ambient capability sets, and has only the reviewed
   provisioning allowlist in its capability bounding set.
8. `bash container/tasks.sh verify` passes, and `serve` is reachable only over
   host loopback.
9. Every unrelated container, image, volume, and network retains its prior ID
   and state, and preserved `.container-output` evidence is unchanged.

Passing this checklist establishes only these operational observations for the
inspected environment. It does not make containers a defense against hostile
code or turn successful tests into a cryptographic proof.
