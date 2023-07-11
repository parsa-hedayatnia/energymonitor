#include "mode2.hpp"
#include "calculate.hpp"
#include "constants.hpp"
#include "server.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <SPIFFS.h>
#include <sqlite3.h>

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

long long tmpRowid[100];
double tmpEnergy[100];
double tmpVoltage[100];
double tmpCurrent[100];
double tmpThdVoltage[100];
double tmpThdCurrent[100];
double tmpPf[100];
int tmpIdx = 0;
long long readOffset = 0;
int clbCount = 0;

const char *data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName)
{
  int i;
  // Serial.printf("%s: ", (char *)data);
  // debugln(argc);
  for (i = 0; i < argc; i++)
  {
    Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }

  if ((argc == 7) && (tmpIdx < 100))
  {
    tmpRowid[tmpIdx] = atoll(argv[0]);
    tmpEnergy[tmpIdx] = atof(argv[1]);
    tmpVoltage[tmpIdx] = atof(argv[2]);
    tmpCurrent[tmpIdx] = atof(argv[3]);
    tmpThdVoltage[tmpIdx] = atof(argv[4]);
    tmpThdCurrent[tmpIdx] = atof(argv[5]);
    tmpPf[tmpIdx] = atof(argv[6]);
    tmpIdx++;
    clbCount++;
  }

  // Serial.printf("\n");
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
  tmpIdx = 0;
  sqlite3_close(db1);
  if (db_open("/spiffs/test1.db", &db1))
    return;
  clbCount = 0;
  sprintf(tmpstr, "SELECT rowid, energy, voltage, current, tvoltage, tcurrent, pf FROM test1 ORDER BY rowid ASC LIMIT 100 OFFSET %ld;", readOffset); // WHERE rowid=%ld;", sqlite3_last_insert_rowid(db1));
  rc = db_exec(db1, tmpstr);
  if (rc != SQLITE_OK)
  {
    sqlite3_close(db1);
    return;
  }
  readOffset += clbCount;
  if (clbCount < 100)
  {
    rc = db_exec(db1, "DELETE FROM test1;");
    if (rc != SQLITE_OK)
    {
      sqlite3_close(db1);
      return;
    }
    readOffset = 0;
  }

  DynamicJsonDocument doc(12288);
  JsonArray data = doc.createNestedArray("data");

  for (size_t i = 0; i < tmpIdx; i++)
  {
    StaticJsonDocument<192> doc2;
    doc2["consumption"] = tmpEnergy[i];
    doc2["voltage"] = tmpVoltage[i];
    doc2["current"] = tmpCurrent[i];
    doc2["THDv"] = tmpThdVoltage[i];
    doc2["THDi"] = tmpThdCurrent[i];
    doc2["mode"] = "mode2";
    String mac = WiFi.macAddress();
    mac.toLowerCase();
    doc2["macAddress"] = mac;
    data.add(doc2);
  }

  AsyncResponseStream *response = request->beginResponseStream("application/json");
  serializeJson(doc, *response);
  // serializeJsonPretty(doc, Serial);
  request->send(response);

  // sprintf(tmpstr, "DELETE FROM test1 WHERE rowid IN (SELECT rowid FROM test1 ORDER BY rowid ASC LIMIT 100);"); // WHERE rowid=%ld;", sqlite3_last_insert_rowid(db1));
  // rc = db_exec(db1, tmpstr);
  // if (rc != SQLITE_OK)
  // {
  //   sqlite3_close(db1);
  //   return;
  // }
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

  static unsigned long lastMillis = 0, lastMillis2 = 0, lastMillis3 = 0, lastMillis4 = 0;

  if (millis() - lastMillis > SAMPLE_PERIOD)
  {
    debugln("[A]: Start Calculating.");
    calculateANDwritenergy();
    lastMillis = millis();
    // saveToFlash();
  }
  // save to flash

  if (millis() - lastMillis2 > 3600000)
  {
    saveToFlash();
    lastMillis2 = millis();
  }
}
