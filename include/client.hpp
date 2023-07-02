#ifndef __ENERGYMONITOR_CLIENT_HPP
#define __ENERGYMONITOR_CLIENT_HPP

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

extern HTTPClient  *http;
extern WiFiClientSecure *https;

void createClient();
void setupHttpsClient();

int sendHttpPOSTrequest(WiFiClient client, const char* serverName, String httpRequestData);
int sendHttpsPOSTrequest(String requestBody, const char* server, String url, String host);
#endif
