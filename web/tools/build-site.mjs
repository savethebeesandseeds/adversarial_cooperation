import { spawnSync } from "node:child_process";
import { randomBytes } from "node:crypto";
import {
  copyFile,
  lstat,
  mkdir,
  open,
  readdir,
  realpath,
  rename,
  rm,
  stat,
  writeFile,
} from "node:fs/promises";
import path from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";

import { demos } from "../src/demo-registry.mjs";
import { bookEditions } from "../src/book-editions.mjs";
import { buildRegisteredWasm, registeredWasmBuilds } from "./build-wasm.mjs";

const toolsDirectory = path.dirname(fileURLToPath(import.meta.url));
export const webRoot = path.resolve(toolsDirectory, "..");
export const repositoryRoot = path.resolve(webRoot, "..");
export const sourceRoot = path.join(webRoot, "src");
export const distributionRoot = path.join(webRoot, "dist");
export const manuscriptPdf = path.join(
  repositoryRoot,
  "document",
  ".temp",
  "pdf",
  "adversarial_cooperation.pdf",
);
export const shortManuscriptPdf = path.join(
  repositoryRoot,
  "document",
  ".temp",
  "pdf",
  "adversarial_cooperation_short.pdf",
);

const editionSources = {
  short: { source: "document/adversarial_cooperation_short.tex", pdf: shortManuscriptPdf },
  companion: { source: "document/adversarial_cooperation.tex", pdf: manuscriptPdf },
};
const reservedWebDirectories = new Set(["src", "tools", "wasm", "tests"]);

/**
 * Build the complete static edition in a private staging directory and then
 * transactionally replace the requested generated output directory.
 */
export async function buildSite({ outputRoot = distributionRoot } = {}) {
  const resolvedOutput = resolveSiteOutput(outputRoot);
  const suffix = `${process.pid}-${randomBytes(8).toString("hex")}`;
  const stageRoot = path.join(path.dirname(resolvedOutput), `.${path.basename(resolvedOutput)}.stage-${suffix}`);
  const backupRoot = path.join(path.dirname(resolvedOutput), `.${path.basename(resolvedOutput)}.backup-${suffix}`);

  assertRegistrationParity();
  await assertSafeOutputParent(resolvedOutput);
  await compileCanonicalPdf();
  await compilePdf(editionSources.short.source, shortManuscriptPdf);
  await mkdir(stageRoot, { recursive: false });

  try {
    const builds = await assembleStaticSite(stageRoot);
    await validateCompleteSite(stageRoot, builds);
    await replaceGeneratedDirectory(stageRoot, resolvedOutput, backupRoot);
    return Object.freeze({
      outputRoot: resolvedOutput,
      pdfPath: path.join(resolvedOutput, "book", "Adversarial-Cooperation.pdf"),
      shortPdfPath: path.join(resolvedOutput, "book", "Adversarial-Cooperation-Short.pdf"),
      wasmBuilds: builds,
    });
  } finally {
    await rm(stageRoot, { recursive: true, force: true });
  }
}

export function assertRegistrationParity() {
  const runtimeIds = demos.map((demo) => demo.id);
  const buildIds = registeredWasmBuilds.map((build) => build.id);
  if (runtimeIds.length !== buildIds.length || runtimeIds.some((id, index) => id !== buildIds[index])) {
    throw new Error(
      "Runtime demo registry and WebAssembly build registry differ: " +
        `runtime=[${runtimeIds.join(", ")}], build=[${buildIds.join(", ")}].`,
    );
  }
}

export async function compileCanonicalPdf() {
  return compilePdf(editionSources.companion.source, manuscriptPdf);
}

async function compilePdf(source, pdf) {
  const result = spawnSync(
    "bash",
    ["compile_latex.sh", "-s", source],
    {
      cwd: repositoryRoot,
      env: { ...process.env, LATEX_OUTDIR: ".temp/pdf" },
      stdio: "inherit",
    },
  );

  if (result.error) {
    throw new Error(
      "Unable to run the canonical LaTeX compiler. Use the dedicated project " +
        `environment, which provisions LaTeX. ${result.error.message}`,
    );
  }
  if (result.status !== 0) {
    throw new Error(`compile_latex.sh exited with status ${result.status} for ${source}.`);
  }

  await assertPdf(pdf);
  return pdf;
}

