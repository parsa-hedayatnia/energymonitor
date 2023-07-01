#ifndef __ENERGYMONITOR_CLIENT_HPP
#define __ENERGYMONITOR_CLIENT_HPP

#include <HTTPClient.h>

extern HTTPClient  *http;

void createClient();

int Send(WiFiClient client, String httpRequestData );

#endif
