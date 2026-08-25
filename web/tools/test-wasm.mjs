import assert from "node:assert/strict";
import { mkdtemp, readFile, rm } from "node:fs/promises";
import { spawnSync } from "node:child_process";
import { tmpdir } from "node:os";
import { fileURLToPath, pathToFileURL } from "node:url";
import path from "node:path";

const toolsDirectory = path.dirname(fileURLToPath(import.meta.url));
const webDirectory = path.resolve(toolsDirectory, "..");
const repositoryRoot = path.resolve(webDirectory, "..");
const modulePath = path.join(webDirectory, "dist", "assets", "ttt-module.mjs");
const moduleUrl = pathToFileURL(modulePath);
const nativeCompiler = process.env.CC || "cc";

const VERDICT = {
  notComputed: 0,
  nonLosing: 1,
  losing: 3,
};

const SCENARIO = {
  referenceX: 1,
  referenceO: 2,
  naiveX: 3,
};

const fixtures = new Map([
  [
    SCENARIO.referenceX,
    {
      role: 1,
      verdict: VERDICT.nonLosing,
      requiredPolicyStates: 2423,
      exploredNodes: 266,
      exploredTerminalNodes: 101,
      counterexample: [],
    },
  ],
  [
    SCENARIO.referenceO,
    {
      role: 2,
      verdict: VERDICT.nonLosing,
      requiredPolicyStates: 2097,
      exploredNodes: 2056,
      exploredTerminalNodes: 681,
      counterexample: [],
    },
  ],
  [
    SCENARIO.naiveX,
    {
      role: 1,
      verdict: VERDICT.losing,
      requiredPolicyStates: 2423,
      exploredNodes: 33,
      exploredTerminalNodes: 9,
      counterexample: [0, 1, 2, 4, 3, 6, 5, 7],
    },
  ],
]);

function run(command, arguments_, options = {}) {
  const result = spawnSync(command, arguments_, {
    cwd: repositoryRoot,
    encoding: "utf8",
    ...options,
  });
  if (result.error) {
    throw new Error(`Unable to execute ${command}: ${result.error.message}`);
  }
  if (result.status !== 0) {
    throw new Error(
      `${command} exited with status ${result.status}.\n` +
        `${result.stdout || ""}${result.stderr || ""}`,
    );
  }
  return result;
}

async function runNativeBoundaryTest() {
  const temporaryDirectory = await mkdtemp(
    path.join(tmpdir(), "ac-ttt-web-native-"),
  );
  const executable = path.join(
    temporaryDirectory,
    process.platform === "win32" ? "test_ttt_web.exe" : "test_ttt_web",
  );

  try {
    run(nativeCompiler, [
      "-std=c11",
      "-Wall",
      "-Wextra",
      "-Wpedantic",
      "-Werror",
      "-Iinclude",
      "web/tests/test_ttt_web.c",
      "web/wasm/ttt_web.c",
      "src/protocols/ttt.c",
      "-o",
      executable,
    ]);
    const result = run(executable, []);
    assert.match(result.stdout, /6 tests, 0 failures/);
    process.stdout.write(result.stdout);
  } finally {
    await rm(temporaryDirectory, { recursive: true, force: true });
  }
}

function readResult(module, scenario) {
  assert.equal(module._ac_ttt_web_run(scenario), 0);
  const counterexampleLength = module._ac_ttt_web_get_counterexample_length();
  const counterexample = [];
  for (let index = 0; index < counterexampleLength; index += 1) {
    counterexample.push(module._ac_ttt_web_get_counterexample_move(index));
  }

  return {
    role: module._ac_ttt_web_get_role(),
    verdict: module._ac_ttt_web_get_verdict(),
    requiredPolicyStates: module._ac_ttt_web_get_required_policy_state_count(),
    exploredNodes: module._ac_ttt_web_get_explored_node_count(),
    exploredTerminalNodes:
      module._ac_ttt_web_get_explored_terminal_node_count(),
    counterexample,
  };
}

await runNativeBoundaryTest();

const wasmBytes = await readFile(path.join(webDirectory, "dist", "assets", "ttt-module.wasm"));
assert.equal(WebAssembly.validate(wasmBytes), true, "generated Tic-Tac-Toe asset is not valid WebAssembly");

const { default: createTttModule } = await import(moduleUrl.href);
const module = await createTttModule({
  locateFile(fileName) {
    return fileURLToPath(new URL(fileName, moduleUrl));
  },
});

assert.equal(module._ac_ttt_web_get_abi_version(), 1);
for (const [scenario, expected] of fixtures) {
  assert.deepEqual(readResult(module, scenario), expected);
  assert.equal(module._ac_ttt_web_get_scenario(), scenario);
  assert.equal(module._ac_ttt_web_get_status(), 0);
  assert.equal(module._ac_ttt_web_get_policy_reason(), 0);
  assert.equal(module._ac_ttt_web_get_reachable_board_count(), 5478);
  assert.equal(module._ac_ttt_web_get_x_to_move_board_count(), 2423);
  assert.equal(module._ac_ttt_web_get_o_to_move_board_count(), 2097);
  assert.equal(module._ac_ttt_web_get_reachable_terminal_board_count(), 958);
  assert.equal(module._ac_ttt_web_get_counterexample_move(0xffffffff), 0xff);
}

const invalidScenario = 0x7fffffff;
assert.equal(module._ac_ttt_web_run(invalidScenario), 1);
assert.equal(module._ac_ttt_web_get_scenario(), invalidScenario);
assert.equal(module._ac_ttt_web_get_status(), 1);
assert.equal(module._ac_ttt_web_get_role(), 0);
assert.equal(module._ac_ttt_web_get_verdict(), VERDICT.notComputed);
assert.equal(module._ac_ttt_web_get_reachable_board_count(), 0);
assert.equal(module._ac_ttt_web_get_counterexample_length(), 0);
assert.equal(module._ac_ttt_web_get_counterexample_move(0), 0xff);

console.log("WebAssembly and native Tic-Tac-Toe bridge fixtures agree for all three registered examples.");
