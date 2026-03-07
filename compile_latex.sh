#!/usr/bin/env bash
# build.sh – Compile a LaTeX document with optional spell-checking
# ---------------------------------------------------------------
#  -s    Skip aspell spell-check
#  Any other non-option argument is treated as the main .tex file
#  If no .tex file is provided, the default one is used.

set -o pipefail               # make pipelines propagate the real exit code

# ─────────────────────────  Colour codes  ──────────────────────
RED='\033[1;31m'      # bold red
GREEN='\033[1;32m'    # bold green
YELLOW='\033[1;33m'   # bold yellow
MAGENTA='\033[1;35m'  # bold magenta
NC='\033[0m'          # reset

# ─────────────────────────  Option parsing  ────────────────────
skip_spell=false
while getopts ":s" opt; do
  case "$opt" in
    s) skip_spell=true ;;
    *) echo -e "${RED}Unknown option: -$OPTARG${NC}"; exit 1 ;;
  esac
done
shift $((OPTIND - 1))   # remove parsed options from $@

# ─────────────────────────  Target document  ───────────────────
tex_file=${1:-document/adversarial_cooperation.tex}

if [[ ! -f "$tex_file" ]]; then
  echo -e "${RED}LaTeX file not found: $tex_file${NC}"
  exit 1
fi

baseDir=$(dirname "$tex_file")

# ─────────────────────────  Spell check  ───────────────────────
if ! $skip_spell; then
  echo -e "${YELLOW}Performing spell check on *.tex in ${baseDir}${NC}"
  find "$baseDir" -type f -name '*.tex' -exec aspell --mode=tex --lang=en_US check {} \;
  if [[ $? -ne 0 ]]; then
    echo -e "${RED}Spell check interrupted. Compilation aborted.${NC}"
    exit 1
  fi
else
  echo -e "${YELLOW}Spell check skipped (-s).${NC}"
fi

# ─────────────────────────  Compile (colourised)  ───────────────
outdir=${LATEX_OUTDIR:-.temp}

echo "Compiling $tex_file (outdir: $outdir)..."
compile_cmd=(latexmk -cd -pdf -interaction=nonstopmode -outdir="$outdir" "$tex_file")

"${compile_cmd[@]}" 2>&1 | awk -v RED="$RED" -v YEL="$YELLOW" -v MAG="$MAGENTA" -v NC="$NC" '
  # TeX errors (start with "!" or contain "error")
  /^[!]/ || tolower($0) ~ /error/      {print RED $0 NC; next}
  # Warnings
  tolower($0) ~ /warning/              {print YEL $0 NC; next}
  # Over/Underfull boxes
  /Overfull|Underfull/                 {print MAG $0 NC; next}
  {print}  # everything else
'

if [[ ${PIPESTATUS[0]} -ne 0 ]]; then   # latexmk failed
  echo -e "${RED}Compilation failed.${NC}"
  exit 1
fi

echo -e "${GREEN}Compilation successful. PDF generated.${NC}"
