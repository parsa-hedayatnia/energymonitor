#include "calculate.hpp"
#include "complex.hpp"
#include "constants.hpp"

boolean CalculateFlag = false;

double power = 0;
double voltage = 0;
double current = 0;
double energy = 0;
const int N = 1000;
const int K = 30;
double xp = 0;
double xq = 0;
double XV = 0;
double XI = 0;
double hV = 0;
double hI = 0;
Complex xv[K + 1];
Complex xi[K + 1];
double abs_xv[K + 1];
double abs_xi[K + 1];
double tv[K + 1];
double ti[K + 1];
double p[K + 1];
double q[K + 1];
double Vrms_fft = 0;
double Irms_fft = 0;
double S = 0;
double P = 0;
double Q = 0;
double D = 0;
double PF_fft;
double THDv = 0;
double THDi = 0;
double v[1000];
double i[1000];
double PF = 0;
double THD_voltage = 0;
double THD_current = 0;

float Current_Scale_FFT = 102.0; // used for assigning the value of current_scale_fft and writing it to flash
float Voltage_Scale_FFT = 232.5; // used for assigning the value of voltage_scale_fft and writing it to flash

int SM_SendDataPeriod = 5;

int dc_voltage = 1892;
int dc_current = 1635;

void resetParameters(void)
{
  xp = 0;
  xq = 0;
  XV = 0;
  XI = 0;
  hV = 0;
  hI = 0;
  for (int i = 1; i <= K; i++)
  {
    xv[i] = Complex(0.0, 0.0);
    xi[i] = Complex(0.0, 0.0);
    abs_xv[i] = 0;
    abs_xi[i] = 0;
    tv[i] = 0;
    ti[i] = 0;
    p[i] = 0;
    q[i] = 0;
  }
  Irms_fft = 0;
  S = 0;
  P = 0;
  Q = 0;
  D = 0;
  PF_fft = 0;
  THDv = 0;
  THDi = 0;
}

void calculate(int Time, int phase, int inPinVoltage, int inPinCurrent)
{
  Complex j(0.0, 1.0);
  uint32_t startTime = millis();
  int No = 0;
  int volt, current_fft, voltage_fft;
  while ((millis() - startTime) < Time)
  {

    volt = analogRead(Constants::inPinVoltage);
    voltage_fft = volt - dc_voltage;
    v[No] = Voltage_Scale_FFT * ((voltage_fft * (3.3)) / 4095);
    current_fft = analogRead(Constants::inPinCurrent);
    current_fft = current_fft - dc_current;
    i[No] = Current_Scale_FFT * ((current_fft * (3.3)) / 4095);
    No++;
  }

  No--;
  for (int h = 1; h <= K; h++)
  {
    for (int k = 0; k < No; k++)
    {
      //    for (int k = 1; k < 850; k++) {
      xv[h] += Complex(v[k], 0.0) * (Complex(cos(2 * PI * h * k / No), 0.0) + j * Complex(sin(2 * PI * h * k / No), 0.0));
      xi[h] += Complex(i[k], 0.0) * (Complex(cos(2 * PI * h * k / No), 0.0) + j * Complex(sin(2 * PI * h * k / No), 0.0));
    }
  }

  for (int i = 1; i <= K; i++)
  {
    tv[i] = atan(xv[i].imag() / xv[i].real());
    ti[i] = atan(xi[i].imag() / xi[i].real());
  }

  for (int i = 1; i <= K; i++)
  {
    abs_xv[i] = (2 * sqrt(xv[i].real() * xv[i].real() + xv[i].imag() * xv[i].imag())) / No;
    abs_xi[i] = (2 * sqrt(xi[i].real() * xi[i].real() + xi[i].imag() * xi[i].imag())) / No;
  }

  for (int h = 1; h <= K; h++)
  {
    p[h] = 0.5 * abs_xv[h] * abs_xi[h] * cos(tv[h] - ti[h]);
    xp += p[h];
    q[h] = 0.5 * abs_xv[h] * abs_xi[h] * sin(tv[h] - ti[h]);
    xq += q[h];
    XV = (abs_xv[h] / sqrt(2)) * (abs_xv[h] / sqrt(2)) + XV;
    XI = (abs_xi[h] / sqrt(2)) * (abs_xi[h] / sqrt(2)) + XI;
  }
  for (int h = 2; h <= K; h++)
  {
    hV = abs_xv[h] * abs_xv[h] + hV;
    hI = abs_xi[h] * abs_xi[h] + hI;
  }

  Vrms_fft = sqrt(XV);
  Irms_fft = sqrt(XI);
  S = Vrms_fft * Irms_fft;

  P = xp;

  Q = xq;
  D = sqrt(S * S - P * P - Q * Q);
  PF_fft = P / S;
  THDv = sqrt(hV) / (abs_xv[1]);
  THDi = sqrt(hI) / (abs_xi[1]);

  /////////////////////////////////CONFIGUING WITH FLUK/////////////////////////////////
  THDv = THDv / 2;
  THDi = THDi * 3 / 4;
  Q = xq / 10;

  THD_voltage = THDv;
  THD_current = THDi;
  voltage = Vrms_fft;
  current = Irms_fft;
  PF = PF_fft; // Must be corrected
  if (PF < 0)
    PF = -PF;
  if (P < 0)
    P = -P;
  power = P;
}

void ConfigSensors(int no)
{
  int i;

  for (i = 0; i < no; i++)
    CalculateVI();
}

void CalculateVI()
{
  resetParameters();
  calculate(20, 0, Constants::inPinVoltage, Constants::inPinCurrent);
}

// double sum = 0;
void SP_CalculateEnergy()
{

  energy += (((SM_SendDataPeriod)*power * 0.001) / 3600); // Wh{
  // sum = sum + energy;

  // if (sum < 0)
    // sum = 0;
}

void calculateANDwritenergy()
{
  CalculateVI();
  SP_CalculateEnergy();
}

double getEnergy()
{
  return energy;
}
double getVoltage()
{
  return voltage;
}
double getCurrent()
{
  return current;
}
double getThdCurrent()
{
  return THD_current;
}
double getThdVoltage()
{
  return THD_voltage;
}

double getPF()
{
  return PF;
}