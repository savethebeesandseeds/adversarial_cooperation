# Development container

Run these commands from the repository root. This folder contains versioned
container instructions and project tasks. Other `.local/` subfolders hold
ignored machine-local data.

The root `setup.sh` remains in place because the existing container directly
bind-mounts that host file at `/bootstrap/setup.sh`. Relocating it would require
an explicitly authorized container configuration change; the organization task
did not recreate the container or install dependencies.


The supported environment is one reusable Linux container named
`adversarial-cooperation-dev`. There is no Dockerfile, Compose file, or host
launcher. The repository is mounted read-write at `/workspace`; this is useful
for development but also gives container processes authority to modify source,
uncommitted work, and `.git`. See
[`SECURITY.md`](SECURITY.md) for the exact boundary.

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
[`SECURITY.md`](SECURITY.md#verification-checklist) before
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
AC_RUN_ID=verify-unique bash .local/container/tasks.sh verify
AC_RUN_ID=web-unique bash .local/container/tasks.sh web
bash .local/container/tasks.sh serve
```

`web` and `verify` publish write-once evidence beneath
`.local/evidence/<run-id>/`; choose a new run ID each time. Preview is
available at `http://127.0.0.1:4173` while `serve` is running; `serve` runs the
same web build and tests but does not publish an evidence bundle. The base and
architecture are pinned, but authenticated Debian repositories can advance, so
recreation is inspectable rather than bit-for-bit frozen.
