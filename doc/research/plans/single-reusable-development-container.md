# Execution Plan: Single Reusable Development Container

## Status

Complete (2026-09-01).

## Objective

Replace the repository's two disposable Docker Compose services with one
explicitly named, reusable Linux container for development, compilation,
testing, manuscript work, WebAssembly generation, and local preview.

The authoritative creation procedure will be a raw `docker run` command in
`README.md`. The command recorded there must be the exact command actually
used and verified on this host; it will not be presented as tested before that
run succeeds. The repository will contain neither a Dockerfile nor a Compose
file.

## Author Decisions

The author explicitly selected all of the following on 2026-09-01:

1. Use one reusable container rather than separate toolchain and preview
   containers.
2. Mount the repository read-write for direct development and testing.
3. Do not use a Dockerfile.
4. Delete `compose.yaml` after its replacement is complete and verified.
5. Do not add Windows `.cmd`, PowerShell, or other host lifecycle launchers.
6. Put the exact raw Docker creation command and the small daily workflow in
   `README.md`.

## Global-Policy Requirements

The migration must satisfy the current global `AGENTS.md` container rules:

- `setup.sh` installs reproducible dependencies and configures the environment
  only. It must not create, start, stop, enter, build, test, publish, serve, or
  otherwise manage the container or project lifecycle.
- Before Docker mutation, inspect and preserve every same-named container and
  named volume. A mismatch is a blocker, not permission to replace it.
- State the exact name, image, command, bind mounts, named volumes, ports,
  restart policy, labels, and GPU/device access before creation.
- Normal use starts and reuses the named container, including when stopped.
- No normal operation deletes or replaces the container or its named volume.
- After creation, verify the immutable ID and every declared runtime field.

## Baseline

- Repository status is clean at `master...origin/master` before edits.
- The current supported definition is `compose.yaml` with `toolchain` and
  `preview` services.
- The current `setup.sh` mixes dependency installation with build, test,
  publication, and preview operations, which conflicts with the newer global
  rule.
- No current or stopped Docker object belongs to the
  `adversarial-cooperation` project, and the intended pinned image was absent
  from the local image store at the initial audit.
- Preserved evidence under
  `.container-output/reader-standard-20260825b/` matches all 135 current build
  inputs and all 21 recorded artifact checksums. It must not be overwritten or
  removed.

## Target Runtime Contract

The verified local object contract is:

- container name: `adversarial-cooperation-dev`;
- hostname: `adversarial-cooperation-dev`;
- base image:
  `debian:13.6-slim@sha256:020c0d20b9880058cbe785a9db107156c3c75c2ac944a6aa7ab59f2add76a7bd`;
- canonical repository bind: the current repository root to `/workspace`,
  read-write;
- named persistent home/cache volume:
  `adversarial-cooperation-dev-home` to `/home/ac`, read-write;
- working directory: `/workspace`;
- host port: `127.0.0.1:4173` to container `4173/tcp`;
- restart policy: `no`;
- privileged mode: disabled;
- Docker socket: absent;
- GPU and explicitly mapped devices: none;
- root filesystem: writable because first creation provisions Debian packages;
- provisioning identity: root with the existing bounded capability allowlist;
- steady-state identity: numeric UID/GID `65532:65532`, no-new-privileges,
  empty inherited, permitted, effective, and ambient capability sets, and a
  bounding set restricted to the reviewed provisioning allowlist
  (`00000000800001db` on the pinned `linux/amd64` runtime);
- steady-state command: an inert long-running process so the user can enter a
  shell and work inside the already-provisioned container;
- explicit management labels identifying the project, role, and configuration
  version;
- health: a first-setup completion marker plus presence of the required core
  tools and libsodium metadata.

The exact raw volume-creation, container-creation, and setup commands now in
`README.md` were run successfully from PowerShell at the repository root. The
container was subsequently inspected against every declared runtime field.

Automated project tasks copy the current declared source inputs to a fresh run
directory beneath the container's `/tmp` tmpfs and build there. Only verified bundles
are published to a new `.container-output/<run-id>/` directory in the checkout.
This is required because pre-existing generated directories from the deleted
container can retain a different numeric owner; they are preserved rather than
deleted or recursively relabeled. Run scratch accumulates only for the current
container session and is discarded when the container stops or restarts.

