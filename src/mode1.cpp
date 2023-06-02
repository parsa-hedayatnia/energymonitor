#include "mode1.hpp"
#include "constants.hpp"
#include <Arduino.h>

// boolean SM_Pressed = false;

// void SM_WiFiInterrupt(void)
// {
//   if (!SM_Pressed)
//   {
//     SM_Pressed = true;
//     SM_WiFiInterruptFlag = true;
//   }
// }

void Mode1_PinInit(void)
{
  pinMode(Constants::inPinCurrent, INPUT);
  pinMode(Constants::inPinVoltage, INPUT);

  pinMode(Constants::PIN_LED, OUTPUT);
  digitalWrite(Constants::PIN_LED, LOW);

  // pinMode(Constants::SM_TRIGGER_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(Constants::SM_TRIGGER_PIN), Constants::SM_WiFiInterrupt, FALLING);
}
