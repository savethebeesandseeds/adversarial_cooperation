# Execution Plan: Honest Dedicated Container Toolchain

## Status

Complete.

## Objective

Replace all use of ambient or unidentified build containers with one
project-owned environment whose setup is visible in `setup.sh`. The environment
must build the C companions, manuscript, static browser edition, and
WebAssembly demo without a Dockerfile and without obtaining dependencies from
JavaScript package registries or ad-hoc download scripts.

"Secure" in this plan means a bounded operational property, not a claim that
containers safely execute arbitrary hostile code. The intended protections are:

- no access to other project containers, Docker sockets, Git metadata,
  explicitly mapped host devices, or host paths outside explicit read-only
  build inputs and one dedicated artifact directory;
- a Docker Official Debian stable-slim base pinned by digest;
- packages obtained only from Debian's configured `main`, updates, and security
  archives with signature verification and insecure/unauthenticated modes
  explicitly disabled;
- no recommended or suggested packages unless named explicitly;
- no retained Linux capabilities or privilege escalation during compilation;
- loopback-only preview publication; and
- an emitted environment manifest containing installed package and tool
  versions, setup hashes, and artifact hashes.

## Honest Limitations

- The provisioning phase necessarily runs as root inside the disposable
  container so APT can modify that container's root filesystem.
- The provisioning phase requires network access to Debian mirrors. The later
  unprivileged build phase retains ordinary outbound network reachability
  because Compose cannot remove a container's network after APT without a
  second opaque image or a much broader host-control mechanism. Compose
  deliberately passes no host credentials into the container and the source
  mounts are read-only. A secret already committed to an allowlisted source
  would nevertheless be visible.
- Docker shares the host kernel. This design is not a sandbox for malicious
  kernel exploits, a compromised Docker daemon, or deliberately hostile source
  code.
- Debian stable package versions may advance with signed security updates.
  Each run records the versions actually installed instead of pretending the
  repository state is frozen.

## Architecture

1. `compose.yaml` references the official `debian:13.6-slim` manifest by digest.
2. Compose exposes only explicit build inputs beneath `/source`, read-only, and
   the project-owned `.container-output/` beneath `/artifacts`, read-write.
3. `setup.sh` validates its container context and APT sources, installs an
   explicit package profile with defensive APT options, audits the package
   database, copies inputs into an ephemeral `/tmp` work tree, then re-executes
   itself as a numeric non-root UID/GID with no-new-privileges and an empty
   capability bounding set.
4. `web`, `verify`, and `serve` modes share the same setup. `serve` binds only
   to port 4173 inside the container and Compose publishes it only on
   `127.0.0.1`.
5. GitHub Pages calls the `web` mode and uploads only the generated static
   artifact.

## Exact File Scope

Modify:

1. `setup.sh`
2. `.gitignore`
3. `README.md`
4. `web/README.md`
5. `.github/workflows/pages.yml`
6. `research/DECISIONS.md`
7. `web/tools/build-wasm.mjs`

Create:

8. `compose.yaml`
9. `container/SECURITY.md`
10. `research/plans/honest-container-toolchain.md`

No manuscript, C protocol, public API, test vector, backup, or cryptographic
backend is in scope.

## Authoritative Preimages

Guarded copyback must refuse an existing file whose SHA-256 differs from:

```text
1ef32f034f9d71d62c70a8a8505e7f2ad0257932052995be73f9eb92c178369e  setup.sh
c20299a4f8a613460d7aaa8c4173438c9beb04ca0d8f6d7d5c976394389a65cd  .gitignore
fab221716f7307cb622270e67b52b63031ab0ab1fbe49ddab465d0c468ee7eb1  README.md
bf14c15da4925db3c84de1a072d559fd21d66973fdb98195238e52b84a02bb28  web/README.md
11919f115652b239842b8b1f42745cb40f37a93f37149845267e921a1e7f9a82  .github/workflows/pages.yml
21fc8ba115220fb6c7d5d0d877c54349252666a07d84193ca9723a2a1d0a7fe7  research/DECISIONS.md
2972bd0124af0caab13b24269a51dd418de11df141cc613d9450d513f2a1ae0a  web/tools/build-wasm.mjs
```