## Source and Trust-Boundary Change

The read-write whole-repository bind deliberately supersedes the former
read-only allowlist. It exposes `.git` and every repository file to processes
inside the container and permits those processes to alter canonical source and
generated files. No Docker socket, host credential, unrelated host path, GPU,
or device is added. This is an explicit usability-for-authority tradeoff, not
a claim that the container can safely execute hostile code.

Docker, the host kernel and daemon, the pinned Debian userspace, live
authenticated Debian repositories, installed packages, and repository source
remain trusted. The base manifest is pinned, but Debian package versions and
multi-architecture resolution are not bit-for-bit frozen.

## Intended File Scope

Create:

1. `research/plans/single-reusable-development-container.md`

Modify as required by consistency checks:

2. `setup.sh`
3. `README.md`
4. `web/README.md`
5. `container/SECURITY.md`
6. `.github/workflows/pages.yml`
7. `research/DECISIONS.md`
8. `web/tools/build-wasm.mjs`
9. `.gitignore`
10. `research/RENAME_MAP.md`

Delete after all authoritative references have migrated:

11. `compose.yaml`

Create only if the operation-preservation audit confirms it is the smallest
coherent surface:

12. `container/tasks.sh`

Any additional task-runner file must be justified as necessary to preserve an
existing build/test/publication capability and must run inside the container;
no host lifecycle script is permitted by this plan.

No manuscript chapter, bibliography, backup, C public API, cryptographic
primitive, protocol, test vector, or scientific claim is in scope.

## Authoritative Preimages

The following SHA-256 values were recorded before modifying each existing
scoped file. A guarded edit must stop if a file no longer matches its preimage
at the moment it is first changed:

```text
2d69aacdc9abbb46c70046112a656deaf17c6d5d68293ecfaaf64e1b4171a392  setup.sh
1612046a9183f6af251577f58e293aae6b25dc2f108647d7586de0dd4c81e2fd  README.md
4ac3afc6dec6ca86544fefc3d7e2615136b855d6850c763c1cff3b2d3f45084d  web/README.md
2bdeabd0d3837486263e111b2987e432cd1739aa4ee09817cc174ab4bdd010a7  container/SECURITY.md
6100a6faa8adeb9851553ad5b72454321eee9a4e38b63354ab4bcccaa2399504  .github/workflows/pages.yml
99a506f61147e60e684696f398b2a915e2ed79599613975cbb596c534d69f4e1  research/DECISIONS.md
7422c31d0ba06cd9d0fcb79947a8aa32f313d93e431a6f58b137c07d0749d5dc  compose.yaml
e9f4fce9454f2e4bf04cb305b0608555b0fb719a963dd273d9ab68c3815fc480  web/tools/build-wasm.mjs
91421443828ddcd9b256c58857d9d67e6646480c159d6391327e1bc5dc8ea499  .gitignore
ec688d177abc4fd02ec0d3ce77181f1cd5caea068c23b9170ce27f2a926b6637  research/RENAME_MAP.md
```

`research/plans/single-reusable-development-container.md` was absent before
this plan was opened. `container/tasks.sh` was also confirmed absent before
creation.

## Work Sequence

1. Inventory every Compose and container-procedure reference.
2. Freeze hashes of all scoped files and recheck the clean worktree.
3. Refactor `setup.sh` to dependency installation and environment setup only.
4. Preserve project build/test/web operations outside `setup.sh` using the
   smallest existing or single in-container task surface justified by the
   audit.
5. Replace local and CI Compose invocations with explicit raw Docker behavior.
6. Rewrite the operational-boundary documentation and record the superseding
   decision without erasing historical decisions or evidence.
7. Delete `compose.yaml` only after no live instruction or workflow depends on
   it.
8. Run shell, YAML, JavaScript, Makefile, documentation-reference, and Git-diff
   checks before Docker mutation.
9. Re-inventory Docker; stop on any name, label, volume, port, or mount
   conflict.
10. State the complete exact runtime configuration to the author, create only
    the named container and named volume, and preserve failure state.
11. Verify the immutable container configuration, dependency environment,
    non-root shell, build, tests, sanitizers, manuscript, WebAssembly, and
    loopback preview.
