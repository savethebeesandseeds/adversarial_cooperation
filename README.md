## License
This work is licensed under a Creative Commons Attribution-NonCommercial 4.0 International License.

![Creative Commons License](https://i.creativecommons.org/l/by-nc/4.0/88x31.png)

For more details, see the [LICENSE](LICENSE) file in this repository.

## Description

This is a work in progress. 

## Create Enviroment
docker run -it --name=adverarial_cooperation -v .:/src debian:11
docker exec -it adverarial_cooperation /bin/bash

## Install dependencies
apt update
apt install --no-install-recommends texlive texlive-latex-extra texlive-lang-spanish latexmk
apt install --no-install-recommends aspell aspell-en

## Compile book
cd /src
sh compile_latex.sh adversarial_cooperation.tex

## Compile scripts
