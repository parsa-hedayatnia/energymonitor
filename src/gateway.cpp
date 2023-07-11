#include "gateway.hpp"
// #include "calculate.hpp"
#include "client.hpp"
#include "constants.hpp"
#include "server.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <SPIFFS.h>
#include <sqlite3.h>
// #include "constants.hpp"

extern String token;
int pckt_cnt = 0;
#define sendInterval 3600000
int lastSend = 0;

String nodes_data[100];
StaticJsonDocument<512> docs[60];

void onToken(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  String temp = String((char *)data);
  DynamicJsonDocument doc(512);
  deserializeJson(doc, temp);
  token = doc["token"].as<String>();
  debugln(token);
  request->send(200);
}

void action(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  debugln("post received");
  // debugln(String((char*) data));
  // NVS.putString(addrSP_ApiToken ,SM_ApiToken);
  request->send(200);
  String temp = String((char *)data);
  deserializeJson(docs[pckt_cnt], temp);
  pckt_cnt++;
}

void Gateway_Init(void)
{
  // nodes_data = new String[100];
  server->on(
      "/token",
      HTTP_POST,
      [](AsyncWebServerRequest *request) {},
      NULL,
      onToken);
  server->on(
      "/publish",
      HTTP_POST,
      [](AsyncWebServerRequest *request) {},
      NULL,
      action);
  server->begin();

  setupHttpsClient();
}

void Gateway_Loop(void)
{
  if ((pckt_cnt >= 50 || (millis() - lastSend >= sendInterval)) && pckt_cnt > 0)
  {
    debugln("sending to server");
    // String sendData="{\"data\":[";
    int status = -1;
    while (status != 201)
    {
      DynamicJsonDocument doc(30 * 1024);
      JsonArray data = doc.createNestedArray("data");
      for (int i = 0; i < pckt_cnt; i++)
      {
        // sendData = sendData + nodes_data[i] + ",";
        data.add(docs[i]);
      }
      // sendData = sendData + nodes_data[pckt_cnt-1] + "]}";
      // sendHttpsPOSTrequest(sendData);
      String sendData;
      // debugln(sendData);
      serializeJson(doc, sendData);
      // if (sendHttpsPOSTrequest(sendData) == 0)
      // {
      //   sendHttpPOSTrequest("http://172.20.88.192:8080/consumption/mode-4", sendData, true);
      // }
      status = sendHttpPOSTrequest("http://172.20.88.192:8080/consumption/mode-4", sendData, true);
    }
    pckt_cnt = 0;
    lastSend = millis();
  }
  delay(1000);
}
