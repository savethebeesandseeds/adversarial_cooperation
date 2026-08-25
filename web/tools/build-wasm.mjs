import { spawnSync } from "node:child_process";
import { mkdir, stat } from "node:fs/promises";
import path from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";

const toolsDirectory = path.dirname(fileURLToPath(import.meta.url));
const webDirectory = path.resolve(toolsDirectory, "..");
const repositoryRoot = path.resolve(webDirectory, "..");
const emcc = process.env.EMCC || "emcc";

const exportedFunctions = [
  "_ac_ttt_web_run",
  "_ac_ttt_web_get_abi_version",
  "_ac_ttt_web_get_scenario",
  "_ac_ttt_web_get_status",
  "_ac_ttt_web_get_role",
  "_ac_ttt_web_get_verdict",
  "_ac_ttt_web_get_policy_reason",
  "_ac_ttt_web_get_invalid_board_index",
  "_ac_ttt_web_get_invalid_move",
  "_ac_ttt_web_get_reachable_board_count",
  "_ac_ttt_web_get_x_to_move_board_count",
  "_ac_ttt_web_get_o_to_move_board_count",
  "_ac_ttt_web_get_reachable_terminal_board_count",
  "_ac_ttt_web_get_required_policy_state_count",
  "_ac_ttt_web_get_explored_node_count",
  "_ac_ttt_web_get_explored_terminal_node_count",
  "_ac_ttt_web_get_counterexample_length",
  "_ac_ttt_web_get_counterexample_move",
];

/**
 * Compile the disclosed-policy Tic-Tac-Toe checker into an explicit asset
 * staging directory. This component builder never removes or replaces a site.
 */
export async function buildTicTacToeWasm(outputDirectory) {
  const resolvedOutput = resolveOutputDirectory(outputDirectory);
  const modulePath = path.join(resolvedOutput, "ttt-module.mjs");
  const wasmPath = path.join(resolvedOutput, "ttt-module.wasm");

  await mkdir(resolvedOutput, { recursive: true });

  const arguments_ = [
    "web/wasm/ttt_web.c",
    "src/protocols/ttt.c",
    "-Iinclude",
    "-std=c11",
    "-O2",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Werror",
    "--no-entry",
    "-sSTRICT=1",
    "-sASSERTIONS=0",
    "-sFILESYSTEM=0",
    "-sENVIRONMENT=web,worker,node",
    "-sMODULARIZE=1",
    "-sEXPORT_ES6=1",
    "-sEXPORT_NAME=createTttModule",
    `-sINCOMING_MODULE_JS_API=${JSON.stringify(["locateFile"])}`,
    `-sEXPORTED_FUNCTIONS=${JSON.stringify(exportedFunctions)}`,
    "-sEXPORTED_RUNTIME_METHODS=[]",
    "-sALLOW_MEMORY_GROWTH=0",
    "-sSTACK_SIZE=524288",
    "-sDYNAMIC_EXECUTION=0",
    "-sERROR_ON_UNDEFINED_SYMBOLS=1",
    "-o",
    modulePath,
  ];

  const result = spawnSync(emcc, arguments_, {
    cwd: repositoryRoot,
    env: process.env,
    stdio: "inherit",
  });

  if (result.error) {
    throw new Error(
      `Unable to execute ${emcc}. Run the dedicated project environment with ` +
        "`docker compose run --rm toolchain web`; its auditable setup.sh " +
        `provisions Emscripten. ${result.error.message}`,
    );
  }
  if (result.status !== 0) {
    throw new Error(`${emcc} exited with status ${result.status}.`);
  }

  const [moduleStat, wasmStat] = await Promise.all([
    stat(modulePath),
    stat(wasmPath),
  ]);
  if (!moduleStat.isFile() || !wasmStat.isFile() || moduleStat.size === 0 || wasmStat.size === 0) {
    throw new Error("Emscripten produced an empty Tic-Tac-Toe artifact.");
  }

  return Object.freeze([
    Object.freeze({ path: modulePath, size: moduleStat.size }),
    Object.freeze({ path: wasmPath, size: wasmStat.size }),
  ]);
}

/**
 * Build-time registry. Adding a future WebAssembly companion means adding one
 * small builder here and one runtime adapter to web/src/demo-registry.mjs.
 */
export const registeredWasmBuilds = Object.freeze([
  Object.freeze({
    id: "ttt-disclosed-policy-v1",
    build: buildTicTacToeWasm,
  }),
]);

export async function buildRegisteredWasm(outputDirectory) {
  const resolvedOutput = resolveOutputDirectory(outputDirectory);
  const results = [];
  const seenIds = new Set();
  const seenArtifacts = new Set();

  for (const registration of registeredWasmBuilds) {
    if (seenIds.has(registration.id)) {
      throw new Error(`Duplicate WebAssembly build registration: ${registration.id}`);
    }
    seenIds.add(registration.id);
    const artifacts = await registration.build(resolvedOutput);
    if (!Array.isArray(artifacts) || artifacts.length === 0) {
      throw new Error(`WebAssembly build ${registration.id} produced no artifacts.`);
    }
    for (const artifact of artifacts) {
      const artifactPath = path.resolve(artifact?.path || "");
      if (!isInside(resolvedOutput, artifactPath)) {
        throw new Error(`WebAssembly build ${registration.id} escaped its output directory.`);
      }
      if (seenArtifacts.has(artifactPath)) {
        throw new Error(`Duplicate WebAssembly artifact path: ${artifactPath}`);
      }
      seenArtifacts.add(artifactPath);
    }
    results.push(Object.freeze({ id: registration.id, artifacts }));
  }

  return Object.freeze(results);
}

function isInside(parent, child) {
  const relative = path.relative(parent, child);
  return relative !== "" && relative !== ".." && !relative.startsWith(`..${path.sep}`) && !path.isAbsolute(relative);
}

function resolveOutputDirectory(outputDirectory) {
  if (typeof outputDirectory !== "string" || outputDirectory.trim() === "") {
    throw new Error("An explicit WebAssembly output directory is required.");
  }
  const resolved = path.resolve(repositoryRoot, outputDirectory);
  const filesystemRoot = path.parse(resolved).root;
  if (resolved === filesystemRoot || resolved === repositoryRoot || resolved === webDirectory) {
    throw new Error(`Unsafe WebAssembly output directory: ${resolved}`);
  }
  return resolved;
}

function parseArguments(args) {
  let outputDirectory = null;
  for (let index = 0; index < args.length; index += 1) {
    const argument = args[index];
    if (argument === "--output" && args[index + 1]) {
      outputDirectory = args[++index];
    } else if (argument === "--help") {
      process.stdout.write("Usage: node web/tools/build-wasm.mjs --output <asset-directory>\n");
      process.exit(0);
    } else {
      throw new Error(`Unknown option: ${argument}`);
    }
  }
  if (!outputDirectory) {
    throw new Error("Missing required --output <asset-directory> argument.");
  }
  return outputDirectory;
}

function isDirectExecution() {
  if (!process.argv[1]) return false;
  return pathToFileURL(path.resolve(process.argv[1])).href === import.meta.url;
}

if (isDirectExecution()) {
  const outputDirectory = parseArguments(process.argv.slice(2));
  const builds = await buildRegisteredWasm(outputDirectory);
  for (const build of builds) {
    for (const artifact of build.artifacts) {
      process.stdout.write(
        `Built ${path.relative(repositoryRoot, artifact.path)} (${artifact.size} bytes)\n`,
      );
    }
  }
}
