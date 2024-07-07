const mqtt = require("mqtt");
const { MongoClient } = require("mongodb");

// constants
const MQTT_BROKER_PROTOCOL = "mqtt";
const MQTT_BROKER_HOST = "broker.emqx.io";
const MQTT_BROKER_PORT = "1883";
const MQTT_BROKER_CLIENTID = `mqtt_${Math.random().toString(16).slice(3)}`;
const MQTT_BROKER_CONNECT_URL = `${MQTT_BROKER_PROTOCOL}://${MQTT_BROKER_HOST}:${MQTT_BROKER_PORT}`;
const MONGODB_URI = "mongodb://127.0.0.1:27017";

let db;
let readingsDocument;

function onMqttMessage(topic, message) {
  // log to console that we received a message
  // this is just for debugging purposes
  // We can remove this line in production
  console.log("Received message from MQTT Broker", topic);

  // process the message
  let strMessage = message.toString();
  let objMessage = JSON.parse(strMessage);

  // get the sensor id from topic
  let sensorId = topic.split("/")[1];

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

  // TODO: handle error if insert fails
  readingsDocument.insertOne(reading);
}

async function main() {
  // first connect to mongodb database
  // TODO: handle error if connection fails
  const mongoClient = new MongoClient(MONGODB_URI);
  try {
    await mongoClient.connect();
    db = mongoClient.db("meter");
    readingsDocument = db.collection("readings");
  } catch (error) {
    console.error("Error connecting to MongoDB", error);
  }

  // todo implement a tcp based client

}

main();
