#include "gateway.hpp"
// #include "calculate.hpp"
// #include "constants.hpp"
#include "server.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <sqlite3.h>
#include <SPIFFS.h>


void action(AsyncWebServerRequest *request) {
  Serial.println("ACTION!");

  int params = request->params();
  for (int i = 0; i < params; i++) {
    AsyncWebParameter* p = request->getParam(i);
    Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
  }
  request->send_P(200, "text/html", NULL);
}

void Gateway_Init(void)
{   
    server->on("/action", HTTP_POST, action);
}

void Gateway_Loop(void)
{
       
}
