import React from "react";
import Register from "../../components/Register";
import { Divider, Sheet, Typography } from "@mui/joy";
import Subscriptions from "../../components/Subscription";

const Home = () => {
  return (
    <>
      <Typography level="h1" sx={{ mb: 1, mt: 1 }} textAlign="center">
        Smart Energy Meter
        <Typography level="body-md" sx={{ pl: 1 }}>
          v0.1.0beta
        </Typography>
      </Typography>
      <Divider sx={{ mb: 2 }} />
      <Sheet sx={{ pl: 4, pr: 4, bgcolor: "#ffffff" }}>
        <Register />
        <Subscriptions />
      </Sheet>
    </>
  );
};

export default Home;
