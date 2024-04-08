//----------------------------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WifiClientSecure.h>
#include <ModbusMaster.h>
#include <SoftwareSerial.h>

//----------------------------------------------------------------------------------

SoftwareSerial pzem(D5, D6);
ModbusMaster node;

WiFiClient wifiClient;
PubSubClient pubsubClient(wifiClient);

WiFiUDP wifiUdp;
NTPClient timeClient(wifiUdp, "pool.ntp.org");

//----------------------------------------------------------------------------------
// wifi
// WiFi credentials.

const char *ssid = "twin";
const char *password = "gogowifi";

//----------------------------------------------------------------------------------
// mqtt
// MQTT broker based detials.

const char *mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

const char *sensorId = "asfas3242d2";
const char *mqtt_pub_topic = "meter/asfas3242d2";

// subscribe to this topic so that when ever there is update to
// daily consumeable units the sensor gets updated about the new unit
const char *mqtt_units_sub_topic = "energy/asfas3242d2/units";

//----------------------------------------------------------------------------------

unsigned long lastUpload;
unsigned long lastLimitSet; // epoch timestamp for last set limit time
const unsigned int limitSetDelay = 3600 * 1000; // 24 hr
const unsigned int uploadThrottle = 900; // 900 ms

double voltage_usage, current_usage, active_power, active_energy;
double frequency, power_factor, over_power_alarm;

// just for debug
float energy = 0;

const char* limitEndpoint = "";

float balance = 0.0f;

//----------------------------------------------------------------------------------
// connectToWifi
// Connect to the WiFi using the credentials provided.

void connectToWifi()
{
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");

    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(++i);
        Serial.print(' ');
    }

    Serial.println('\n');
    Serial.println("Connection established!");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
}

//----------------------------------------------------------------------------------
// reconnect
// Call this when we want to reconnect to the MQTT broker.

void reconnect()
{
    // loop until we're reconnected
    while (!pubsubClient.connected())
    {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);

        if (pubsubClient.connect(clientId.c_str()))
        {
            Serial.println("Connected to broker!");
            // pubsubClient.subscribe(mqtt_sub_topic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(pubsubClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

//----------------------------------------------------------------------------------
// callback
// Callback function gets invoked whenever our device recieves a message
// from the MQTT broker.

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message received from: ");
    Serial.println(topic);
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }

    Serial.println();
    Serial.println();
}

//----------------------------------------------------------------------------------
// publishMessage
// Send message from sensor to the MQTT broker.

void publishMessage(const char *topic, String payload, boolean retained)
{
    if (pubsubClient.publish(topic, payload.c_str(), true))
    {
        Serial.println("Message publised [" + String(topic) + "]: " + payload);
    }
}

//----------------------------------------------------------------------------------
// float_rand
// Use to generate random floating point numbers.

float float_rand(float min, float max)
{
    float scale = rand() / (float)RAND_MAX;
    return min + scale * (max - min);
}

//----------------------------------------------------------------------------------
// setup
// Arduino run this code once on startup.

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("Start serial");

    connectToWifi();

    // initialize the pzem serial port
    pzem.begin(9600);
    Serial.println("Start PZEM serial");

    // initialize the pzem module 
    node.begin(1, pzem);
    Serial.println("Start PZEM");

    // we are going to subscribe to the limit set topic
    pubsubClient.setCallback(callback);
    pubsubClient.setServer(mqtt_server, mqtt_port);

    // initialize the time clinet and set it to IST
    timeClient.begin();
    timeClient.setTimeOffset(19800); // IST time offset in seconds

    // lets query the limit once at startup
    // wifiClient.
}

//----------------------------------------------------------------------------------
// loop
// This code will run infinitely until the code is switched off.

void loop()
{
    if (!pubsubClient.connected())
    {
        reconnect();
    }
    
    pubsubClient.loop();
    timeClient.update();

    // real pzem values
    //  result = node.readInputRegisters(0x0000, 10);
    //  if (result == node.ku8MBSuccess)
    //  {
    //    voltage_usage = (node.getResponseBuffer(0x00) / 10.0f); // V
    //    current_usage = (node.getResponseBuffer(0x01) / 1000.000f); // A
    //    active_power = (node.getResponseBuffer(0x03) / 10.0f); // W
    //    active_energy = (node.getResponseBuffer(0x05) / 1000.0f); // kWh
    //    frequency = (node.getResponseBuffer(0x07) / 10.0f); // Hz
    //    power_factor = (node.getResponseBuffer(0x08) / 100.0f);
    //  }

    // we would actully get this value from pzem
    float voltage = float_rand(220,225);
    float frequency = float_rand(50,51);
    float power = float_rand(0.00003,0.00004);
    energy = energy + power;
    float current = float_rand(2,3);

    DynamicJsonDocument doc(1024);
    doc["timestamp"] = timeClient.getEpochTime();
    doc["voltage"] = voltage;
    doc["current"] = current;
    doc["energy"] = energy;
    doc["power"] = power;
    doc["frequency"] = frequency;

    char mqtt_message[256];
    serializeJson(doc, mqtt_message);

    // upload the readings every second
    unsigned long currentTime = millis();
    if ((currentTime - lastUpload) >= uploadThrottle)
    {
        publishMessage(mqtt_pub_topic, mqtt_message, true);
        lastUpload = currentTime;
    }

    delay(100);
}

//----------------------------------------------------------------------------------
