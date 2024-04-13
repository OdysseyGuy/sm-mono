const express = require("express");
const cors = require("cors");
const app = express();

const http = require('http');
const sever = http.createServer(app);

const { Server } = require("socket.io");
const io = new Server(sever);

app.use(cors());
app.use(express.json());
app.use("/api", require("./route"));

app.get("*", function (req, res) {
  res.status(404).send("404 Not Found");
});

io.on("connection", (socket) => {
  console.log('a user connected');
  socket.on("disconnect", () => {
    console.log("user disconnected");
  });
});

io.use((socket, next) => {
  const token = socket.handshake.auth.token;
  if (isValid(token)) {
    return next();
  }
  return next(new Error("authentication error"));
});

app.listen(8080, function () {
  console.log("Server is running on port 3000");
});
