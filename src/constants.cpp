#include "constants.hpp"
#include "WiFi.h"

String GetLast6Mac(void)
{
  String BUFF = WiFi.macAddress();
  String OutPutData;

  for (int i = 9; i <= 16; i++)
  {
    OutPutData += BUFF[i];
  }

  return OutPutData;
}

String OpModeNames[] = {
    "Operation Mode 1",
    "Operation Mode 2",
    "Operation Mode 3",
    "Operation Mode Gateway",
};

namespace Constants
{
  // Pins
  const int CurrentPin = 35;
  const int VoltagePin = 34;
  const int LedPin = 14;
  const int WifiButtonPin = 0;

  // NVS Addresses
  const char *WifiSSIDAddress = "WifiSSID";
  const char *WifiPasswordAddress = "WifiPassword";
  const char *SelectedBoardAddress = "SelectedBoard";
  const char *TokenAddress = "Token";

  // Config Portal
  const char *ConfigStationSSID = "ConfigMePlease";
  const char *ConfigStationPassword = "12345678";
  const char *ConfigPortalAddress = "sem";

  // Others
  const char *MDNSAddress = "sem";
  const char *GatewayMDNSAddress = "gateway";
}
