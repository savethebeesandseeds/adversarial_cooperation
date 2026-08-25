import { defineDemo } from "../demo-contract.mjs";

const SCENARIOS = Object.freeze({
  "reference-x": 1,
  "reference-o": 2,
  "naive-x": 3,
});

const VERDICTS = Object.freeze({
  0: Object.freeze({
    verdict: "not-computed",
    title: "Not computed",
    tone: "neutral",
    summary: "The checker did not return a mathematical verdict.",
  }),
  1: Object.freeze({
    verdict: "non-losing",
    title: "Non-losing",
    tone: "positive",
    summary: "Against every legal opponent reply in the fixed model, this disclosed policy does not lose.",
  }),
  2: Object.freeze({
    verdict: "invalid-policy",
    title: "Invalid policy",
    tone: "warning",
    summary: "The disclosed table fails the checker's canonical-policy requirements.",
  }),
  3: Object.freeze({
    verdict: "losing",
    title: "Losing",
    tone: "negative",
    summary: "The checker found a legal opponent line that defeats this disclosed policy.",
  }),
});

const POLICY_REASONS = Object.freeze({
  0: "canonical",
  1: "missing move",
  2: "move outside the board",
  3: "move targets an occupied cell",
  4: "unexpected policy entry",
});

function integer(value) {
  return Number.isFinite(value) ? new Intl.NumberFormat("en").format(value) : "Not reported";
}

function roleName(value) {
  if (value === 1) return "X";
  if (value === 2) return "O";
  return `Unknown (${value})`;
}

function resultFromBridge(result, requestedScenario) {
  if (!result || typeof result !== "object") {
    throw new Error("The WebAssembly checker returned no result.");
  }
  if (result.abiVersion !== 1) {
    throw new Error(`The WebAssembly checker returned unsupported ABI ${result.abiVersion}.`);
  }
  if (result.runStatus !== 0 || result.status !== 0) {
    throw new Error(`The C checker reported an execution error (${result.runStatus}/${result.status}).`);
  }
  if (!Object.values(SCENARIOS).includes(result.scenario) || result.scenario !== requestedScenario) {
    throw new Error("The WebAssembly result does not match the requested fixture.");
  }
  const expectedRole = requestedScenario === SCENARIOS["reference-o"] ? 2 : 1;
  if (result.role !== expectedRole || !Object.hasOwn(VERDICTS, result.verdict)) {
    throw new Error("The WebAssembly checker returned an invalid role or verdict.");
  }
  const countFields = [
    result.reachableBoardCount,
    result.requiredPolicyStateCount,
    result.exploredNodeCount,
    result.exploredTerminalNodeCount,
  ];
  if (countFields.some((value) => !Number.isSafeInteger(value) || value < 0)) {
    throw new Error("The WebAssembly checker returned an invalid count.");
  }
  if (!Number.isInteger(result.policyReason) || !Object.hasOwn(POLICY_REASONS, result.policyReason)) {
    throw new Error("The WebAssembly checker returned an invalid policy reason.");
  }
  if (!Array.isArray(result.counterexampleMoves) ||
      result.counterexampleMoves.length > 9 ||
      result.counterexampleMoves.some((move) => !Number.isInteger(move) || move < 0 || move > 8)) {
    throw new Error("The WebAssembly checker returned an invalid replay line.");
  }
  const verdict = VERDICTS[result.verdict];
  const moves = result.counterexampleMoves;
  const steps = moves.map((cell, index) => ({
    label: `Move ${index + 1}`,
    detail: `${index % 2 === 0 ? "X" : "O"} plays cell ${Number(cell) + 1}.`,
  }));
  return {
    ...verdict,
    metrics: [
      { label: "Claimant role", value: roleName(result.role) },
      { label: "Reachable boards", value: integer(result.reachableBoardCount) },
      { label: "Policy states required", value: integer(result.requiredPolicyStateCount) },
      { label: "Nodes explored", value: integer(result.exploredNodeCount) },
      { label: "Terminal nodes", value: integer(result.exploredTerminalNodeCount) },
      {
        label: "Policy validation",
        value: POLICY_REASONS[result.policyReason] ?? `reason ${result.policyReason}`,
      },
    ],
    steps,
    scope: moves.length > 0
      ? "This run establishes only that the disclosed fixture has the reported verdict under Game and Policy Version 1. The listed line is replayable by the recursive C checker."
      : "This run establishes only the reported verdict for one fully disclosed deterministic fixture under Game and Policy Version 1. It makes no privacy, cryptographic, authorship, or future-behaviour claim.",
  };
}

