import assert from "node:assert/strict";
import { readFile, readdir, stat } from "node:fs/promises";
import { fileURLToPath } from "node:url";
import path from "node:path";

const toolsDirectory = path.dirname(fileURLToPath(import.meta.url));
const webDirectory = path.resolve(toolsDirectory, "..");
const sourceRoot = path.join(webDirectory, "src");
const distributionRoot = path.join(webDirectory, "dist");

async function listFiles(root, relative = "") {
  const directory = path.join(root, relative);
  const entries = await readdir(directory, { withFileTypes: true });
  const files = [];
  for (const entry of entries) {
    const entryRelative = path.join(relative, entry.name);
    if (entry.isDirectory()) {
      files.push(...await listFiles(root, entryRelative));
    } else if (entry.isFile()) {
      files.push(entryRelative.split(path.sep).join("/"));
    }
  }
  return files.sort();
}

async function assertCopiedSourceAssets() {
  const sourceFiles = await listFiles(sourceRoot);
  for (const relativePath of sourceFiles) {
    const sourceBytes = await readFile(path.join(sourceRoot, ...relativePath.split("/")));
    const copiedBytes = await readFile(path.join(distributionRoot, ...relativePath.split("/")));
    assert.deepEqual(copiedBytes, sourceBytes, `generated asset differs: ${relativePath}`);
  }

  const noJekyll = await stat(path.join(distributionRoot, ".nojekyll"));
  assert.equal(noJekyll.isFile(), true);
  assert.equal(noJekyll.size, 0);
}

async function assertPdf() {
  const pdfPath = path.join(
    distributionRoot,
    "book",
    "Adversarial-Cooperation.pdf",
  );
  const pdf = await readFile(pdfPath);
  assert.ok(pdf.length >= 1024, "compiled book PDF is implausibly small");
  assert.equal(pdf.subarray(0, 5).toString("ascii"), "%PDF-", "compiled book has no PDF header");
  assert.match(pdf.subarray(Math.max(0, pdf.length - 1024)).toString("latin1"), /%%EOF\s*$/u);
}

function moduleSpecifiers(source) {
  const specifiers = [];
  for (const pattern of [
    /\b(?:from\s*|import\s*\()\s*["']([^"']+)["']/gu,
    /\bimport\s*["']([^"']+)["']/gu,
  ]) {
    for (const match of source.matchAll(pattern)) {
      specifiers.push(match[1]);
    }
  }
  return specifiers;
}

async function assertStaticBoundaries() {
  const files = await listFiles(distributionRoot);
  for (const required of [
    ".nojekyll",
    "index.html",
    "styles.css",
    "app.mjs",
    "demo-contract.mjs",
    "demo-registry.mjs",
    "demos/ttt-demo.mjs",
    "ttt-worker.mjs",
    "book/Adversarial-Cooperation.pdf",
    "assets/ttt-module.mjs",
    "assets/ttt-module.wasm",
  ]) {
    assert.ok(files.includes(required), `missing public asset: ${required}`);
  }

  assert.deepEqual(
    files.filter((file) => file.startsWith("assets/")),
    ["assets/ttt-module.mjs", "assets/ttt-module.wasm"],
    "the public asset directory must contain only the one registered demo",
  );
  for (const forbidden of [
    "data/book.json",
    "latex.mjs",
    "render.mjs",
    "views.mjs",
  ]) {
    assert.equal(files.includes(forbidden), false, `obsolete browser manuscript asset leaked: ${forbidden}`);
  }
  assert.equal(files.some((file) => file.endsWith(".tex")), false, "raw LaTeX leaked into the static site");
  assert.equal(files.some((file) => file.endsWith(".json")), false, "serialized manuscript or package data leaked into the static site");

  const textAssets = files.filter((file) => /\.(?:html|mjs|css)$/iu.test(file));
  for (const relativePath of textAssets) {
    const source = await readFile(
      path.join(distributionRoot, ...relativePath.split("/")),
      "utf8",
    );
    assert.doesNotMatch(source, /(?:src|href|data)\s*=\s*["']\/(?!\/)/iu, `${relativePath}: root-relative HTML URL breaks project Pages`);
    assert.doesNotMatch(source, /(?:fetch|new\s+Worker)\s*\(\s*["']\/(?!\/)/u, `${relativePath}: root-relative runtime URL breaks project Pages`);
    assert.doesNotMatch(source, /url\(\s*["']?\/(?!\/)/iu, `${relativePath}: root-relative CSS URL breaks project Pages`);
    assert.doesNotMatch(source, /\b(?:https?:)?\/\//iu, `${relativePath}: external runtime URL violates the dependency-free static boundary`);
    assert.doesNotMatch(source, /\.(?:innerHTML|outerHTML)\s*=|insertAdjacentHTML\s*\(/u, `${relativePath}: HTML-string injection API is not permitted`);

    if (relativePath.endsWith(".mjs")) {
      for (const specifier of moduleSpecifiers(source)) {
        const compilerGeneratedNodeCompatibilityImport =
          relativePath === "assets/ttt-module.mjs" && specifier === "module";
        const allowed = compilerGeneratedNodeCompatibilityImport ||
          specifier.startsWith("./") ||
          specifier.startsWith("../") ||
          (relativePath.startsWith("assets/") && specifier.startsWith("node:"));
        assert.equal(allowed, true, `${relativePath}: external or bare module dependency ${specifier}`);
      }
    }
  }
}

async function assertNoJavaScriptDependencies() {
  const packageSource = await readFile(path.join(webDirectory, "package.json"), "utf8");
  const packageJson = JSON.parse(packageSource);
  assert.equal(Object.hasOwn(packageJson, "dependencies"), false);
  assert.equal(Object.hasOwn(packageJson, "devDependencies"), false);
}

await assertCopiedSourceAssets();
await assertPdf();
await assertStaticBoundaries();
await assertNoJavaScriptDependencies();

process.stdout.write("Static site tests passed: compiled PDF, complete reader assets, one demo, relative URLs, and no manuscript serialization or JavaScript dependencies.\n");
