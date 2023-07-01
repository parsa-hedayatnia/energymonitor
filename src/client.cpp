#include "client.hpp"

HTTPClient *http;

void createClient() {
  http = new HTTPClient();
}

int Send(WiFiClient client, String httpRequestData, const char* serverName)
{
    http->begin(client, serverName);
    http->addHeader("Content-Type", "application/json");
    return http->POST(httpRequestData);
}
