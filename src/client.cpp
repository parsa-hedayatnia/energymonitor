#include "client.hpp"
#include "constants.hpp"

HTTPClient *http;

void createClient() {
  http = new HTTPClient();
}

int sendHttpPOSTrequest(WiFiClient client, const char* serverName, String httpRequestData)
{
    http->begin(client, serverName);
    // http->addHeader("Content-Type", "application/json");
    http->addHeader("Content-Type", "text/plain");
    int httpResponseCode = http->POST(httpRequestData);
    debugln(httpResponseCode);
    debugln(http->getString());
    http->end();
    return httpResponseCode;
}
