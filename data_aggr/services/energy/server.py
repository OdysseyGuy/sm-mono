import logging

import paho.mqtt.client as mqtt
import json

from firebase_admin import firestore

import grpc
from google.protobuf.timestamp_pb2 import Timestamp

from generated import energy_pb2
from generated import energy_pb2_grpc

import datetime

# thresholds
DAILY_LIMIT = 1000


class EnergyDataService(energy_pb2_grpc.EnergyDataServiceServicer):
    def __init__(self):
        self.db: firestore.firestore.Client = firestore.client()
        self.yesturday_cache = {}

        # initialize MQTT client
        self.mqtt_client = mqtt.Client()
        self.mqtt_client.on_connect = self.on_connect
        self.mqtt_client.on_message = self.on_message

        # initialize firestore batch
        self.batch: firestore.firestore.WriteBatch = self.db.batch()
        self.batch_count = 0

        # connect to the mqtt broker
        try:
            self.mqtt_client.connect("broker.emqx.io")
        except ConnectionRefusedError:
            logging.error(
                "Could not connect to MQTT broker. Make sure it is running on localhost."
            )
            exit()

        # start the mqtt client loop
        self.mqtt_client.loop_start()

    def on_connect(self, client: mqtt.Client, userdata, flags, rc):
        logging.debug("Connected to MQTT brodker with result code" + str(rc))
        # TODO: use hierarchical topic structure
        client.subscribe("energy/asfas3242d2")

    def on_message(self, client: mqtt.Client, userdata, msg: mqtt.MQTTMessage):
        logging.debug(
            "Recieved message from MQTT broker: " + msg.topic + " " + str(msg.payload)
        )

        # parse the sensor data
        sensor_data = json.loads(msg.payload)

        # get the sensor id from the topic
        topic_parts = msg.topic.split("/")
        sensor_id = topic_parts[1]
        sensor_data["sensor_id"] = sensor_id

        # display the sensor data
        logging.debug(sensor_data)

        # add sensor data to the batch
        doc_ref = self.db.collection("sensor_data").document()
        # doc_ref.set(sensor_data)
        self.batch.set(doc_ref, sensor_data)

        # # increment the batch count
        self.batch_count += 1

        # # commit the batch if it is full
        if self.batch_count == 10:
            self.batch.commit()
            self.batch = self.db.batch()  # create a new batch
            self.batch_count = 0

        return
        # write the firebase query to convert unixstamp to epoc data
        ecpoh_data = datetime.datetime.fromtimestamp(sensor_data["timestamp"])
        if (
            (ecpoh_data.hour == 0)
            and (ecpoh_data.minute == 0)
            and (ecpoh_data.second == 0)
        ):
            # store todays reading
            self.yesturday_cache[sensor_id] = sensor_data["energy_consumed"]

        if sensor_data["energy_consumed"] > self.yesturday_cache[sensor_id] + 1000:
            # send a notification
            doc_ref = self.db.collection('consumers').document()
            # write the firebase query to get the row with certain sensor id
            query_ref = doc_ref.where("sensor_id", "==", sensor_id).document()
            docs = query_ref.get()
            # use firebase fcm
            pass

    # def GetSensorData(self, request: energy_pb2.GetSensorDataRequest, context):
    #     logging.debug("Received QueryEnergyData request")
    #     consumer_id = request.consumer_id
    #     date = request.date

    #     # get sensor data form firestore
    #     collec_ref = self.db.collection("sensor_data")
    #     query_ref = collec_ref.where("consumer_id", "==", consumer_id).document()
    #     docs = query_ref.get()

    #     timestamp = Timestamp()
    #     sensor_data = energy_pb2.DataPoint(
    #         timestamp=timestamp,
    #         energy_consumed=energy_consumed,
    #         active_power=active_power,
    #         voltage=voltage,
    #         current=current,
    #         factor=factor,
    #         frequency=frequency,
    #     )

    #     response = energy_pb2.QueryEnergyDataResponse(sensor_data=[sensor_data])
    #     return response, grpc.StatusCode.OK

    def SetDailyLimit(self, request: energy_pb2.SetDailyLimitRequest, context):
        doc_ref = self.db.collection("daily_limit").document(request.consumer_id)
        doc_ref.set({"limit": request.daily_limit})
        response = energy_pb2.SetDailyLimitResponse()
        return response, grpc.StatusCode.OK

    def RegisterUser(self, request, context):
        doc_ref = self.db.collection("user_registrations").document(request.consumer_id)
        doc_ref.set(
            {
                "sensorId": request.sensor_id,
                "tokenId": request.token_id,
            }
        )

        response = energy_pb2
        return super().RegisterUser(request, context)
