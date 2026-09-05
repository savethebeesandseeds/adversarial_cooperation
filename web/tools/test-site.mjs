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

async function assertEditionCoverage() {
  const documentRoot = path.resolve(webDirectory, "..", "document");
  const companionMain = await readFile(path.join(documentRoot, "adversarial_cooperation.tex"), "utf8");
  const shortMain = await readFile(path.join(documentRoot, "adversarial_cooperation_short.tex"), "utf8");
  const companionPaths = [...companionMain.matchAll(/^\s*\\include\{([^}]+)\}/gmu)]
    .map((match) => match[1]).filter((file) => file !== "content/research_draft_status");
  const shortPaths = [...shortMain.matchAll(/^\s*\\input\{(short\/\d{2}-[^}]+)\}/gmu)]
    .map((match) => match[1]);
  assert.equal(companionPaths.length, 27, "companion must retain 22 chapters and five appendices");
  assert.equal(shortPaths.length, 27, "short book must cover every companion chapter and appendix");
  assert.equal(new Set(companionPaths).size, 27, "duplicate companion chapter");
  assert.equal(new Set(shortPaths).size, 27, "duplicate short chapter");
  assert.ok(companionMain.includes("\\appendix"), "companion appendix boundary missing");
  assert.ok(shortMain.includes("\\appendix"), "short appendix boundary missing");
  for (let index = 0; index < 27; index += 1) {
    const companion = await readFile(path.join(documentRoot, companionPaths[index] + ".tex"), "utf8");
    const short = await readFile(path.join(documentRoot, shortPaths[index] + ".tex"), "utf8");
    const title = companion.match(/^\s*\\chapter\{([^}]+)\}/mu)?.[1];
    assert.ok(title, "missing companion title: " + companionPaths[index]);
    assert.equal(short.match(/^\s*\\chapter\{([^}]+)\}/mu)?.[1], title,
      "short chapter title/order differs from companion: " + shortPaths[index]);
    assert.ok(shortPaths[index].startsWith("short/" + String(index + 1).padStart(2, "0") + "-"),
      "short source numbering differs from its reading order");
    const isAppendix = index >= 22;
    const number = isAppendix ? String.fromCharCode(65 + index - 22) : String(index + 1);
    const destination = (isAppendix ? "appendix." : "chapter.") + number;
    const references = [...short.matchAll(/\\companionref\{([^}]+)\}\{([^}]+)\}/gu)];
    assert.equal(references.length, 1, "each short chapter needs one companion reference");
    assert.deepEqual(references[0].slice(1), [number, destination],
      "incorrect companion number/destination: " + shortPaths[index]);
    assert.equal(
      companionMain.indexOf("\\include{" + companionPaths[index] + "}") > companionMain.indexOf("\\appendix"),
      isAppendix, "companion appendix boundary moved",
    );
    assert.equal(
      shortMain.indexOf("\\input{" + shortPaths[index] + "}") > shortMain.indexOf("\\appendix"),
      isAppendix, "short appendix boundary moved",
    );
  }
  const layout = await readFile(path.join(documentRoot, "short", "layout.tex"), "utf8");
  assert.ok(layout.includes("\\href{Adversarial-Cooperation.pdf\\##2}"),
    "short chapter links must address named destinations in the preserved companion PDF");
}

async function assertPdfs() {
  const editions = [
    ["Adversarial-Cooperation-Short.pdf", "adversarial_cooperation_short.pdf"],
    ["Adversarial-Cooperation.pdf", "adversarial_cooperation.pdf"],
  ];
  const published = [];
  for (const [publicName, compiledName] of editions) {
    const pdf = await readFile(path.join(distributionRoot, "book", publicName));
    const compiled = await readFile(path.join(webDirectory, "..", "document", ".temp", "pdf", compiledName));
    assert.ok(pdf.length >= 1024, publicName + ": compiled PDF is implausibly small");
    assert.equal(pdf.subarray(0, 5).toString("ascii"), "%PDF-", publicName + ": missing PDF header");
    assert.match(pdf.subarray(Math.max(0, pdf.length - 1024)).toString("latin1"), /%%EOF\s*$/u);
    assert.deepEqual(pdf, compiled, publicName + ": differs from its canonical compiled edition");
    published.push(pdf);
  }
  assert.notDeepEqual(published[0], published[1], "the short book must not duplicate the companion PDF");
  assert.deepEqual(
    (await listFiles(distributionRoot)).filter((file) => file.startsWith("book/")),
    editions.map(([name]) => "book/" + name).sort(),
    "the reading surface must contain exactly the two canonical PDFs",
  );
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
    "book/Adversarial-Cooperation-Short.pdf",
    "book-editions.mjs",
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

await assertEditionCoverage();
await assertCopiedSourceAssets();
await assertPdfs();
await assertStaticBoundaries();
await assertNoJavaScriptDependencies();

process.stdout.write("Static site tests passed: 27 matching chapters and companion links, both canonical PDFs, complete reader assets, one demo, relative URLs, and no manuscript serialization or JavaScript dependencies.\n");
