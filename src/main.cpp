#include "calculate.hpp"
#include "configportal.hpp"
#include "constants.hpp"
#include "mode1.hpp"
#include <Arduino.h>
#include <Preferences.h>

Preferences NVS;
OpMode selectedMode;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  NVS.begin("SM1001", false);
  // NVS.clear();

  if (NVS.isKey(Constants::addrSelectedBoard))
  {
    selectedMode = (OpMode)NVS.getInt(Constants::addrSelectedBoard, 0);
    Serial.println(OpModeNames[selectedMode] + " Was Selected.");
    Mode1_Init();
    return;
  }

  ConfigPoralParameters params = startConfigPoral();
  Serial.println("Done config");
  Serial.println("Operation Mode: " + OpModeNames[params.operationMode]);
  NVS.putInt(Constants::addrSelectedBoard, params.operationMode);
  ESP.restart();
}

void loop()
{
  switch (selectedMode)
  {
  case MODE1:
    delay(5000);
    Serial.println("[A]: Start Calculating.");
    calculateANDwritenergy();
    break;

  default:
    break;
  }
}
