# Documents

- [Evolving white paper](Adversarial-Cooperation-White-Paper.pdf), with
  [editable source and build instructions](white-paper/README.md).
- [Short book](adversarial_cooperation_short.tex): the brief reading edition.
- [Research companion](adversarial_cooperation.tex): expanded chapters.
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
