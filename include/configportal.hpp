#ifndef __ENERGYMONITOR_CONFIG_PORTAL_HPP
#define __ENERGYMONITOR_CONFIG_PORTAL_HPP

#include <WString.h>

struct ConfigPortalParameters
{
  int operationMode;
  bool isAP;
  String ssid;
  String password;
};

ConfigPortalParameters startConfigPoral();

#endif
