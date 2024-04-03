#include <DS3231.h>
#include <PZEM004Tv30.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <NTPClient.h>
#include <SoftwareSerial.h>
#include <WiFiUdp.h>
#include <MqttClient.h>

#define DEBUG_PRINT(x) \
  Serial.print(x);\
  lcd.print(x);

#define DEBUG_PRINTLN(x) \
  Serial.println(x);\
  lcd.print(x);

#define WIFI_RX 2
#define WIFI_TX 3

SoftwareSerial esp8266Serial(WIFI_RX, WIFI_TX);
uint8_t lastMonth;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DS3231 rtc(SDA, SCL);

SoftwareSerial pzemSerial(2, 3);
PZEM004Tv30 pzem(2, 3);

Time lastResetTime;

// ist offset
const long utcTimeOffset = 19800;
WiFiUdp wifiUdp;
NTPClient timeClient(wifiUdp, "pool.ntp.org", utcTimeOffset);

MqttClient mqttClient(wifiUdp);
const char broker[] = "test.mosquitto.org";
int port = 1883;

// certificate to connect to mqtt broker
const char *cert = "";

// interval in milliseconds
const long mqttInterval = 1000;

const char* SSID = "WIFI_NAME";
const char* PASS = "WIFI_PASSWORD";

void setup_wifi() {
  // set the wifi client
  DEBUG_PRINT("Connecting to ");
  DEBUG_PRINT(SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
 
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    DEBUG_PRINT(++i);
    DEBUG_PRINTLN('');
  }

  DEBUG_PRINTLN('Connection Established!');
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());
}

void connectToMqtt() {
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  pzemSerial.begin(9600);

  // initialize the lcd module
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  
  setup_wifi();

  // get time from the ntp client
  timeClient.begin();
  int hours = timeClient.getHours();
  int mins = timeClient.getMinutes();
  int secs = timeClient.getSeconds();
  int date = timeClient.getDay();

  // initialize the rtc clock
  rtc.begin();
  rtc.setTime(hours, mins, secs);
  rtc.setDate(27, 1, 2024);
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // current timestamp unix
  long timestamp = rtc.getUnixTIme();

  // electrical parameters
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();

  // get the time slapsed in milliseconds
  unsigned long currentMillis = millis();
  // we need to upload the value every one second
  if (currentMillis - previousMillis >= mqttInterval) {
    mqttClient.beginMessage(topic);
    mqttClient.print("{energy:")
    mqttClient.print(energy);
    mqttClient.print(",timestamp:")
    mqttClient.print(timestamp);
    mqttClient.print("}")
    mqttClient.endMessage();
  }

  // update the lcd
  lcd.setCursor(0, 0);
  lcd.print("V/I: ");
  lcd.print(voltage);
  lcd.print("V / ");
  lcd.print(current);
  lcd.print("A");

  lcd.setCursor(0, 1);
  lcd.print("f: ");
  lcd.print(frequency);
  lcd.print("Hz E: ");
  lcd.print(energy);
  lcd.print("units");

  delay(400);
}
