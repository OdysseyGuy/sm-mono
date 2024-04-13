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

//----------------------------------------------------------------------------------

unsigned long lastUpload;
unsigned long lastLimitSet; // epoch timestamp for last set limit time
const unsigned int limitSetDelay = 3600 * 1000; // 24 hr
const unsigned int uploadThrottle = 900; // 900 ms

//----------------------------------------------------------------------------------
// electrical parameters

double voltage, current, power, energy;
double frequency, power_factor;

//----------------------------------------------------------------------------------

// modbus result
uint8_t result;

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

    result = node.readInputRegisters(0x0000, 10);
    if (result == node.ku8MBSuccess)
    {
        voltage = (node.getResponseBuffer(0x00) / 10.0f); // V
        current = (node.getResponseBuffer(0x01) / 1000.000f); // A
        active_power = (node.getResponseBuffer(0x03) / 10.0f); // W
        active_energy = (node.getResponseBuffer(0x05)/1000.0f); // kWh
        frequency = (node.getResponseBuffer(0x07) / 10.0f); // Hz
        power_factor = (node.getResponseBuffer(0x08) / 100.0f);
    }
    
    Serial.print("Voltage: ");
    Serial.print(voltage_usage);
    Serial.print(" | Frequency: ");
    Serial.print(frequency);
    Serial.print(" | Current: ");
    Serial.print(current_usage);
    Serial.print(" | Power: ");
    Serial.print(active_power);
    Serial.print(" | Energy: ");
    Serial.print(active_energy,3);
    Serial.print(" | Power Factor: ");
    Serial.println(power_factor);

    DynamicJsonDocument doc(1024);
    doc["timestamp"] = timeClient.getEpochTime();
    doc["voltage"] = voltage;
    doc["current"] = current;
    doc["energy"] = active_energy;
    doc["power"] = active_power;
    doc["frequency"] = frequency;
    doc["powerFactor"] = power_factor;

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
