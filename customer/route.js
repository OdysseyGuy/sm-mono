const express = require("express");
const router = express.Router();

router.post("/register", function (req, res) {
  // 1. check if the sonsumer is already registered
  // 2. if registered return error else register the consumer
  const { consumerId, meterId } = req.body;
  console.log(`${consumerId} registered with meterId: ${meterId}`);
  res.status(200).send();
});

router.post("/subscribe", function (req, res) {
  // 1. check if the consumer if registered
  // 2. if not registered, return error else if registered, subscribe to the pack
  // 4. update the meter with the pack details
  const { consumerId, packId } = req.body;
});

router.post("/bill", function (req, res) {
  // 1. get the last billing date
  // 2. from the last date calculate the energy consumed
  // 3. calculate the bill based on the tariff
  const { consumerId, dailyLimit } = req.body;
});

module.exports = router;
