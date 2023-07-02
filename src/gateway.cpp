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

#include "constants.hpp"

void action(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
  debugln(String((char*) data));
  // NVS.putString(addrSP_ApiToken ,SM_ApiToken);
  request->send(200);
}

void Gateway_Init(void)
{   
    server->on(
              "/publish",
              HTTP_POST,
              [](AsyncWebServerRequest * request){},
              NULL,
              action);
  server->begin();
}

void Gateway_Loop(void)
{

}
