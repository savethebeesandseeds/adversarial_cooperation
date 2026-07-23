## License
This work is licensed under a Creative Commons Attribution-NonCommercial 4.0 International License.

![Creative Commons License](https://i.creativecommons.org/l/by-nc/4.0/88x31.png)

For more details, see the [LICENSE](LICENSE) file in this repository.

## Description

This is a work in progress. 

## Create Environment

```bash
docker run -it --name=adversarial_cooperation -v .:/src debian:11
docker exec -it adversarial_cooperation /bin/bash
```

## Install dependencies

The C companions use the C11 standard library plus one cryptographic backend,
libsodium. The protocol and state-machine code do not depend on any other
third-party C library.

```bash
apt-get update
apt-get install -y --no-install-recommends \
  build-essential make pkg-config libsodium-dev \
  texlive texlive-latex-extra texlive-lang-spanish texlive-science latexmk \
  aspell aspell-en coreutils
```

## Compile book

The corrected full-book entry point is:

```bash
cd /src
bash compile_latex.sh -s document/adversarial_cooperation.tex
```

The currently included main manuscript (Trust Establishment plus the placeholder
appendix) builds as a complete PDF in the recorded disposable environment.
Dormant chapters outside that include graph remain unfinished and are not
implied to compile.

## Compile scripts

Build section PDFs with the `book` helper script:

```bash
cd /src
bash book -list
bash book -pdf section_poker
bash book -pdf section_rps
bash book -pdf section_hash
bash book -pdf section_trust
bash book -pdf section_ttt
```

Output:

- `demostrations/.build/pdf/section_poker.pdf`
- `demostrations/.build/pdf/section_rps.pdf`
- `demostrations/.build/pdf/section_hash.pdf`
- `demostrations/.build/pdf/section_trust.pdf`
- `demostrations/.build/pdf/section_ttt.pdf`

## C demonstrations
The repository now includes:

- `src/`: C source code for game demonstrations.
- `demostrations/`: `.tex` notes mirroring `document/content` structure.

Implemented examples:

```bash
cd /src
make -C demostrations demo_poker
make -C demostrations demo_rps RPS_ARGS='--alice rock --bob scissors'
make -C demostrations demo_rps RPS_ARGS='--alice rock --bob scissors --selective-abort'
make -C demostrations demo_hash
make -C demostrations demo_ttt
make -C tests test
```

The hash companion calls the existing libsodium BLAKE2b backend through one
fixed-output wrapper. It does not add a library, implement a hash algorithm, or
provide password hashing, authentication, signatures, encryption, or a
production API.

The Tic-Tac-Toe companion is a plain-C11 public checker for a fully disclosed
policy. It adds no library or cryptographic primitive and does not implement
commitment, zero knowledge, proof of knowledge, private evaluation, or
anti-cloning. It establishes the public finite predicate that a later private
construction would first have to preserve.
