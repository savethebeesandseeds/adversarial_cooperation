#!/usr/bin/env bash
# Dependency and identity setup for the reusable Adversarial Cooperation
# development container. Project operations belong in container/tasks.sh.

set -Eeuo pipefail
IFS=$'\n\t'
umask 022

readonly EXPECTED_CONTEXT='adversarial-cooperation-dev-v1'
readonly EXPECTED_BASE='debian:13.6-slim@sha256:020c0d20b9880058cbe785a9db107156c3c75c2ac944a6aa7ab59f2add76a7bd'
readonly EXPECTED_APT_SOURCES_SHA256='b3fc5df8940d5fdba90aedab7abdd56e9fee2dc04c5dfb23735a3eaa4790587d'
readonly BOOTSTRAP_SCRIPT='/bootstrap/setup.sh'
readonly WORKSPACE_ROOT='/workspace'
readonly HOME_ROOT='/home/ac'
readonly STATE_ROOT='/var/lib/adversarial-cooperation'
readonly COMPLETE_MARKER="$STATE_ROOT/setup-complete"
readonly IN_PROGRESS_MARKER="$STATE_ROOT/setup-in-progress"
readonly ACCOUNT_NAME='ac'
readonly -a PACKAGES=(
  build-essential
  ca-certificates
  emscripten
  latexmk
  libsodium-dev
  make
  nodejs
  pkg-config
  python3
  texlive-fonts-recommended
  texlive-latex-base
  texlive-latex-extra
  texlive-latex-recommended
  texlive-science
  util-linux
)

die() {
  printf 'setup.sh: %s\n' "$*" >&2
  exit 1
}

note() {
  printf '\n==> %s\n' "$*"
}

