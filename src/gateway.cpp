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

String nodes_data[100];
StaticJsonDocument<512> docs[60];

void action(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
  debugln("post received");
  // debugln(String((char*) data));
  // NVS.putString(addrSP_ApiToken ,SM_ApiToken);
  request->send(200);
  String temp = String((char*) data);
  deserializeJson(docs[pckt_cnt], temp);
  pckt_cnt++;
}

void Gateway_Init(void)
{   
    // nodes_data = new String[100];
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
  if((pckt_cnt>=50||(millis()-lastSend>=sendInterval)) && pckt_cnt > 0)
  {
    debugln("sending to server");
    // String sendData="{\"data\":[";
    DynamicJsonDocument doc(30 * 1024);
    JsonArray data = doc.createNestedArray("data");
    for(int i=0;i<pckt_cnt;i++)
    {
      // sendData = sendData + nodes_data[i] + ",";
      data.add(docs[i]);
    }
    // sendData = sendData + nodes_data[pckt_cnt-1] + "]}";
    // sendHttpsPOSTrequest(sendData);
    String sendData;
    serializeJson(doc, sendData);
    sendHttpPOSTrequest("http://5.160.40.125:8080/consumption/mode-4", sendData, true);
    pckt_cnt = 0;
    lastSend = millis();
  }
  delay(1000);
}
