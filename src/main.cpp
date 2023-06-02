#include "configportal.hpp"
#include "constants.hpp"
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

  if (NVS.isKey(Constants::addrSelectedBoard))
  {
    selectedMode = (OpMode)NVS.getInt(Constants::addrSelectedBoard, 0);
    Serial.println(OpModeNames[selectedMode] + " Was Selected.");
    return;
  }

  ConfigPoralParameters params = startConfigPoral();
  Serial.println("Done config");
  Serial.println("Operation Mode: " + OpModeNames[params.operationMode]);
  NVS.putInt(Constants::addrSelectedBoard, params.operationMode);
}

void loop() {}
