#!/usr/bin/env bash
# Auditable, Dockerfile-free toolchain for Adversarial Cooperation.
#
# This script intentionally refuses to provision a host.  compose.yaml mounts
# it at /bootstrap/setup.sh inside a pinned Debian container and supplies the
# context marker checked below.

set -Eeuo pipefail
IFS=$'\n\t'
umask 022

readonly EXPECTED_CONTEXT='adversarial-cooperation-toolchain-v1'
readonly EXPECTED_BASE='debian:13.6-slim@sha256:020c0d20b9880058cbe785a9db107156c3c75c2ac944a6aa7ab59f2add76a7bd'
readonly EXPECTED_APT_SOURCES_SHA256='b3fc5df8940d5fdba90aedab7abdd56e9fee2dc04c5dfb23735a3eaa4790587d'
readonly BOOTSTRAP_SCRIPT='/bootstrap/setup.sh'
readonly BOOTSTRAP_COMPOSE='/bootstrap/compose.yaml'
readonly SOURCE_ROOT='/source'
readonly ARTIFACT_ROOT='/artifacts'

die() {
  printf 'setup.sh: %s\n' "$*" >&2
  exit 1
}

note() {
  printf '\n==> %s\n' "$*"
}

require_container_context() {
  [[ -f /.dockerenv ]] || die 'refusing to run outside a Docker container'
  [[ "${AC_CONTAINER_CONTEXT:-}" == "$EXPECTED_CONTEXT" ]] ||
    die 'missing or incorrect AC_CONTAINER_CONTEXT marker'
  [[ "${AC_BASE_IMAGE:-}" == "$EXPECTED_BASE" ]] ||
    die 'the declared base image does not match the audited digest'
  [[ -r "$BOOTSTRAP_SCRIPT" && -r "$BOOTSTRAP_COMPOSE" ]] ||
    die 'the read-only bootstrap files are missing'
  [[ ! -e /var/run/docker.sock ]] ||
    die 'the Docker socket must never be mounted into this environment'
}

validate_parameters() {
  case "${AC_MODE:-${1:-}}" in
    web|verify|serve) AC_MODE="${AC_MODE:-${1:-}}" ;;
    *) die 'mode must be exactly web, verify, or serve' ;;
  esac

  AC_RUN_ID="${AC_RUN_ID:-local}"
  [[ "$AC_RUN_ID" =~ ^[a-z0-9][a-z0-9._-]{0,63}$ ]] ||
    die 'AC_RUN_ID must be 1-64 lowercase letters, digits, dots, underscores, or hyphens'

  AC_UID="${AC_UID:-65532}"
  AC_GID="${AC_GID:-65532}"
  [[ "$AC_UID" =~ ^[0-9]+$ && "$AC_GID" =~ ^[0-9]+$ ]] ||
    die 'AC_UID and AC_GID must be decimal integers'
  (( AC_UID >= 1 && AC_UID <= 2147483647 )) || die 'AC_UID is outside the allowed non-root range'
  (( AC_GID >= 1 && AC_GID <= 2147483647 )) || die 'AC_GID is outside the allowed non-root range'

  export AC_MODE AC_RUN_ID AC_UID AC_GID
}

assert_mount_option() {
  local target=$1
  local expected=$2
  local options
  options=$(findmnt -T "$target" -n -o OPTIONS) || die "cannot inspect mount options for $target"
  [[ ",$options," == *",$expected,"* ]] || die "$target is not mounted $expected"
}

