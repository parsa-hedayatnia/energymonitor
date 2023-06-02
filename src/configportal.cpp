#include "configportal.hpp"
#include "constants.hpp"
#include "pages.hpp"
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>

bool done = false;
ConfigPoralParameters parameters;

void onRoot(AsyncWebServerRequest *request)
{
  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->print(Pages::configPage);
  request->send(response);
}

void onSetSettings(AsyncWebServerRequest *request)
{
  if (!request->hasParam("selectedMode"))
  {
    return;
  }

  parameters.operationMode = request->getParam("selectedMode")->value().toInt();

  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->print(Pages::configDone);
  request->send(response);
  done = true;

  delay(1000);
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
}

ConfigPoralParameters startConfigPoral()
{
  done = false;

  Serial.println("Setting up access point");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(Constants::StationSSID, Constants::StationPassword);

  if (MDNS.begin(Constants::ConfigPoralAddress))
  {
    Serial.println("MDNS responder started");
  }

  AsyncWebServer server(80);
  server.on("/", HTTP_GET, onRoot);
  server.on("/settings", HTTP_GET, onSetSettings);
  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404, "text/plain", "Not found"); });
  server.begin();
  Serial.printf("Go to http://%s, I'll wait here", Constants::ConfigPoralAddress);

  while (!done)
  {
    delay(1000);
  }

  server.end();

  return parameters;
}
