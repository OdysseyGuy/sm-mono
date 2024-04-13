import { extendTheme } from "@mui/joy";

const theme = extendTheme({
  components: {
    JoyCard: {
      styleOverrides: {
        root: {
          borderRadius: 0,
        },
      },
    },
  }
});

export default theme;