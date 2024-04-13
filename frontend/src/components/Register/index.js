import {
  Alert,
  Button,
  FormControl,
  Grid,
  Input,
  Stack,
  Typography,
} from "@mui/joy";
import React from "react";
import axios, { HttpStatusCode } from "axios";
import { CheckCircleOutline } from "@mui/icons-material";

const Register = () => {
  const [success, setSuccess] = React.useState(false);

  handleRegistration = (event) => {
    console.log("start registration");
    axios({
      method: "post",
      url: "http://localhost:8080/api/register",
      withCredentials: false,
      headers: {
        "Content-Type": "application/json",
      },
      data: {
        consumerId: "event.target.consumerId.value",
        meterId: "event.target.meterId.value",
      },
    }).then((response) => {
      if (response.status == HttpStatusCode.Ok) {
        setSuccess(true);
      }
    });
  };

  return (
    <>
      <Typography level="h3">Registration</Typography>
      <Typography sx={{ mb: 2 }}>
        Register a new consumer. Registration is required to subscribe to a
        plan.
      </Typography>
      <form>
        <Stack spacing={2} sx={{ mb: 2 }}>
          <Grid
            container
            spacing={2}
            sx={{ flexGrow: 1, justifyContent: "center" }}
          >
            <Grid>
              <FormControl>
                <Input
                  name="consumerId"
                  placeholder="Consumer ID"
                  required
                  sx={{
                    "--Input-radius": "0px",
                    borderBottom: "2px solid",
                    borderColor: "neutral.outlinedBorder",
                    "&:hover": {
                      borderColor: "neutral.outlinedHoverBorder",
                    },
                    "&::before": {
                      border: "1px solid var(--Input-focusedHighlight)",
                      transform: "scaleX(0)",
                      left: 0,
                      right: 0,
                      bottom: "-2px",
                      top: "unset",
                      borderRadius: 0,
                    },
                    "&:focus-within::before": {
                      transform: "scaleX(1)",
                    },
                  }}
                />
              </FormControl>
            </Grid>
            <Grid>
              <FormControl>
                <Input
                  name="meterId"
                  placeholder="Meter ID"
                  required
                  sx={{
                    "--Input-radius": "0px",
                    borderBottom: "2px solid",
                    borderColor: "neutral.outlinedBorder",
                    "&:hover": {
                      borderColor: "neutral.outlinedHoverBorder",
                    },
                    "&::before": {
                      border: "1px solid var(--Input-focusedHighlight)",
                      transform: "scaleX(0)",
                      left: 0,
                      right: 0,
                      bottom: "-2px",
                      top: "unset",
                      borderRadius: 0,
                    },
                    "&:focus-within::before": {
                      transform: "scaleX(1)",
                    },
                  }}
                />
              </FormControl>
            </Grid>
            <Grid>
              <Button onClick={handleRegistration} sx={{ borderRadius: 0 }}>
                Register
              </Button>
            </Grid>
          </Grid>
          {success && (
            <Alert
              size="lg"
              color="success"
              startDecorator={<CheckCircleOutline />}
            >
              Consumer registered successfully.
            </Alert>
          )}
        </Stack>
      </form>
    </>
  );
};

export default Register;
