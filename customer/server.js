const express = require("express");
const cors = require("cors");
const app = express();

const http = require("http");
const sever = http.createServer(app);

const { Server } = require("socket.io");
const logger = require("./logger");
const io = new Server(sever);

app.use(cors());
app.use(express.json());

// routes
app.use("/api", require("./route"));
app.get("*", function (req, res) {
  res.status(404).send("404 Not Found");
});

// socket.io
io.on("connection", (socket) => {
  console.log("a user connected");
  socket.on("disconnect", () => {
    console.log("user disconnected");
  });
});

io.use((socket, next) => {
  // TODO: find a way to verify the user on the first handshake
  // but we also need to store the data about a particular user so that we can query our
  // backend about the user details
  const token = socket.handshake.auth.token;
  if (isValid(token)) {
    return next();
  }
  return next(new Error("authentication error"));
});

// start server
app.listen(8080, function () {
  logger.info("Server is running on port 3000");
});
