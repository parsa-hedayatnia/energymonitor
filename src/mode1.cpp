#include "mode1.hpp"
#include "calculate.hpp"
#include "constants.hpp"
#include "server.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

#define SAMPLE_PERIOD 5000

extern Preferences NVS;

void onData(AsyncWebServerRequest *request)
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

  AsyncResponseStream *response = request->beginResponseStream("application/json");
  serializeJson(doc, *response);
  request->send(response);
}

void Mode1_Init(void)
{
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(200, "text/plain", "hello"); });
  server->on("/data", HTTP_GET, onData);
  server->onNotFound([](AsyncWebServerRequest *request)
                     { request->send(404, "text/plain", "Not found"); });
  server->begin();
  debugln("started mode 1 server");
}

void Mode1_Loop(void)
{
  delay(SAMPLE_PERIOD);
  debugln("[A]: Start Calculating.");
  calculateANDwritenergy();
}
