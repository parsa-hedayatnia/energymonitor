#include "client.hpp"
#include "constants.hpp"

extern String token;

const char *ca_cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDNTCCAh0CFAubIsNyuv42gJlHM7rQyUmNmauVMA0GCSqGSIb3DQEBCwUAMFcx\n"
    "CzAJBgNVBAYTAklSMRMwEQYDVQQIDApTb21lLVN0YXRlMRAwDgYDVQQHDAdNYXNo\n"
    "aGFkMSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMjMwNjIz\n"
    "MTY1NjU4WhcNMjQwNjIyMTY1NjU4WjBXMQswCQYDVQQGEwJJUjETMBEGA1UECAwK\n"
    "U29tZS1TdGF0ZTEQMA4GA1UEBwwHTWFzaGhhZDEhMB8GA1UECgwYSW50ZXJuZXQg\n"
    "V2lkZ2l0cyBQdHkgTHRkMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA\n"
    "lU/24YTABAgFpiVE+4tqakat0OMVCLgUN9794IKBZk3JusrsX4QwjdjhOoqQNPTv\n"
    "9E42SoaI1Vvf4mPVtj44V5WLgAU2YHwaJzCm7nXvKqjWQ3M5OGYOVPe0FBR5TP80\n"
    "sqLRD0UbjWncC+tqDnmmrJrdmcjC4NkUNBtfFkrv/5gRRGhqJDvm+yxwN1GwNiP9\n"
    "HojtV2HsTcBd6W08DtC8p1SfUDZLASBFRUHk40AU6U5rXtgLFQhcX1evIpuMyGQX\n"
    "OpxHdBJKs6i3LqKL556Qmhf47ONnv1lYfg/rmB5gInbtiJ1ALvJKqUpPzNMu5A8I\n"
    "T16k7gEIWMSzCwf1VvjrBQIDAQABMA0GCSqGSIb3DQEBCwUAA4IBAQBdBt5V4plF\n"
    "N11I5dR/XQfqRAZBQPtq6hutJwg4pqqjp6cF7DhwOqhoLZe6nHKkRwN37mGKW8AI\n"
    "BJJgzuksRXmZb8Oy3bL+jAFpiUn/RjjZYY7uCdohSTtQoDQvbRx2K2p1Cugg/XK7\n"
    "ecnnj1pmCexy5z5H8LEaHJumjvkQ3VF1wQMeoONYltXifiJDMNGkWzvDtGNrHxBz\n"
    "jFFkFn9/kf0S5f7nA+z+SdxeFVuSW6bXTULybAq0NfBox48Y529+tG+WJHF5aDxv\n"
    "dFSPhXFsKtSK9ZG6wAMeLh861C2OB+M0XVsE1pujVupHfEm1QcwgYhbn9T+nO9p5\n"
    "5qNr4uJvhG+c\n"
    "-----END CERTIFICATE-----";

HTTPClient *http;

const char *hostAddr = "5.160.40.125";
const int port = 443;
WiFiClientSecure clientSecure;

void createClient()
{
  http = new HTTPClient();
}

int sendHttpPOSTrequest(const char *url, String httpRequestData, bool isAuthorized)
{
  debugln(httpRequestData);
  http->begin(url);
  http->addHeader("Content-Type", "application/json");
  if (isAuthorized)
  {
    http->addHeader("Authorization", "Bearer " + token);
  }
  int httpResponseCode = http->POST(httpRequestData);
  debugln(httpResponseCode);
  debugln(http->getString());
  http->end();

  return httpResponseCode;
}

void setupHttpsClient()
{
  clientSecure.setCACert(ca_cert);
  delay(2000);
}

int sendHttpsPOSTrequest(String body)
{
  int conn = clientSecure.connect(hostAddr, port);
  if (conn == 1)
  {
    int body_len = body.length();
    Serial.println();
    Serial.print("Sending Parameters...");
    // Request
    clientSecure.println("POST https://5.160.40.125/api/consumption/mode-4 HTTP/1.1");
    // Headers
    clientSecure.print("Host: ");
    clientSecure.println(hostAddr);
    clientSecure.println("Content-Type: application/json");
    clientSecure.print("Content-Length: ");
    clientSecure.println(body_len);
    clientSecure.print("Authorization: Bearer ");
    clientSecure.println(token);
    clientSecure.println("Connection: Close");
    clientSecure.println();
    // Body
    clientSecure.println(body);
    clientSecure.println();

    // Wait for server response
    while (clientSecure.available() == 0)
      ;

    // Print Server Response
    while (clientSecure.available())
    {
      char c = clientSecure.read();
      Serial.write(c);
    }
  }
  else
  {
    clientSecure.stop();
    Serial.println("Connection Failed");
  }
  //  delay(5000);
  return 0;
}
