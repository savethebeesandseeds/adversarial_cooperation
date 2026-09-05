import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { fileURLToPath, pathToFileURL } from "node:url";
import path from "node:path";

const toolsDirectory = path.dirname(fileURLToPath(import.meta.url));
const webDirectory = path.resolve(toolsDirectory, "..");
const roots = [path.join(webDirectory, "src"), path.join(webDirectory, "dist")];

for (const root of roots) {
  const {
    bookEditions, editionPdfPath, readingEdition,
  } = await import(pathToFileURL(path.join(root, "book-editions.mjs")).href);
  assert.deepEqual(bookEditions.map(({ id, title, pdfName }) => [id, title, pdfName]), [
    ["short", "Short book", "Adversarial-Cooperation-Short.pdf"],
    ["companion", "Research companion", "Adversarial-Cooperation.pdf"],
  ]);
  for (const hash of ["", "#", "#read", "#read=short"]) {
    assert.equal(readingEdition(hash), bookEditions[0], "default reading route must select the short book");
  }
  assert.equal(readingEdition("#read=companion"), bookEditions[1]);
  for (const hash of ["#read=unknown", "#read=COMPANION", "#demos", "#about"]) {
    assert.equal(readingEdition(hash), null, "unrecognized reading route must not silently select an edition");
  }
  assert.equal(editionPdfPath(bookEditions[0]), "./book/Adversarial-Cooperation-Short.pdf");
  assert.equal(editionPdfPath(bookEditions[1]), "./book/Adversarial-Cooperation.pdf");
  const html = await readFile(path.join(root, "index.html"), "utf8");
  const css = await readFile(path.join(root, "styles.css"), "utf8");
  const app = await readFile(path.join(root, "app.mjs"), "utf8");

  const routes = [...html.matchAll(/<a\b[^>]*data-route="([^"]+)"[^>]*>([^<]+)<\/a>/gu)]
    .map((match) => [match[1], match[2].trim()]);
  assert.deepEqual(routes, [
    ["read", "Read"],
    ["demos", "Demos"],
    ["about", "About"],
  ], `${root}: public navigation must contain only Read, Demos, and About`);

  assert.match(html, /<main\b[^>]*id="main-content"/u);
  assert.match(html, /<noscript>[\s\S]*\.\/book\/Adversarial-Cooperation\.pdf[\s\S]*<\/noscript>/u);
  assert.match(html, /<script\b[^>]*type="module"[^>]*src="\.\/app\.mjs"/u);
  assert.match(html, /<noscript>[\s\S]*\.\/book\/Adversarial-Cooperation-Short\.pdf[\s\S]*<\/noscript>/u);
  assert.match(app, /readingEdition\(hash\)/u);
  assert.match(app, /renderRead\(route\.edition\)/u);
  assert.match(app, /editionPdfPath\(edition\)/u);
  assert.match(app, /"aria-label": "Book edition"/u);
  assert.match(app, /choice\.id === edition\.id[\s\S]*setAttribute\("aria-current", "page"\)/u);
  assert.match(app, /bookEditions\.map[\s\S]*editionPdfPath\(choice\)/u);
  assert.match(app, /createDemoRunner/u);
  assert.match(app, /demoRegistry|\bdemos\b/u);
  assert.match(app, /addEventListener\("pageshow"[\s\S]*event\.persisted[\s\S]*renderRoute/u);
  for (const chapterSpecificShellCopy of [
    /Checker verdict/u,
    /Replayable losing line/u,
    /C checker ready/u,
    /Run exhaustive check/u,
    /Choose a public fixture/u,
  ]) {
    assert.doesNotMatch(app, chapterSpecificShellCopy, `${root}: shared demo shell contains chapter-specific copy`);
  }
  for (const route of ["read", "demos", "about"]) {
    assert.match(app, new RegExp(`["']${route}["']`, "u"));
  }

  for (const obsolete of [
    /book\.json/u,
    /latex\.mjs/u,
    /render\.mjs/u,
    /views\.mjs/u,
    /showDirectoryPicker/u,
    /selection-action/u,
    /indication-output/u,
    /source-document/u,
    /data-selectable/u,
    /Save to LaTeX/u,
    /Indicate a manuscript change/u,
  ]) {
    assert.doesNotMatch(html, obsolete, `${root}: obsolete browser-review UI remains in HTML`);
    assert.doesNotMatch(app, obsolete, `${root}: obsolete browser-review runtime remains in app`);
  }
  assert.doesNotMatch(app, /\.(?:innerHTML|outerHTML)\s*=|insertAdjacentHTML\s*\(/u);

  assert.match(css, /\.site-navigation/u);
  assert.match(css, /\.edition-selector/u);
  assert.match(css, /\.edition-downloads/u);
  assert.match(css, /\.pdf-(?:reader|frame|fallback)/u);
  assert.match(css, /\.demo-(?:gallery|card|workspace)/u);
  assert.match(css, /@media\s*\(max-width:/u);
  for (const obsolete of [
    /\.view-tabs\b/u,
    /\.source-block\b/u,
    /\.source-document\b/u,
    /\.selection-action\b/u,
    /\.source-dialog\b/u,
  ]) {
    assert.doesNotMatch(css, obsolete, `${root}: obsolete browser-review CSS remains`);
  }
}

process.stdout.write("Static UI tests passed: two edition routes and PDF fallbacks, Read/Demos/About shell, reusable demo runtime, and no editor or source-map surface.\n");
