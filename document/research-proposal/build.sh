#!/usr/bin/env bash
# Standalone proposal build. Uses the installed TeX toolchain and vendored IEEEtran.
set -euo pipefail
paper_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$paper_dir"
build_dir=${1:-"$paper_dir/.build"}
mkdir -p "$build_dir"
build_dir=$(CDPATH= cd -- "$build_dir" && pwd)
export TEXINPUTS="$paper_dir/vendor/IEEEtran//:${TEXINPUTS:-}"
export BSTINPUTS="$paper_dir/vendor/IEEEtran/bibtex//:${BSTINPUTS:-}"
latexmk -pdf -interaction=nonstopmode -halt-on-error -file-line-error \
  -outdir="$build_dir" adversarial-cooperation-research-proposal.tex
