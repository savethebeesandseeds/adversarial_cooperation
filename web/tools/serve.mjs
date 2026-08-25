import { createReadStream } from "node:fs";
import { stat } from "node:fs/promises";
import { createServer } from "node:http";
import path from "node:path";
import { fileURLToPath } from "node:url";

const toolsDirectory = path.dirname(fileURLToPath(import.meta.url));
const options = parseArguments(process.argv.slice(2));
const root = options.root;

const mimeTypes = new Map([
  [".css", "text/css; charset=utf-8"],
  [".html", "text/html; charset=utf-8"],
  [".ico", "image/x-icon"],
  [".jpg", "image/jpeg"],
  [".jpeg", "image/jpeg"],
  [".json", "application/json; charset=utf-8"],
  [".mjs", "text/javascript; charset=utf-8"],
  [".pdf", "application/pdf"],
  [".png", "image/png"],
  [".svg", "image/svg+xml"],
  [".txt", "text/plain; charset=utf-8"],
  [".wasm", "application/wasm"],
  [".webmanifest", "application/manifest+json"],
]);

const server = createServer(async (request, response) => {
  if (request.method !== "GET" && request.method !== "HEAD") {
    setSecurityHeaders(response);
    response.writeHead(405, { Allow: "GET, HEAD", "Content-Type": "text/plain; charset=utf-8" });
    response.end("Method not allowed.\n");
    return;
  }

  try {
    const requestUrl = new URL(request.url || "/", "http://localhost");
    const components = safePathComponents(requestUrl.pathname);
    let target = path.join(root, ...components);
    let details = await stat(target);
    if (details.isDirectory()) {
      target = path.join(target, "index.html");
      details = await stat(target);
    }
    if (!details.isFile()) throw Object.assign(new Error("Not a file"), { code: "ENOENT" });
    const extension = path.extname(target).toLowerCase();
    setSecurityHeaders(response, { embeddedPdf: extension === ".pdf" });
    response.writeHead(200, {
      "Content-Type": mimeTypes.get(extension) || "application/octet-stream",
      "Content-Length": details.size,
      "Cache-Control": "no-store",
      "X-Content-Type-Options": "nosniff",
    });
    if (request.method === "HEAD") response.end();
    else createReadStream(target).pipe(response);
  } catch (error) {
    setSecurityHeaders(response);
    const badRequest = error?.code === "BAD_PATH" || error instanceof URIError;
    response.writeHead(badRequest ? 400 : 404, { "Content-Type": "text/plain; charset=utf-8" });
    response.end(badRequest ? "Invalid path.\n" : "Not found. Build the static edition with npm run build.\n");
  }
});

server.listen(options.port, options.host, () => {
  const shownHost = options.host === "0.0.0.0" ? "localhost" : options.host;
  console.log(`Adversarial Cooperation reader: http://${shownHost}:${options.port}/`);
  console.log(`Serving generated files from ${root}`);
});

for (const signal of ["SIGINT", "SIGTERM"]) {
  process.on(signal, () => server.close(() => process.exit(0)));
}

function parseArguments(args) {
  const result = {
    host: "127.0.0.1",
    port: 4173,
    root: path.resolve(toolsDirectory, "..", "dist"),
  };
  for (let index = 0; index < args.length; index += 1) {
    const argument = args[index];
    if (argument === "--host" && args[index + 1]) result.host = args[++index];
    else if (argument === "--port" && args[index + 1]) result.port = Number(args[++index]);
    else if (argument === "--root" && args[index + 1]) result.root = path.resolve(args[++index]);
    else if (argument === "--help") {
      console.log("Usage: npm run serve -- [--host 127.0.0.1] [--port 4173] [--root web/dist]");
      process.exit(0);
    } else {
      throw new Error(`Unknown option: ${argument}`);
    }
  }
  if (!Number.isInteger(result.port) || result.port < 1 || result.port > 65535) {
    throw new Error("Port must be an integer between 1 and 65535.");
  }
  if (!/^[a-zA-Z0-9.:[\]-]+$/u.test(result.host)) throw new Error("Host contains unsupported characters.");
  if (result.root === path.parse(result.root).root) throw new Error("Refusing to serve a filesystem root.");
  return result;
}

function safePathComponents(pathname) {
  const decoded = decodeURIComponent(pathname);
  if (decoded.includes("\0") || decoded.includes("\\")) throw Object.assign(new Error("Unsafe path"), { code: "BAD_PATH" });
  const components = decoded.split("/").filter(Boolean);
  if (components.some((component) => component === "." || component === "..")) {
    throw Object.assign(new Error("Unsafe path"), { code: "BAD_PATH" });
  }
  return components.length ? components : ["index.html"];
}

function setSecurityHeaders(response, { embeddedPdf = false } = {}) {
  const frameAncestors = embeddedPdf ? "'self'" : "'none'";
  response.setHeader("Content-Security-Policy", `default-src 'self'; script-src 'self' 'wasm-unsafe-eval'; style-src 'self'; img-src 'self' data:; connect-src 'self'; worker-src 'self'; object-src 'self'; base-uri 'none'; frame-ancestors ${frameAncestors}`);
  response.setHeader("Referrer-Policy", "no-referrer");
  response.setHeader("Permissions-Policy", "camera=(), microphone=(), geolocation=(), payment=(), usb=()");
}
