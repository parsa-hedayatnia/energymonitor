#ifndef __ENERGYMONITOR_CLIENT_HPP
#define __ENERGYMONITOR_CLIENT_HPP

#include <HTTPClient.h>

extern HTTPClient  *http;

void createClient();

int sendHttpPOSTrequest(WiFiClient client, const char* serverName, String httpRequestData);

#endif
