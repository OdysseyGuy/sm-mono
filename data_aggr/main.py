import logging
from concurrent import futures

import firebase_admin
from firebase_admin import credentials

from services.energy.server import EnergyDataService

import grpc
from generated import energy_pb2_grpc

import schedule
import time

def serve():
    port = "50051"
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    energy_pb2_grpc.add_EnergyDataServiceServicer_to_server(EnergyDataService(), server)
    server.add_insecure_port("[::]:" + port)
    server.start()
    logger.info("Server started, listening on " + port)

    try:
        while True:
            schedule.run_pending()  # Run scheduled jobs
            time.sleep(1)
    except KeyboardInterrupt:
        server.stop(0)



if __name__ == "__main__":
    logging.basicConfig(
        format="%(asctime)s %(levelname)s (%(filename)s:%(lineno)d) %(message)s",
        datefmt="%d-%b-%y %H:%M:%S",
    )
    logger = logging.getLogger(__name__)
    logger.setLevel(logging.INFO)

    logger.info("Starting server...")
    try:
        # initialize the firebase certificate
        cred = credentials.Certificate("./serviceAccount.json")
    except IOError as e:
        logging.error(
            "Could not find the serviceAccount.json file. Make sure it is in the root of the project."
        )
        exit()
    except ValueError as e:
        logging.error(e)
        exit()

    try:
        firebase_admin.initialize_app(cred)
    except ValueError:
        logging.error(
            "Firebase app already initialized. Make sure you don't call firebase_admin.initialize_app() more than once."
        )
        exit()
    # run the server
    serve()
