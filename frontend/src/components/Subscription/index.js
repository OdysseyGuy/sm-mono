import { Card, CardContent, Grid, Stack, Typography } from "@mui/joy";
import React from "react";

const prepaidPlans = [
  { Description: "Prepaid Plan 1",validity: "3 Months" },
  { Description: "Prepaid Plan 2" },
];

const Subscriptions = () => {
  return (
    <>
      <Typography level="h3" sx={{ mb: 1 }}>
        Subscriptions
      </Typography>
      <Typography sx={{ mb: 1 }}>Prepaid Plans</Typography>
      <Grid container spacing={2} sx={{ mb: 2 }}>
        {prepaidPlans.map((plan) => (
          <Grid item key={plan.Description}>
            <Card sx={{ width: 320 }}>
              <CardContent>
                <Typography>{plan.Description}</Typography>
              </CardContent>
            </Card>
          </Grid>
        ))}
      </Grid>
    </>
  );
};

export default Subscriptions;
