// ESP32 智能家居环境监测与控制系统示例程序
// 该文件基于Arduino框架设计，适配ESP32-WROOM-32E模块

#include "../inc/main.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <BH1750.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi and MQTT settings
const char *WIFI_SSID = "your_ssid";
const char *WIFI_PASSWORD = "your_password";
const char *MQTT_SERVER = "mqtt.example.com";
const int MQTT_PORT = 1883;
const char *MQTT_TOPIC_DATA = "home/env/data";
const char *MQTT_TOPIC_CMD = "home/env/cmd";

// Relay output pins
const int RELAY_FAN = 16;
const int RELAY_LIGHT = 17;
const int RELAY_PURIFIER = 18;

// Thresholds (可远程调整)
int thresholdTemp = 28;
int thresholdLight = 150;
int thresholdTVOC = 200;
int thresholdCO2 = 1000;

// Sensor and display objects
Adafruit_SHT31 sht30 = Adafruit_SHT31();
BH1750 bh1750;
Adafruit_SGP30 sgp30;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

float temperature = 0.0;
float humidity = 0.0;
uint16_t luxValue = 0;
uint16_t tvoc = 0;
uint16_t co2 = 0;

void setup()
{
    Serial.begin(115200);
    pinMode(RELAY_FAN, OUTPUT);
    pinMode(RELAY_LIGHT, OUTPUT);
    pinMode(RELAY_PURIFIER, OUTPUT);
    digitalWrite(RELAY_FAN, LOW);
    digitalWrite(RELAY_LIGHT, LOW);
    digitalWrite(RELAY_PURIFIER, LOW);

    Wire.begin();
    if (!sht30.begin(0x44))
    {
        Serial.println("SHT30 init failed");
    }
    if (!bh1750.begin())
    {
        Serial.println("BH1750 init failed");
    }
    if (!sgp30.begin())
    {
        Serial.println("SGP30 init failed");
    }
    else if (!sgp30.IAQinit())
    {
        Serial.println("SGP30 IAQ init failed");
    }

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("SSD1306 init failed");
    }
    display.clearDisplay();
    display.display();

    setupWiFi();
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(handleMqtt);
}

void loop()
{
    if (!mqttClient.connected())
    {
        reconnectMqtt();
    }
    mqttClient.loop();

    readSensors();
    updateDisplay();
    publishData();
    controlDevices();
    delay(2000);
}

void setupWiFi()
{
    Serial.printf("Connecting to WiFi %s...\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print('.');
    }
    Serial.println(" connected");
}

void reconnectMqtt()
{
    while (!mqttClient.connected())
    {
        if (mqttClient.connect("ESP32SmartHome"))
        {
            mqttClient.subscribe(MQTT_TOPIC_CMD);
            Serial.println("MQTT connected");
        }
        else
        {
            Serial.print("MQTT connect failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying...");
            delay(2000);
        }
    }
}

void handleMqtt(char *topic, byte *payload, unsigned int length)
{
    String msg;
    for (unsigned int i = 0; i < length; i++)
    {
        msg += (char)payload[i];
    }
    if (String(topic) == MQTT_TOPIC_CMD)
    {
        parseCommand(msg);
    }
}

void parseCommand(const String &cmd)
{
    if (cmd.startsWith("temp:"))
    {
        thresholdTemp = cmd.substring(5).toInt();
    }
    else if (cmd.startsWith("light:"))
    {
        thresholdLight = cmd.substring(6).toInt();
    }
    else if (cmd.startsWith("tvoc:"))
    {
        thresholdTVOC = cmd.substring(5).toInt();
    }
    else if (cmd.startsWith("co2:"))
    {
        thresholdCO2 = cmd.substring(4).toInt();
    }
    else if (cmd == "fan:on")
    {
        digitalWrite(RELAY_FAN, HIGH);
    }
    else if (cmd == "fan:off")
    {
        digitalWrite(RELAY_FAN, LOW);
    }
    else if (cmd == "light:on")
    {
        digitalWrite(RELAY_LIGHT, HIGH);
    }
    else if (cmd == "light:off")
    {
        digitalWrite(RELAY_LIGHT, LOW);
    }
    else if (cmd == "purifier:on")
    {
        digitalWrite(RELAY_PURIFIER, HIGH);
    }
    else if (cmd == "purifier:off")
    {
        digitalWrite(RELAY_PURIFIER, LOW);
    }
}

void readSensors()
{
    temperature = sht30.readTemperature();
    humidity = sht30.readHumidity();
    luxValue = bh1750.readLightLevel();
    if (!sgp30.IAQmeasure())
    {
        Serial.println("SGP30 measure failed");
    }
    tvoc = sgp30.TVOC;
    co2 = sgp30.eCO2;
}

void updateDisplay()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Temp: ");
    display.print(temperature, 1);
    display.println(" C");
    display.print("Hum: ");
    display.print(humidity, 1);
    display.println(" %");
    display.print("Lux: ");
    display.println(luxValue);
    display.print("TVOC: ");
    display.print(tvoc);
    display.println(" ppb");
    display.print("CO2: ");
    display.print(co2);
    display.println(" ppm");
    display.display();
}

void publishData()
{
    String payload = "{";
    payload += "\"temperature\":" + String(temperature, 1) + ",";
    payload += "\"humidity\":" + String(humidity, 1) + ",";
    payload += "\"lux\":" + String(luxValue) + ",";
    payload += "\"tvoc\":" + String(tvoc) + ",";
    payload += "\"co2\":" + String(co2) + "}";
    mqttClient.publish(MQTT_TOPIC_DATA, payload.c_str());
}

void controlDevices()
{
    bool fanOn = temperature > thresholdTemp;
    bool lightOn = luxValue < thresholdLight;
    bool purifierOn = tvoc > thresholdTVOC || co2 > thresholdCO2;

    digitalWrite(RELAY_FAN, fanOn ? HIGH : LOW);
    digitalWrite(RELAY_LIGHT, lightOn ? HIGH : LOW);
    digitalWrite(RELAY_PURIFIER, purifierOn ? HIGH : LOW);
}
