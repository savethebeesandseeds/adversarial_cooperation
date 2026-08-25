import assert from "node:assert/strict";

import {
  createDemoRegistry,
  createDemoRunner,
  defineDemo,
  normalizeDemoResult,
} from "../src/demo-contract.mjs";
import { demoRegistry, demos, getDemo } from "../src/demo-registry.mjs";
import { registeredWasmBuilds } from "./build-wasm.mjs";

function validDefinition(overrides = {}) {
  return {
    id: "fixture-demo",
    chapter: {
      title: "Fixture chapter",
      sourcePath: "document/content/fixture/fixture.tex",
    },
    title: "Fixture demonstration",
    summary: "Exercises the reusable contract without a WebAssembly runtime.",
    warning: "This fixture establishes only contract behavior.",
    examples: [
      {
        id: "positive",
        title: "Positive fixture",
        description: "Return one normalized positive result.",
        expected: "A positive fixture verdict.",
      },
    ],
    createRunner() {
      return {
        ready: Promise.resolve(),
        async run(exampleId) {
          assert.equal(exampleId, "positive");
          return {
            verdict: "fixture-pass",
            title: "Fixture passed",
            tone: "positive",
            summary: "The fixture runner returned its expected value.",
            metrics: [{ label: "Runs", value: "1" }],
            steps: [{ label: "Input", detail: "positive" }],
            scope: "Contract fixture only.",
          };
        },
        dispose() {},
      };
    },
    ...overrides,
  };
}

let disposeCount = 0;
const fixture = defineDemo(validDefinition({
  createRunner() {
    return {
      ready: Promise.resolve(),
      async run(exampleId) {
        assert.equal(exampleId, "positive");
        return {
          verdict: "fixture-pass",
          title: "Fixture passed",
          tone: "positive",
          summary: "The fixture runner returned its expected value.",
          metrics: [{ label: "Runs", value: 1 }],
          steps: [{ label: "Input", detail: "positive" }],
          scope: "Contract fixture only.",
        };
      },
      dispose() {
        disposeCount += 1;
      },
    };
  },
}));
assert.equal(fixture.id, "fixture-demo");
assert.ok(Object.isFrozen(fixture));
assert.ok(Object.isFrozen(fixture.chapter));
assert.ok(Object.isFrozen(fixture.examples));
assert.ok(Object.isFrozen(fixture.examples[0]));

const runner = createDemoRunner(fixture);
await runner.ready;
const normalized = await runner.run("positive");
assert.deepEqual(normalized, {
  verdict: "fixture-pass",
  title: "Fixture passed",
  tone: "positive",
  summary: "The fixture runner returned its expected value.",
  metrics: [{ label: "Runs", value: "1" }],
  steps: [{ label: "Input", detail: "positive" }],
  scope: "Contract fixture only.",
});
await assert.rejects(runner.run("missing"), /Unknown example/u);
await runner.dispose();
await runner.dispose();
assert.equal(disposeCount, 1, "runner disposal is not idempotent");
await assert.rejects(runner.run("positive"), /disposed/u);

const malformedDefinitions = [
  undefined,
  null,
  {},
  validDefinition({ id: "" }),
  validDefinition({ chapter: { title: "Fixture chapter", sourcePath: "" } }),
  validDefinition({ title: "" }),
  validDefinition({ summary: "" }),
  validDefinition({ warning: "" }),
  validDefinition({ examples: [] }),
  validDefinition({ examples: [{ id: "positive", title: "", description: "Description", expected: "Expected" }] }),
  validDefinition({ createRunner: null }),
];

for (const definition of malformedDefinitions) {
  assert.throws(
    () => defineDemo(definition),
    Error,
    `malformed demo definition was accepted: ${JSON.stringify(definition)}`,
  );
}

assert.throws(
  () => defineDemo(validDefinition({
    examples: [
      validDefinition().examples[0],
      validDefinition().examples[0],
    ],
  })),
  Error,
  "duplicate example IDs were accepted",
);

const second = defineDemo(validDefinition({
  id: "second-demo",
  chapter: {
    title: "Second chapter",
    sourcePath: "document/content/second/second.tex",
  },
  title: "Second demonstration",
}));
const registry = createDemoRegistry([fixture, second]);
assert.deepEqual(registry.list().map((demo) => demo.id), ["fixture-demo", "second-demo"]);
assert.equal(registry.get("fixture-demo"), fixture);
assert.equal(registry.get("missing"), null);
assert.equal(registry.has("second-demo"), true);
assert.ok(Object.isFrozen(registry));

assert.throws(
  () => createDemoRegistry([fixture, fixture]),
  Error,
  "duplicate demo IDs were accepted",
);
assert.throws(
  () => createDemoRegistry([
    fixture,
    defineDemo(validDefinition({
      id: "same-chapter-demo",
      title: "Same chapter demonstration",
    })),
  ]),
  Error,
  "duplicate chapter source paths were accepted",
);

for (const malformedResult of [
  null,
  {},
  { ...normalized, tone: "excellent" },
  { ...normalized, metrics: [{ label: "", value: "1" }] },
  { ...normalized, steps: [{ label: "Input", detail: "" }] },
]) {
  assert.throws(
    () => normalizeDemoResult(malformedResult),
    Error,
    `malformed demo result was accepted: ${JSON.stringify(malformedResult)}`,
  );
}

assert.ok(Object.isFrozen(demos));
assert.deepEqual(demos, demoRegistry.list());
assert.equal(demos.length, 1, "the public gallery must contain only the working Tic-Tac-Toe demo");
assert.equal(demos[0].id, "ttt-disclosed-policy-v1");
assert.equal(getDemo("ttt-disclosed-policy-v1"), demos[0]);
assert.equal(getDemo("missing"), null);
assert.deepEqual(
  demos[0].examples.map((example) => example.id),
  ["reference-x", "reference-o", "naive-x"],
);
assert.deepEqual(
  demos.map((demo) => demo.id),
  registeredWasmBuilds.map((entry) => entry.id),
  "runtime demo registry and WebAssembly build registry drifted",
);
for (const example of demos[0].examples) {
  assert.ok(example.title.length > 0, `${example.id}: missing title`);
  assert.ok(example.description.length > 0, `${example.id}: missing description`);
  assert.ok(example.expected.length > 0, `${example.id}: missing expected outcome`);
}

process.stdout.write("Demo contract tests passed: validation, immutability, duplicate rejection, one registered demo, and three Tic-Tac-Toe examples.\n");
