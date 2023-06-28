#ifndef __ENERGYMONITOR_CALCULATE_HPP
#define __ENERGYMONITOR_CALCULATE_HPP

#ifdef __cplusplus
// extern "C"{
#endif

#include <Arduino.h>

void SP_CalculateEnergy();
void ConfigSensors(int no);
void CalculateVI();
void resetParameters(void);
void calculateANDwritenergy(void);


double getEnergy();
double getVoltage();
double getCurrent();
double getThdCurrent();
double getThdVoltage();
double getPF();

#ifdef __cplusplus
// }
#endif
#endif //__ENERGYMONITOR_CALCULATE_HPP