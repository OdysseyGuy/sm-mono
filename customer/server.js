const express = require("express");
const app = express();

const fs = require("fs");
const serviceAccount = JSON.parse(fs.readFileSync("serviceaccount.json"));

var firebase = require("firebase-admin");
firebase.initializeApp({
  credential: firebase.credential.cert(serviceAccount),
});

app.get("/", (req, res) => {
  res.send("Hello World!");
});

// endpoint to set the limit of the customer
app.post("/limit", (req, res) => {
  res.send("Hello World!");
});

app.listen(3000, () => {
  console.log("Server is running on port 3000");
});
