#include "configportal.hpp"
#include "constants.hpp"
#include "pages.hpp"
#include "server.hpp"
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>

volatile bool done = false;
ConfigPortalParameters parameters = ConfigPortalParameters();

void onRoot(AsyncWebServerRequest *request)
{
  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->print(Pages::configPage);
  request->send(response);
}

void onSetSettings(AsyncWebServerRequest *request)
{
  if (!request->hasParam("selectedMode") || !request->hasParam("ssid") || !request->hasParam("password") || !request->hasParam("token"))
  {
    return;
  }

  parameters.operationMode = request->getParam("selectedMode")->value().toInt();
  parameters.isAP = parameters.operationMode == MODE1 || parameters.operationMode == MODE2;
  parameters.ssid = request->getParam("ssid")->value();
  parameters.password = request->getParam("password")->value();
  parameters.token = request->getParam("token")->value();

  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->print(Pages::configDone);
  request->send(response);
  done = true;
}

ConfigPortalParameters startConfigPoral()
{
  createServer();
  done = false;

  debugln("Setting up access point");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(Constants::ConfigStationSSID + String(WiFi.macAddress()), Constants::ConfigStationPassword);

  if (MDNS.begin(Constants::ConfigPortalAddress))
  {
    debugln("MDNS responder started");
  }

  server->on("/", HTTP_GET, onRoot);
  server->on("/settings", HTTP_GET, onSetSettings);
  server->onNotFound([](AsyncWebServerRequest *request)
                     { request->send(404, "text/plain", "Not found"); });
  server->begin();
  debugln("Go to http://sem.local, I'll wait");

  while (!done)
    ;

  server->reset();

  return parameters;
}
