#!/usr/bin/env bash
# In-container project operations for the reusable development environment.

set -Eeuo pipefail
IFS=$'\n\t'
umask 022

readonly EXPECTED_CONTEXT='adversarial-cooperation-dev-v1'
readonly EXPECTED_BASE='debian:13.6-slim@sha256:020c0d20b9880058cbe785a9db107156c3c75c2ac944a6aa7ab59f2add76a7bd'
readonly EXPECTED_CAP_BOUNDING='00000000800001db'
readonly SETUP_SCRIPT='/bootstrap/setup.sh'
readonly SETUP_STATE_ROOT='/var/lib/adversarial-cooperation'
readonly SETUP_MARKER="$SETUP_STATE_ROOT/setup-complete"
readonly WORKSPACE_ROOT='/workspace'
readonly HOME_ROOT='/home/ac'
readonly ARTIFACT_ROOT="$WORKSPACE_ROOT/.container-output"
readonly TASK_SCRIPT="$WORKSPACE_ROOT/container/tasks.sh"
readonly -a SOURCE_INPUTS=(
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

PUBLICATION_STAGE=''
RUN_ROOT=''
RUN_REPOSITORY=''

die() {
  printf 'container/tasks.sh: %s\n' "$*" >&2
  exit 1
}

note() {
  printf '\n==> %s\n' "$*"
}

validate_parameters() {
  (($# == 1)) || die 'usage: container/tasks.sh web|verify|serve'
  case "$1" in
    web|verify|serve) AC_MODE=$1 ;;
    *) die 'task must be exactly web, verify, or serve' ;;
  esac

  AC_RUN_ID="${AC_RUN_ID:-local}"
  [[ "$AC_RUN_ID" =~ ^[a-z0-9][a-z0-9._-]{0,63}$ ]] ||
    die 'AC_RUN_ID must be 1-64 lowercase letters, digits, dots, underscores, or hyphens'
  AC_UID="${AC_UID:-65532}"
  AC_GID="${AC_GID:-65532}"
  [[ "$AC_UID" =~ ^[0-9]+$ && "$AC_GID" =~ ^[0-9]+$ ]] ||
    die 'AC_UID and AC_GID must be decimal integers'
  export AC_MODE AC_RUN_ID AC_UID AC_GID
}

assert_mount_option() {
  local target=$1
  local expected=$2
  local options

  options=$(findmnt -T "$target" -n -o OPTIONS) ||
    die "cannot inspect mount options for $target"
  [[ ",$options," == *",$expected,"* ]] ||
    die "$target is not mounted $expected"
}

assert_unprivileged_context() {
  local field value target

  [[ -f /.dockerenv ]] || die 'refusing to operate outside a Docker container'
  [[ "${AC_CONTAINER_CONTEXT:-}" == "$EXPECTED_CONTEXT" ]] ||
    die 'missing or incorrect AC_CONTAINER_CONTEXT marker'
  [[ "${AC_BASE_IMAGE:-}" == "$EXPECTED_BASE" ]] ||
    die 'the declared base image does not match the audited digest'
  [[ $(id -u) == "$AC_UID" && $(id -g) == "$AC_GID" && "$AC_UID" != 0 ]] ||
    die 'project tasks require the configured non-root identity'
  [[ "${NoNewPrivs:-$(awk '/^NoNewPrivs:/ {print $2}' /proc/self/status)}" == 1 ]] ||
    die 'NoNewPrivs is not active'
  for field in CapInh CapPrm CapEff CapAmb; do
    value=$(awk -v key="$field:" '$1 == key {print $2}' /proc/self/status)
    [[ "$value" == 0000000000000000 ]] || die "$field is not empty"
  done
  value=$(awk '$1 == "CapBnd:" {print $2}' /proc/self/status)
  [[ "$value" == "$EXPECTED_CAP_BOUNDING" ]] ||
    die 'CapBnd does not match the reviewed provisioning allowlist'
  [[ ! -e /var/run/docker.sock ]] || die 'the Docker socket is present'
  [[ -d "$WORKSPACE_ROOT" && -d "$HOME_ROOT" ]] ||
    die 'the workspace or persistent home mount is missing'
  [[ -f "$TASK_SCRIPT" && -r "$TASK_SCRIPT" ]] || die 'the task runner is missing'

  target=$(findmnt -T "$WORKSPACE_ROOT" -n -o TARGET) ||
    die 'cannot identify the workspace mount'
  [[ "$target" == "$WORKSPACE_ROOT" ]] || die 'the workspace is not a dedicated mount'
  target=$(findmnt -T "$HOME_ROOT" -n -o TARGET) ||
    die 'cannot identify the home mount'
  [[ "$target" == "$HOME_ROOT" ]] || die 'the home path is not a dedicated mount'
  assert_mount_option "$SETUP_SCRIPT" ro
  assert_mount_option "$WORKSPACE_ROOT" rw
  assert_mount_option "$HOME_ROOT" rw
}

validate_setup_record() {
  local actual_hash recorded_hash

  [[ -r "$SETUP_MARKER" ]] || die 'dependency setup has not completed'
  grep -Fqx 'status=complete' "$SETUP_MARKER" ||
    die 'dependency setup is incomplete'
  actual_hash=$(sha256sum "$SETUP_SCRIPT" | awk '{print $1}')
  recorded_hash=$(awk -F= '$1 == "setup_sha256" {print $2}' "$SETUP_MARKER")
  [[ "$recorded_hash" == "$actual_hash" ]] ||
    die 'setup.sh changed; rerun dependency setup as root before project tasks'
}

validate_inputs() {
  local input

  for input in "${SOURCE_INPUTS[@]}"; do
    [[ -e "$WORKSPACE_ROOT/$input" ]] || die "required input is absent: $input"
  done
}

prepare_run_repository() {
  local runs_root='/tmp/adversarial-cooperation-runs'

  mkdir -p "$runs_root"
  [[ -d "$runs_root" && ! -L "$runs_root" ]] ||
    die 'the temporary run root must be a real directory'
  RUN_ROOT=$(mktemp -d "$runs_root/$AC_RUN_ID.XXXXXX")
  RUN_REPOSITORY="$RUN_ROOT/repository"
  mkdir "$RUN_REPOSITORY"

  note "Copying current declared inputs to isolated run root $RUN_ROOT"
  (
    cd "$WORKSPACE_ROOT"
    tar \
      --exclude='.temp' \
      --exclude='.temp/*' \
      --exclude='*/.temp' \
      --exclude='*/.temp/*' \
      --exclude='*/.build' \
      --exclude='*/.build/*' \
      --exclude='web/dist' \
      --exclude='web/dist/*' \
      -cf - "${SOURCE_INPUTS[@]}"
  ) | tar -C "$RUN_REPOSITORY" -xf -

  [[ -f "$RUN_REPOSITORY/Makefile.config" &&
     -f "$RUN_REPOSITORY/document/adversarial_cooperation.tex" &&
     -f "$RUN_REPOSITORY/web/tools/build-site.mjs" ]] ||
    die 'the isolated run repository is incomplete'
}

record_tool_versions() {
  local destination=$1

  {
    printf 'gcc: '; gcc --version | sed -n '1p'
    printf 'make: '; make --version | sed -n '1p'
    printf 'node: '; node --version
    printf 'python: '; python3 --version
    printf 'emcc: '; emcc --version | sed -n '1p'
    printf 'latexmk: '; latexmk -version | sed -n '1p'
    printf 'pdflatex: '; pdflatex --version | sed -n '1p'
    printf 'pkg-config: '; pkg-config --version
    printf 'libsodium: '; pkg-config --modversion libsodium
  } > "$destination"
}

record_security_status() {
  local destination=$1

  {
    printf 'declared_base=%s\n' "$EXPECTED_BASE"
    printf 'mode=%s\nrun_id=%s\n' "$AC_MODE" "$AC_RUN_ID"
    printf 'isolated_run_repository=%s\n' "$RUN_REPOSITORY"
    printf 'kernel_machine=%s\n' "$(uname -m)"
    printf 'debian_architecture=%s\n' "$(dpkg --print-architecture)"
    id
    grep -E '^(NoNewPrivs|CapInh|CapPrm|CapEff|CapBnd|CapAmb):' /proc/self/status
    printf '\nbootstrap_setup_mount='; findmnt -T "$SETUP_SCRIPT" -n -o OPTIONS
    printf 'workspace_mount='; findmnt -T "$WORKSPACE_ROOT" -n -o OPTIONS
    printf 'workspace_mount_filesystem='; findmnt -T "$WORKSPACE_ROOT" -n -o FSTYPE
    printf 'home_mount='; findmnt -T "$HOME_ROOT" -n -o OPTIONS
    printf 'home_mount_filesystem='; findmnt -T "$HOME_ROOT" -n -o FSTYPE
    printf 'docker_socket_present=%s\n' "$([[ -e /var/run/docker.sock ]] && printf yes || printf no)"
    printf 'git_metadata_present=%s\n' "$([[ -e "$WORKSPACE_ROOT/.git" ]] && printf yes || printf no)"
    printf 'workspace_writable=%s\n' "$([[ -w "$WORKSPACE_ROOT" ]] && printf yes || printf no)"
  } > "$destination"
}

record_build_inputs() {
  local destination=$1

  (
    cd "$RUN_REPOSITORY"
    find "${SOURCE_INPUTS[@]}" -type f \
      ! -path '*/.build/*' \
      ! -path '*/.temp/*' \
      ! -path 'web/dist/*' \
      -print0 |
      LC_ALL=C sort -z |
      xargs -0 sha256sum
  ) > "$destination"
}

assert_publication_destination_available() {
  local destination="$ARTIFACT_ROOT/$AC_RUN_ID"

  [[ ! -e "$destination" && ! -L "$destination" ]] ||
    die "artifact destination already exists: $AC_RUN_ID"
}

record_environment() {
  local environment_dir=$1

  mkdir -p "$environment_dir/setup"
  cp -a "$SETUP_STATE_ROOT/." "$environment_dir/setup/"
  cp /etc/os-release "$environment_dir/os-release"
  cp /etc/apt/sources.list.d/debian.sources "$environment_dir/debian.sources"
  dpkg-query -W -f='${binary:Package}\t${Version}\n' | LC_ALL=C sort \
    > "$environment_dir/packages.tsv"
  record_tool_versions "$environment_dir/tool-versions.txt"
  record_security_status "$environment_dir/security-status.txt"
  record_build_inputs "$environment_dir/build-inputs.sha256"
  sha256sum "$SETUP_SCRIPT" > "$environment_dir/setup.sha256"
  sha256sum "$TASK_SCRIPT" > "$environment_dir/tasks.sha256"
}

prepare_publication_stage() {
  assert_publication_destination_available
  [[ ! -L "$ARTIFACT_ROOT" ]] || die 'the artifact root must not be a symbolic link'
  mkdir -p "$ARTIFACT_ROOT"
  [[ -d "$ARTIFACT_ROOT" && -w "$ARTIFACT_ROOT" ]] ||
    die 'the artifact root is not writable'
  PUBLICATION_STAGE=$(mktemp -d "$ARTIFACT_ROOT/.ac-publish-$AC_RUN_ID.XXXXXX")
  mkdir -p "$PUBLICATION_STAGE/environment" "$PUBLICATION_STAGE/web-dist"
  record_environment "$PUBLICATION_STAGE/environment"
}

build_web() {
  local module

  cd "$RUN_REPOSITORY"
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
  cd "$RUN_REPOSITORY"
  note 'Building every current C demonstration'
  make -C src all
  note 'Running the complete native C test suite'
  make -C tests test
  note 'Running AddressSanitizer and UndefinedBehaviorSanitizer tests'
  make -C tests sanitize
  build_web
}

publish_artifacts() {
  local destination="$ARTIFACT_ROOT/$AC_RUN_ID"

  [[ -n "$PUBLICATION_STAGE" && -d "$PUBLICATION_STAGE" ]] ||
    die 'the publication stage is missing'
  [[ ! -e "$destination" && ! -L "$destination" ]] ||
    die "artifact destination appeared: $AC_RUN_ID"
  [[ -d "$RUN_REPOSITORY/web/dist" ]] || die 'the generated web site is missing'
  cp -a "$RUN_REPOSITORY/web/dist/." "$PUBLICATION_STAGE/web-dist/"

  if [[ "$AC_MODE" == verify ]]; then
    mkdir -p "$PUBLICATION_STAGE/pdf"
    cp "$RUN_REPOSITORY/document/.temp/pdf/adversarial_cooperation.pdf" \
      "$PUBLICATION_STAGE/pdf/Adversarial-Cooperation.pdf"
  fi

  {
    printf 'mode=%s\n' "$AC_MODE"
    printf 'complete_static_edition=passed\n'
    printf 'compiled_pdf_in_site=passed\n'
    printf 'registered_wasm_demos=passed\n'
    printf 'native_and_wasm_parity=passed\n'
    if [[ "$AC_MODE" == verify ]]; then
      printf 'native_tests=passed\n'
      printf 'sanitizer_tests=passed\n'
      printf 'manuscript_build=passed\n'
    fi
  } > "$PUBLICATION_STAGE/verification.txt"
  (
    cd "$PUBLICATION_STAGE"
    find . -type f ! -name SHA256SUMS -print0 |
      LC_ALL=C sort -z |
      xargs -0 sha256sum
  ) > "$PUBLICATION_STAGE/SHA256SUMS"

  local completed_stage=$PUBLICATION_STAGE
  mv -Tn -- "$completed_stage" "$destination"
  [[ ! -e "$completed_stage" && ! -L "$completed_stage" && -d "$destination" ]] ||
    die "artifact destination won the publication race: $AC_RUN_ID"
  PUBLICATION_STAGE=''
  note "Published only verified artifacts to $destination"
}

main() {
  validate_parameters "$@"
  assert_unprivileged_context
  validate_setup_record
  validate_inputs
  case "$AC_MODE" in
    web|verify) assert_publication_destination_available ;;
  esac
  prepare_run_repository

  case "$AC_MODE" in
    web)
      prepare_publication_stage
      build_web
      publish_artifacts
      ;;
    verify)
      prepare_publication_stage
      build_and_test_all
      publish_artifacts
      ;;
    serve)
      build_web
      note 'Serving inside the container; host publication must remain loopback-only'
      exec node "$RUN_REPOSITORY/web/tools/serve.mjs" \
        --host 0.0.0.0 \
        --port 4173 \
        --root "$RUN_REPOSITORY/web/dist"
      ;;
  esac
}

main "$@"
