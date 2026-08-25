import { createDemoRunner } from "./demo-contract.mjs";
import { demos, getDemo } from "./demo-registry.mjs";

const app = document.querySelector("#app");
const main = document.querySelector("#main-content");
const routeLinks = [...document.querySelectorAll("[data-route]")];
const PDF_PATH = "./book/Adversarial-Cooperation.pdf";

let activeRunner = null;
let renderGeneration = 0;

function node(tag, options = {}, children = []) {
  const element = document.createElement(tag);
  if (options.className) element.className = options.className;
  if (options.text !== undefined) element.textContent = options.text;
  if (options.id) element.id = options.id;
  if (options.attributes) {
    for (const [name, value] of Object.entries(options.attributes)) {
      element.setAttribute(name, String(value));
    }
  }
  if (options.dataset) {
    for (const [name, value] of Object.entries(options.dataset)) {
      element.dataset[name] = String(value);
    }
  }
  element.append(...children.filter(Boolean));
  return element;
}

function text(value) {
  return document.createTextNode(value);
}

function link(label, href, className = "") {
  return node("a", { className, text: label, attributes: { href } });
}

function pageIntro(kicker, title, description) {
  return node("header", { className: "page-intro" }, [
    node("p", { className: "eyebrow", text: kicker }),
    node("h1", { text: title }),
    node("p", { className: "lede", text: description }),
  ]);
}

function parseRoute(hash) {
  if (!hash || hash === "#" || hash === "#read") return { name: "read" };
  if (hash === "#demos") return { name: "demos" };
  if (hash === "#about") return { name: "about" };
  if (hash.startsWith("#demo=")) {
    try {
      const id = decodeURIComponent(hash.slice("#demo=".length));
      return id ? { name: "demo", id } : { name: "not-found" };
    } catch {
      return { name: "not-found" };
    }
  }
  return { name: "not-found" };
}

function setActiveNavigation(route) {
  const active = route.name === "demo" ? "demos" : route.name;
  for (const routeLink of routeLinks) {
    if (routeLink.dataset.route === active) routeLink.setAttribute("aria-current", "page");
    else routeLink.removeAttribute("aria-current");
  }
}

async function disposeActiveRunner() {
  const runner = activeRunner;
  activeRunner = null;
  if (!runner) return;
  try {
    await runner.dispose();
  } catch (error) {
    console.warn("The previous demo did not dispose cleanly.", error);
  }
}

function show(view) {
  app.replaceChildren(view);
}

function renderRead() {
  const openPdf = link("Open PDF in a new tab", PDF_PATH, "button secondary");
  openPdf.target = "_blank";
  openPdf.rel = "noopener";

  const fallback = node("div", { className: "pdf-fallback" }, [
    node("p", { text: "This browser could not place the PDF inside the page." }),
    link("Open the complete PDF", PDF_PATH, "text-link"),
  ]);
  const object = node("object", {
    className: "pdf-object",
    attributes: {
      data: `${PDF_PATH}#view=FitH`,
      type: "application/pdf",
      "aria-label": "Adversarial Cooperation manuscript PDF",
    },
  }, [fallback]);

  return node("section", { className: "page read-page" }, [
    node("div", { className: "reader-heading" }, [
      pageIntro(
        "The complete manuscript",
        "Read the book",
        "This is the PDF compiled from the canonical LaTeX manuscript, including its current chapters, front matter, and references.",
      ),
      openPdf,
    ]),
    node("div", { className: "pdf-shell" }, [object]),
  ]);
}

function demoCard(demo) {
  return node("article", { className: "demo-card" }, [
    node("div", { className: "demo-card-top" }, [
      node("p", { className: "eyebrow", text: demo.chapter.title }),
      node("span", {
        className: "runtime-chip",
        text: "C · WebAssembly",
        attributes: { "aria-label": "C compiled to WebAssembly" },
      }),
    ]),
    node("h2", { text: demo.title }),
    node("p", { text: demo.summary }),
    node("p", {
      className: "example-count",
      text: `${demo.examples.length} explained usage ${demo.examples.length === 1 ? "example" : "examples"}`,
    }),
    link("Open demonstration", `#demo=${encodeURIComponent(demo.id)}`, "card-link"),
  ]);
}

