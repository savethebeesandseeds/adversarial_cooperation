# Adversarial Cooperation research proposal

**Santiago Restrepo | contact@waajacu.com | https://waajacu.com/**

Two-page research proposal, September 2026, prepared for professional
cryptographers and prospective research sponsors. The proposal presents the
program and a bounded path from protocol specification to hardware evaluation.
It distinguishes existing educational software from proposed private proofs
and synthesis. It does not introduce a theorem or claim novelty.

Revised 2026-09-12 in response to the author's six PDF comments: the title is
now simply "Adversarial Cooperation," the affiliation line is removed, the
abstract and introduction focus on referee-free game-theoretic protocols and
strategic privacy, the keyword is "hardware," and the closing section directly
requests funding, collaborators and computing/hardware resources. The author's
poker and chess images are preserved. A subsequent author clarification on
the same date defines the chess goal as privately computing the outcome of
fixed move-selection algorithms, avoiding the strategic disclosure of public
play. Rules, computation bounds and termination must be specified; internal
simulation may still be necessary. This remains proposed work. The email and
website remain in the author block.

Two further comments on 2026-09-12 make participation conditional on each
party independently assessing the algorithm and fabricating its own trusted
hardware. Preventing early result disclosure followed by strategic withdrawal
is now stated as a research objective. The release mechanism and any reliance
on another party's hardware remain to be justified; local fabrication alone
is not presented as establishing those properties.

The author's final naming revision on 2026-09-12 designates this document a
research proposal, reflecting the beginning of the program. The delivered PDF
and source archive are now at the repository root and linked prominently from
its README. The scientific text and all earlier author revisions are preserved.

## Read and edit offline

The delivered `Adversarial-Cooperation-Research-Proposal.pdf` is self-contained and
needs no internet connection to read. The source archive includes that PDF,
this directory's editable LaTeX and bibliography, the build helper, the full
unmodified IEEEtran distribution, and the manuscript license. Internet access
is needed only to follow external reference links or install a missing TeX
toolchain. No reference is loaded over the network during compilation.

Main source: `adversarial-cooperation-research-proposal.tex`.
References: `references.bib`.

The proposal uses the official IEEEtran 1.8b conference class, US Letter pages,
two columns, normal 10-point body type, and IEEEtran bibliography style.
This is an independent research proposal using IEEE formatting, not an IEEE
publication, acceptance, endorsement, or venue-specific compliance certificate.
No margins or body font sizes were reduced to achieve the two-page length.

## Rebuild

Required existing tools: Bash, pdfLaTeX, BibTeX, latexmk, and the usual TeX Live
packages for Times fonts, amsmath/amssymb, cite, balance, and hyperref. IEEEtran
is supplied locally. The repository's provisioned development container has
the other requirements already installed.

From this directory (Linux, a suitable Bash environment, or the existing
project container):

```bash
bash build.sh
```

The result is `.build/adversarial-cooperation-research-proposal.pdf`. An optional
first argument selects another output directory. The helper does not install
dependencies or manage containers.

From the repository root on the original Windows workstation, with the
documented container already running:

```powershell
docker exec adversarial-cooperation-dev /bin/bash document/research-proposal/build.sh /workspace/.temp/research-proposal/build
```

Refer to the root README for inspecting and reusing the existing container.
Do not create a replacement container or volume just to build this proposal.

The `.bib` retains DOI fields and duplicates each DOI as a linked note because
the unmodified IEEEtran 1.14 bibliography style does not print the DOI field.
The `balance` package balances the final page's columns.

## Evidence and scope

The repository's full verification task passed on 2026-09-11: 72 C test groups,
the corresponding AddressSanitizer/UndefinedBehaviorSanitizer runs, both book
builds, static-site checks, and native/WebAssembly parity checks. The proposal
does not present those tests as cryptographic proofs. It was separately built,
checked for resolved references and embedded fonts, and visually reviewed.

The local implementation baseline is Git commit
`9a919ed9a8cb3b44f93277c7e020bba96891797d`. The proposal describes that baseline;
its editable source and offline bundle are provided with this repository.
Supporting working artifacts can also be obtained from the author. The local audit trail
retains its historical path, `research/plans/adversarial-cooperation-white-paper.md`, with the baseline
verification bundle under `.container-output/white-paper-baseline-20260911/`.

## Third-party formatting files

See `vendor/PROVENANCE.md`. The complete IEEEtran source distribution remains
unmodified with all its notices. It is a document-formatting dependency only;
no project cryptographic or runtime dependency was added.

The manuscript follows the repository's Creative Commons
Attribution-NonCommercial 4.0 license. IEEEtran retains its separate LaTeX
Project Public License; the manuscript license does not replace that license.
