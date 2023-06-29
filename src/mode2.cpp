#include "mode2.hpp"
#include "calculate.hpp"
#include "constants.hpp"
#include "server.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <sqlite3.h>
#include <SPIFFS.h>

#define SAMPLE_PERIOD 5000

extern Preferences NVS;

sqlite3 *db1;
int rc;
int id = 0;

#define FORMAT_SPIFFS_IF_FAILED true

int db_open(const char *filename, sqlite3 **db)
{
  int rc = sqlite3_open(filename, db);
  if (rc)
  {
    Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
    return rc;
  }
  else
  {
    Serial.printf("Opened database successfully\n");
  }
  return rc;
}

double tmpEnergy;
double tmpVoltage;
double tmpCurrent;
double tmpThdVoltage;
double tmpThdCurrent;
double tmpPf;

const char *data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName)
{
  int i;
  // Serial.printf("%s: ", (char *)data);
  for (i = 0; i < argc; i++)
  {
    Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }

  if (argc == 6)
  {
    tmpEnergy = atof(argv[0]);
    tmpVoltage = atof(argv[1]);
    tmpCurrent = atof(argv[2]);
    tmpThdVoltage = atof(argv[3]);
    tmpThdCurrent = atof(argv[4]);
    tmpPf = atof(argv[5]);
  }
  Serial.printf("\n");
  return 0;
}

char *zErrMsg = 0;
int db_exec(sqlite3 *db, const char *sql)
{
  debugln(sql);
  // long start = micros();
  int rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
  if (rc != SQLITE_OK)
  {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else
  {
    Serial.printf("Operation done successfully\n");
  }
  // debug(F("Time taken:"));
  // debugln(micros() - start);
  return rc;
}

void printSPIFFSInfo()
{
  debugln("===== File system info =====");

  uint totalBytes = SPIFFS.totalBytes();
  uint usedBytes = SPIFFS.usedBytes();
  debug("Total space:      ");
  debug(totalBytes);
  debugln("byte");

  debug("Total space used: ");
  debug(usedBytes);
  debugln("byte");
  debug("usage: ");
  debugln((usedBytes * 100) / totalBytes);

  debugln("===== File system info =====");
}

void dbInit()
{
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    debugln("Failed to mount file system");
    while (1)
      ;
    return;
  }
  else
  {
    debugln("mounted SPIFFS successfully");
  }
  // SPIFFS.format();
  printSPIFFSInfo();
  // list SPIFFS contents
  // File root = SPIFFS.open("/");
  // if (!root)
  // {
  //   debugln("- failed to open directory");
  //   return;
  // }
  // if (!root.isDirectory())
  // {
  //   debugln(" - not a directory");
  //   return;
  // }
  // File file = root.openNextFile();
  // while (file)
  // {
  //   if (file.isDirectory())
  //   {
  //     debug("  DIR : ");
  //     debugln(file.name());
  //   }
  //   else
  //   {
  //     debug("  FILE: ");
  //     debug(file.name());
  //     debug("\tSIZE: ");
  //     debugln(file.size());
  //   }
  //   file = root.openNextFile();
  // }

  // remove existing file
  // SPIFFS.remove("/test1.db");
  printSPIFFSInfo();

  sqlite3_initialize();

  if (db_open("/spiffs/test1.db", &db1))
    return;

  rc = db_exec(db1, "CREATE TABLE IF NOT EXISTS test1 (energy REAL, voltage REAL,current REAL, tvoltage REAL, tcurrent REAL, pf REAL);");
  if (rc != SQLITE_OK)
  {
    sqlite3_close(db1);
    return;
  }
}

void mode2OnData(AsyncWebServerRequest *request)
{
  // read from flash and send
  char tmpstr[200];
  sprintf(tmpstr, "SELECT * FROM test1 WHERE rowid=%ld;", sqlite3_last_insert_rowid(db1));
  rc = db_exec(db1, tmpstr);
  if (rc != SQLITE_OK)
  {
    sqlite3_close(db1);
    return;
  }

  DynamicJsonDocument doc(1024);
  doc["mode"] = "mode1";
  doc["energy"] = tmpEnergy;
  doc["mac"] = WiFi.macAddress();
  doc["voltage"] = tmpVoltage;
  doc["current"] = tmpCurrent;
  doc["pf"] = tmpPf;
  doc["thdVoltage"] = tmpThdVoltage;
  doc["thdCurrent"] = tmpThdCurrent;

  AsyncResponseStream *response = request->beginResponseStream("application/json");
  serializeJson(doc, *response);
  request->send(response);
}

void Mode2_Init(void)
{
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(200, "text/plain", "hello"); });
  server->on("/data", HTTP_GET, mode2OnData);
  server->onNotFound([](AsyncWebServerRequest *request)
                     { request->send(404, "text/plain", "Not found"); });
  server->begin();
  debugln("started mode 2 server");
  dbInit();
}

void saveToFlash()
{
  char tmpstr[200];
  sprintf(tmpstr, "INSERT INTO test1 (energy, voltage, current, tvoltage, tcurrent, pf) VALUES (%lf, %lf, %lf, %lf, %lf, %lf);",
          getEnergy(), getVoltage(), getCurrent(), getThdVoltage(), getThdCurrent(), getPF());
  rc = db_exec(db1, tmpstr);
  if (rc != SQLITE_OK)
  {
    debugln("falied to exec");
    sqlite3_close(db1);
    return;
  }
}

void Mode2_Loop(void)
{
  delay(SAMPLE_PERIOD);
  debugln("[A]: Start Calculating.");
  calculateANDwritenergy();
  // save to flash
  saveToFlash();
}
