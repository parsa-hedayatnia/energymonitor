#ifndef __ENERGYMONITOR_CONFIG_PORTAL_HPP
#define __ENERGYMONITOR_CONFIG_PORTAL_HPP

#include <WString.h>

struct ConfigPortalParameters
{
  int operationMode;
  String ssid;
  String password;
  String token;
};

ConfigPortalParameters startConfigPoral();

#endif