validate_apt_sources() {
  local source_file='/etc/apt/sources.list.d/debian.sources'
  local -a configured=()
  local actual_hash

  mapfile -t configured < <(
    find /etc/apt \( -name '*.list' -o -name '*.sources' \) -print | LC_ALL=C sort
  )
  [[ ${#configured[@]} -eq 1 && "${configured[0]}" == "$source_file" ]] ||
    die 'unexpected APT source files are present'
  [[ -f "$source_file" && ! -L "$source_file" ]] ||
    die 'the expected APT source must be one regular, non-symlink file'

  [[ -r /usr/share/keyrings/debian-archive-keyring.pgp ]] ||
    die 'the Debian archive signing keyring is missing'
  actual_hash=$(sha256sum "$source_file" | awk '{print $1}')
  [[ "$actual_hash" == "$EXPECTED_APT_SOURCES_SHA256" ]] ||
    die 'APT sources differ from the exact two-stanza configuration in the pinned base image'
}

install_packages() {
  local -a apt_options=(
    -o APT::Get::AllowUnauthenticated=false
    -o Acquire::AllowInsecureRepositories=false
    -o Acquire::AllowDowngradeToInsecureRepositories=false
    -o Acquire::Check-Valid-Until=true
    -o APT::Install-Recommends=false
    -o APT::Install-Suggests=false
  )
  local -a packages=(
    build-essential
    ca-certificates
    emscripten
    latexmk
    make
    nodejs
    python3
    texlive-fonts-recommended
    texlive-latex-base
    texlive-latex-extra
    texlive-latex-recommended
    texlive-science
    util-linux
  )

  if [[ "$AC_MODE" == verify ]]; then
    packages+=(
      libsodium-dev
      pkg-config
    )
  fi

  validate_apt_sources
  export DEBIAN_FRONTEND=noninteractive

  note 'Refreshing signed Debian package indexes'
  apt-get "${apt_options[@]}" update
  note "Installing the explicit $AC_MODE package profile without recommends or suggests"
  apt-get "${apt_options[@]}" install -y --no-install-recommends "${packages[@]}"
  apt-get "${apt_options[@]}" check
  [[ -z $(dpkg --audit) ]] || die 'dpkg reports an incomplete package state'
  apt-get clean
  rm -rf /var/lib/apt/lists/*
}

validate_source_mounts() {
  local -a inputs=(
    Makefile.config
    book
    compile_latex.sh
    demostrations
    document
    include
    src
    test-vectors
    tests
    web
  )
  local artifact_filesystem input

  assert_mount_option "$BOOTSTRAP_SCRIPT" ro
  assert_mount_option "$BOOTSTRAP_COMPOSE" ro
  assert_mount_option "$ARTIFACT_ROOT" rw
  [[ $(findmnt -T "$ARTIFACT_ROOT" -n -o TARGET) == "$ARTIFACT_ROOT" ]] ||
    die 'the artifact path is not a dedicated mount'
  artifact_filesystem=$(findmnt -T "$ARTIFACT_ROOT" -n -o FSTYPE)
  if [[ "$AC_MODE" == serve ]]; then
    [[ "$artifact_filesystem" == tmpfs ]] ||
      die 'preview mode requires an isolated tmpfs at /artifacts'
  else
    [[ "$artifact_filesystem" != tmpfs ]] ||
      die 'publishing modes require the dedicated host artifact bind mount'
  fi
  for input in "${inputs[@]}"; do
    [[ -e "$SOURCE_ROOT/$input" ]] || die "required input is absent: $input"
    assert_mount_option "$SOURCE_ROOT/$input" ro
  done

  [[ -z $(find "$SOURCE_ROOT" -name .git -print -quit) ]] ||
    die 'Git metadata must not be exposed to the build container'
  if touch "$SOURCE_ROOT/web/.ac-source-write-probe" 2>/dev/null; then
    rm -f "$SOURCE_ROOT/web/.ac-source-write-probe"
    die 'a declared source mount accepted a write probe'
  fi
}

prepare_work_tree() {
  local work_root=$1
  local home_root=$2
  local -a inputs=(
    Makefile.config
    book
    compile_latex.sh
    demostrations
    document
    include
    src
    test-vectors
    tests
    web
  )
  local input

  [[ "$work_root" == "/tmp/ac-work-$AC_RUN_ID" ]] || die 'unsafe work-tree path'
  [[ "$home_root" == "/tmp/ac-home-$AC_RUN_ID" ]] || die 'unsafe home path'
  [[ ! -e "$work_root" && ! -e "$home_root" ]] || die 'ephemeral work paths already exist'
  install -d -m 0755 "$work_root" "$home_root"

  for input in "${inputs[@]}"; do
    cp -a "$SOURCE_ROOT/$input" "$work_root/$input"
  done

  # These are generated directories from a developer tree, never canonical
  # inputs.  The guard above confines removal to this fresh /tmp copy.
  rm -rf \
    "$work_root/.temp" \
    "$work_root/demostrations/.build" \
    "$work_root/document/.temp" \
    "$work_root/web/dist"

  install -d -m 0755 "$work_root/.ac-environment"
  cp /etc/os-release "$work_root/.ac-environment/os-release"
  cp /etc/apt/sources.list.d/debian.sources "$work_root/.ac-environment/debian.sources"
  dpkg-query -W -f='${binary:Package}\t${Version}\n' | LC_ALL=C sort \
    > "$work_root/.ac-environment/packages.tsv"
  sha256sum "$BOOTSTRAP_SCRIPT" > "$work_root/.ac-environment/setup.sha256"
  sha256sum "$BOOTSTRAP_COMPOSE" > "$work_root/.ac-environment/compose.sha256"
  (
    cd "$work_root"
    find . -type f ! -path './.ac-environment/*' -print0 |
      LC_ALL=C sort -z |
      xargs -0 sha256sum
  ) > "$work_root/.ac-environment/build-inputs.sha256"

  chown -R "$AC_UID:$AC_GID" "$work_root" "$home_root"
}

enter_unprivileged_build() {
  local work_root="/tmp/ac-work-$AC_RUN_ID"
  local home_root="/tmp/ac-home-$AC_RUN_ID"

  if [[ "$AC_MODE" == serve ]]; then
    # A preview never publishes artifacts. These exact paths contain only an
    # earlier disposable preview copy when a stopped container is restarted.
    [[ "$work_root" == "/tmp/ac-work-$AC_RUN_ID" ]] || die 'unsafe preview work-tree path'
    [[ "$home_root" == "/tmp/ac-home-$AC_RUN_ID" ]] || die 'unsafe preview home path'
    rm -rf "$work_root" "$home_root"
  else
    [[ ! -e "$ARTIFACT_ROOT/$AC_RUN_ID" ]] ||
      die "artifact destination already exists: $AC_RUN_ID"
    [[ -w "$ARTIFACT_ROOT" ]] || die 'artifact mount is not writable before privilege drop'
  fi

  prepare_work_tree "$work_root" "$home_root"
  note "Dropping permanently to uid=$AC_UID gid=$AC_GID with no capabilities"
  exec setpriv \
    --reuid="$AC_UID" \
    --regid="$AC_GID" \
    --clear-groups \
    --no-new-privs \
    --bounding-set=-all \
    --inh-caps=-all \
    --ambient-caps=-all \
    env -i \
      "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
      "HOME=$home_root" \
      'LANG=C.UTF-8' \
      'LC_ALL=C.UTF-8' \
      "AC_CONTAINER_CONTEXT=$EXPECTED_CONTEXT" \
      "AC_BASE_IMAGE=$EXPECTED_BASE" \
      'AC_PHASE=build' \
      "AC_MODE=$AC_MODE" \
      "AC_RUN_ID=$AC_RUN_ID" \
      "AC_UID=$AC_UID" \
      "AC_GID=$AC_GID" \
      /bin/bash "$BOOTSTRAP_SCRIPT" "$AC_MODE"
}

assert_unprivileged_build() {
  local field value
  (( EUID != 0 )) || die 'build phase must not run as root'
  [[ $(id -u) == "$AC_UID" && $(id -g) == "$AC_GID" ]] ||
    die 'build identity does not match the requested numeric uid/gid'
  [[ "${NoNewPrivs:-$(awk '/^NoNewPrivs:/ {print $2}' /proc/self/status)}" == 1 ]] ||
    die 'NoNewPrivs is not active'
  for field in CapInh CapPrm CapEff CapBnd CapAmb; do
    value=$(awk -v key="$field:" '$1 == key {print $2}' /proc/self/status)
    [[ "$value" == 0000000000000000 ]] || die "$field is not empty"
  done
  [[ ! -e /var/run/docker.sock ]] || die 'Docker socket appeared during the build phase'
  if touch "$SOURCE_ROOT/web/.ac-build-write-probe" 2>/dev/null; then
    rm -f "$SOURCE_ROOT/web/.ac-build-write-probe"
    die 'a declared source mount accepted a non-root write probe'
  fi
  if [[ "$AC_MODE" != serve ]]; then
    [[ -w "$ARTIFACT_ROOT" ]] || die 'the unprivileged build cannot write the artifact mount'
  fi
}

record_security_status() {
  local environment_dir=$1
  local source_input
  local -a source_inputs=(
    Makefile.config book compile_latex.sh demostrations document include src
    test-vectors tests web
  )
  {
    printf 'declared_base=%s\n' "$EXPECTED_BASE"
    printf 'mode=%s\nrun_id=%s\n' "$AC_MODE" "$AC_RUN_ID"
    printf 'kernel_machine=%s\n' "$(uname -m)"
    printf 'debian_architecture=%s\n' "$(dpkg --print-architecture)"
    id
    grep -E '^(NoNewPrivs|CapInh|CapPrm|CapEff|CapBnd|CapAmb):' /proc/self/status
    printf '\nbootstrap_setup_mount='; findmnt -T "$BOOTSTRAP_SCRIPT" -n -o OPTIONS
    printf 'bootstrap_compose_mount='; findmnt -T "$BOOTSTRAP_COMPOSE" -n -o OPTIONS
    for source_input in "${source_inputs[@]}"; do
      printf 'source_mount_%s=' "${source_input//[^a-zA-Z0-9]/_}"
      findmnt -T "$SOURCE_ROOT/$source_input" -n -o OPTIONS
    done
    printf 'artifact_mount='; findmnt -T "$ARTIFACT_ROOT" -n -o OPTIONS
    printf 'artifact_mount_filesystem='; findmnt -T "$ARTIFACT_ROOT" -n -o FSTYPE
    printf 'docker_socket_present=%s\n' "$([[ -e /var/run/docker.sock ]] && printf yes || printf no)"
    printf 'git_metadata_present=%s\n' "$([[ -n $(find "$SOURCE_ROOT" -name .git -print -quit) ]] && printf yes || printf no)"
  } > "$environment_dir/security-status.txt"
}

record_tool_versions() {
  local destination=$1
  {
    printf 'gcc: '; gcc --version | head -n 1
    printf 'make: '; make --version | head -n 1
    printf 'node: '; node --version
    printf 'python: '; python3 --version
    printf 'emcc: '; emcc --version | head -n 1
    if command -v latexmk >/dev/null 2>&1; then
      printf 'latexmk: '; latexmk -version | head -n 1
      printf 'pdflatex: '; pdflatex --version | head -n 1
    fi
  } > "$destination"
}

build_web() {
  local module
  note 'Checking every browser module before assembling static assets'
  while IFS= read -r -d '' module; do
    node --check "$module"
  done < <(find web/src -type f -name '*.mjs' -print0 | LC_ALL=C sort -z)
  note 'Compiling the canonical PDF and assembling the complete staged site'
  node web/tools/build-site.mjs
  note 'Running the static, demo-contract, and native/WebAssembly parity tests'
  node web/tools/test-site.mjs
  node web/tools/test-demo-contract.mjs
  node web/tools/test-static-ui.mjs
  node web/tools/test-wasm.mjs
}

build_and_test_all() {
  note 'Building every current C demonstration'
  make -C src all
  note 'Running the complete native C test suite'
  make -C tests test
  note 'Running AddressSanitizer and UndefinedBehaviorSanitizer tests'
  make -C tests sanitize
  build_web
}

publish_artifacts() {
  local work_root=$1
  local bundle="$work_root/.ac-artifact"
  local destination="$ARTIFACT_ROOT/$AC_RUN_ID"
  local publication_stage="$ARTIFACT_ROOT/.ac-publish-$AC_RUN_ID-$$"

  [[ "$work_root" == "/tmp/ac-work-$AC_RUN_ID" ]] || die 'unsafe publication work path'
  [[ ! -e "$bundle" && ! -e "$destination" && ! -e "$publication_stage" ]] ||
    die 'artifact path already exists'
  install -d -m 0755 "$bundle/environment" "$bundle/web-dist"
  cp -a "$work_root/web/dist/." "$bundle/web-dist/"
  cp -a "$work_root/.ac-environment/." "$bundle/environment/"

  if [[ "$AC_MODE" == verify ]]; then
    install -d -m 0755 "$bundle/pdf"
    cp "$work_root/document/.temp/pdf/adversarial_cooperation.pdf" \
      "$bundle/pdf/Adversarial-Cooperation.pdf"
  fi

  record_security_status "$bundle/environment"
  record_tool_versions "$bundle/environment/tool-versions.txt"
  {
    printf 'mode=%s\n' "$AC_MODE"
    printf 'complete_static_edition=passed\n'
    printf 'compiled_pdf_in_site=passed\n'
    printf 'registered_wasm_demos=passed\n'
    printf 'native_and_wasm_parity=passed\n'
    if [[ "$AC_MODE" == verify ]]; then
      printf 'native_tests=passed\nsanitizer_tests=passed\nmanuscript_build=passed\n'
    fi
  } > "$bundle/verification.txt"
  (
    cd "$bundle"
    find . -type f ! -name SHA256SUMS -print0 |
      LC_ALL=C sort -z |
      xargs -0 sha256sum
  ) > "$bundle/SHA256SUMS"

  mkdir -m 0755 "$publication_stage"
  if ! cp -a "$bundle/." "$publication_stage/"; then
    rm -rf -- "$publication_stage"
    die 'failed to stage the verified artifact on the publication filesystem'
  fi
  [[ ! -e "$destination" ]] || {
    rm -rf -- "$publication_stage"
    die 'artifact destination appeared during publication'
  }
  if ! mv -T -- "$publication_stage" "$destination"; then
    rm -rf -- "$publication_stage"
    die 'failed to publish the verified artifact atomically'
  fi
  note "Published only verified artifacts to $destination"
}

run_build_phase() {
  local work_root="/tmp/ac-work-$AC_RUN_ID"
  [[ "$PWD" == "$work_root" ]] || cd "$work_root"
  assert_unprivileged_build

  case "$AC_MODE" in
    web)
      build_web
      publish_artifacts "$work_root"
      ;;
    verify)
      build_and_test_all
      publish_artifacts "$work_root"
      ;;
    serve)
      build_web
      note 'Serving only the ephemeral generated site; Compose publishes loopback only'
      exec node web/tools/serve.mjs --host 0.0.0.0 --port 4173 --root "$work_root/web/dist"
      ;;
  esac
}

main() {
  require_container_context
  validate_parameters "$@"

  if [[ "${AC_PHASE:-provision}" == build ]]; then
    run_build_phase
    return
  fi
  [[ "${AC_PHASE:-provision}" == provision ]] || die 'unknown execution phase'
  (( EUID == 0 )) || die 'APT provisioning must begin as root inside the disposable container'

  install_packages
  validate_source_mounts
  enter_unprivileged_build
}

main "$@"
