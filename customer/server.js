const express = require("express");
const cors = require("cors");
const app = express();

app.use(cors());
app.use(express.json());
app.use("/api", require("./route"));

app.get("*", function (req, res) {
  res.status(404).send("404 Not Found");
});

app.listen(8080, function () {
  console.log("Server is running on port 3000");
});
