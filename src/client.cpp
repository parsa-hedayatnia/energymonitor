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

void setupHttpsClient()
{
  https = new WiFiClientSecure();
  https.setCACert(test_root_ca);
}

int sendHttpsPOSTrequest(String requestBody)
{
  if (!client.connect(server, 443))
  {
    Serial.println("Connection failed!");
    return -1;
  }

client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + requestBody.length() + "\r\n\r\n" +
               requestBody + "\r\n");

               /* HTTPS read
                while (client.connected()) {
                  String line = client.readStringUntil('\n');
                  if (line == "\r") {
                    break;
                  }
                  Serial.println(line);
                 }

                  String line = client.readStringUntil('\n');
               */

}
