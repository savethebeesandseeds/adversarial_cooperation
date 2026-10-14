# Documents

Most chapters remain names and small notes that the author has not yet
revisited. Both editions assemble these starting points alongside a few
developed technical studies. A chapter's presence or length does not establish
that the author has developed its intended meaning; technical evidence applies
only to the specific work recorded in the research registers.

- [Evolving white paper](adversarial-cooperation-white-paper.pdf), with
  [editable source and build instructions](white-paper/README.md).
- [Short book](adversarial_cooperation_short.tex): the brief reading edition.
- [Research companion](adversarial_cooperation.tex): research notes and developed
  technical studies, with their evidence and open questions.
- [Edition map](research/EDITION_MAP.md): correspondence between both books,
  their chapters, and executable demonstrations.
- [Research records](research/README.md): charter, claims, plans, open problems,
  and preserved earlier writing.

Book sources use `content/`, `short/`, `utils/`, and `references.bib`.
Document build helpers are in `tools/`. From the repository root inside the
existing development environment:

```bash
bash doc/tools/book.sh -list
bash doc/tools/book.sh -pdf short_book
bash doc/tools/book.sh -pdf research_companion
bash doc/white-paper/build.sh
```

Generated PDFs go under `.local/build/`. The checked-in white-paper PDF is a
deliberate reader-facing document; building does not silently replace it.
Each proposal revision includes rebuilding and updating the reader-facing PDF
from the revised source. Page-count and visual layout checks wait until the
final review, as requested by the author.
