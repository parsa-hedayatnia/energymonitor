#include "mode1.hpp"
#include "calculate.hpp"
#include "constants.hpp"
#include "server.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#define TIMER_INTERVAL_MS 5000

ESP32Timer ITimer(0);

// boolean SM_Pressed = false;

// void SM_WiFiInterrupt(void)
// {
//   if (!SM_Pressed)
//   {
//     SM_Pressed = true;
//     SM_WiFiInterruptFlag = true;
//   }
// }

bool IRAM_ATTR SM_TimerInterrupt(void *)
{
  CalculateFlag = true;
}

void onData(AsyncWebServerRequest *request)
{
  DynamicJsonDocument doc(1024);
  doc["mode"] = "mode1";
  doc["energy"] = getEnergy();
  doc["mac"] = WiFi.macAddress();

  AsyncResponseStream *response = request->beginResponseStream("application/json");
  serializeJson(doc, *response);
  request->send(response);
}

void Mode1_PinInit(void)
{
  pinMode(Constants::inPinCurrent, INPUT);
  pinMode(Constants::inPinVoltage, INPUT);

  pinMode(Constants::PIN_LED, OUTPUT);
  digitalWrite(Constants::PIN_LED, LOW);

  // pinMode(Constants::SM_TRIGGER_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(Constants::SM_TRIGGER_PIN), Constants::SM_WiFiInterrupt, FALLING);
}

void Mode1_TimerInit(void)
{
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, SM_TimerInterrupt))
    Serial.println("[A]: Starting  ITimer0 OK");
  else
    Serial.println("[A]: Can't set ITimer0. Select another freq. or timer");
}

void Mode1_Init(void)
{
  Mode1_PinInit();

  Serial.println("Setting up access point");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(Constants::StationSSID, Constants::StationPassword);

  if (MDNS.begin(Constants::ConfigPoralAddress))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "hello"); });
  server.on("/data", HTTP_GET, onData);
  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404, "text/plain", "Not found"); });
  server.begin();
  Serial.printf("started mode 1 server");
}
