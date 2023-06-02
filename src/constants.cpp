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
  const char *ConfigStationSSID = "ConfigMePlease";
  const char *StationSSID = "SEM";
  const char *StationPassword = "12345678";
  const char *ConfigPoralAddress = "sem";
  const char *addrSelectedBoard = "AdSelectedBoard";
  const int inPinCurrent = 35;
  const int inPinVoltage = 34;
  const int PIN_LED = 14;
  const int SM_TRIGGER_PIN = 0;
}
