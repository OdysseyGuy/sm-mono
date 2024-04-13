//----------------------------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WifiClientSecure.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>
#include <SimpleTimer.h>

//----------------------------------------------------------------------------------
// Build Configuration

#define RESET_ENERGY_AT_STARTUP 0

//----------------------------------------------------------------------------------

SoftwareSerial pzemSerial(D5, D6);
PZEM004Tv30 pzem(pzemSerial);

WiFiClient wifiClient;
PubSubClient pubsubClient(wifiClient);

WiFiUDP wifiUdp;
NTPClient timeClient(wifiUdp, "pool.ntp.org");

//----------------------------------------------------------------------------------
// wifi
// WiFi credentials.

const char *ssid = "Rishi";
const char *password = "5688b6d56682";

//----------------------------------------------------------------------------------
// mqtt
// MQTT broker based detials.

const char *mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

const char *sensorId = "asfas3242d2";
const char *mqtt_pub_topic = "meter/asfas3242d2";

//----------------------------------------------------------------------------------
// Timer

SimpleTimer timer(1000); // run every 1 sec

//----------------------------------------------------------------------------------
// Electrical Parameters

float voltage, current, power, energy, frequency, pf;

//----------------------------------------------------------------------------------
// JSON file for MQTT

DynamicJsonDocument doc(1024);

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
        //        Serial.println("Message publised [" + String(topic) + "]: " + payload);
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
    Serial.println("Start PZEM serial");

    // we are going to subscribe to the limit set topic
    pubsubClient.setCallback(callback);
    pubsubClient.setServer(mqtt_server, mqtt_port);

    // initialize the time clinet and set it to IST
    timeClient.begin();
    timeClient.setTimeOffset(19800); // IST time offset in seconds

#if RESET_ENERGY_AT_STARTUP
    pzem.resetEnergy();
#endif

    // lets query the limit once at startup
}

//----------------------------------------------------------------------------------
// loop
// This code will run infinitely until the code is switched off.

void loop()
{
    // make sure we are connected to the MQTT broker
    if (!pubsubClient.connected())
    {
        reconnect();
    }

    pubsubClient.loop();
    timeClient.update();

    voltage = pzem.voltage();
    current = pzem.current();
    power = pzem.power();
    energy = pzem.energy();
    frequency = pzem.frequency();
    pf = pzem.pf();

    if (isnan(voltage))
    {
        Serial.println("Error reading voltage");
    }
    else if (isnan(current))
    {
        Serial.println("Error reading current");
    }
    else if (isnan(power))
    {
        Serial.println("Error reading power");
    }
    else if (isnan(energy))
    {
        Serial.println("Error reading energy");
    }
    else if (isnan(frequency))
    {
        Serial.println("Error reading frequency");
    }
    else if (isnan(pf))
    {
        Serial.println("Error reading power factor");
    }
    else
    {
        Serial.print("Voltage: ");
        Serial.print(voltage);
        Serial.print("V");
        Serial.print(" | Current: ");
        Serial.print(current);
        Serial.print("A");
        Serial.print(" | Power: ");
        Serial.print(power);
        Serial.print("W");
        Serial.print(" | Energy: ");
        Serial.print(energy, 3);
        Serial.print("kWh");
        Serial.print(" | Frequency: ");
        Serial.print(frequency, 1);
        Serial.print("Hz");
        Serial.print(" | PF: ");
        Serial.println(pf);
    }

    doc["timestamp"] = timeClient.getEpochTime();
    doc["voltage"] = voltage;
    doc["current"] = current;
    doc["energy"] = energy;
    doc["power"] = power;
    doc["frequency"] = frequency;
    doc["powerFactor"] = pf;

    char mqtt_message[256];
    serializeJson(doc, mqtt_message);

    if (timer.isReady())
    {
        publishMessage(mqtt_pub_topic, mqtt_message, true);
        timer.reset();
    }

    delay(50);
}

//----------------------------------------------------------------------------------
