#ifndef __ENERGYMONITOR_CONSTANTS_HPP
#define __ENERGYMONITOR_CONSTANTS_HPP

#include "Arduino.h"

#define DEBUG

#ifdef DEBUG
#define debug_begin(x) Serial.begin(x); while (!Serial);
#define debugln(x) Serial.println(x)
#else
#define debug_begin(x)
#define debug(x)
#define debugln(x)
#endif

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
  // Pins
  extern const int CurrentPin;
  extern const int VoltagePin;
  extern const int LedPin;
  extern const int WifiButtonPin;

  // NVS Addresses
  extern const char *WifiSSIDAddress;
  extern const char *WifiPasswordAddress;
  extern const char *SelectedBoardAddress;
  extern const char *IsAPAddress;

  // Config Portal
  extern const char *ConfigStationSSID;
  extern const char *ConfigStationPassword;
  extern const char *ConfigPortalAddress;
}

#endif
