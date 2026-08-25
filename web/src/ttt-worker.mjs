const ABI_VERSION = 1;
const VALID_SCENARIOS = new Set([1, 2, 3]);
const MAX_COUNTEREXAMPLE_MOVES = 9;

let moduleInstance = null;
let initializationError = null;

const exportNames = [
  "ac_ttt_web_run",
  "ac_ttt_web_get_abi_version",
  "ac_ttt_web_get_scenario",
  "ac_ttt_web_get_status",
  "ac_ttt_web_get_role",
  "ac_ttt_web_get_verdict",
  "ac_ttt_web_get_policy_reason",
  "ac_ttt_web_get_invalid_board_index",
  "ac_ttt_web_get_invalid_move",
  "ac_ttt_web_get_reachable_board_count",
  "ac_ttt_web_get_x_to_move_board_count",
  "ac_ttt_web_get_o_to_move_board_count",
  "ac_ttt_web_get_reachable_terminal_board_count",
  "ac_ttt_web_get_required_policy_state_count",
  "ac_ttt_web_get_explored_node_count",
  "ac_ttt_web_get_explored_terminal_node_count",
  "ac_ttt_web_get_counterexample_length",
  "ac_ttt_web_get_counterexample_move",
];

function exported(name) {
  const candidate = moduleInstance?.[`_${name}`];
  if (typeof candidate !== "function") {
    throw new Error(`The WebAssembly bridge is missing ${name}.`);
  }
  return candidate;
}

function scalar(name) {
  return Number(exported(name)());
}

function collectResult(runStatus) {
  const length = scalar("ac_ttt_web_get_counterexample_length");
  if (!Number.isInteger(length) || length < 0 || length > MAX_COUNTEREXAMPLE_MOVES) {
    throw new Error("The WebAssembly bridge returned an invalid counterexample length.");
  }
  const getMove = exported("ac_ttt_web_get_counterexample_move");
  const counterexampleMoves = [];
  for (let index = 0; index < length; index += 1) {
    const move = Number(getMove(index));
    if (!Number.isInteger(move) || move < 0 || move > 8) {
      throw new Error("The WebAssembly bridge returned an invalid counterexample move.");
    }
    counterexampleMoves.push(move);
  }
  return {
    abiVersion: scalar("ac_ttt_web_get_abi_version"),
    scenario: scalar("ac_ttt_web_get_scenario"),
    runStatus: Number(runStatus),
    status: scalar("ac_ttt_web_get_status"),
    role: scalar("ac_ttt_web_get_role"),
    verdict: scalar("ac_ttt_web_get_verdict"),
    policyReason: scalar("ac_ttt_web_get_policy_reason"),
    invalidBoardIndex: scalar("ac_ttt_web_get_invalid_board_index"),
    invalidMove: scalar("ac_ttt_web_get_invalid_move"),
    reachableBoardCount: scalar("ac_ttt_web_get_reachable_board_count"),
    xToMoveBoardCount: scalar("ac_ttt_web_get_x_to_move_board_count"),
    oToMoveBoardCount: scalar("ac_ttt_web_get_o_to_move_board_count"),
    reachableTerminalBoardCount: scalar("ac_ttt_web_get_reachable_terminal_board_count"),
    requiredPolicyStateCount: scalar("ac_ttt_web_get_required_policy_state_count"),
    exploredNodeCount: scalar("ac_ttt_web_get_explored_node_count"),
    exploredTerminalNodeCount: scalar("ac_ttt_web_get_explored_terminal_node_count"),
    counterexampleMoves,
  };
}

async function initialize() {
  try {
    const bridge = await import("./assets/ttt-module.mjs");
    if (typeof bridge.default !== "function") {
      throw new Error("The generated WebAssembly module has no factory export.");
    }
    moduleInstance = await bridge.default({
      locateFile(fileName) {
        return new URL(fileName, new URL("./assets/", import.meta.url)).href;
      },
      print() {},
      printErr(message) {
        console.warn(`[tic-tac-toe wasm] ${message}`);
      },
    });
    exportNames.forEach(exported);
    const version = scalar("ac_ttt_web_get_abi_version");
    if (version !== ABI_VERSION) {
      throw new Error(`Unsupported Tic-Tac-Toe bridge ABI ${version}; expected ${ABI_VERSION}.`);
    }
    self.postMessage({ type: "ready", abiVersion: version });
  } catch (error) {
    initializationError = error instanceof Error ? error.message : String(error);
    console.warn(`[tic-tac-toe wasm] ${initializationError}`);
    moduleInstance = null;
    self.postMessage({
      type: "unavailable",
      message: "The compiled C checker is unavailable in this build. The book remains available in read-only form.",
    });
  }
}

self.addEventListener("message", (event) => {
  const message = event.data || {};
  if (message.type !== "run") return;
  const requestId = Number(message.requestId);
  const scenario = Number(message.scenario);
  if (!Number.isSafeInteger(requestId) || requestId < 0 || !VALID_SCENARIOS.has(scenario)) {
    self.postMessage({ type: "error", requestId, message: "The requested demo scenario is invalid." });
    return;
  }
  if (!moduleInstance) {
    self.postMessage({
      type: "error",
      requestId,
      message: initializationError || "The compiled C checker is not ready.",
    });
    return;
  }
  try {
    const runStatus = exported("ac_ttt_web_run")(scenario);
    const result = collectResult(runStatus);
    if (result.abiVersion !== ABI_VERSION || result.scenario !== scenario) {
      throw new Error("The WebAssembly report does not match the requested scenario or ABI.");
    }
    self.postMessage({ type: "result", requestId, result });
  } catch (error) {
    self.postMessage({
      type: "error",
      requestId,
      message: error instanceof Error ? error.message : String(error),
    });
  }
});

initialize();
