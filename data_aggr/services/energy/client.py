import grpc
from generated import energy_pb2_grpc, energy_pb2

def run():
    print("Trying to send request to tbe background service")
    with grpc.insecure_channel("localhost:50051") as channel:
        stub = energy_pb2_grpc.EnergyDataServiceStub(channel=channel)
        request = energy_pb2.QueryEnergyDataRequest(consumer_id="asdfasdf")
        response = stub.QueryEnergyData(request)
        print(response)

if __name__ == "__main__":
    run()
