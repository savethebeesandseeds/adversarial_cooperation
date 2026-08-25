const DEMO_ID_PATTERN = /^[a-z0-9]+(?:-[a-z0-9]+)*$/u;
const SOURCE_PATH_PATTERN = /^document\/content\/.+\.tex$/u;
const RESULT_TONES = new Set(["neutral", "positive", "warning", "negative"]);
const definedDemos = new WeakSet();

function requiredText(value, label) {
  if (typeof value !== "string" || value.trim().length === 0) {
    throw new TypeError(`${label} must be a non-empty string.`);
  }
  return value.trim();
}

function stableId(value, label) {
  const id = requiredText(value, label);
  if (!DEMO_ID_PATTERN.test(id)) {
    throw new TypeError(`${label} must contain lowercase words separated by hyphens.`);
  }
  return id;
}

function chapterSourcePath(value) {
  const sourcePath = requiredText(value, "Demo chapter sourcePath");
  const segments = sourcePath.split("/");
  if (
    sourcePath.includes("\\") ||
    sourcePath.includes("\0") ||
    !SOURCE_PATH_PATTERN.test(sourcePath) ||
    segments.some((segment) => !segment || segment === "." || segment === "..") ||
    /\.bak(?:$|\.)/iu.test(sourcePath)
  ) {
    throw new TypeError("Demo chapter sourcePath must name a canonical LaTeX chapter.");
  }
  return sourcePath;
}

function freezeExample(example, index, ids) {
  if (!example || typeof example !== "object" || Array.isArray(example)) {
    throw new TypeError(`Demo example ${index + 1} must be an object.`);
  }
  const id = stableId(example.id, `Demo example ${index + 1} id`);
  if (ids.has(id)) throw new TypeError(`Duplicate demo example id: ${id}`);
  ids.add(id);
  return Object.freeze({
    id,
    title: requiredText(example.title, `Demo example ${id} title`),
    description: requiredText(example.description, `Demo example ${id} description`),
    expected: requiredText(example.expected, `Demo example ${id} expected outcome`),
  });
}

/**
 * Validate and freeze the metadata shared by every browser demonstration.
 * The factory is kept as code; all descriptive values are copied into inert,
 * immutable records before the public shell sees them.
 */
export function defineDemo(definition) {
  if (!definition || typeof definition !== "object" || Array.isArray(definition)) {
    throw new TypeError("A demo definition must be an object.");
  }
  if (!definition.chapter || typeof definition.chapter !== "object" || Array.isArray(definition.chapter)) {
    throw new TypeError("A demo definition must identify its chapter.");
  }
  if (!Array.isArray(definition.examples) || definition.examples.length === 0) {
    throw new TypeError("A demo definition must provide at least one usage example.");
  }
  if (typeof definition.createRunner !== "function") {
    throw new TypeError("A demo definition must provide createRunner().");
  }

  const exampleIds = new Set();
  const examples = Object.freeze(
    definition.examples.map((example, index) => freezeExample(example, index, exampleIds)),
  );
  const demo = Object.freeze({
    id: stableId(definition.id, "Demo id"),
    chapter: Object.freeze({
      title: requiredText(definition.chapter.title, "Demo chapter title"),
      sourcePath: chapterSourcePath(definition.chapter.sourcePath),
    }),
    title: requiredText(definition.title, "Demo title"),
    summary: requiredText(definition.summary, "Demo summary"),
    warning: requiredText(definition.warning, "Demo educational warning"),
    examples,
    createRunner: definition.createRunner,
  });
  definedDemos.add(demo);
  return demo;
}

function normalizedMetric(metric, index) {
  if (!metric || typeof metric !== "object" || Array.isArray(metric)) {
    throw new TypeError(`Demo result metric ${index + 1} must be an object.`);
  }
  const valueType = typeof metric.value;
  if (valueType !== "string" && valueType !== "number") {
    throw new TypeError(`Demo result metric ${index + 1} value must be text or a number.`);
  }
  if (valueType === "number" && !Number.isFinite(metric.value)) {
    throw new TypeError(`Demo result metric ${index + 1} value must be finite.`);
  }
  return Object.freeze({
    label: requiredText(metric.label, `Demo result metric ${index + 1} label`),
    value: String(metric.value),
  });
}

