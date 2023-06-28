#include "server.hpp"

AsyncWebServer *server;

void createServer() {
  server = new AsyncWebServer(80);
}
