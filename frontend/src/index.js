import { createRoot } from "react-dom/client";
import App from "./App";

const rootElem = document.getElementById("react-root");
const root = createRoot(rootElem);
root.render(<App />);
