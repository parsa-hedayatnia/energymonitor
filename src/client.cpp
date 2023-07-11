#include "client.hpp"
#include "constants.hpp"

extern String token;

const char *ca_cert = \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIEIjCCAwqgAwIBAgISBJ5QgtpE+LtkceB3/aufPOYoMA0GCSqGSIb3DQEBCwUA\n" \
    "MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n" \
    "EwJSMzAeFw0yMzA3MDQwNTU2MjhaFw0yMzEwMDIwNTU2MjdaMBsxGTAXBgNVBAMT\n" \
    "EHNlbTIuc21hcnQtY28uaXIwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARqQ767\n" \
    "CeyfFtxqLihcVH5XhzQaDWJ6fY+LZCyJNpIRtZqLZriNhdTOvIV0APIQ19G6kHNf\n" \
    "P8rWCkdcUKDNPXafo4ICEjCCAg4wDgYDVR0PAQH/BAQDAgeAMB0GA1UdJQQWMBQG\n" \
    "CCsGAQUFBwMBBggrBgEFBQcDAjAMBgNVHRMBAf8EAjAAMB0GA1UdDgQWBBTx3x5W\n" \
    "OkkbylNFK0yaK8GPFgiUnTAfBgNVHSMEGDAWgBQULrMXt1hWy65QCUDmH6+dixTC\n" \
    "xjBVBggrBgEFBQcBAQRJMEcwIQYIKwYBBQUHMAGGFWh0dHA6Ly9yMy5vLmxlbmNy\n" \
    "Lm9yZzAiBggrBgEFBQcwAoYWaHR0cDovL3IzLmkubGVuY3Iub3JnLzAbBgNVHREE\n" \
    "FDASghBzZW0yLnNtYXJ0LWNvLmlyMBMGA1UdIAQMMAowCAYGZ4EMAQIBMIIBBAYK\n" \
    "KwYBBAHWeQIEAgSB9QSB8gDwAHYAejKMVNi3LbYg6jjgUh7phBZwMhOFTTvSK8E6\n" \
    "V6NS61IAAAGJH7AzTwAABAMARzBFAiAVtDqK3272za27rJ1GDHRYguKd0SpAo2fu\n" \
    "NSdhZlM08AIhAO6DRI8uVhLRA4B53n1g+lULAvxRs5gg4SlKWFirvqS1AHYArfe+\n" \
    "+nz/EMiLnT2cHj4YarRnKV3PsQwkyoWGNOvcgooAAAGJH7AzsgAABAMARzBFAiEA\n" \
    "/QDs/EFoAaH159i/ediJI5IPGFEqMNf13hp0kJeKchcCIGrEsZYCx7VaZP8s7jWK\n" \
    "yEKZ8nMGyJxaxtuX2VdSbEBHMA0GCSqGSIb3DQEBCwUAA4IBAQAEUpgeIsYD5nrd\n" \
    "jLzcKUBM/iA0xOtKtav1WX9CmCBapMzruZzl+3OdBej/8BqnC/CvdmeoLgx/+AkJ\n" \
    "hFjfveE0C940BroX2ogSNiderRzrftSb/meTDDMuy64yKg1p9FZELxxDTfpsx3gL\n" \
    "JxEjlfiF8an1AcwZK5MaTKr2AmpO/NzFetO1+MLKs6A4VtfoUA1kavmDhgT8u8Xy\n" \
    "R8yraS8B4dfEUM74ZzWI7dBi0Vih5mzNTaRdZQ5eWr0/o7P1Fk9Rg6mtM03iOvQV\n" \
    "MfbHNZdGxKsh2AXgrDgHCJGDB/QQHKpkyvmmPumBL8yB6Zri0HWI0s9hnA6dlwgS\n" \
    "9ddR22pW\n" \
    "-----END CERTIFICATE-----\n";

HTTPClient *http;

// const char *hostAddr = "sem2.smart-co.ir";
const char* hostAddr = "172.20.88.192";
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
    debugln(token);
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
  // clientSecure.setCACert(ca_cert);
  clientSecure.setInsecure();
  delay(2000);
}

int sendHttpsPOSTrequest(String body)
{
  int conn = clientSecure.connect(hostAddr, port);
  if (conn == 1)
  {
    int body_len = body.length();
    // Request
    clientSecure.println("POST /consumption/mode-4 HTTP/1.1");
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
      debug(c);
    }
    clientSecure.stop();
    return 1;
  }
  else
  {
    clientSecure.stop();
    debugln("Connection Failed");
  }
  //  delay(5000);
  return 0;
}