function normalizedStep(step, index) {
  if (!step || typeof step !== "object" || Array.isArray(step)) {
    throw new TypeError(`Demo result step ${index + 1} must be an object.`);
  }
  return Object.freeze({
    label: requiredText(step.label, `Demo result step ${index + 1} label`),
    detail: requiredText(step.detail, `Demo result step ${index + 1} detail`),
  });
}

/** Normalize the output of a chapter-specific runner for the shared renderer. */
export function normalizeDemoResult(result) {
  if (!result || typeof result !== "object" || Array.isArray(result)) {
    throw new TypeError("A demo result must be an object.");
  }
  const tone = requiredText(result.tone, "Demo result tone");
  if (!RESULT_TONES.has(tone)) {
    throw new TypeError(`Unsupported demo result tone: ${tone}`);
  }
  if (!Array.isArray(result.metrics)) {
    throw new TypeError("A demo result must provide a metrics array.");
  }
  if (result.steps !== undefined && !Array.isArray(result.steps)) {
    throw new TypeError("Demo result steps must be an array when provided.");
  }
  return Object.freeze({
    verdict: stableId(result.verdict, "Demo result verdict"),
    title: requiredText(result.title, "Demo result title"),
    tone,
    summary: requiredText(result.summary, "Demo result summary"),
    metrics: Object.freeze(result.metrics.map(normalizedMetric)),
    steps: Object.freeze((result.steps ?? []).map(normalizedStep)),
    scope: requiredText(result.scope, "Demo result scope"),
  });
}

/**
 * Instantiate a demo through the common asynchronous lifecycle. The returned
 * facade validates example identifiers and normalizes every result.
 */
export function createDemoRunner(demo, options) {
  if (!definedDemos.has(demo)) {
    throw new TypeError("createDemoRunner() requires a definition returned by defineDemo().");
  }
  const runner = demo.createRunner(options);
  if (!runner || typeof runner !== "object" || Array.isArray(runner)) {
    throw new TypeError(`Demo ${demo.id} createRunner() must return an object.`);
  }
  if (!runner.ready || typeof runner.ready.then !== "function") {
    throw new TypeError(`Demo ${demo.id} runner.ready must be a promise.`);
  }
  if (typeof runner.run !== "function" || typeof runner.dispose !== "function") {
    throw new TypeError(`Demo ${demo.id} runner must provide run() and dispose().`);
  }

  const exampleIds = new Set(demo.examples.map((example) => example.id));
  const ready = Promise.resolve(runner.ready);
  let disposed = false;
  return Object.freeze({
    ready,
    async run(exampleId) {
      if (disposed) throw new Error(`Demo ${demo.id} runner has been disposed.`);
      if (!exampleIds.has(exampleId)) throw new Error(`Unknown example for ${demo.id}: ${exampleId}`);
      await ready;
      return normalizeDemoResult(await runner.run(exampleId));
    },
    async dispose() {
      if (disposed) return;
      disposed = true;
      await runner.dispose();
    },
  });
}

/** Build a collision-checked, read-only registry for the public shell. */
export function createDemoRegistry(definitions) {
  if (!Array.isArray(definitions)) throw new TypeError("Demo registry input must be an array.");
  const byId = new Map();
  const byChapter = new Map();
  const demos = definitions.map((candidate) => (
    definedDemos.has(candidate) ? candidate : defineDemo(candidate)
  ));
  for (const demo of demos) {
    if (byId.has(demo.id)) throw new TypeError(`Duplicate demo registration: ${demo.id}`);
    if (byChapter.has(demo.chapter.sourcePath)) {
      throw new TypeError(`Duplicate demo chapter registration: ${demo.chapter.sourcePath}`);
    }
    byId.set(demo.id, demo);
    byChapter.set(demo.chapter.sourcePath, demo);
  }
  const list = Object.freeze([...demos]);
  return Object.freeze({
    list() {
      return list;
    },
    get(id) {
      return byId.get(id) ?? null;
    },
    has(id) {
      return byId.has(id);
    },
  });
}
