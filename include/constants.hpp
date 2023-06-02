#ifndef __ENERGYMONITOR_CONSTANTS_HPP
#define __ENERGYMONITOR_CONSTANTS_HPP

#include "Arduino.h"

enum OpMode
{
  MODE1,  // accumlative offline
  MODE2,  // time slice offline
  MODE3,  // send to gateway
  GATEWAY // act as gateway
};

extern String OpModeNames[];

namespace Constants
{
  extern const char *StationSSID;
  extern const char *StationPassword;
  extern const char *ConfigPoralAddress;
  extern const char *addrSelectedBoard;
  extern const int inPinCurrent;
  extern const int inPinVoltage;
  extern const int PIN_LED;
  extern const int SM_TRIGGER_PIN;
}

#endif
