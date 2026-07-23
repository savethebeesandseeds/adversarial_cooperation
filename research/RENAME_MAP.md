# Rename Map

## Policy

This is a proposal register, not an authorization to rename. Phase 0 performs
**no renames**. Any approved migration must use `git mv` for tracked paths, move
each `.bak` with its live source, update every build/document reference, account
for ignored and empty directories, and build/test before and after.

Prefer conservative ASCII filesystem names. Preserve typographic Unicode,
Latin, Chinese, Spanish, and the author's intended titles in display prose.

| ID | Current path / identifier | Proposed path / identifier | Display title | Known fan-out | Status and reason |
|---|---|---|---|---|---|
| RN-001 | `demostrations/` | `demonstrations/` | Demonstrations | Root `book`; `Makefile.config`; root, source, and demonstration READMEs; poker manuscript and demo note; all generated output paths; many empty mirror directories | **Defer, high fan-out.** Obvious spelling fix, but generated-artifact policy and canonical poker-source decision should happen first. |
| RN-002 | `document/content/aprendix/` | `document/content/appendix/` | Appendix | Five current source files, four backups, eight empty document directories, and future include paths | **Proposed.** High-confidence spelling correction. No current appendix file is included by the main document. |
| RN-003 | `demostrations/aprendix/` | `demonstrations/appendix/` | Appendix demonstrations | Eleven currently empty mirror directories plus root rename RN-001 | **Defer with RN-001/RN-002.** Empty directories are not represented in Git and need placeholders or an explicit decision. |
| RN-004 | `document/content/adversarial_aftermatch/adversarial_aftermatch.tex` and `.bak` | `document/content/adversarial_aftermath/adversarial_aftermath.tex` and `.bak` | Adversarial Aftermath | Empty mirrored demonstration directory; research charter/roadmap/matrix references | **Author review.** “Aftermath” is used in current prose and charter, but “aftermatch” might be deliberate wordplay. |
| RN-005 | `demostrations/adversarial_aftermatch/` | `demonstrations/adversarial_aftermath/` | Adversarial Aftermath demonstration | RN-001 and RN-004 | **Defer.** Apply only if the author confirms RN-004. |
| RN-006 | `document/content/aprendix/cryptographic_forgivness/cryptographic_forgivness.tex` | `document/content/appendix/cryptographic_forgiveness/cryptographic_forgiveness.tex` | Cryptographic Forgiveness | Empty mirrored demonstration directory; charter and registers | **Proposed.** High-confidence English spelling correction; retain the evocative display title. |
| RN-007 | `demostrations/aprendix/cryptographic_forgivness/` | `demonstrations/appendix/cryptographic_forgiveness/` | Cryptographic Forgiveness demonstration | RN-001, RN-002, RN-003, RN-006 | **Defer as grouped migration.** |
| RN-008 | `document/content/figthing_dicipates_strength/figthing_dicipates_strength.tex` | `document/content/fighting_dissipates_strength/fighting_dissipates_strength.tex` | Fighting Dissipates Strength | Charter and registers; no mirrored demo directory currently exists | **Proposed.** High-confidence correction of both words, but display-title nuance remains author-reviewable. |
| RN-009 | `document/content/law_without_referencering_the_law/law_without_referencering_the_law.tex` | `document/content/law_without_referencing_the_law/law_without_referencing_the_law.tex` | Law Without Referencing the Law | Empty mirrored demonstration directory; charter and all registers | **Proposed.** High-confidence spelling correction. |
| RN-010 | `demostrations/law_without_referencering_the_law/` | `demonstrations/law_without_referencing_the_law/` | Law Without Referencing the Law demonstration | RN-001 and RN-009 | **Defer as grouped migration.** |
| RN-011 | `document/content/two_oracles_play_rock–paper–scissors/two_oracles_play_rock–paper–scissors.tex` | `document/content/two_oracles_play_rock-paper-scissors/two_oracles_play_rock-paper-scissors.tex` | Two Oracles Play Rock–Paper–Scissors | Charter, chapter matrix, claim/open-problem registers, standalone section target, and future include/build paths | **Deferred; current Unicode path retained for the first slice.** Approval to proceed with the protocol was not treated as rename approval. New C/test/target names use ASCII; any later migration remains a separate `git mv` task with full reference and build checks. |
| RN-012 | `document/content/aprendix/ISO-IEC/` | `document/content/appendix/iso_iec/` | ISO/IEC Standards | `iso_iec.tex`; empty mirrored demonstration directory | **Optional consistency only.** Existing path is ASCII and usable; lower-case underscore is conventional, not scientifically important. |
| RN-013 | `demostrations/aprendix/ISO-IEC/` | `demonstrations/appendix/iso_iec/` | ISO/IEC Standards demonstration | RN-001, RN-002, RN-003, RN-012 | **Optional/defer.** |
| RN-014 | `document/content/aprendix/block_chains/` | `document/content/appendix/blockchains/` | Blockchains | Empty document and demonstration directories only | **Optional/defer.** Conventional spelling; no content exists to preserve. |
| RN-015 | `demostrations/aprendix/block_chains/` | `demonstrations/appendix/blockchains/` | Blockchain demonstrations | RN-001, RN-002, RN-003, RN-014 | **Optional/defer.** |
| RN-016 | `document/content/adversario_sibi_perfidiam_revelare/` and source/backup | **No proposal yet** | Intended Latin title pending | Empty mirrored demonstration directory; registers currently use the filesystem slug plus an English gloss | **Blocked on author/Latin review.** The charter explicitly prohibits guessing the intended Latin. |
| RN-017 | README container name `adverarial_cooperation` | `adversarial_cooperation` | Adversarial Cooperation container | Root README only; may affect users with an existing container | **Documentation identifier, defer.** Correct spelling, but changing a container name can break copied workflows and should accompany build-doc repair. |

## Empty Directory Preservation

The following document appendix directories contain no tracked file and will
not survive a clone or a tracked-only rename:

- `abstract_zero_knowledge_proofs`
- `block_chains`
- `complexity_theoretic_security`
- `digital_signatures`
- `fully_homomorphic_encryption`
- `information_theoretic_security`
- `security_models`
- `zero_knowledge_proofs`

Their demonstration mirrors are also empty. Before a directory migration,
either add intentional placeholder/index files in an approved task or decide
that the empty structure is non-authoritative.

## High-Fan-Out References for `demostrations/`

Static audit found references in:

- `book:5,25,33`
- `Makefile.config:15`
- `README.md:38,44,50`
- `demostrations/README.md:8,15,20`
- `src/README.md:13,18`
- `document/content/poker_without_revealing_the_cards/poker_without_revealing_the_cards.tex:151`
- `demostrations/poker_without_revealing_the_cards/poker_without_revealing_the_cards.tex:34,69,74-76`

Generated and tracked build artifacts contain additional absolute/relative
paths and should not drive source naming; they require a separate artifact
policy.