function renderDemos() {
  const gallery = node("div", { className: "demo-grid" }, demos.map(demoCard));
  if (demos.length === 0) {
    gallery.append(node("p", {
      className: "empty-message",
      text: "No browser demonstration has passed the publication checks yet.",
    }));
  }
  return node("section", { className: "page" }, [
    pageIntro(
      "Executable companions",
      "Working demonstrations",
      "Each item here runs a chapter companion compiled from C. Chapters without a working browser build are simply not listed yet.",
    ),
    gallery,
  ]);
}

function resultView(result) {
  const metrics = node("dl", { className: "result-metrics" }, result.metrics.map((metric) => (
    node("div", { className: "metric" }, [
      node("dt", { text: metric.label }),
      node("dd", { text: metric.value }),
    ])
  )));

  const parts = [
    node("header", { className: "result-heading" }, [
      node("p", { className: "eyebrow", text: "Demonstration result" }),
      node("h2", { text: result.title }),
      node("p", { text: result.summary }),
    ]),
    metrics,
  ];
  if (result.steps.length > 0) {
    parts.push(node("section", { className: "result-steps" }, [
      node("h3", { text: "Execution details" }),
      node("ol", {}, result.steps.map((step) => node("li", {}, [
        node("strong", { text: step.label }),
        text(` — ${step.detail}`),
      ]))),
    ]));
  }
  parts.push(node("section", { className: "result-scope" }, [
    node("h3", { text: "What this run establishes" }),
    node("p", { text: result.scope }),
  ]));
  return node("article", {
    className: "demo-result",
    attributes: { "aria-live": "polite" },
    dataset: { tone: result.tone, verdict: result.verdict },
  }, parts);
}

function errorView(message) {
  return node("div", { className: "runtime-error", attributes: { role: "alert" } }, [
    node("strong", { text: "The demonstration could not run." }),
    node("p", { text: message }),
  ]);
}

function exampleChoice(example, checked) {
  const input = node("input", {
    attributes: {
      type: "radio",
      name: "example",
      value: example.id,
    },
  });
  input.checked = checked;
  return node("label", { className: "example-choice" }, [
    input,
    node("span", { className: "example-copy" }, [
      node("strong", { text: example.title }),
      node("span", { text: example.description }),
      node("small", { text: example.expected }),
    ]),
  ]);
}

function renderDemoDetail(demo, generation) {
  const runtimeLabel = node("span", { text: "Loading demonstration…" });
  const runtimeStatus = node("div", {
    className: "runtime-status",
    attributes: { role: "status", "aria-live": "polite" },
    dataset: { state: "loading" },
  }, [node("span", { className: "status-dot", attributes: { "aria-hidden": "true" } }), runtimeLabel]);
  const submit = node("button", {
    className: "button primary",
    text: "Run example",
    attributes: { type: "submit", disabled: "" },
  });
  const fieldset = node("fieldset", { className: "example-list" }, [
    node("legend", { text: "Choose an example" }),
    ...demo.examples.map((example, index) => exampleChoice(example, index === 0)),
  ]);
  const form = node("form", { className: "demo-form" }, [fieldset, submit]);
  const output = node("div", { className: "demo-output" }, [
    node("div", { className: "demo-placeholder" }, [
      node("div", { className: "board-mark", text: "C → WebAssembly", attributes: { "aria-hidden": "true" } }),
      node("p", { text: "Choose an example, then run its compiled C companion in this page." }),
    ]),
  ]);

  const view = node("section", { className: "page demo-detail" }, [
    link("← All demonstrations", "#demos", "back-link"),
    node("div", { className: "demo-title-row" }, [
      pageIntro(demo.chapter.title, demo.title, demo.summary),
      runtimeStatus,
    ]),
    node("aside", { className: "educational-warning", attributes: { role: "note" } }, [
      node("strong", { text: "Boundary of this demonstration" }),
      node("p", { text: demo.warning }),
    ]),
    node("div", { className: "demo-workbench" }, [form, output]),
  ]);

  let runner;
  try {
    runner = createDemoRunner(demo);
    activeRunner = runner;
    runner.ready.then(() => {
      if (generation !== renderGeneration || activeRunner !== runner) return;
      runtimeStatus.dataset.state = "ready";
      runtimeLabel.textContent = "Demonstration ready";
      submit.disabled = false;
    }).catch((error) => {
      if (generation !== renderGeneration || activeRunner !== runner) return;
      runtimeStatus.dataset.state = "error";
      runtimeLabel.textContent = "WebAssembly unavailable";
      submit.disabled = true;
      output.replaceChildren(errorView(error instanceof Error ? error.message : String(error)));
    });
  } catch (error) {
    runtimeStatus.dataset.state = "error";
    runtimeLabel.textContent = "WebAssembly unavailable";
    output.replaceChildren(errorView(error instanceof Error ? error.message : String(error)));
  }

  form.addEventListener("submit", async (event) => {
    event.preventDefault();
    if (!runner || activeRunner !== runner) return;
    const formData = new FormData(form);
    const exampleId = String(formData.get("example") ?? "");
    submit.disabled = true;
    submit.textContent = "Running…";
    runtimeStatus.dataset.state = "running";
    runtimeLabel.textContent = "Demonstration running";
    try {
      const result = await runner.run(exampleId);
      if (generation !== renderGeneration || activeRunner !== runner) return;
      output.replaceChildren(resultView(result));
      runtimeStatus.dataset.state = "ready";
      runtimeLabel.textContent = "Run complete";
    } catch (error) {
      if (generation !== renderGeneration || activeRunner !== runner) return;
      output.replaceChildren(errorView(error instanceof Error ? error.message : String(error)));
      runtimeStatus.dataset.state = "error";
      runtimeLabel.textContent = "Run failed";
    } finally {
      if (generation === renderGeneration && activeRunner === runner) {
        submit.disabled = runtimeStatus.dataset.state === "error";
        submit.textContent = "Run example";
      }
    }
  });

  return view;
}