function defaultWorkerFactory(url) {
  if (typeof Worker !== "function") throw new Error("This browser cannot start a Web Worker.");
  return new Worker(url, { type: "module" });
}

export function createTttRunner(options = {}) {
  const workerFactory = options.workerFactory ?? defaultWorkerFactory;
  let worker = null;
  let disposed = false;
  let nextRequestId = 0;
  let settleReady;
  let rejectReady;
  const pending = new Map();
  const ready = new Promise((resolve, reject) => {
    settleReady = resolve;
    rejectReady = reject;
  });

  function failPending(error) {
    for (const request of pending.values()) {
      clearTimeout(request.timeout);
      request.reject(error);
    }
    pending.clear();
  }

  function handleMessage(event) {
    const message = event.data ?? {};
    if (message.type === "ready") {
      settleReady();
      return;
    }
    if (message.type === "unavailable") {
      const error = new Error(message.message || "The compiled C checker is unavailable in this build.");
      rejectReady(error);
      failPending(error);
      return;
    }
    if (message.type !== "result" && message.type !== "error") return;
    const request = pending.get(message.requestId);
    if (!request) return;
    pending.delete(message.requestId);
    clearTimeout(request.timeout);
    if (message.type === "error") request.reject(new Error(message.message || "The C checker failed."));
    else {
      try {
        request.resolve(resultFromBridge(message.result, request.scenario));
      } catch (error) {
        request.reject(error);
      }
    }
  }

  function handleWorkerError(event) {
    const error = new Error(event?.message || "The WebAssembly worker stopped unexpectedly.");
    rejectReady(error);
    failPending(error);
  }

  try {
    const workerUrl = new URL("../ttt-worker.mjs", import.meta.url);
    worker = workerFactory(workerUrl);
    if (!worker || typeof worker.addEventListener !== "function" || typeof worker.postMessage !== "function") {
      throw new Error("The Tic-Tac-Toe worker factory returned an invalid worker.");
    }
    worker.addEventListener("message", handleMessage);
    worker.addEventListener("error", handleWorkerError);
  } catch (error) {
    rejectReady(error instanceof Error ? error : new Error(String(error)));
  }

  return Object.freeze({
    ready,
    async run(exampleId) {
      if (disposed) throw new Error("The Tic-Tac-Toe demo has been closed.");
      const scenario = SCENARIOS[exampleId];
      if (!scenario) throw new Error(`Unknown Tic-Tac-Toe example: ${exampleId}`);
      await ready;
      const requestId = ++nextRequestId;
      return new Promise((resolve, reject) => {
        const timeout = setTimeout(() => {
          pending.delete(requestId);
          reject(new Error("The checker did not answer within 30 seconds."));
        }, 30000);
        pending.set(requestId, { resolve, reject, timeout, scenario });
        worker.postMessage({ type: "run", requestId, scenario });
      });
    },
    dispose() {
      if (disposed) return;
      disposed = true;
      const error = new Error("The Tic-Tac-Toe demo was closed.");
      rejectReady(error);
      failPending(error);
      worker?.removeEventListener?.("message", handleMessage);
      worker?.removeEventListener?.("error", handleWorkerError);
      worker?.terminate?.();
      worker = null;
    },
  });
}

export const ticTacToeDemo = defineDemo({
  id: "ttt-disclosed-policy-v1",
  chapter: {
    title: "Tic-Tac-Toe without Revealing the Strategy",
    sourcePath: "document/content/tic_tac_toe_without_revealing_the_strategy/tic_tac_toe_without_revealing_the_strategy.tex",
  },
  title: "Disclosed-policy Tic-Tac-Toe checker",
  summary: "Run the existing C11 checker in WebAssembly against complete public policy tables and inspect its exhaustive verdict.",
  warning: "Educational public baseline: every fixture discloses the complete policy to the checker. This is not strategy privacy, zero knowledge, a proof of knowledge, authorship evidence, or a promise of future play.",
  examples: [
    {
      id: "reference-x",
      title: "Reference policy for X",
      description: "Build the deterministic public X fixture and explore every legal O reply from the empty board.",
      expected: "Expected: non-losing under the fixed public model.",
    },
    {
      id: "reference-o",
      title: "Reference policy for O",
      description: "Build the deterministic public O fixture and explore every legal X continuation from the empty board.",
      expected: "Expected: non-losing under the fixed public model.",
    },
    {
      id: "naive-x",
      title: "Naive first-empty policy for X",
      description: "Choose the lowest-numbered empty cell at each X state so the checker can expose a concrete losing branch.",
      expected: "Expected: losing, with a replayable counterexample line.",
    },
  ],
  createRunner: createTttRunner,
});

export default ticTacToeDemo;
