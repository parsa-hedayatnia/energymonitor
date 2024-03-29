#include "calculate.hpp"
#include "client.hpp"
#include "configportal.hpp"
#include "constants.hpp"
#include "gateway.hpp"
#include "mode1.hpp"
#include "mode2.hpp"
#include "mode3.hpp"
#include "server.hpp"
#include <Arduino.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <WiFi.h>

Preferences NVS;
OpMode selectedMode;
String token;

void onWifiButtonInterrupt(void)
{
  debugln("Clear and Restart");
  NVS.clear();
  ESP.restart();
}

void setup()
{
  Serial.begin(115200);

  NVS.begin("SM1001", false);

  pinMode(Constants::LedPin, OUTPUT);
  digitalWrite(Constants::LedPin, LOW);
  pinMode(Constants::WifiButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(Constants::WifiButtonPin), onWifiButtonInterrupt, FALLING);

  if (NVS.isKey(Constants::SelectedBoardAddress))
  {
    selectedMode = (OpMode)NVS.getInt(Constants::SelectedBoardAddress);
    String ssid = NVS.getString(Constants::WifiSSIDAddress);
    String password = NVS.getString(Constants::WifiPasswordAddress);
    token = NVS.getString(Constants::TokenAddress);

    debugln(OpModeNames[selectedMode] + " Was Selected.");
    debugln("SSID: " + ssid + " | Password: " + password);
    debugln(WiFi.macAddress());

    if (selectedMode != GATEWAY)
    {
      pinMode(Constants::CurrentPin, INPUT);
      pinMode(Constants::VoltagePin, INPUT);
    }

    if (selectedMode == MODE1 || selectedMode == MODE2)
    {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
    }
    else if (selectedMode == MODE3)
    {
      WiFi.mode(WIFI_STA);
      WiFi.begin("SEMGW", "11223344");

      int i = 0;
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(1000);
        debug(++i);
        debug(' ');
      }
    }
    else
    {
      WiFi.mode(WIFI_AP_STA);
      WiFi.softAP("SEMGW", "11223344");
      WiFi.begin(ssid.c_str(), password.c_str());

      int i = 0;
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(1000);
        debug(++i);
        debug(' ');
      }

      debugln(WiFi.localIP());
    }

    if (MDNS.begin(selectedMode == GATEWAY ? Constants::GatewayMDNSAddress : Constants::MDNSAddress))
    {
      debugln("MDNS responder started");
    }

    if (selectedMode == MODE1 || selectedMode == MODE2 || selectedMode == GATEWAY)
    {
      createServer();
    }

    if (selectedMode == MODE3 || selectedMode == GATEWAY)
    {
      createClient();
    }

    switch (selectedMode)
    {
    case MODE1:
      Mode1_Init();
      break;
    case MODE2:
      Mode2_Init();
      break;
    case MODE3:
      Mode3_Init();
      break;
    case GATEWAY:
      Gateway_Init();
      break;
    default:
      break;
    }

    return;
  }

  ConfigPortalParameters params = startConfigPoral();
  debugln("Done config");
  debugln("Operation Mode: " + OpModeNames[params.operationMode]);

  NVS.putInt(Constants::SelectedBoardAddress, params.operationMode);
  NVS.putString(Constants::WifiSSIDAddress, params.ssid);
  NVS.putString(Constants::WifiPasswordAddress, params.password);
  NVS.putString(Constants::TokenAddress, params.token);

  ESP.restart();
}

void loop()
{
  switch (selectedMode)
  {
  case MODE1:
    Mode1_Loop();
    break;
  case MODE2:
    Mode2_Loop();
    break;
  case MODE3:
    Mode3_Loop();
    break;
  case GATEWAY:
    Gateway_Loop();
    break;
  default:
    break;
  }
}
