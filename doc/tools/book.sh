#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OUT_DIR="${BOOK_OUT_DIR:-${ROOT_DIR}/.local/build/pdf}"
TMP_DIR="${BOOK_TMP_DIR:-${ROOT_DIR}/.local/scratch/book_sources}"

usage() {
  cat <<'EOF'
Usage:
  bash doc/tools/book.sh -pdf <target>
  bash doc/tools/book.sh -list
  bash doc/tools/book.sh -h | --help

Targets:
  short_book          Build the complete short reading edition PDF
  research_companion  Build the complete expanded research companion PDF
  section_poker   Build the canonical Poker chapter PDF
  section_rps     Build the canonical Two Oracles RPS chapter PDF
  section_hash    Build the canonical Hash Functions appendix PDF
  section_trust   Build the canonical Trust Establishment chapter PDF
  section_ttt     Build the canonical Tic-Tac-Toe Core-Bind reference chapter PDF

Examples:
  bash doc/tools/book.sh -pdf section_poker
  bash doc/tools/book.sh -pdf section_rps
  bash doc/tools/book.sh -pdf section_hash
  bash doc/tools/book.sh -pdf section_trust
  bash doc/tools/book.sh -pdf section_ttt
EOF
}

list_targets() {
  cat <<'EOF'
short_book -> doc/adversarial_cooperation_short.tex
research_companion -> doc/adversarial_cooperation.tex
section_poker -> doc/content/poker_without_revealing_the_cards/poker_without_revealing_the_cards.tex
section_rps -> doc/content/two_oracles_play_rock–paper–scissors/two_oracles_play_rock–paper–scissors.tex
section_hash -> doc/content/aprendix/hash_functions/hash_functions.tex
section_trust -> doc/content/trust_establishment/trust_establishment.tex
section_ttt -> doc/content/tic_tac_toe_without_revealing_the_strategy/tic_tac_toe_without_revealing_the_strategy.tex
EOF
}

target_to_tex() {
  local target="$1"
  case "$target" in
    short_book)
      printf '%s\n' "doc/adversarial_cooperation_short.tex"
      ;;
    research_companion)
      printf '%s\n' "doc/adversarial_cooperation.tex"
      ;;
    section_poker)
      printf '%s\n' "doc/content/poker_without_revealing_the_cards/poker_without_revealing_the_cards.tex"
      ;;
    section_rps)
      printf '%s\n' "doc/content/two_oracles_play_rock–paper–scissors/two_oracles_play_rock–paper–scissors.tex"
      ;;
    section_hash)
      printf '%s\n' "doc/content/aprendix/hash_functions/hash_functions.tex"
      ;;
    section_trust)
      printf '%s\n' "doc/content/trust_establishment/trust_establishment.tex"
      ;;
    section_ttt)
      printf '%s\n' "doc/content/tic_tac_toe_without_revealing_the_strategy/tic_tac_toe_without_revealing_the_strategy.tex"
      ;;
    *)
      return 1
      ;;
  esac
}

compile_pdf_target() {
  local target="$1"
  local section_tex_rel section_tex_abs wrapper_tex

  section_tex_rel="$(target_to_tex "$target")" || {
    echo "Unknown target: $target" >&2
    echo "Run 'bash doc/tools/book.sh -list' to see available targets." >&2
    exit 1
  }

  section_tex_abs="${ROOT_DIR}/${section_tex_rel}"
  if [[ ! -f "$section_tex_abs" ]]; then
    echo "Target tex file not found: $section_tex_abs" >&2
    exit 1
  fi

  if ! command -v latexmk >/dev/null 2>&1; then
    echo "latexmk is required but not installed." >&2
    exit 1
  fi

  mkdir -p "$OUT_DIR"
  if [[ "$target" == "short_book" || "$target" == "research_companion" ]]; then
    local resolved_output compiled_name public_name
    if [[ "$target" == "short_book" ]]; then
      public_name="Adversarial-Cooperation-Short.pdf"
    else
      public_name="Adversarial-Cooperation.pdf"
    fi
    resolved_output="$(cd "$OUT_DIR" && pwd)"
    compiled_name="$(basename "$section_tex_abs" .tex).pdf"
    (
      cd "$ROOT_DIR"
      LATEX_OUTDIR="$resolved_output" bash doc/tools/compile_latex.sh -s "$section_tex_rel"
    )
    cp -- "$resolved_output/$compiled_name" "$resolved_output/$target.pdf"
    cp -- "$resolved_output/$compiled_name" "$resolved_output/$public_name"
    echo "PDF generated: $resolved_output/$target.pdf"
    echo "Linked edition: $resolved_output/$public_name"
    return
  fi

  mkdir -p "$TMP_DIR"
  wrapper_tex="${TMP_DIR}/${target}.tex"

  cat >"$wrapper_tex" <<EOF
\\documentclass[12pt,openany]{book}
\\input{${ROOT_DIR}/doc/utils/text_utils.tex}
\\input{${ROOT_DIR}/doc/utils/math_utils.tex}
\\hypersetup{hidelinks}
\\begin{document}
\\input{${section_tex_abs}}
EOF

  if [[ "$target" == "section_rps" || "$target" == "section_hash" || "$target" == "section_trust" || "$target" == "section_ttt" ]]; then
    cat >>"$wrapper_tex" <<'EOF'
\bibliographystyle{plain}
\bibliography{references}
EOF
  fi

  cat >>"$wrapper_tex" <<'EOF'
\end{document}
EOF

  echo "Compiling target '${target}'..."
  if [[ "$target" == "section_rps" || "$target" == "section_hash" || "$target" == "section_trust" || "$target" == "section_ttt" ]]; then
    BIBINPUTS="${ROOT_DIR}/doc:${BIBINPUTS:-}:" \
      latexmk -pdf -interaction=nonstopmode -halt-on-error -file-line-error \
        -outdir="$OUT_DIR" "$wrapper_tex"
  else
    latexmk -pdf -interaction=nonstopmode -halt-on-error -file-line-error \
      -outdir="$OUT_DIR" "$wrapper_tex"
  fi

  echo "PDF generated: ${OUT_DIR}/${target}.pdf"
}

main() {
  if [[ $# -eq 0 ]]; then
    usage
    exit 1
  fi

  case "$1" in
    -h|--help)
      usage
      ;;
    -list)
      list_targets
      ;;
    -pdf)
      if [[ $# -ne 2 ]]; then
        echo "Expected exactly one target after -pdf." >&2
        usage
        exit 1
      fi
      compile_pdf_target "$2"
      ;;
    *)
      echo "Unknown command: $1" >&2
      usage
      exit 1
      ;;
  esac
}

main "$@"
