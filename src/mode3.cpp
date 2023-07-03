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
  JsonArray data = doc.createNestedArray("data");
  StaticJsonDocument<192> doc2;
  doc2["consumption"] = getEnergy();
  doc2["voltage"] = getVoltage();
  doc2["current"] = getCurrent();
  doc2["THDv"] = getThdVoltage();
  doc2["THDi"] = getThdCurrent();
  data.add(doc2);

  doc["mode"] = "mode1";
  doc["mac"] = WiFi.macAddress();

  String output;
  serializeJson(doc, output);
  // send http post req
}

void Mode3_Init(void)
{
    
}

void Mode3_Loop(void)
{
  delay(SAMPLE_PERIOD);
  debugln("[A]: Start Calculating.");
  calculateANDwritenergy();
  sendDataToGW();
}
