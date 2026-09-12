# Execution Plan: Standard Chapter Views and Read-Only Change Indications

## Status

Complete (2026-08-01).

## Objective

Repair the first static browser edition without creating a second manuscript
or giving a hosted page authority to modify the repository.

Every chapter will expose the same three views:

1. `Standard`: a conservative readable rendering derived from canonical LaTeX;
2. `Demo`: the registered executable companion, or an explicit unavailable
   state; and
3. `Document`: the exact canonical LaTeX with source-block provenance.

Text selection may produce a copyable change indication for later human/Codex
review. It must never write, upload, or silently reinterpret manuscript text.

## Superseded Boundary

This plan succeeds `browser-book-studio-wasm.md`. It preserves that plan's
single-source, static-build, source-range, WebAssembly, and GitHub Pages
boundaries. It supersedes only the optional direct browser-to-filesystem edit
path. The completed first-attempt plan remains as the historical record of
what was built and tested.

## Safety Contract

- `document/**/*.tex` remains the only authored manuscript.
- The browser receives no repository directory handle and invokes no file
  write API.
- Standard-view selections identify complete enclosing source blocks. They do
  not claim a character-perfect reverse mapping through LaTeX formatting.
- Document-view selections use the same block-level provenance in this slice.
- A copied indication includes chapter identity, file hash, source path, block
  identities, line and byte/character ranges, selected visible text, exact raw
  LaTeX for the enclosing blocks, and the reader's note.
- Copying is a local clipboard action. No network submission or manuscript
  mutation occurs.
- Unsupported LaTeX is shown as an explicitly labeled fallback in Standard
  view and exactly in Document view; it is never silently discarded or
  presented as successfully typeset.
- Demonstrations are enabled only by an exact generated registry entry.
  Missing demonstrations remain visible as unavailable.

## Exact File Scope

Modify:

1. `README.md`
2. `research/DECISIONS.md`
3. `setup.sh`
4. `web/README.md`
5. `web/package.json`
6. `web/src/app.mjs`
7. `web/src/index.html`
8. `web/src/latex.mjs`
9. `web/src/styles.css`
10. `web/tools/build-site.mjs`
11. `web/tools/test-site.mjs`
12. `container/SECURITY.md`

Create:

13. `research/plans/browser-chapter-views-source-indications.md`
14. `web/src/render.mjs`
15. `web/src/views.mjs`
16. `web/tests/rendering-fixtures.mjs`
17. `web/tools/test-render.mjs`
18. `web/tools/test-views.mjs`
19. `web/tools/test-change-indications.mjs`
20. `web/tools/test-static-ui.mjs`

No manuscript, C implementation, cryptographic primitive, backup, dependency
lockfile, container definition, or generated `web/dist` artifact is in scope.

## Work Sequence

1. Record the current static and native/WebAssembly test baseline.
2. Change the generated manifest to an explicit three-view contract and exact
   demo registry.
3. Replace direct editing with immutable, copyable change indications.
4. Add a dependency-free Standard renderer for common chapter constructs and
   visible fallbacks for everything else.
5. Add the Standard, Demo, and Document tab panels and stable hash routing.
6. Add schema, renderer, view, static-UI, and no-write regression tests.
7. Rebuild through the dedicated container and inspect representative chapters
   and both available/unavailable Demo states in the live browser.
8. Run the lossless manuscript-source corpus, native bridge, and WebAssembly
   parity checks. Do not rebuild the unchanged PDF for this presentation-only
   slice.

## Acceptance Criteria

- all 27 canonical sources still appear once in book order;
- every generated chapter advertises Standard, Demo, and Document views;
- Tic-Tac-Toe is the only currently available browser demo;
- every other Demo view explains that no browser companion is ready;
- old `#chapter=<id>` links still open Standard view;
- direct folder grants and file-writing code are absent from built assets;
- the build rejects canonical source roots or path segments that are symbolic
  links, non-regular source files, and invalid UTF-8 before publication;
- selecting text creates a block-provenance indication without changing the
  source or generated manifest;
- labels and layout commands do not appear as ordinary reading prose;
- quotes, abstracts, lists, common tables, code, and math receive readable
  semantic treatment or a conspicuous exact-source fallback;
- Document view exposes the exact raw LaTeX and line ranges;
- the Tic-Tac-Toe demo still executes the compiled C checker through
  WebAssembly and retains its disclosure warning;
