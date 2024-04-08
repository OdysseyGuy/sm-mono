const mqtt = require("mqtt");
const firebase = require("firebase-admin");
const fs = require("fs");

const protocol = "mqtt";
const host = "broker.emqx.io";
const port = "1883";
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`;
const connectUrl = `${protocol}://${host}:${port}`;

const serviceAccount = JSON.parse(fs.readFileSync("serviceaccount.json"));
firebase.initializeApp({
  credential: firebase.credential.cert(serviceAccount),
});

// const firestore = firebase.firestore();

const client = mqtt.connect(connectUrl, {
  clientId,
  clean: true,
  connectTimeout: 4000,
  reconnectPeriod: 1000,
});

let topic = "meter/#";

client.on("connect", function () {
  console.log("Connected to MQTT Broker");
  client.subscribe(topic, function (error) {
    if (error) {
      console.error("Error subscribing to topic", error);
    } else {
      console.log("Subscribed to topic", topic);
    }
  });
});

client.on("reconnect", function (error) {
  console.error("Reconnecting to MQTT Broker");
  if (error) {
    console.error("Error", error);
  }
});

client.on("message", function (topic, message) {
  console.log("Received message from MQTT Broker", topic);
  let strMessage = message.toString();
  let objMessage = JSON.parse(strMessage);

  // get the sensor id from topic
  let sensorId = topic.split("/")[1];

  let payload = {
    sensorId: sensorId,
    timestamp: objMessage['timestamp'],
    voltage: objMessage['voltage'],
    current: objMessage['current'],
    energy: objMessage['energy'],
    power: objMessage['power'],
  }

  console.log(payload);
  // firestore.collection("sensor_data").add(payload);
});

client.on("close", function () {
  console.log("Disconnected from MQTT Broker");
});

client.on("offline", function () {
  console.log("MQTT Broker is offline");
});

client.on("error", function (error) {
  console.error(error);
});

module.exports = client;
