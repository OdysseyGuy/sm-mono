const express = require("express");
const router = express.Router();

router.post("/register", function (req, res) {
  const { consumerId, meterId } = req.body;
  console.log(`${consumerId} registered with meterId: ${meterId}`);
  res.send("Hello")
  // res.status(200).send({});
});

router.post("/subscribe", function (req, res) {
  const { consumerId, packId } = req.body;
  try {
  } catch {}
});

router.post("/generate-bill", function (req, res) {
  const { consumerId, dailyLimit } = req.body;
});

module.exports = router;