function renderAbout() {
  const principles = node("div", { className: "about-grid" }, [
    node("article", { className: "about-card" }, [
      node("p", { className: "step-number", text: "01" }),
      node("h2", { text: "The manuscript remains authoritative" }),
      node("p", { text: "The Read view opens the PDF compiled from the project's LaTeX manuscript. The website does not maintain an editable shadow copy of the book." }),
    ]),
    node("article", { className: "about-card" }, [
      node("p", { className: "step-number", text: "02" }),
      node("h2", { text: "The demonstrations come from C" }),
      node("p", { text: "A listed demonstration is an adapter around an existing C companion compiled to WebAssembly. Adding another chapter demo reuses the same registry, controls, status, and result presentation." }),
    ]),
    node("article", { className: "about-card" }, [
      node("p", { className: "step-number", text: "03" }),
      node("h2", { text: "Execution is evidence with a boundary" }),
      node("p", { text: "A successful run is an implementation observation for its displayed model. It is not, by itself, a cryptographic proof, a security theorem, or a guarantee about the world outside the program." }),
    ]),
  ]);
  return node("section", { className: "page about-page" }, [
    pageIntro(
      "How this edition works",
      "One book, one codebase",
      "The public website has a deliberately small job: make the manuscript comfortable to read and make the working executable companions easy to inspect.",
    ),
    principles,
    node("div", { className: "about-note" }, [
      node("h2", { text: "No browser editor" }),
      node("p", { text: "Selecting text here behaves like selecting text on any reading website. Manuscript changes continue in the canonical LaTeX source, where they can be reviewed, compiled, and tested honestly." }),
    ]),
  ]);
}

function renderNotFound() {
  return node("section", { className: "page not-found" }, [
    node("p", { className: "eyebrow", text: "Page not found" }),
    node("h1", { text: "This view is not part of the reader." }),
    node("p", { text: "Open the book or choose one of the demonstrations that is ready today." }),
    node("div", { className: "button-row" }, [
      link("Read the book", "#read", "button primary"),
      link("See demonstrations", "#demos", "button secondary"),
    ]),
  ]);
}

async function renderRoute() {
  const generation = ++renderGeneration;
  await disposeActiveRunner();
  if (generation !== renderGeneration) return;

  const route = parseRoute(location.hash);
  setActiveNavigation(route);
  if (route.name === "read") show(renderRead());
  else if (route.name === "demos") show(renderDemos());
  else if (route.name === "about") show(renderAbout());
  else if (route.name === "demo") {
    const demo = getDemo(route.id);
    show(demo ? renderDemoDetail(demo, generation) : renderNotFound());
  } else show(renderNotFound());

  if (location.hash === "" || location.hash === "#") history.replaceState(null, "", "#read");
  main.focus({ preventScroll: true });
}

window.addEventListener("hashchange", renderRoute);
window.addEventListener("pagehide", () => {
  void disposeActiveRunner();
});
window.addEventListener("pageshow", (event) => {
  if (event.persisted) void renderRoute();
});

void renderRoute();