12. Confirm all unrelated Docker object IDs/states and preserved artifact
    evidence are unchanged.
13. Copy the exact successful creation command into `README.md`, rerun
    consistency checks, and complete this record.

## Verification Standard

At minimum, completion requires:

- no Dockerfile or Compose file anywhere in the current repository;
- no active documentation, workflow, or code invocation of or dependency on
  Docker Compose;
- `setup.sh` syntax validation and proof that it exposes no lifecycle or
  project-operation modes;
- exact container name, labels, image ID/reference, command, mounts, volume
  identity, port binding, restart policy, privilege/capability configuration,
  and absence of devices/socket confirmed by inspection;
- an interactive non-root shell in `/workspace` with the read-write bind
  demonstrated by a harmless generated-file path, not by modifying canonical
  source;
- native builds and the complete test and sanitizer suites passing;
- manuscript and browser/WebAssembly builds and their existing checks passing;
- local preview reachable only through `127.0.0.1:4173`;
- the README command byte-for-byte equivalent to the successfully used
  creation command apart from the documented repository-path expansion;
- no unrelated Docker object changed state; and
- no pre-existing artifact or source file overwritten outside the reviewed
  scope.

## Claim Boundary

This migration changes developer operations and container authority only. It
introduces, strengthens, weakens, or removes no cryptographic, mathematical,
historical, protocol, or novelty claim. Passing builds and tests will establish
only implementation observations for the inspected container and repository
state.

## Progress Record

- 2026-09-01: Goal created and author decisions recorded.
- 2026-09-01: Global/project instructions reread; clean repository baseline
  confirmed. No repository or Docker mutation had occurred before this plan
  was created.
- 2026-09-01: Dependency setup was separated from project operations;
  `container/tasks.sh` became the one in-container `web`, `verify`, and `serve`
  surface; raw Docker replaced the active Compose procedure; and
  `compose.yaml` was removed. No Dockerfile or host launcher was added.
- 2026-09-01: Static checks passed with Git Bash `bash -n` and `git diff
  --check`. No Dockerfile or Compose definition remains in the repository.
- 2026-09-01: Created the labeled volume
  `adversarial-cooperation-dev-home` and container
  `adversarial-cooperation-dev` with the exact README commands. The immutable
  container ID is
  `087f758ebdc8c512223a1414e871d84b3850a6491483b390900b12b2dc661738`;
  the resolved image ID is
  `sha256:2a96bd19eac5b76f4930e550d8a34d16ede5e791cad461e8c526d3a19b2d5eeb`.
- 2026-09-01: First dependency setup completed with 274 installed binary
  package records. A second execution returned the recorded-current path and
  installed nothing, establishing setup idempotence for the inspected state.
- 2026-09-01: Runtime inspection found the ordinary non-root process has zero
  inherited, permitted, effective, and ambient capabilities. Docker retains
  `00000000800001db` in `CapBnd`, exactly matching the creation-time
  provisioning allowlist. The task assertion and documentation were corrected
  to this observed model without changing or rebuilding the container.
- 2026-09-01: The first task attempt stopped before building because GNU
  `install -m` cannot chmod the existing Windows 9p-backed artifact directory.
  Ordinary `mkdir` replaced permission-setting directory creation. The second
  attempt reached compilation but stopped because generated `.temp` and
  `demostrations/.build` trees left by the deleted container are owned by root.
  Those trees and the failed publication stage were preserved; none was
  deleted, relabeled, or overwritten. Automated tasks now build from a fresh
  declared-input snapshot in the container's `/tmp` tmpfs and publish only a new
  verified bundle.
- 2026-09-01: Full verification passed from isolated run
  `verify-single-dev-20260901c`: all 72 native unit/adversarial tests and the
  same 72 tests under AddressSanitizer/UndefinedBehaviorSanitizer passed; the
  112-page manuscript compiled; the static-site, demo-contract, UI, and six
  native/WebAssembly bridge tests passed; and all 27 published artifact hashes
  verified. The record contains 145 input hashes and the 274-package audit.
- 2026-09-01: The earlier preserved
  `.container-output/reader-standard-20260825b` bundle still verifies all 21
  hashes. The new PDF is 638,816 bytes and the WebAssembly module is 5,610
  bytes.
