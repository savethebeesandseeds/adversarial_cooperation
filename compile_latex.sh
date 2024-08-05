#!/bin/bash
# Bash script to compile a specified LaTeX document using latexmk, check spelling in all .tex files, and open the PDF on Linux

# Color definitions
RED='\033[0;31m'    # Red color for errors
GREEN='\033[0;32m'  # Green color for success messages
YELLOW='\033[0;33m' # Yellow color for warnings
NC='\033[0m'        # No color, reset to default terminal color

if [ -z "$1" ]; then
    echo -e "${YELLOW}No document specified. Running script for default document.${NC}"
    sh compile_latex.sh document/adversarial_cooperation.tex
    exit 0
fi

# Determine the base directory from the first argument
baseDir=$(dirname "$1")

# Spell check all .tex files in the base directory
echo -e "${YELLOW}Performing spell check on all .tex files in the directory: $baseDir${NC}"
find "$baseDir" -type f -name '*.tex' -exec aspell --mode=tex --lang=en_US check {} \;

# Check if aspell was canceled or finished
if [ $? -ne 0 ]; then
    echo -e "${RED}Spell check was interrupted. Compilation aborted.${NC}"
    exit 1
fi

echo "Compiling $1..."
latexmk -cd -pdf -interaction=nonstopmode -outdir=temp "$1"

# Check if LaTeX compilation was successful
if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed with errors.${NC}"
    exit 1
fi

echo -e "${GREEN}Compilation successful. PDF generated.${NC}"