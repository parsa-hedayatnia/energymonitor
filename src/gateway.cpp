#include "gateway.hpp"
// #include "calculate.hpp"
#include "constants.hpp"
#include "client.hpp"
#include "server.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <sqlite3.h>
#include <SPIFFS.h>
// #include "constants.hpp"

int pckt_cnt=0;
#define sendInterval 10000
int lastSend=0;

String *nodes_data;
String sendData="";

void action(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
  debugln(String((char*) data));
  // NVS.putString(addrSP_ApiToken ,SM_ApiToken);
  request->send(200);
  nodes_data[pckt_cnt++] = String((char*) data);
}

void Gateway_Init(void)
{   
    nodes_data = new String[100];
    server->on(
              "/publish",
              HTTP_POST,
              [](AsyncWebServerRequest * request){},
              NULL,
              action);
  server->begin();

  setupHttpsClient();
}

void Gateway_Loop(void)
{
  if(pckt_cnt>=50||(millis()-lastSend>=sendInterval))
  {
    sendData="[";
    for(int i=0;i<pckt_cnt-1;i++)
    {
      sendData = sendData + nodes_data[i] + ",";
    }
    sendData = sendData + nodes_data[pckt_cnt-1] + "]";
    sendHttpsPOSTrequest(sendData);
    pckt_cnt = 0;
    lastSend = millis();
  }
  delay(1000);
}
