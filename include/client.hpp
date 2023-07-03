#ifndef __ENERGYMONITOR_CLIENT_HPP
#define __ENERGYMONITOR_CLIENT_HPP

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

extern HTTPClient  *http;
extern WiFiClientSecure clientSecure;

void createClient();
void setupHttpsClient();

int sendHttpPOSTrequest(WiFiClient client, const char* serverName, String httpRequestData);
int sendHttpsPOSTrequest(String body);
#endif
