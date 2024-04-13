import axios from "axios";

const CONSUMER_API_URL = "http://localhost:8080/api/";

const ConsumerAPI = {
  registerUser: async () => {
    const response = await axios.request({});
    return response.json();
  },
  getSubscriptions: async () => {
    const response = await axios(`/api/consumers/${consumerId}/subscriptions`);
    return response.json();
  },
};

export default ConsumerAPI;
