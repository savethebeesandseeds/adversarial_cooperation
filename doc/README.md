# Documents

- [Research proposal](Adversarial-Cooperation-Research-Proposal.pdf), with
  [editable source and build instructions](research-proposal/README.md).
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
bash doc/research-proposal/build.sh
```

Generated PDFs go under `.local/build/`. The checked-in proposal PDF is a
deliberate reader-facing document; building does not silently replace it.
