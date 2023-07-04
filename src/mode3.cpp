#include "mode3.hpp"
#include "client.hpp"
#include <Arduino.h>
#include "constants.hpp"
#include "calculate.hpp"
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <sqlite3.h>
#include <SPIFFS.h>

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
  doc["macAddress"] = WiFi.macAddress();

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
  delay(SAMPLE_PERIOD);
  debugln("[A]: Start Calculating.");
  calculateANDwritenergy();
  sendDataToGW();
}
