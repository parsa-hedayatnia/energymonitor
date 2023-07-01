#include "calculate.hpp"
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
  debug_begin(115200);

  NVS.begin("SM1001", false);

  pinMode(Constants::LedPin, OUTPUT);
  digitalWrite(Constants::LedPin, LOW);
  pinMode(Constants::WifiButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(Constants::WifiButtonPin), onWifiButtonInterrupt, FALLING);

  if (NVS.isKey(Constants::SelectedBoardAddress))
  {
    selectedMode = (OpMode)NVS.getInt(Constants::SelectedBoardAddress);
    bool isAP = NVS.getBool(Constants::IsAPAddress);
    String ssid = NVS.getString(Constants::WifiSSIDAddress);
    String password = NVS.getString(Constants::WifiPasswordAddress);
    token = NVS.getString(Constants::TokenAddress);

    debugln(OpModeNames[selectedMode] + " Was Selected.");
    debugln("SSID: " + ssid + " | Password: " + password);

    if (selectedMode != GATEWAY)
    {
      pinMode(Constants::CurrentPin, INPUT);
      pinMode(Constants::VoltagePin, INPUT);
    }

    if (isAP)
    {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
    }
    else
    {
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(), password.c_str());

      int i = 0;
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(1000);
        Serial.print(++i);
        Serial.print(' ');
      }
    }

    if (MDNS.begin(selectedMode == GATEWAY ? Constants::GatewayMDNSAddress : Constants::MDNSAddress))
    {
      debugln("MDNS responder started");
    }

    if (selectedMode == MODE1 || selectedMode == MODE2)
    {
      createServer();
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
  NVS.putBool(Constants::IsAPAddress, params.isAP);
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