validate_parameters() {
  (($# == 0)) || die 'this dependency setup accepts no operation arguments'

  AC_UID="${AC_UID:-65532}"
  AC_GID="${AC_GID:-65532}"
  [[ "$AC_UID" =~ ^[0-9]+$ && "$AC_GID" =~ ^[0-9]+$ ]] ||
    die 'AC_UID and AC_GID must be decimal integers'
  ((AC_UID >= 1 && AC_UID <= 2147483647)) ||
    die 'AC_UID is outside the allowed non-root range'
  ((AC_GID >= 1 && AC_GID <= 2147483647)) ||
    die 'AC_GID is outside the allowed non-root range'
  export AC_UID AC_GID
}

require_container_context() {
  [[ -f /.dockerenv ]] || die 'refusing to provision outside a Docker container'
  ((EUID == 0)) || die 'dependency provisioning must execute as root'
  [[ "${AC_CONTAINER_CONTEXT:-}" == "$EXPECTED_CONTEXT" ]] ||
    die 'missing or incorrect AC_CONTAINER_CONTEXT marker'
  [[ "${AC_BASE_IMAGE:-}" == "$EXPECTED_BASE" ]] ||
    die 'the declared base image does not match the audited digest'
  [[ -f "$BOOTSTRAP_SCRIPT" && ! -L "$BOOTSTRAP_SCRIPT" && -r "$BOOTSTRAP_SCRIPT" ]] ||
    die 'the read-only bootstrap setup file is missing'
  [[ ! -e /var/run/docker.sock ]] ||
    die 'the Docker socket must never be mounted into this environment'
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
    die 'APT sources differ from the audited configuration in the pinned base image'
}

package_set_is_installed() {
  local package status

  for package in "${PACKAGES[@]}"; do
    status=$(dpkg-query -W -f='${db:Status-Status}' "$package" 2>/dev/null || true)
    [[ "$status" == installed ]] || return 1
  done
  [[ -z $(dpkg --audit) ]]
}

setup_is_current() {
  local setup_hash=$1

  [[ -r "$COMPLETE_MARKER" ]] || return 1
  grep -Fqx 'status=complete' "$COMPLETE_MARKER" || return 1
  grep -Fqx "setup_sha256=$setup_hash" "$COMPLETE_MARKER" || return 1
  package_set_is_installed
}

mark_setup_in_progress() {
  local setup_hash=$1
  local marker_stage="$STATE_ROOT/.setup-in-progress.$$"

  install -d -o root -g root -m 0755 "$STATE_ROOT"
  rm -f -- "$COMPLETE_MARKER"
  {
    printf 'status=in-progress\n'
    printf 'setup_sha256=%s\n' "$setup_hash"
  } > "$marker_stage"
  chmod 0644 "$marker_stage"
  mv -f -- "$marker_stage" "$IN_PROGRESS_MARKER"
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

  export DEBIAN_FRONTEND=noninteractive
  note 'Refreshing authenticated Debian package indexes'
  apt-get "${apt_options[@]}" update
  note 'Installing the complete development and verification dependency set'
  apt-get "${apt_options[@]}" install -y --no-install-recommends "${PACKAGES[@]}"
  apt-get "${apt_options[@]}" check
  [[ -z $(dpkg --audit) ]] || die 'dpkg reports an incomplete package state'
  apt-get clean
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

validate_mounts() {
  local target

  [[ -d "$WORKSPACE_ROOT" ]] || die 'the workspace bind mount is missing'
  [[ -d "$HOME_ROOT" ]] || die 'the persistent home volume is missing'

  target=$(findmnt -T "$WORKSPACE_ROOT" -n -o TARGET) ||
    die 'cannot identify the workspace mount'
  [[ "$target" == "$WORKSPACE_ROOT" ]] ||
    die 'the workspace is not a dedicated mount'
  target=$(findmnt -T "$HOME_ROOT" -n -o TARGET) ||
    die 'cannot identify the home mount'
  [[ "$target" == "$HOME_ROOT" ]] ||
    die 'the home path is not a dedicated mount'

  assert_mount_option "$BOOTSTRAP_SCRIPT" ro
  assert_mount_option "$WORKSPACE_ROOT" rw
  assert_mount_option "$HOME_ROOT" rw
}

configure_identity() {
  local entry name password uid gid gecos home shell

  if entry=$(getent group "$ACCOUNT_NAME"); then
    IFS=: read -r name password gid _ <<< "$entry"
    [[ "$name" == "$ACCOUNT_NAME" && "$gid" == "$AC_GID" ]] ||
      die "group $ACCOUNT_NAME does not match requested gid=$AC_GID"
  elif entry=$(getent group "$AC_GID"); then
    IFS=: read -r name _ <<< "$entry"
    die "requested gid=$AC_GID already belongs to group $name"
  else
    groupadd --gid "$AC_GID" "$ACCOUNT_NAME"
  fi

  if entry=$(getent passwd "$ACCOUNT_NAME"); then
    IFS=: read -r name password uid gid gecos home shell <<< "$entry"
    [[ "$name" == "$ACCOUNT_NAME" && "$uid" == "$AC_UID" &&
       "$gid" == "$AC_GID" && "$home" == "$HOME_ROOT" &&
       "$shell" == /bin/bash ]] ||
      die "account $ACCOUNT_NAME does not match the requested identity"
  elif entry=$(getent passwd "$AC_UID"); then
    IFS=: read -r name _ <<< "$entry"
    die "requested uid=$AC_UID already belongs to account $name"
  else
    useradd \
      --uid "$AC_UID" \
      --gid "$AC_GID" \
      --home-dir "$HOME_ROOT" \
      --no-create-home \
      --shell /bin/bash \
      "$ACCOUNT_NAME"
  fi

  chown "$AC_UID:$AC_GID" "$HOME_ROOT"
  install -d -o "$AC_UID" -g "$AC_GID" -m 0700 "$HOME_ROOT/.cache"
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

record_setup_evidence() {
  local setup_hash=$1
  local marker_stage="$STATE_ROOT/.setup-complete.$$"

  install -d -o root -g root -m 0755 "$STATE_ROOT"
  cp /etc/os-release "$STATE_ROOT/os-release"
  cp /etc/apt/sources.list.d/debian.sources "$STATE_ROOT/debian.sources"
  dpkg-query -W -f='${binary:Package}\t${Version}\n' | LC_ALL=C sort \
    > "$STATE_ROOT/packages.tsv"
  record_tool_versions "$STATE_ROOT/tool-versions.txt"
  printf '%s  %s\n' "$setup_hash" "$BOOTSTRAP_SCRIPT" \
    > "$STATE_ROOT/setup.sha256"
  {
    printf 'status=complete\n'
    printf 'context=%s\n' "$EXPECTED_CONTEXT"
    printf 'declared_base=%s\n' "$EXPECTED_BASE"
    printf 'setup_sha256=%s\n' "$setup_hash"
    printf 'uid=%s\n' "$AC_UID"
    printf 'gid=%s\n' "$AC_GID"
    printf 'kernel_machine=%s\n' "$(uname -m)"
    printf 'debian_architecture=%s\n' "$(dpkg --print-architecture)"
  } > "$marker_stage"
  chmod 0644 \
    "$STATE_ROOT/os-release" \
    "$STATE_ROOT/debian.sources" \
    "$STATE_ROOT/packages.tsv" \
    "$STATE_ROOT/tool-versions.txt" \
    "$STATE_ROOT/setup.sha256" \
    "$marker_stage"
  rm -f -- "$IN_PROGRESS_MARKER"
  mv -f -- "$marker_stage" "$COMPLETE_MARKER"
}

main() {
  local setup_hash
  local setup_record_current=false
  local dependencies_current=false

  validate_parameters "$@"
  require_container_context
  setup_hash=$(sha256sum "$BOOTSTRAP_SCRIPT" | awk '{print $1}')

  if setup_is_current "$setup_hash"; then
    setup_record_current=true
  fi
  if package_set_is_installed; then
    dependencies_current=true
  fi

  mark_setup_in_progress "$setup_hash"
  validate_apt_sources
  if [[ "$setup_record_current" == true ]]; then
    note 'The recorded dependency set already matches this setup script'
  elif [[ "$dependencies_current" == true ]]; then
    note 'The dependency set is installed; refreshing the environment record'
  else
    install_packages
  fi

  validate_mounts
  configure_identity
  record_setup_evidence "$setup_hash"
  note 'Dependency and environment setup completed; no project task was executed'
}

main "$@"