- static, renderer, view, indication, UI, native bridge, WebAssembly parity,
  and lossless manuscript-source corpus checks pass in the dedicated
  container;
- no PDF rebuild is required because this slice changes no manuscript source;
  and
- no unrelated dirty-worktree file is changed.

## Research and Claim Boundary

This work changes presentation and editorial coordination only. It introduces,
strengthens, weakens, or removes no cryptographic, mathematical, historical,
novelty, or implementation claim in the manuscript. A rendered formula is not
new evidence, a browser demo is not a proof, and a copied change indication is
not an accepted manuscript edit.

## Completion Record

The generated manifest now uses schema version 2 and gives all 27 canonical
sources the same Standard, Demo, and Document contract. Tic-Tac-Toe is the
only registered browser demo. The browser application contains no directory
grant, writable file handle, source replacement, or manuscript save path. The
obsolete browser-edit validation and hashing helpers were removed as dead code,
not merely disconnected from the interface.

Canonical-source ingestion now hashes the exact UTF-8 bytes it publishes. The
builder resolves and contains both lexical and real paths, rejects symbolic
links at the source root and at every source segment, rejects non-regular files,
and decodes with fatal UTF-8 validation. Temporary-fixture tests exercise the
valid, traversal, symlinked-file, symlinked-directory, and invalid-UTF-8 cases.

The Standard renderer is a dependency-free inert-data adapter. Browser code
creates DOM nodes through `createElement`, `createTextNode`, and `textContent`;
it does not inject generated HTML. Common headings, paragraphs, styled text,
abstracts, quotations, lists, framed material, tables, code, theorem-like
environments, and mathematics receive bounded treatment. Unsupported syntax
retains its raw source in a labeled fallback. Document view remains the exact
read-only source view.

Verification ran as numeric user 65532 with no host Node installation, inside
the dedicated `adversarial-cooperation-preview-1` container and its ephemeral
`/tmp/ac-work-preview` work copy:

```text
node --check web/src/app.mjs
node --check web/src/latex.mjs
node --check web/src/render.mjs
node --check web/src/ttt-worker.mjs
node --check web/src/views.mjs
node web/tools/build-site.mjs
node web/tools/build-wasm.mjs
node web/tools/test-site.mjs
node web/tools/test-render.mjs
node web/tools/test-views.mjs
node web/tools/test-change-indications.mjs
node web/tools/test-static-ui.mjs
node web/tools/test-wasm.mjs
```

Results:

- static book: five parts and 27 ordered, losslessly mapped sources, with the
  canonical real-path, symbolic-link, regular-file, and UTF-8 boundary tested;
- renderer: 17 block fixtures, two inline fixtures, 28 `.tex` files, and
  1,294 source blocks passed the inert-data, no-hidden-prose, and corpus checks;
- view routing, exact demo registration, unavailable-demo states, immutable
  indications, and absence of browser write paths across both source and built
  runtime assets passed;
- native bridge: six tests, zero failures;
- native/WebAssembly fixtures agreed; and
- `git diff --check` reported no patch whitespace error.

The live loopback site was inspected in the in-app browser. Standard rendering
was checked on Tic-Tac-Toe, Introduction, Hash Functions, and Addressing
Goodhart's Law. The Tic-Tac-Toe WebAssembly lab returned the expected
`Non-losing` reference-policy verdict. Introduction displayed the explicit
unavailable Demo state. Document view displayed exact LaTeX and line ranges,
and opening a source block produced a read-only, hash-bound change indication.
Responsive navigation was also exercised. At the final narrow live viewport,
both body and document scroll widths equaled their 562-pixel client widths:
long paths and ordinary inline code wrapped, while wide tables retained a
local horizontal scroller rather than widening the page.

## Remaining Presentation Limits

Standard view is intentionally not TeX. It displays mathematical source in a
readable formula treatment but does not perform full mathematical typesetting,
and cross-references currently expose their stable label keys rather than the
PDF's resolved numbers. The compiled PDF remains the typographic authority;
Document view remains the exact fallback. No manuscript or claim was changed,
so this slice did not rebuild or reinterpret the PDF. Source blocks remain
keyboard focusable to preserve a no-mouse indication path; very long chapters
therefore have many tab stops. A later accessibility pass should replace that
with a roving source-block navigator without removing keyboard access.
