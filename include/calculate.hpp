#ifndef CALCULATE_H
#define CALCULATE_H

#ifdef __cplusplus
// extern "C"{
#endif

#include <Arduino.h>

extern boolean CalculateFlag;

void SP_SHA256_key(void);
void SP_makeJSONdata(void);
void SP_CalculateEnergy();
void ConfigSensors(int no);
void CalculateVI();
void resetParameters(void);
void calculateANDwritenergy(void);
void SaveMode1Parameters(int Type);
void GetMode1Parameters(int Type);
void SP_login(void);
void SP_data(void);


double getEnergy();
double getVoltage();
double getCurrent();
double getThdCurrent();
double getThdVoltage();
double getPF();

#ifdef __cplusplus
// }
#endif
#endif//CALCULATE_H