/**
 * Assemble one complete candidate site. The caller supplies a fresh staging
 * directory; this function never removes or publishes web/dist.
 */
export async function assembleStaticSite(stageRoot) {
  const resolvedStage = path.resolve(stageRoot);
  await copyDirectory(sourceRoot, resolvedStage);
  await mkdir(path.join(resolvedStage, "book"), { recursive: true });
  for (const edition of bookEditions) {
    await copyFile(editionSources[edition.id].pdf, path.join(resolvedStage, "book", edition.pdfName));
  }
  await writeFile(path.join(resolvedStage, ".nojekyll"), "", "utf8");
  return buildRegisteredWasm(path.join(resolvedStage, "assets"));
}

export async function validateCompleteSite(siteRoot, wasmBuilds) {
  const resolvedSite = path.resolve(siteRoot);
  const requiredFiles = [
    "index.html",
    "app.mjs",
    "styles.css",
    ...bookEditions.map((edition) => path.join("book", edition.pdfName)),
  ];

  for (const relativePath of requiredFiles) {
    const details = await stat(path.join(resolvedSite, relativePath));
    if (!details.isFile() || details.size === 0) {
      throw new Error(`Static build is missing a non-empty ${relativePath}.`);
    }
  }
  for (const edition of bookEditions) {
    await assertPdf(path.join(resolvedSite, "book", edition.pdfName));
  }

  if (!Array.isArray(wasmBuilds) || wasmBuilds.length === 0) {
    throw new Error("A complete site must contain at least one registered WebAssembly demo.");
  }
  for (const build of wasmBuilds) {
    if (!build || typeof build.id !== "string" || !Array.isArray(build.artifacts)) {
      throw new Error("A WebAssembly builder returned an invalid build record.");
    }
    for (const artifact of build.artifacts) {
      const artifactPath = path.resolve(artifact.path);
      if (!isInside(resolvedSite, artifactPath)) {
        throw new Error(`WebAssembly artifact escaped the staged site: ${artifactPath}`);
      }
      const details = await stat(artifactPath);
      if (!details.isFile() || details.size === 0) {
        throw new Error(`WebAssembly artifact is empty: ${artifactPath}`);
      }
    }
  }

  const files = await listFiles(resolvedSite);
  for (const filename of files) {
    const basename = path.basename(filename).toLowerCase();
    if (basename === "book.json" || basename.endsWith(".tex") || basename.endsWith(".bak")) {
      throw new Error(`Private manuscript/source-map material reached the static site: ${filename}`);
    }
  }
}

async function copyDirectory(sourceDirectory, destinationDirectory) {
  await mkdir(destinationDirectory, { recursive: true });
  const entries = await readdir(sourceDirectory, { withFileTypes: true });

  for (const entry of entries) {
    const sourcePath = path.join(sourceDirectory, entry.name);
    const destinationPath = path.join(destinationDirectory, entry.name);
    if (entry.isSymbolicLink()) {
      throw new Error(`Refusing to copy symbolic link from web/src: ${sourcePath}`);
    }
    if (entry.isDirectory()) {
      await copyDirectory(sourcePath, destinationPath);
    } else if (entry.isFile()) {
      await copyFile(sourcePath, destinationPath);
    }
  }
}

async function listFiles(root) {
  const files = [];
  const entries = await readdir(root, { withFileTypes: true });
  for (const entry of entries) {
    const absolutePath = path.join(root, entry.name);
    if (entry.isSymbolicLink()) {
      throw new Error(`Static build contains a symbolic link: ${absolutePath}`);
    }
    if (entry.isDirectory()) files.push(...await listFiles(absolutePath));
    else if (entry.isFile()) files.push(absolutePath);
  }
  return files;
}

