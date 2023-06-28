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
  // read from flash and send
}

void Mode2_Init(void)
{
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "hello"); });
  server->on("/data", HTTP_GET, onData);
  server->onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404, "text/plain", "Not found"); });
  server->begin();
  debugln("started mode 2 server");
}

void Mode2_Loop(void)
{
  delay(SAMPLE_PERIOD);
  debugln("[A]: Start Calculating.");
  calculateANDwritenergy();
  // save to flash
}
