import React from "react";
import "./App.css";
import Home from "./pages/Home";
import { CssVarsProvider } from "@mui/joy";
import theme from "./theme";

const App = () => {
  return (
    <CssVarsProvider theme={theme}>
      <Home />
    </CssVarsProvider>
  );
};

export default App;
