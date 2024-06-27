const express = require("express");
const http = require("http");
const jwt = require("jsonwebtoken");
const WebSocket = require('ws');

const app = express();
app.use(express.json());
const server = http.createServer(app);
const wss = new WebSocket.Server({server});

const SECRET_KEY = 'your_secret_key';

app.post("/login", (req, res) => {
  const { consumerId, meterId } = req.body;
  const user = { consumerId, meterId };
  const token = jwt.sign(user, SECRET_KEY, { expiresIn: '1h' });
  res.status(200).json({ token });
});

wss.on('connection', (ws, req) => {
  const token = req.url.split('token=')[1];
  if (!token) {
    ws.close();
    return;
  }

  jwt.verify(token, SECRET_KEY, (err, decoded) => {
    console.log(decoded);
    if (err) {
      ws.close();
      return;
    }
  })
  
  wss.on('message', (message) => {
    console.log(`Received message: ${message}`);
    wss.send(`Hello, ${decoded.username}!`);
  });
})

server.listen(3000, () => {
  console.log("Server started on port 3000");
});
