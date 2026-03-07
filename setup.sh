#!/usr/bin/env bash
set -euo pipefail

if [[ "${EUID}" -ne 0 ]]; then
  SUDO="sudo"
else
  SUDO=""
fi

export DEBIAN_FRONTEND=noninteractive

$SUDO apt-get update
$SUDO apt-get install -y --no-install-recommends \
  build-essential \
  make \
  pkg-config \
  libssl-dev \
  libsodium-dev

echo "Setup complete: C build tooling, OpenSSL, and libsodium are installed."
