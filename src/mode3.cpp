#include "mode3.hpp"
#include "calculate.hpp"
#include "client.hpp"
#include "constants.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <SPIFFS.h>
#include <sqlite3.h>

#define SAMPLE_PERIOD 5000

void sendDataToGW()
{
  DynamicJsonDocument doc(1024);
  doc["consumption"] = getEnergy();
  doc["voltage"] = getVoltage();
  doc["current"] = getCurrent();
  doc["THDv"] = getThdVoltage();
  doc["THDi"] = getThdCurrent();
  doc["mode"] = "mode3";
  String mac = WiFi.macAddress();
  mac.toLowerCase();
  doc["macAddress"] = mac;

  String output;
  serializeJson(doc, output);
  // send http post req
  sendHttpPOSTrequest("http://gateway.local/publish", output, false);
}

void Mode3_Init(void)
{
  createClient();
}

void Mode3_Loop(void)
{
  static unsigned long lastMillis = 0, lastMillis2 = 0, lastMillis3 = 0, lastMillis4 = 0;

  if (millis() - lastMillis > SAMPLE_PERIOD)
  {
    debugln("[A]: Start Calculating.");
    calculateANDwritenergy();
    lastMillis = millis();
    // saveToFlash();
  }
  // save to flash

  if (millis() - lastMillis2 > 3600000)
  {
    sendDataToGW();
    lastMillis2 = millis();
  }
}
