const fs = require("fs");
const serviceAccount = JSON.parse(fs.readFileSync("serviceaccount.json"));

var firebase = require("firebase-admin");
firebase.initializeApp({
  credential: firebase.credential.cert(serviceAccount),
});

module.exports = firebase.firestore();