- 2026-09-01: `serve` rebuilt successfully. Windows received HTTP 200 for the
  reader and PDF at `127.0.0.1:4173`; Docker reported only
  `127.0.0.1:4173`, and the endpoint closed after Ctrl+C.
- 2026-09-01: A stop/start reuse test preserved the same immutable container
  ID and returned to `running/healthy`. Idle processes are only `docker-init`
  and `/bin/sleep infinity`.
- 2026-09-01: Final Docker inventory retained all seven unrelated container
  IDs and running/exited states, all three network IDs, and all 31 pre-existing
  named volumes. The sole added volume is the documented development-home
  volume. No Docker object was deleted or replaced.
- 2026-09-01: After the final setup-marker changes, dependency setup refreshed
  the evidence without reinstalling the already-present package set. An
  immediate second run recognized the exact setup hash and recorded package set,
  confirming idempotence for the final script. The container returned healthy.
- 2026-09-01: Final verification passed from isolated `/tmp` run
  `verify-single-dev-20260901f`: all 72 native tests and the same 72 tests under
  AddressSanitizer/UndefinedBehaviorSanitizer passed; the 112-page manuscript,
  static reader, demo contract, UI, and six native/WebAssembly bridge checks
  passed. All 27 published hashes verified. The bundle records 145 snapshotted
  input hashes, 274 installed-package records, and hashes matching the final
  `setup.sh` and `container/tasks.sh`.
- 2026-09-01: Final `serve` run
  `serve-single-dev-final2-20260901` rebuilt and retested from its isolated
  snapshot without publishing an evidence bundle. Windows received HTTP 200
  for the 1,925-byte reader and 638,816-byte PDF; Docker reported only
  `127.0.0.1:4173`. Ctrl+C closed the endpoint and left the named container
  running and healthy with only its init and idle sleep processes.
- 2026-09-01: Final static checks passed: both shell files and all eight GitHub
  Actions `run` blocks passed Bash syntax checks, `git diff --check` passed, and
  no Dockerfile, Compose definition, active Compose invocation, or host
  lifecycle launcher remains. The CI inspection checks cover its resolved
  image, command, identity, mounts, labels, capabilities, devices, restart
  behavior, volume labels, and absence of published ports.
- 2026-09-01: Independent final review tightened publication to GNU `mv`'s
  no-clobber mode with a postcondition that detects a lost destination race,
  corrected future-recreation ID guidance, documented Docker health's setup-hash
  boundary, and made CI create and inspect the same `/tmp` and `/run` tmpfs plus
  the expected `linux/amd64` image architecture. The final setup and verification
  runs above cover the resulting local scripts.
- 2026-09-01: The closing Docker inventory again retained the same seven
  unrelated container IDs/states, the same three network IDs, and the 31
  pre-existing named volumes; the documented home volume is the sole addition.
  The pre-existing `reader-standard-20260825b` bundle still verifies all 21
  hashes. No Docker object or prior evidence was deleted or replaced.

## Remaining Limitations

- The GitHub Actions workflow was not executed on GitHub during this local
  task. Its raw-Docker build path uses the same successfully exercised setup
  and `web` task, and the upload path now targets the verified
  `.container-output/local/web-dist`. Its eight shell blocks passed syntax
  validation, but no standalone YAML parser was available locally; YAML and
  hosted-runner integration remain to be observed on the next workflow run.
- Authenticated Debian repositories remain live inputs, so future recreation
  can select newer compatible package versions even though the base image
  manifest and architecture are pinned.
- The read-write repository mount deliberately permits container processes to
  change source and `.git`. The isolated automated build copy avoids stale
  generated ownership; it does not turn the container into a boundary for
  hostile project code.
- Successful builds and tests establish execution results for this inspected
  source and environment only. They do not establish any cryptographic,
  mathematical, protocol-security, or novelty claim.
- Docker Desktop's Windows bind-mount view twice exposed an atomically renamed
  artifact directory only after a short delay. Container-side checksum
  verification and a later host-side read both passed; automation that consumes
  a just-published directory from Windows should retry brief absence rather than
  infer build failure immediately.