All three create targets were absent from the authoritative destination when
this plan was opened.

## Verification

1. Validate `compose.yaml` using `docker compose config`.
2. Create only containers whose Compose project name begins with
   `adversarial-cooperation` and prove no unrelated container changes state.
3. Run `web` from a fresh pinned base and verify 27-source coverage plus native
   and WebAssembly parity.
4. Run `verify` from a fresh pinned base and build all C demos, run the complete
   native and sanitizer suites, and compile the manuscript non-interactively.
5. Verify the build phase UID is nonzero, its capability sets are empty, the
   source mounts reject writes, Git metadata is absent, and only the dedicated
   artifact mount receives host writes.
6. Serve the static artifact through the dedicated preview service and check
   HTML, JSON, and WebAssembly content types over loopback.
7. Inspect the emitted environment manifest and SHA-256 file.
8. Recheck authoritative preimages and copy back only the ten enumerated paths.

## Verification Record

Performed on 2026-08-01 with Docker Engine 29.6.1 and Docker Compose 5.1.4
on the local Docker Desktop Linux/amd64 engine.

- `docker compose config --quiet` passed, and `bash -n` passed inside the
  pinned base image. No Dockerfile exists in the staged or authoritative tree.
- A fresh preliminary `web` run built five parts and 27 manuscript sources.
  Its six native WebAssembly-boundary tests and native/WebAssembly fixtures
  passed.
- A fresh `verify` run under ID `verify-final` built all current C companions,
  passed 72 native tests, repeated all 72 under AddressSanitizer and
  UndefinedBehaviorSanitizer, passed the six WebAssembly-boundary tests and
  native/WebAssembly parity, and compiled the 112-page manuscript.
- The full run produced
  `pdf/Adversarial-Cooperation.pdf` with SHA-256
  `7512add4cb8320790d6ff1fa546de94834ca59cde1c4527607ad87fd49a1cdee`.
  The WebAssembly binary was 5,610 bytes with SHA-256
  `860fd217fba908129b69d7a06f23e52fe7d247e11e476e387783f0b2d1391013`.
- The full evidence bundle recorded 274 installed/base package entries. All
  19 entries in its `SHA256SUMS` were independently recomputed with zero
  missing files or mismatches.
- Runtime evidence recorded UID/GID `65532:65532`, `NoNewPrivs: 1`, and zero
  `CapInh`, `CapPrm`, `CapEff`, `CapBnd`, and `CapAmb`. Every bootstrap and
  canonical source mount reported `ro`; the artifact mount reported `rw`;
  Docker socket and Git metadata checks both reported absent.
- The preview built again under ID `preview-final`. Host publication resolved
  only to `127.0.0.1:4173`. HEAD requests returned the expected MIME types for
  HTML, JSON, and WebAssembly, with CSP, `nosniff`, and no-referrer headers.
  POST returned 405 with `Allow: GET, HEAD`.
- `docker compose down --remove-orphans` removed only the dedicated preview
  and `adversarial-cooperation_default` network. A before/after container
  inventory retained every unrelated container ID and state. The earlier
  foreign-container cleanup removed only
  `/tmp/ac-browser-wasm-verify-20260801`; the pre-existing tree in that
  container remained present, and that container was not used again.
- Immediately before copyback, all seven authoritative preimages matched their
  recorded SHA-256 values and all three create targets remained absent. Exactly
  the ten enumerated files were copied individually; every destination hash
  then matched its staged source.

Honest residual limitations remain those in `container/SECURITY.md`: root and
network are required during APT provisioning, build networking remains
available, all of `.container-output/` is writable to trusted build source,
signed Debian packages are not thereby proven harmless, and Docker shares the
host kernel.
