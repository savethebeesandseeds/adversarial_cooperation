import { createDemoRegistry } from "./demo-contract.mjs";
import { ticTacToeDemo } from "./demos/ttt-demo.mjs";

export const demoRegistry = createDemoRegistry([ticTacToeDemo]);
export const demos = demoRegistry.list();

export function getDemo(id) {
  return demoRegistry.get(id);
}

export default demoRegistry;
