# Adversarial Cooperation white paper

**Santiago Restrepo | contact@waajacu.com | https://waajacu.com/**

An evolving white paper, presented as a research proposal for professional
cryptographers and prospective research sponsors. The proposal presents the
program and an intended research direction from protocol specification to
hardware evaluation.
It distinguishes existing educational software from proposed private proofs
and synthesis. It does not introduce a theorem or claim novelty.

**Revision status (2026-09-13):** the source and PDF use standard IEEEtran
10-point type and margins. New Section II lists five
research targets: Rock-Paper-Scissors, Tic-Tac-Toe, Poker, Stag Hunt,
and Joint Optimization. Section I's author-intuition quotation and adjacent
poker/chess explanation have been removed. The earlier abstract is restored
with only "classical and modern" removed. The closing contact/peace paragraph
and the final demonstrator paragraph are removed; the author-block contact
remains. Section I's earlier opening is restored, and its candidate-tool
paragraph includes the charter's broader inventory. The five cases use a thin
gray vertical rule with individual bold headings and normal column flow.
Hardware evaluation
uses "would address." All five cited references remain.
The separate Scope and detailed Tic-Tac-Toe sections remain removed. Page-count
and visual layout checks are deferred until the final review at the author's
request.

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

The author chose `white-paper` filenames on 2026-09-13 as a stable home for
this developing document, distinct from the broader `doc/research/` records.
Its text still identifies it as a research proposal, reflecting the beginning
of the program. The [delivered PDF](../adversarial-cooperation-white-paper.pdf)
is directly in `doc/`; the editable source is this directory. The root README
links both. The redundant source ZIP remains archived locally. The scientific
text and all author revisions are preserved; this naming change did not
regenerate the PDF or change its contents.

## Read and edit offline

The delivered `adversarial-cooperation-white-paper.pdf` is self-contained and
needs no internet connection to read. This directory contains the editable
LaTeX, bibliography, build helper, and full unmodified IEEEtran distribution.
The manuscript license is at the repository root. Internet access
is needed only to follow external reference links or install a missing TeX
toolchain. No reference is loaded over the network during compilation.

Main source: `adversarial-cooperation-white-paper.tex`.
References: `references.bib`.

The proposal uses the official IEEEtran 1.8b conference class, US Letter pages,
two columns, normal 10-point body type, and IEEEtran bibliography style.
This is an independent research proposal using IEEE formatting, not an IEEE
publication, acceptance, endorsement, or venue-specific compliance certificate.
No margins or body font sizes were reduced to achieve the one-page length.

## Rebuild

The author's standing instruction is to rebuild the PDF whenever the manuscript
changes. Use the existing project environment, then update
`doc/adversarial-cooperation-white-paper.pdf` with the rebuilt result. Source
edits and PDF updates belong to the same revision. During iterative edits,
defer page-count and visual layout checks until the final review, as the
author clarified on 2026-09-13.

Required existing tools: Bash, pdfLaTeX, BibTeX, latexmk, and the usual TeX Live
packages for Times fonts, amsmath/amssymb, cite, color, and
hyperref. IEEEtran
is supplied locally. The repository's provisioned development container has
the other requirements already installed.

From this directory (Linux, a suitable Bash environment, or the existing
project container):

```bash
bash build.sh
```

The result, relative to the repository root, is
`.local/build/white-paper/adversarial-cooperation-white-paper.pdf`. An optional
first argument selects another output directory. The helper does not install
dependencies or manage containers.

From the repository root on the original Windows workstation, with the
documented container already running:

```powershell
docker exec adversarial-cooperation-dev /bin/bash doc/white-paper/build.sh
```

Refer to the [container guide](../../.local/container/README.md) for inspecting
and reusing the existing container.
Do not create a replacement container or volume just to build this proposal.

The `.bib` retains DOI fields and duplicates each DOI as a linked note because
the unmodified IEEEtran 1.14 bibliography style does not print the DOI field.
The document uses normal IEEEtran column flow.

## Evidence and scope

The repository's full verification task passed on 2026-09-11: 72 C test groups,
the corresponding AddressSanitizer/UndefinedBehaviorSanitizer runs, both book
builds, static-site checks, and native/WebAssembly parity checks. The proposal
does not present those tests as cryptographic proofs. It was separately built,
checked for resolved references and embedded fonts, and visually reviewed.

The local implementation baseline is Git commit
`9a919ed9a8cb3b44f93277c7e020bba96891797d`. The proposal describes that baseline;
its editable source is provided with this repository.
Supporting working artifacts can also be obtained from the author. The local audit trail
is recorded at
`doc/research/plans/adversarial-cooperation-white-paper.md`, with the baseline
verification bundle now under `.local/evidence/white-paper-baseline-20260911/`.

## Third-party formatting files

See `vendor/PROVENANCE.md`. The complete IEEEtran source distribution remains
unmodified with all its notices. It is a document-formatting dependency only;
no project cryptographic or runtime dependency was added.

The manuscript follows the repository's Creative Commons
Attribution-NonCommercial 4.0 license. IEEEtran retains its separate LaTeX
Project Public License; the manuscript license does not replace that license.
