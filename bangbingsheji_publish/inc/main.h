#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <BH1750.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

void setup();
void loop();
void setupWiFi();
void reconnectMqtt();
void handleMqtt(char *topic, byte *payload, unsigned int length);
void parseCommand(const String &cmd);
void readSensors();
void updateDisplay();
void publishData();
void controlDevices();

#endif