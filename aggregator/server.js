const mqtt = require("mqtt");
const { MongoClient } = require("mongodb");
const mongoose = require('mongoose');
// const firebase = require("firebase-admin");
// const fs = require("fs");

const protocol = "mqtt";
const host = "broker.emqx.io";
const port = "1883";
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`;
const connectUrl = `${protocol}://${host}:${port}`;

// let firestore;
let client;
let db;
let readingsDoc;

const uri = "mongodb://127.0.0.1:27017";
const mongoClient = new MongoClient(uri);

function onMqttMessage(topic, message) {
  console.log("Received message from MQTT Broker", topic);
  let strMessage = message.toString();
  let objMessage = JSON.parse(strMessage);

  // get the sensor id from topic
  let sensorId = topic.split("/")[1];

  // console.log(objMessage);

  var utcSeconds = parseInt(objMessage["timestamp"]);
  var date = new Date(0);
  date.setUTCSeconds(utcSeconds);
  let reading = {
    timestamp: date,
    metadata: {
      sensorId: sensorId.toString(),
    },
    voltage: parseFloat(objMessage["voltage"]),
    frequency: parseFloat(objMessage["frequency"]),
    current: parseFloat(objMessage["current"]),
    energy: parseFloat(objMessage["energy"]),
    power: parseFloat(objMessage["power"]),
    powerFactor: parseFloat(objMessage["powerFactor"]),
  };

  // console.log(reading);
  // console.log(readingsDoc)
  readingsDoc.insertOne(reading);
  // firestore.collection("sensor_data").add(payload);
}

async function main() {
  try {
    await mongoClient.connect();
    db = mongoClient.db("meter");
    readingsDoc = db.collection("readings");
  } catch (error) {
    console.error("Error connecting to MongoDB", error);
  }

  // await mongoose.connect()

  // const serviceAccount = JSON.parse(fs.readFileSync("serviceaccount.json"));
  // firebase.initializeApp({
  //   credential: firebase.credential.cert(serviceAccount),
  // });

  // firestore = firebase.firestore();

  client = mqtt.connect(connectUrl, {
    clientId,
    clean: true,
    connectTimeout: 4000,
    reconnectPeriod: 1000,
  });

  // topic to subscribe to
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

  client.on("message", onMqttMessage);

  client.on("close", function () {
    console.log("Disconnected from MQTT Broker");
  });

  client.on("offline", function () {
    console.log("MQTT Broker is offline");
  });

  client.on("error", function (error) {
    console.error(error);
  });
}

main();