async function assertPdf(filename) {
  const details = await stat(filename);
  if (!details.isFile() || details.size < 1024) {
    throw new Error(`Compiled manuscript PDF is missing or implausibly small: ${filename}`);
  }
  const handle = await open(filename, "r");
  try {
    const header = Buffer.alloc(5);
    const { bytesRead } = await handle.read(header, 0, header.length, 0);
    if (bytesRead !== header.length || header.toString("ascii") !== "%PDF-") {
      throw new Error(`Compiled manuscript does not begin with a PDF header: ${filename}`);
    }
  } finally {
    await handle.close();
  }
}

async function replaceGeneratedDirectory(stageRoot, outputRoot, backupRoot) {
  if (await lstatOrNull(backupRoot)) {
    throw new Error(`Refusing to overwrite a pre-existing build backup: ${backupRoot}`);
  }
  const existing = await lstatOrNull(outputRoot);
  if (existing && (!existing.isDirectory() || existing.isSymbolicLink())) {
    throw new Error(`Refusing to replace a non-directory or symbolic-link output: ${outputRoot}`);
  }

  let oldOutputMoved = false;
  try {
    if (existing) {
      await rename(outputRoot, backupRoot);
      oldOutputMoved = true;
    }
    await rename(stageRoot, outputRoot);
  } catch (error) {
    if (oldOutputMoved && !await lstatOrNull(outputRoot)) {
      await rename(backupRoot, outputRoot);
    }
    throw error;
  }

  if (oldOutputMoved) {
    await rm(backupRoot, { recursive: true, force: true });
  }
}

async function assertSafeOutputParent(outputRoot) {
  const parent = path.dirname(outputRoot);
  const parentStats = await lstat(parent);
  if (!parentStats.isDirectory() || parentStats.isSymbolicLink()) {
    throw new Error(`Static output parent must be a real directory: ${parent}`);
  }
  const [realWebRoot, realParent] = await Promise.all([
    realpath(webRoot),
    realpath(parent),
  ]);
  if (realParent !== realWebRoot && !isInside(realWebRoot, realParent)) {
    throw new Error(`Static output parent resolves outside web/: ${parent}`);
  }
}

async function lstatOrNull(filename) {
  try {
    return await lstat(filename);
  } catch (error) {
    if (error?.code === "ENOENT") return null;
    throw error;
  }
}

function resolveSiteOutput(outputRoot) {
  if (typeof outputRoot !== "string" || outputRoot.trim() === "") {
    throw new Error("Site output must be a non-empty path.");
  }
  const resolved = path.resolve(repositoryRoot, outputRoot);
  if (!isInside(webRoot, resolved)) {
    throw new Error(`Static output must stay inside the generated web tree: ${resolved}`);
  }
  const firstSegment = path.relative(webRoot, resolved).split(path.sep)[0];
  if (reservedWebDirectories.has(firstSegment)) {
    throw new Error(`Refusing to publish over canonical web input: ${resolved}`);
  }
  return resolved;
}

function isInside(parent, child) {
  const relative = path.relative(parent, child);
  return relative !== "" && relative !== ".." && !relative.startsWith(`..${path.sep}`) && !path.isAbsolute(relative);
}

function parseArguments(args) {
  let outputRoot = distributionRoot;
  for (let index = 0; index < args.length; index += 1) {
    const argument = args[index];
    if (argument === "--output" && args[index + 1]) {
      outputRoot = args[++index];
    } else if (argument === "--help") {
      process.stdout.write("Usage: node web/tools/build-site.mjs [--output web/dist]\n");
      process.exit(0);
    } else {
      throw new Error(`Unknown option: ${argument}`);
    }
  }
  return outputRoot;
}

function isDirectExecution() {
  if (!process.argv[1]) return false;
  return pathToFileURL(path.resolve(process.argv[1])).href === import.meta.url;
}

if (isDirectExecution()) {
  const outputRoot = parseArguments(process.argv.slice(2));
  const result = await buildSite({ outputRoot });
  process.stdout.write(
    `Built both book editions in ${path.relative(repositoryRoot, result.outputRoot)} ` +
      `with ${result.wasmBuilds.length} registered WebAssembly demo(s).\n`,
  );
}
