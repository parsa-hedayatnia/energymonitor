#include <Arduino.h>

#include "soc/rtc_wdt.h"
#include <driver/adc.h>
#include "uRTCLib.h"
#include "Wire.h"
#include "uEEPROMLib.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "NTPClient.h"
#include "WiFiUdp.h"
#include <ADS1115_WE.h> 
#include "EEPROM.h"
#include "Complex.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
#include "mbedtls/rsa.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/md.h"
#include <ArduinoOTA.h>
#include <ESP32httpUpdate.h>
#include "Preferences.h"

#define TIMER_INTERRUPT_DEBUG      1

#include "ESP32TimerInterrupt.h"

// #include <ESP32WebServer.h>
#include "Preferences.h"

#include "Printable.h"
#include "esp_system.h"
#include <esp_wifi.h>

// Use From 0 to 4. Higher Number, More Debugging Messages And Memory Usage.
#define _WIFIMGR_LOGLEVEL_ 3

#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiClient.h>


#define ESP_getChipId() ((uint32_t)ESP.getEfuseMac())

#define LED_ON HIGH
#define LED_OFF LOW

#define PIN_SCL 22 // Pin SCL mapped to pin GPIO22/SCL of ESP32
#define PIN_SDA 21 // Pin SDA mapped to pin GPIO21/SDA of ESP32
 
#define EM_UserNameLabel "User Name" // Label That Show In Config Portal Page.
#define EM_PassWordLabel "Password"  // Label That Show In Config Portal Page.

// SSID And PassWord For Config Portal (In AP Mode).
String ssid = "ESP32_";
String PassWord_CP = "123456789";

// SSID And PassWord For Your Router (To Connect This).
String Router_SSID;
String Router_Pass;

// Indicates Whether ESP has WiFi Credentials Saved From Previous Session.
bool initialConfig = false;

// Use True For Dynamic DHCP IP, False To Use Static IP And  You Have To Change The IP Accordingly To Your Network.
#define USE_DHCP_IP true

#if USE_DHCP_IP
// Use DHCP
IPAddress stationIP = IPAddress(0, 0, 0, 0);
IPAddress gatewayIP = IPAddress(192, 168, 2, 1);
IPAddress netMask = IPAddress(255, 255, 255, 0);
#else
// Use static IP
IPAddress stationIP = IPAddress(192, 168, 2, 232);
IPAddress gatewayIP = IPAddress(192, 168, 2, 1);
IPAddress netMask = IPAddress(255, 255, 255, 0);
#endif

IPAddress dns1IP = gatewayIP;
IPAddress dns2IP = IPAddress(8, 8, 8, 8);

// Use False If You Don't Like To Display Available Pages In Information Page Of Config Portal
// Comment Out Or Use True To Display Available Pages In Information Page Of Config Portal
// Must Be Placed Before #include <ESP_WiFiManager.h>
#define USE_AVAILABLE_PAGES true

// Use False To Disable NTP Config.
#define USE_ESP_WIFIMANAGER_NTP true

// Use True To Enable CloudFlare NTP Service. System Can Hang If You Don't Have Internet Access While Accessing CloudFlare
// See Issue #21: CloudFlare Link In The Default Portal => https://github.com/khoih-prog/ESP_WiFiManager/issues/21
#define USE_CLOUDFLARE_NTP false

//https://github.com/khoih-prog/ESP_WiFiManager
#include <ESP_WiFiManager.h>

#include "Complex.h"
#include "Complex.hpp"

#include "ESPAsyncWebServer.h"

#define COMPLEX_LIB_VERSION "0.1.12"
HTTPClient http;
WiFiClient client;


#define EM_RedLED                      32
#define EM_BlueLED                     26
#define EM_GreenLED                    25

#define EM_NumberOfCalculate           4      

// Set Voltage & Current Range.
#define EM_MaximumSupplyVoltage        250
#define EM_MinimumSupplyVoltage        200
#define EM_MaximumCurrent              14

#define EM_RelayPin                    27                       
#define EM_WaitForSafeCurrentTime      120000L                 

#define EM_RTCaddress                  0x68
#define EM_EEPROMaddress               0x57
#define EM_ADS1115address              0x48                                // Address Pin Connect To GND.

// Posision Of Data In EEprom That CalculateEnergy() Function Called.
#define EM_LastDataPossionV            4
#define EM_LastDataPossionI            8
#define EM_LastDataPossionP            12
#define EM_LastDataPossionR            16
#define EM_LastDataPossionS            20
#define EM_LastDataPossionM            21
#define EM_LastDataPossionH            22
#define EM_LastDataPossionDOW          23 

#define EM_HashOutPossion              25  
#define EM_HashOutSizePossion          95        
#define EM_UserIDPosition              100
#define EM_PasswordPosition            120
#define EM_UserIDSizePosition          140
#define EM_PasswordSizePosition        144
#define EM_TokenPosition               150
#define EM_TokenSizePosition           300

#define EM_NubmberOfDataCanStored      160                                      

// Posision Of Data In EEprom When WiFi Disconnected.
#define EM_DataPossionV                304  
#define EM_DataPossionI                308
#define EM_DataPossionP                312
#define EM_DataPossionR                316
#define EM_DataPossionTv               320
#define EM_DataPossionTi               324
#define EM_DataPossionS                325 
#define EM_DataPossionM                326
#define EM_DataPossionH                327
#define EM_DataPossionDOW              328

#define EM_UserNameLabel               "User Name"                         // Label That Show In Config Portal Page.    
#define EM_PassWordLabel               "Password"                          // Label That Show In Config Portal Page. 

#define EM_ADS1115alertReadyPin        17
#define EM_ADS1115addressPin           16                 
#define EM_VoltageADCpin               34                          
#define EM_CurrentADCpin               35        
#define EM_TRIGGER_PIN                 33       
 
#define EM_NTP_OFFSET                  12600 
#define EM_NTP_INTERVAL                60 * 1000   
#define EM_NTP_ADDRESS                 "3.ir.pool.ntp.org"

#define EM_SendPktTimeOut              4000L                            // Send Data Pack Time To Site If Do Not Recieve True.

#define EM_ADC_BITS                    16
#define EM_ADC_COUNTS                  (1<<EM_ADC_BITS)
#define EM_VCCVoltage                  5000

#define EnergyMonitor                  1
#define GateWay                        2
#define SmartPlug                      3

#define WIFI_CONNECT_TIMEOUT           9000L
#define WHILE_LOOP_DELAY               500L
#define WHILE_LOOP_STEPS               (WIFI_CONNECT_TIMEOUT / ( 3 * WHILE_LOOP_DELAY ))

#define TIMER0_INTERVAL_MS             1000
#define TIMER1_INTERVAL_MS             10000
#define TIMER2_INTERVAL_MS             50000

 

String SiteIP = "";
String HttpString = "http://";
String DataString = "/device/dataControl?token=";
String LoginString = "/login";
String RegisterString = "/device?token=";
const char *addr_SiteIP  = "adsite";

/*  *** Energy Monitor Variable Definition ***  */
HTTPClient EM_HTTP_Handle;
WiFiClient EM_Client_Handle;
AsyncWebServer AcyncWeb(80);
ESP32Timer ITimer0(0);
ESP32Timer ITimer1(1);
ESP32Timer ITimer2(2);
Preferences NVS;


boolean EM_WiFiInterruptFlag = false;                                      // Set In WiFiInterrupt() Function And Used In loop().
boolean EM_Pressed = false;
boolean EM_CalculateFlag = true;                                                                    
boolean EM_SendDataToServerFlag = false;
boolean EM_SendPeviousDataFlag = false;
boolean EM_WaitForSafeCurrentFlag = false;                                 // Flag To Determine That Connect Line After A While.
boolean EM_FirstTime = false;                                              // To Avoid Running The First Time ControlRelay().
boolean EM_WiFiOff = true;                                                 // Determind That ESP32 Connected Or Disconnected To Router.
boolean EM_LastRelayStatus = false;                                        // Avoid Operating The Relay In Vain.
boolean EM_CurrentRelayStatus = false;                                     // Avoid Operating The Relay In Vain. 
boolean EM_lastVCross;                                                     // Used To Measure Number Of Times Threshold Is Crossed.
boolean EM_checkVCross;                                                    // Used To Measure Number Of Times Threshold Is Crossed.
boolean EM_Bilink = false;
boolean EM_ConfigPortalRun = false; 

// Get Time From DS3231 And NTP Server.
byte EM_Seconde = 30;                                                           
byte EM_Minute = 30;
byte EM_Hour = 19;
byte EM_DayOfWeek = 3;
byte EM_DayOfMonth = 24;
byte EM_Month = 9;
byte EM_Year = 20;

byte EM_StateLED = LOW;

int SelectedBoard = 0;
int EM_EEPROMPosisionShift;          
int EM_SendDataPeriod = 5;                                                 // Period Of Time That Send Data To Site.
int EM_Counter = 0;                                                        // Use In Timer1Trriger().
int EM_UserNameSize = 0;
int EM_PasswordSize = 0;
int SendDataPeriod_EM = 14;

unsigned long EM_HoldTime;                                                 // Used In ControlRelay() Function.

char EM_SHA256key[17];                                                     // The Key That Create base64 Code. Used In EM_SHA256_key() Function.
char EM_plainText[17] = "";                                                
char EM_UserName[17];
char EM_Pass[17];
char EM_UserParameter[304] = "";
char EM_EnergyParameter[304] = "";

unsigned char EM_cipherTextOutput[17] = "";                       
unsigned char EM_finalCipheredText[305] = "";              
unsigned char EM_DSTSend[409];                

const char *addrUserName_EM   = "adusernameem";
const char *addrUserPass_EM   = "aduserpassem";
const char *addrSSID_EM       = "adssidem";
const char *addrPass_EM       = "adpassem";

const char *addr_ApiToken_EM  = "adapiTokenem";
const char *addr_Period_EM    = "adperiodem";
const char *addr_ShaKey_EM    = "adshaKeyem";

// Used In CalculateEnergy().
double EM_lastFilteredV;
double EM_filteredV =1; 
double EM_filteredI;
double EM_offsetV;
double EM_offsetI; 
double EM_THD_voltage;
double EM_THD_current;
double EM_P;

// Set Calibration Parameters.
double EM_VoltageCallibration  = 20400;
double EM_CurrentCallibration  = 9900;  
double EM_FaseShiftCalibration = 1.39;

// Seve Average Of Calculated Values.
double EM_HoldRealPower;                                                    
double EM_HoldPowerFactor;
double EM_HoldSupplyVoltage;
double EM_HoldCurrent;

// Use For Comprar Values Of Current And Power Factor To Set Calibration.
double EM_ComrarCurrent = 5;
double EM_ComrarCurrent2 = 0.1;
double EM_ComprarPowerFactor = 1; 
                                                                                                      
String EM_serverURL     = "www.organization.socialgrid.ir/public/";                                                 
String MacAdd        = "";

String SSID_EM = "";
String Pass_EM = "";
String UserName_EM = "";
String UserPass_EM = "";

String ApiToken_EM;

size_t EM_dlen = 409;                                                            
size_t EM_slen = 304;


/*  *** GateWay Variable Definition ***  */
boolean GW_WiFiInterruptFlag = false;
boolean GW_Pressed = false;
boolean GW_ConfigPortalRun = false;
boolean GW_StartSending = false;
boolean ApiStatus = false;

String SSIDsg = "";
String Passsg = "";
String UserNamesg = "";
String UserPasssg = "";

String GW_Email = "";
String GW_EmailPass = "";

String GW_MacAdd = "";
String GW_IP = "";

String GW_apiToken = "";
String GW_PER = "";
String GW_last_version = "";
String GW_StatusRelay = "";
String GW_code = "";
String GW_BUF = "";

int GW_TRIGGER_PIN = 0;
int GW_SendDataPeriod = 20;

const char *addrUserNameSG = "AdUserNameSG";
const char *addrUserPassSG = "AdUserPassSG";
const char *addrSSIDsg     = "AdSSIDsg";
const char *addrPasssg     = "AdPasssg";

const char *addrDevice1    = "AdDevice1";
const char *addrDevice2    = "AdDevice2";
const char *addrDevice3    = "AdDevice3";
const char *addrDevice4    = "AdDevice4";
const char *addrDevice5    = "AdDevice5";
const char *addrDevice6    = "AdDevice6";
const char *addrDevice7    = "AdDevice7";
const char *addrDevice8    = "AdDevice8";

const char *addrDeviceStatus1    = "AdDeviceStatus1";
const char *addrDeviceStatus2    = "AdDeviceStatus2";
const char *addrDeviceStatus3    = "AdDeviceStatus3";
const char *addrDeviceStatus4    = "AdDeviceStatus4";
const char *addrDeviceStatus5    = "AdDeviceStatus5";
const char *addrDeviceStatus6    = "AdDeviceStatus6";
const char *addrDeviceStatus7    = "AdDeviceStatus7";
const char *addrDeviceStatus8    = "AdDeviceStatus8";

unsigned char GW_DSTSend[409];

struct DeviceMacAdd
{
  String Device1Mac = "";
  String Device2Mac = "";
  String Device3Mac = "";
  String Device4Mac = "";
  String Device5Mac = "";
  String Device6Mac = "";
  String Device7Mac = "";
  String Device8Mac = "";
  int Device1Status = false;
  int Device2Status = false;
  int Device3Status = false;
  int Device4Status = false;
  int Device5Status = false;
  int Device6Status = false;
  int Device7Status = false;
  int Device8Status = false;
};

DeviceMacAdd GatewayMacadd;


/*  *** Smart Plug Variable Definition ***  */
boolean SM_WiFiInterruptFlag = false;
boolean SM_CalculateFlag = false;
boolean SM_WiFiOff = false;
boolean SM_Pressed = false;
boolean SM_ConfigPortalRun = false;

String userID;
String password;
String SM_ApiToken = "";

String SSIDcg = "";
String Passcg = "";
String UserPasscg = "";
String TestUser = "";


int PIN_LED = 14;
int inPinCurrent = 35;
int inPinVoltage = 34;
int SM_TRIGGER_PIN = 0;
int SM_SendDataPeriod = 14;
int dc_voltage = 1892;
int dc_current = 1635;

const char *addrSelectedBoard     = "AdSelectedBoard";
const char *addrSP_ApiToken       = "AdSPApiToken";
const char *addrSP_ShaKey         = "AdSP_ShaKey";
const char *addrSP_Period         = "AdSP_Period";
const char *addrDeviceMacAdd      = "AdDeviceMacAdd";

unsigned char SP_DSTSend[409];  
unsigned char SP_EncryptedData[305] = "";
char UserAccont[304] = "";

const char *addrUserNameCG = "AdUserNameCG";
const char *addrUserPassCG = "AdUserPassCG";
const char *addrSSIDcg     = "AdSSIDcg";
const char *addrPasscg     = "AdPasscg";

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

float Current_Scale_FFT = 102.0; //used for assigning the value of current_scale_fft and writing it to flash
float Voltage_Scale_FFT = 232.5; //used for assigning the value of voltage_scale_fft and writing it to flash


/*  *** Smart Plug Function Declaration ***  */
void SM_PinInit(void);
void SM_ServerInit(void);
void SM_TimerInit(void);
void IRAM_ATTR SM_Timer1Interrupt(void);
void SM_WiFiInterrupt(void);
void SP_SHA256_key(void);
void SP_makeJSONdata(void);
void SP_CalculateEnergy();
void ConfigSensors(int no);
void CalculateVI();
void resetParameters(void);
void calculateANDwritenergy(void);
void SaveSmartPlugParameters(int Type);
void GetSmartPlugParameters(int Type);
void SP_login(void);
void SP_data(void);


/*  *** Energy Monitor Function Declaration ***  */
void GetEnergyMonitorParameters(int Type);
void SaveEnergyMonitorParameters(int Type);
void EM_SHA256_key(void);
void EM_TimerInit(void);
void IRAM_ATTR EM_TimerHandler0(void);
void IRAM_ATTR EM_TimerHandler1(void);
void EM_PinInit(void);
void EM_WiFiInterrupt(void);
void EM_SendData(void);
void EM_sendpkt(int type, String dest, char * payload);
void EM_registerToServer(void);
void EM_SetConnection(String dest);
void ConfigPortal(void);
void ReConfigPortal(void);
void EM_makeJSONdata(void);
void EM_loginToServer(void);
float readChannel(ADS1115_MUX channel);
String GetMAcAdd(void);
String GetLast6Mac(void);
void EM_decrypt(unsigned char * chipherText, char * key, unsigned char * outputBuffer);
void EM_encrypt(char * plainText, char * key, unsigned char * outputBuffer);


/*  *** GateWay Function Declaration ***  */
void GW_PinInit(void);
void httpPOSTrequest(const char* serverName, String postdata);
String httpGETRequest(const char* serverName);
void GW_loginToServer(void);
void SaveGateWayParameters(int Type);
void GW_registerToServer(void);
void GW_sendpkt(int type, String dest, char * payload ,String sendData);
void GW_senddata(String data ,String api);
void GW_WiFiInterrupt(void);
void GW_TimerInit(void);
void IRAM_ATTR TimerHandler2(void);
void GetGateWayParameters(int Type);


void SaveDeviceParameter(void);
void GetDeviceParameter(void);


void setup()
{
  Serial.begin(115200);
  while (!Serial);

  NVS.begin("SM1001", false);
  //TODO
  SelectedBoard = NVS.getInt(addrSelectedBoard);

  if (SelectedBoard == EnergyMonitor) {
    Serial.println("[A]: EnergyMonitor Was Selected.");
  }
  
  if (SelectedBoard == GateWay) {
    Serial.println("[A]: GateWay Was Selected.");
  }
  
  if (SelectedBoard == SmartPlug) {
    Serial.println("[A]: SmartPlug Was Selected.");
  }
  
  if (SelectedBoard != EnergyMonitor && SelectedBoard != GateWay && SelectedBoard != SmartPlug) {
    Serial.println("[A]: No Board Was Selected.");
  }

  MacAdd = GetMAcAdd();

  ConfigPortal();

  Serial.print("[A]: Value Of SelectedBoard After Config Portal Is : ");
  Serial.println(SelectedBoard);

  if (SelectedBoard == EnergyMonitor) 
  {
    GetEnergyMonitorParameters(1);
    
    SendDataPeriod_EM = NVS.getInt(addr_Period_EM);
    if (SendDataPeriod_EM <= 14) 
    {
      SendDataPeriod_EM = 14;
    }
    
    Serial.print("[A]: Energy Monitor Period Is : " + SendDataPeriod_EM);

    EM_PinInit();

    EM_TimerInit();

    if ((UserName_EM != "") && (UserPass_EM != "")) 
    {
      EM_SHA256_key();
    }
  }
  
  if (SelectedBoard == GateWay) 
  {
    GetGateWayParameters(1);

    GW_PinInit();

    GW_TimerInit();
  }
  
  if (SelectedBoard == SmartPlug) 
  {
    GetSmartPlugParameters(1);

    SM_ApiToken = NVS.getString(addrSP_ApiToken);
    Serial.println("[A]: Smart Plug Token Is : ");
    Serial.println(SM_ApiToken);

    SM_SendDataPeriod = 14;

    SM_PinInit();

    SM_TimerInit();

    SM_ServerInit();
  }
  
  if (SelectedBoard != EnergyMonitor && SelectedBoard != GateWay && SelectedBoard != SmartPlug) 
  {
    Serial.println("[A]: SelectedBoard Not Initialized (Setup).");
  }
}

void loop()
{
  if (SelectedBoard == EnergyMonitor)
  {
    if (EM_WiFiInterruptFlag) 
    {
      ReConfigPortal();
      EM_Pressed = false;
      EM_WiFiInterruptFlag = false;
    }

    if (EM_CalculateFlag)
    {
      resetParameters();
      calculateANDwritenergy();

      EM_CalculateFlag = false;
    }
    
    if (EM_SendDataToServerFlag)
    {
      EM_SendData();
      EM_SendDataToServerFlag = false;
    }
    
    if (WiFi.status() != WL_CONNECTED) 
    {
      if (SSID_EM != "")
      {
        WiFi.begin(SSID_EM.c_str(), Pass_EM.c_str());
    
        Serial.print("\n[A]: GateWay WiFiOff , Try To Connecting To ");
        Serial.println(SSID_EM.c_str());

        int StartTime = 0;

        while (WiFi.status() != WL_CONNECTED && ( (millis() - StartTime) < 3000) )
        {
          Serial.print(" .");
          delay(500);
        }
        
      } else 
      {
        ReConfigPortal();
      } 

      delay(1000);
    }
  }
  
  if (SelectedBoard == GateWay)
  {
    if (GW_WiFiInterruptFlag) 
    {
      ReConfigPortal();
      GW_Pressed = false;
      GW_WiFiInterruptFlag = false;
    }

    if (GW_StartSending && WiFi.status() == WL_CONNECTED) 
    {
      Serial.println("\n\n[A]: We Are Searching For Connected Devices.");

      wifi_sta_list_t wifi_sta_list;
      tcpip_adapter_sta_list_t adapter_sta_list;
      
      memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
      memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
      
      esp_wifi_ap_get_sta_list(&wifi_sta_list);
      tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);
          
      String MacBuff = "";
      int Macflag[20] = {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1};
      int pos = 0;
      String BUFFER;
      int clear[8] = {1 ,1 ,1 ,1 ,1 ,1 ,1 ,1};
      StaticJsonDocument<305> JsonEmail;

      GetDeviceParameter();

      Serial.println("[A]: Value OF Macflag : ");
      for (int i = 0; i < 20; i++)
      {
        Serial.print(Macflag[i]);
      }
      
      for (int i = 0; i < adapter_sta_list.num; i++) 
      {
        tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];

        char n[18];
        sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
        MacBuff = String(n);

        Serial.print("\n\n[A]: MAC");
        Serial.print(i);
        Serial.print(" --> ");
        Serial.println(MacBuff);

        Serial.print("[A]: IP : ");
        Serial.println(String(ip4addr_ntoa(reinterpret_cast<ip4_addr_t*>(&(station.ip)))));
        
        if (GatewayMacadd.Device1Mac == MacBuff)
        {
          Serial.println("[A]: Saved");
          Macflag[pos] = i;
          pos++;
          Macflag[pos] = 1;
          pos++;
        }else if (GatewayMacadd.Device2Mac == MacBuff)
        {
          Serial.println("[A]: Saved");
          Macflag[pos] = i;
          pos++;
          Macflag[pos] = 2;
          pos++;
        }else if (GatewayMacadd.Device3Mac == MacBuff)
        {
          Serial.println("[A]: Saved");
          Macflag[pos] = i;
          pos++;
          Macflag[pos] = 3;
          pos++;
        }else if (GatewayMacadd.Device4Mac == MacBuff)
        {
          Serial.println("[A]: Saved");
          Macflag[pos] = i;
          pos++;
          Macflag[pos] = 4;
          pos++;
        }else if (GatewayMacadd.Device5Mac == MacBuff)
        {
          Serial.println("[A]: Saved");
          Macflag[pos] = i;
          pos++;
          Macflag[pos] = 5;
          pos++;
        }else if (GatewayMacadd.Device6Mac == MacBuff)
        {
          Serial.println("[A]: Saved");
          Macflag[pos] = i;
          pos++;
          Macflag[pos] = 6;
          pos++;
        }else if (GatewayMacadd.Device7Mac == MacBuff)
        {
          Serial.println("[A]: Saved");
          Macflag[pos] = i;
          pos++;
          Macflag[pos] = 7;
          pos++;
        }else if (GatewayMacadd.Device8Mac == MacBuff)
        {
          Serial.println("[A]: Saved");
          Macflag[pos] = i;
          pos++;
          Macflag[pos] = 8;
          pos++;
        }else
        {
          Serial.println("[A]: Not Saved");
          Macflag[pos] = i;
          pos++;
          Macflag[pos] = 9;
          pos++;
        }
      }

      Serial.println("\n[A]: Value OF Macflag : ");
      for (int i = 0; i < 20; i++)
      {
        Serial.print(Macflag[i]);
      }
      Serial.println();

      for (int i = 0; i < adapter_sta_list.num; i++)
      {
        String ServerAddr;
        tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];

        if (Macflag[2 * i + 1] == 9)
        {
          int f[10] = {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};
          int pos2 = 0;
          for (int j = 0; j < adapter_sta_list.num; j++)
          {
            f[pos2] = Macflag[2 * j + 1];
            pos2++;
          }

          // For Test.
          Serial.println("[A]: Value OF f : ");
          for (int i = 0; i < 10; i++)
          {
            Serial.print(f[i]);
          }

          for (int k = 0; k <= 8; k++)
          {
            bool empty = true;
            for (int g = 0; g < 10; g++)
            {
              if (empty)
              {
                if (f[g] == k)
                {
                  empty = false;
                }
              }
            }

            if (empty)
            {
              if (k == 1)
              {
                clear[0] = 0;
                char n[18];
                sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
                MacBuff = String(n);

                Serial.println("[A]: New Device Was Saved In 1");
                GatewayMacadd.Device1Mac = MacBuff;
                GatewayMacadd.Device1Status = false;
              }
              if (k == 2)
              {
                clear[1] = 0;
                char n[18];
                sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
                MacBuff = String(n);

                Serial.print("[A]: New Device Was Saved In 2");
                GatewayMacadd.Device2Mac = MacBuff;
                GatewayMacadd.Device2Status = false;
              }
              if (k == 3)
              {
                clear[2] = 0;
                char n[18];
                sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
                MacBuff = String(n);

                Serial.print("[A]: New Device Was Saved In 3");
                GatewayMacadd.Device3Mac = MacBuff;
                GatewayMacadd.Device3Status = false;
              }
              if (k == 4)
              {
                clear[3] = 0;
                char n[18];
                sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
                MacBuff = String(n);

                Serial.print("[A]: New Device Was Saved In 4");
                GatewayMacadd.Device4Mac = MacBuff;
                GatewayMacadd.Device4Status = false;
              }
              if (k == 5)
              {
                clear[4] = 0;
                char n[18];
                sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
                MacBuff = String(n);

                Serial.print("[A]: New Device Was Saved In 5");
                GatewayMacadd.Device5Mac = MacBuff;
                GatewayMacadd.Device5Status = false;
              }
              if (k == 6)
              {
                clear[5] = 0;
                char n[18];
                sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
                MacBuff = String(n);

                Serial.print("[A]: New Device Was Saved In 6");
                GatewayMacadd.Device6Mac = MacBuff;
                GatewayMacadd.Device6Status = false;
              }
              if (k == 7)
              {
                clear[6] = 0;
                char n[18];
                sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
                MacBuff = String(n);

                Serial.print("[A]: New Device Was Saved In 7");
                GatewayMacadd.Device7Mac = MacBuff;
                GatewayMacadd.Device7Status = false;
              }
              if (k == 8)
              {
                clear[7] = 0;
                char n[18];
                sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
                MacBuff = String(n);

                Serial.print("[A]: New Device Was Saved In 8");
                GatewayMacadd.Device8Mac = MacBuff;
                GatewayMacadd.Device8Status = false;
              }
            }
          } 
        }

        if (Macflag[2 * i + 1] == 1)
        {
          clear[0] = 0;
          char n[18];
          sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
          MacBuff = String(n);

          GW_IP = String(ip4addr_ntoa(reinterpret_cast<ip4_addr_t*>(&(station.ip))));

          if (GatewayMacadd.Device1Status)
          {
            ServerAddr = "http://" + GW_IP + "/apitoken";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            GW_apiToken = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_apiToken : " + GW_apiToken);

            ServerAddr = "http://" + GW_IP + "/encryptdata";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            String GW_BUF = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_BUF : " + GW_BUF);
            
            if (GW_BUF != "" && GW_apiToken != "")
            {
              GW_senddata(GW_BUF ,GW_apiToken);
              GW_BUF = "";
              GW_apiToken = "";
            }
          } else
          {
            GW_MacAdd = MacBuff;
            ServerAddr = "http://" + GW_IP + "/email";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            BUFFER = httpGETRequest(ServerAddr.c_str());

            deserializeJson(JsonEmail ,BUFFER);
            GW_Email = JsonEmail["email"].as<String>();
            GW_EmailPass = JsonEmail["emailpass"].as<String>();

            Serial.println("[A]: GW_Email : " + GW_Email);
      
                
            if (GW_Email != "" && GW_EmailPass != "")
            {
              GW_loginToServer();

              if(GW_code == "true" && GW_apiToken != "") 
              {
                GW_registerToServer();

                if (GW_code == "true" && GW_PER != "")
                {
                  // Send apitoken .
                  ServerAddr = "http://" + GW_IP + "/api";
                  httpPOSTrequest(ServerAddr.c_str(), GW_apiToken);
                  GatewayMacadd.Device1Status = true;
                }
              }
            }
          }
        }
      
        if (Macflag[2 * i + 1] == 2)
        {
          clear[1] = 0;
          char n[18];
          sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
          MacBuff = String(n);

          GW_IP = String(ip4addr_ntoa(reinterpret_cast<ip4_addr_t*>(&(station.ip))));

          if (GatewayMacadd.Device2Status)
          {
            ServerAddr = "http://" + GW_IP + "/apitoken";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            GW_apiToken = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_apiToken : " + GW_apiToken);

            ServerAddr = "http://" + GW_IP + "/encryptdata";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            String GW_BUF = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_BUF : " + GW_BUF);
            
            if (GW_BUF != "" && GW_apiToken != "")
            {
              GW_senddata(GW_BUF ,GW_apiToken);
              GW_BUF = "";
              GW_apiToken = "";
            }
          } else
          {
            GW_MacAdd = MacBuff;
            ServerAddr = "http://" + GW_IP + "/email";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            BUFFER = httpGETRequest(ServerAddr.c_str());

            deserializeJson(JsonEmail ,BUFFER);
            GW_Email = JsonEmail["email"].as<String>();
            GW_EmailPass = JsonEmail["emailpass"].as<String>();

            Serial.println("[A]: GW_Email : " + GW_Email);
      
                
            if (GW_Email != "" && GW_EmailPass != "")
            {
              GW_loginToServer();

              if(GW_code == "true" && GW_apiToken != "") 
              {
                GW_registerToServer();

                if (GW_code == "true" && GW_PER != "")
                {
                  // Send apitoken .
                  ServerAddr = "http://" + GW_IP + "/api";
                  httpPOSTrequest(ServerAddr.c_str(), GW_apiToken);
                  GatewayMacadd.Device2Status = true;
                }
              }
            }
          }
        }
      
        if (Macflag[2 * i + 1] == 3)
        {
          clear[2] = 0;
          char n[18];
          sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
          MacBuff = String(n);

          GW_IP = String(ip4addr_ntoa(reinterpret_cast<ip4_addr_t*>(&(station.ip))));

          if (GatewayMacadd.Device3Status)
          {
            ServerAddr = "http://" + GW_IP + "/apitoken";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            GW_apiToken = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_apiToken : " + GW_apiToken);

            ServerAddr = "http://" + GW_IP + "/encryptdata";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            String GW_BUF = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_BUF : " + GW_BUF);
            
            if (GW_BUF != "" && GW_apiToken != "")
            {
              GW_senddata(GW_BUF ,GW_apiToken);
              GW_BUF = "";
              GW_apiToken = "";
            }
          } else
          {
            GW_MacAdd = MacBuff;
            ServerAddr = "http://" + GW_IP + "/email";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            BUFFER = httpGETRequest(ServerAddr.c_str());

            deserializeJson(JsonEmail ,BUFFER);
            GW_Email = JsonEmail["email"].as<String>();
            GW_EmailPass = JsonEmail["emailpass"].as<String>();

            Serial.println("[A]: GW_Email : " + GW_Email);
      
                
            if (GW_Email != "" && GW_EmailPass != "")
            {
              GW_loginToServer();

              if(GW_code == "true" && GW_apiToken != "") 
              {
                GW_registerToServer();

                if (GW_code == "true" && GW_PER != "")
                {
                  // Send apitoken .
                  ServerAddr = "http://" + GW_IP + "/api";
                  httpPOSTrequest(ServerAddr.c_str(), GW_apiToken);
                  GatewayMacadd.Device3Status = true;
                }
              }
            }
          }
        }
      
        if (Macflag[2 * i + 1] == 4)
        {
          clear[3] = 0;
          char n[18];
          sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
          MacBuff = String(n);

          GW_IP = String(ip4addr_ntoa(reinterpret_cast<ip4_addr_t*>(&(station.ip))));

          if (GatewayMacadd.Device4Status)
          {
            ServerAddr = "http://" + GW_IP + "/apitoken";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            GW_apiToken = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_apiToken : " + GW_apiToken);

            ServerAddr = "http://" + GW_IP + "/encryptdata";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            String GW_BUF = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_BUF : " + GW_BUF);
            
            if (GW_BUF != "" && GW_apiToken != "")
            {
              GW_senddata(GW_BUF ,GW_apiToken);
              GW_BUF = "";
              GW_apiToken = "";
            }
          } else
          {
            GW_MacAdd = MacBuff;
            ServerAddr = "http://" + GW_IP + "/email";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            BUFFER = httpGETRequest(ServerAddr.c_str());

            deserializeJson(JsonEmail ,BUFFER);
            GW_Email = JsonEmail["email"].as<String>();
            GW_EmailPass = JsonEmail["emailpass"].as<String>();

            Serial.println("[A]: GW_Email : " + GW_Email);
      
                
            if (GW_Email != "" && GW_EmailPass != "")
            {
              GW_loginToServer();

              if(GW_code == "true" && GW_apiToken != "") 
              {
                GW_registerToServer();

                if (GW_code == "true" && GW_PER != "")
                {
                  // Send apitoken .
                  ServerAddr = "http://" + GW_IP + "/api";
                  httpPOSTrequest(ServerAddr.c_str(), GW_apiToken);
                  GatewayMacadd.Device4Status = true;
                }
              }
            }
          }
        }
      
        if (Macflag[2 * i + 1] == 5)
        {
          clear[4] = 0;
          char n[18];
          sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
          MacBuff = String(n);

          GW_IP = String(ip4addr_ntoa(reinterpret_cast<ip4_addr_t*>(&(station.ip))));

          if (GatewayMacadd.Device5Status)
          {
            ServerAddr = "http://" + GW_IP + "/apitoken";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            GW_apiToken = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_apiToken : " + GW_apiToken);

            ServerAddr = "http://" + GW_IP + "/encryptdata";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            String GW_BUF = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_BUF : " + GW_BUF);
            
            if (GW_BUF != "" && GW_apiToken != "")
            {
              GW_senddata(GW_BUF ,GW_apiToken);
              GW_BUF = "";
              GW_apiToken = "";
            }
          } else
          {
            GW_MacAdd = MacBuff;
            ServerAddr = "http://" + GW_IP + "/email";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            BUFFER = httpGETRequest(ServerAddr.c_str());

            deserializeJson(JsonEmail ,BUFFER);
            GW_Email = JsonEmail["email"].as<String>();
            GW_EmailPass = JsonEmail["emailpass"].as<String>();

            Serial.println("[A]: GW_Email : " + GW_Email);
      
                
            if (GW_Email != "" && GW_EmailPass != "")
            {
              GW_loginToServer();

              if(GW_code == "true" && GW_apiToken != "") 
              {
                GW_registerToServer();

                if (GW_code == "true" && GW_PER != "")
                {
                  // Send apitoken .
                  ServerAddr = "http://" + GW_IP + "/api";
                  httpPOSTrequest(ServerAddr.c_str(), GW_apiToken);
                  GatewayMacadd.Device5Status = true;
                }
              }
            }
          }
        }
      
        if (Macflag[2 * i + 1] == 6)
        {
          clear[5] = 0;
          char n[18];
          sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
          MacBuff = String(n);

          GW_IP = String(ip4addr_ntoa(reinterpret_cast<ip4_addr_t*>(&(station.ip))));

          if (GatewayMacadd.Device6Status)
          {
            ServerAddr = "http://" + GW_IP + "/apitoken";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            GW_apiToken = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_apiToken : " + GW_apiToken);

            ServerAddr = "http://" + GW_IP + "/encryptdata";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            String GW_BUF = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_BUF : " + GW_BUF);
            
            if (GW_BUF != "" && GW_apiToken != "")
            {
              GW_senddata(GW_BUF ,GW_apiToken);
              GW_BUF = "";
              GW_apiToken = "";
            }
          } else
          {
            GW_MacAdd = MacBuff;
            ServerAddr = "http://" + GW_IP + "/email";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            BUFFER = httpGETRequest(ServerAddr.c_str());

            deserializeJson(JsonEmail ,BUFFER);
            GW_Email = JsonEmail["email"].as<String>();
            GW_EmailPass = JsonEmail["emailpass"].as<String>();

            Serial.println("[A]: GW_Email : " + GW_Email);
      
                
            if (GW_Email != "" && GW_EmailPass != "")
            {
              GW_loginToServer();

              if(GW_code == "true" && GW_apiToken != "") 
              {
                GW_registerToServer();

                if (GW_code == "true" && GW_PER != "")
                {
                  // Send apitoken .
                  ServerAddr = "http://" + GW_IP + "/api";
                  httpPOSTrequest(ServerAddr.c_str(), GW_apiToken);
                  GatewayMacadd.Device6Status = true;
                }
              }
            }
          }
        }
      
        if (Macflag[2 * i + 1] == 7)
        {
          clear[6] = 0;
          char n[18];
          sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
          MacBuff = String(n);

          GW_IP = String(ip4addr_ntoa(reinterpret_cast<ip4_addr_t*>(&(station.ip))));

          if (GatewayMacadd.Device7Status)
          {
            ServerAddr = "http://" + GW_IP + "/apitoken";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            GW_apiToken = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_apiToken : " + GW_apiToken);

            ServerAddr = "http://" + GW_IP + "/encryptdata";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            String GW_BUF = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_BUF : " + GW_BUF);
            
            if (GW_BUF != "" && GW_apiToken != "")
            {
              GW_senddata(GW_BUF ,GW_apiToken);
              GW_BUF = "";
              GW_apiToken = "";
            }
          } else
          {
            GW_MacAdd = MacBuff;
            ServerAddr = "http://" + GW_IP + "/email";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            BUFFER = httpGETRequest(ServerAddr.c_str());

            deserializeJson(JsonEmail ,BUFFER);
            GW_Email = JsonEmail["email"].as<String>();
            GW_EmailPass = JsonEmail["emailpass"].as<String>();

            Serial.println("[A]: GW_Email : " + GW_Email);
      
                
            if (GW_Email != "" && GW_EmailPass != "")
            {
              GW_loginToServer();

              if(GW_code == "true" && GW_apiToken != "") 
              {
                GW_registerToServer();

                if (GW_code == "true" && GW_PER != "")
                {
                  // Send apitoken .
                  ServerAddr = "http://" + GW_IP + "/api";
                  httpPOSTrequest(ServerAddr.c_str(), GW_apiToken);
                  GatewayMacadd.Device7Status = true;
                }
              }
            }
          }
        }
      
        if (Macflag[2 * i + 1] == 8)
        {
          clear[7] = 0;
          char n[18];
          sprintf(n,"%02X:%02X:%02X:%02X:%02X:%02X" ,station.mac[0] ,station.mac[1] ,station.mac[2], station.mac[3], station.mac[4] ,station.mac[5]);
          MacBuff = String(n);

          GW_IP = String(ip4addr_ntoa(reinterpret_cast<ip4_addr_t*>(&(station.ip))));

          if (GatewayMacadd.Device8Status)
          {
            ServerAddr = "http://" + GW_IP + "/apitoken";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            GW_apiToken = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_apiToken : " + GW_apiToken);

            ServerAddr = "http://" + GW_IP + "/encryptdata";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            String GW_BUF = httpGETRequest(ServerAddr.c_str());
            Serial.println("[A]: GW_BUF : " + GW_BUF);
            
            if (GW_BUF != "" && GW_apiToken != "")
            {
              GW_senddata(GW_BUF ,GW_apiToken);
              GW_BUF = "";
              GW_apiToken = "";
            }
          } else
          {
            GW_MacAdd = MacBuff;
            ServerAddr = "http://" + GW_IP + "/email";
            Serial.println("[A]: ServerAddr : " + ServerAddr);

            BUFFER = httpGETRequest(ServerAddr.c_str());

            deserializeJson(JsonEmail ,BUFFER);
            GW_Email = JsonEmail["email"].as<String>();
            GW_EmailPass = JsonEmail["emailpass"].as<String>();

            Serial.println("[A]: GW_Email : " + GW_Email);
      
                
            if (GW_Email != "" && GW_EmailPass != "")
            {
              GW_loginToServer();

              if(GW_code == "true" && GW_apiToken != "") 
              {
                GW_registerToServer();

                if (GW_code == "true" && GW_PER != "")
                {
                  // Send apitoken .
                  ServerAddr = "http://" + GW_IP + "/api";
                  httpPOSTrequest(ServerAddr.c_str(), GW_apiToken);
                  GatewayMacadd.Device8Status = true;
                }
              }
            }
          }
        }
        
        delay(4000);
      }

      for (int i = 0; i < 8; i++)
      {
        if (clear[i])
        {
          if (i == 0)
          {
            GatewayMacadd.Device1Mac = "";
            GatewayMacadd.Device1Status = false;
          }
          if (i == 1)
          {
            GatewayMacadd.Device2Mac = "";
            GatewayMacadd.Device2Status = false;
          }
          if (i == 2)
          {
            GatewayMacadd.Device3Mac = "";
            GatewayMacadd.Device3Status = false;
          }
          if (i == 3)
          {
            GatewayMacadd.Device4Mac = "";
            GatewayMacadd.Device4Status = false;
          }
          if (i == 4)
          {
            GatewayMacadd.Device5Mac = "";
            GatewayMacadd.Device5Status = false;
          }
          if (i == 5)
          {
            GatewayMacadd.Device6Mac = "";
            GatewayMacadd.Device6Status = false;
          }
          if (i == 6)
          {
            GatewayMacadd.Device7Mac = "";
            GatewayMacadd.Device7Status = false;
          }
          if (i == 7)
          {
            GatewayMacadd.Device8Mac = "";
            GatewayMacadd.Device8Status = false;
          }  
        }
      }
      
      
      SaveDeviceParameter();

      GW_StartSending = false;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
      if (SSIDsg != "")
      {
        WiFi.begin(SSIDsg.c_str(), Passsg.c_str());
          
        Serial.println();
        Serial.print("[A]: GateWay WiFiOff , Try To Connecting To ");
        Serial.print(SSIDsg.c_str());

        int StartTime = 0;

        while (WiFi.status() != WL_CONNECTED && ( (millis() - StartTime) < 3000) )
        {
          Serial.print(" .");
          delay(500);
        }
        
      } else {
        ReConfigPortal();
      } 

      delay(1000);
    }
  }
  
  if (SelectedBoard == SmartPlug)
  {
    if (SM_WiFiInterruptFlag) 
    {
      ReConfigPortal();
      SM_Pressed = false;
      SM_WiFiInterruptFlag = false;
    }

    if (SM_CalculateFlag) 
    {
      resetParameters();

      Serial.println("[A]: Start Calculating.");
      calculateANDwritenergy();

      SP_makeJSONdata();
      SP_data();

      SM_CalculateFlag = false;
    }

    if (WiFi.status() != WL_CONNECTED) 
    {
      if (SSIDcg != "")
      {
        WiFi.begin(SSIDcg.c_str(), Passcg.c_str());
    
        Serial.print("\n[A]: GateWay WiFiOff , Try To Connecting To ");
        Serial.println(SSIDcg.c_str());

        int StartTime = 0;

        while (WiFi.status() != WL_CONNECTED && ( (millis() - StartTime) < 3000) )
        {
          Serial.print(" .");
          delay(500);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
          SP_SHA256_key();
          SP_login();
        }
        
      } else 
      {
        ReConfigPortal();
      } 

      delay(1000);
    }
  } 
  
  if (SelectedBoard != EnergyMonitor && SelectedBoard != GateWay && SelectedBoard != SmartPlug) 
  {
    Serial.println("\n\n[A]: Starting ReConfigPortal Function In loop.");
    ReConfigPortal();
  }
  
  delay(100);
}


/*  *** Energy Monitor Function Defenition ***  */
void GetEnergyMonitorParameters(int Type)
{
  if (Type == 1)
  {
    SSID_EM     = NVS.getString(addrSSID_EM);
    Pass_EM     = NVS.getString(addrPass_EM);
    UserName_EM = NVS.getString(addrUserName_EM);
    UserPass_EM = NVS.getString(addrUserPass_EM);
    ApiToken_EM = NVS.getString(addr_ApiToken_EM);
    SiteIP      = NVS.getString(addr_SiteIP);

    Serial.println("[A]: Get Energy Monitor Parameters (type 1) -->");
    Serial.println("\n\n");
    Serial.print("[A]: SSID Energy Monitor : ");
    Serial.println(SSID_EM);
    Serial.print("[A]: Pass Energy Monitor : ");
    Serial.println(Pass_EM);

    Serial.print("[A]: UserName Energy Monitor : ");
    Serial.println(UserName_EM);
    Serial.print("[A]: UserPass Energy Monitor : ");
    Serial.println(UserPass_EM);

    Serial.println("[A]: Energy Monitor Token Is : ");
    Serial.println(ApiToken_EM);

    Serial.println("[A]: SiteIP Is : ");
    Serial.println(SiteIP);
    Serial.println("\n\n");
  }

  if (Type == 2)
  {
    SSID_EM = NVS.getString(addrSSID_EM);
    Pass_EM = NVS.getString(addrPass_EM);

    Serial.println("\n\n");
    Serial.println("[A]: Get Energy Monitor Parameters (type 2) -->");
    Serial.print("[A]: SSID Energy Monitor : ");
    Serial.println(SSID_EM);
    Serial.print("[A]: Pass Energy Monitor : ");
    Serial.println(Pass_EM);
    Serial.println("\n\n");
  }
}

void SaveEnergyMonitorParameters(int Type)
{
  if (Type == 1)
  {
    Serial.println("\n\n");
    Serial.println("[A]: Save Energy Monitor Parameters -->");
    Serial.print("[A]: SSID Energy Monitor : ");
    Serial.println(SSID_EM);
    Serial.print("[A]: Pass Energy Monitor : ");
    Serial.println(Pass_EM);

    Serial.print("[A]: UserName Energy Monitor : ");
    Serial.println(UserName_EM);
    Serial.print("[A]: UserPass Energy Monitor : ");
    Serial.println(UserPass_EM);

    Serial.print("[A]: SiteIP Energy Monitor : ");
    Serial.println(SiteIP);
    Serial.println("\n\n");

    NVS.putString(addrSSID_EM, SSID_EM);
    NVS.putString(addrPass_EM, Pass_EM);
    NVS.putString(addrUserName_EM, UserName_EM);
    NVS.putString(addrUserPass_EM, UserPass_EM);
    NVS.putString(addr_SiteIP ,SiteIP);
  }
}

void EM_SHA256_key(void)
{
  char SHA256key_EM[17];
  char payload[100];
  byte shaResult[32];

  String str = UserName_EM + UserPass_EM;
  str.toCharArray(payload, 100);

  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

  const size_t payloadLength = strlen(payload);

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char *) payload, payloadLength);
  mbedtls_md_finish(&ctx, shaResult);
  mbedtls_md_free(&ctx);
  
  String HashOut = "";

  for (int i = 0; i < 16; i++)
  {
    char str[3];
    sprintf(str, "%02x", (int)shaResult[i]);
    HashOut = HashOut + str;
  }

  HashOut.toCharArray(SHA256key_EM, 17);
  Serial.println("[A]: Shakey Is : ");
  Serial.println(HashOut);
  NVS.putString(addr_ShaKey_EM ,HashOut);
}

void EM_TimerInit(void)
{
  if (ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS * 1000, EM_TimerHandler1))
    Serial.println("[A]: Starting  ITimer1 OK");
  else
    Serial.println("[A]: Can't set ITimer1. Select another freq. or timer");

  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, EM_TimerHandler0))
    Serial.println("[A]: Starting  ITimer0 OK");
  else
    Serial.println("[A]: Can't set ITimer0. Select another freq. or timer");
}

void IRAM_ATTR EM_TimerHandler0(void)
{
  EM_Counter++;

  if (EM_Counter == EM_SendDataPeriod)
  {
    EM_SendDataToServerFlag = true;
  }
}

void IRAM_ATTR EM_TimerHandler1(void)
{
  EM_CalculateFlag = true;
}

void EM_PinInit(void)
{
  pinMode(inPinCurrent, INPUT);
  pinMode(inPinVoltage, INPUT);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  
  pinMode(SM_TRIGGER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SM_TRIGGER_PIN), EM_WiFiInterrupt, FALLING);
}

void EM_SendData(void)
{
  Serial.println("----------------DATA----------------");
  // String dest = "http://" + EM_serverURL + "device/data" + "?" + "token=" + ApiToken_EM;
  char NO[2];

  String SiteAddrBuff = HttpString + SiteIP + DataString;
  EM_sendpkt(4, SiteAddrBuff + ApiToken_EM, NO);
}

void EM_sendpkt(int type, String dest, char * payload)
{
  char SHA256key_EM[17];
  String HashOut = NVS.getString(addr_ShaKey_EM);
  HashOut.toCharArray(SHA256key_EM, 17);

  char *key = (char*) SHA256key_EM;
  int httpCode = 0;
  uint32_t startMillis = millis(); 

  while (1)
  {
    EM_SetConnection(dest);

     
    Serial.println("** Before HTTP Post **");

    if (type == 1) 
    {
      Serial.println("\n\nSend Login PKT");
      httpCode = http.POST((char *) payload);   
    }

    if (type == 2) 
    {
      Serial.println("\n\nSend Register PKT");
      httpCode = http.POST((char *) payload);
    }

    if (type == 3) 
    {
      Serial.println("Send Energy PKT");
      httpCode = http.GET();
    }

    if (type == 4)
    {
      Serial.println("Send Data PKT");
      EM_makeJSONdata();
      httpCode = http.POST((char *) EM_DSTSend);
    }

    Serial.print("\n\n[A]: HTTP Response Delay=");
    Serial.print(millis()-startMillis);
    Serial.println("ms");
    // delay(21000);
    Serial.print("[A]: HTTP Code Is = ");
    Serial.println(httpCode);

    // char *key = (char*) primary_key;
    String code = "";
    StaticJsonDocument<300>  ServerResponseJsonBuffer;

    if ( (httpCode == 200) || (httpCode == 1) )
    {
      String Str = http.getString();
      Serial.println("\n\n[A]: Send Data : " + Str);

      if (type == 1)
      {
        deserializeJson(ServerResponseJsonBuffer, Str);
        code = ServerResponseJsonBuffer["success"].as<String>();
        ApiToken_EM = ServerResponseJsonBuffer["token"].as<String>();

        NVS.putString(addr_ApiToken_EM ,ApiToken_EM);
      }

      if (type == 2)
      {
        deserializeJson(ServerResponseJsonBuffer, Str);
        code = ServerResponseJsonBuffer["success"].as<String>();
        String PER = ServerResponseJsonBuffer["period"].as<String>();
        // last_version = ServerResponseJsonBuffer["version"].as<String>();

        int Period = PER.toInt();
        NVS.putInt(addr_Period_EM ,Period);

        if (EM_SendDataPeriod != Period)
        {
          EM_SendDataPeriod = Period;
        }
      }

      if (type == 3)
      {
        deserializeJson(ServerResponseJsonBuffer, Str);
        code = ServerResponseJsonBuffer["success"].as<String>();
      }

      if (type == 4)
      {
        char DSTRecive[409];
        Str.toCharArray(DSTRecive, 409);
        unsigned char plainText[17] = "" ;
        unsigned char cipherTextOutput[17] = "";
        unsigned char decipheredTextOutput[17] = "";
        unsigned char finalCipheredText[305] = "";
        unsigned char finalDecipheredText[305] = "";

        size_t dlen1 = 304;
        unsigned char  base64_decoded[dlen1];
        size_t  olen1;
        size_t dlen = 409;
        size_t slen1 = dlen - 1;

        int result = mbedtls_base64_decode((unsigned char*)base64_decoded, dlen1, &olen1, ( unsigned char*)DSTRecive, slen1);
        
        //Serial.println("result encode:");
        //Serial.println(result);
        //Serial.println("slen:");
        //Serial.println(slen);
        //Serial.println("olen:");
        //Serial.println( olen);
        //Serial.println("base64 encrypted:");
        //for (int i=0; i<dlen; i++)
        // Serial.print((char)DSTRecive[i]);
        //Serial.println();
        
        //Decrypting base64 encoded:

        for (int i = 0; i < 19; i++) 
        {
          for (int j = 0; j < 16; j++) 
          {
            cipherTextOutput[j] = base64_decoded[i * 16 + j];
          }

          EM_decrypt(cipherTextOutput, key, (unsigned char*)plainText);

          for (int k = i * 16; k < (i + 1) * 16; k++)
          {
            finalDecipheredText[k] = plainText[k % 16];
          }   
        }

        String s((const __FlashStringHelper*) finalDecipheredText);
        Serial.println("S is :");
        Serial.println(s);
        deserializeJson(ServerResponseJsonBuffer, s);
        code = ServerResponseJsonBuffer["success"].as<String>();
        // String StatusRelay = ServerResponseJsonBuffer["status"].as<String>();
      }

      if (code == "true")
      {
        Serial.println("\n\n[A]: True Recieved From Site.");
        http.end();                                                      //close http socket
        client.stop();                                                   //Close connection
        break;                                                           // break from while (1)
      }
    }
    
    if (millis() - startMillis > EM_SendPktTimeOut)
    {
      break;
    }

    Serial.print("\n\nVariable Code Is :");
    Serial.println(code);
    Serial.println("http 200ok or true not recieved");
    http.end();                                                         //close http socket
    client.stop();                                                      //Close connection
  }
}

void EM_WiFiInterrupt(void)
{
  if (!EM_Pressed)
  {
    EM_Pressed = true;
    EM_WiFiInterruptFlag = true;
  }
}

void EM_SetConnection(String dest)
{
  int EM_Port = 80;  
  int EM_ClientConnectTimeOut = 60000;                                       // Used In EM_SetConnection() Function When Not Connect To Server.
  int EM_HttpReadTimeOut = 60000;                                            // Used In EM_SetConnection() Function When Connect To Server.

  while (1)
  {
    EM_HTTP_Handle.begin(dest);
    EM_HTTP_Handle.addHeader("Content-Type", "application/json");
    

    bool c = EM_Client_Handle.connect(SiteIP.c_str(), EM_Port);

    if (c == 1)
    {
      Serial.println("Connected to Smart Energy Site");
      EM_HTTP_Handle.setTimeout(EM_HttpReadTimeOut);
    }
    else
    {
      EM_HTTP_Handle.setConnectTimeout(EM_ClientConnectTimeOut);
    }

    if ((c == true) || (c == 1)) break;
  }
}

// This Function crete Json Document Of Calculate Data.
void EM_makeJSONdata(void)
{
  char primary_key[17];
  String HashOut = NVS.getString(addr_ShaKey_EM);
  HashOut.toCharArray(primary_key, 17);

  char *key = (char*) primary_key;
  StaticJsonDocument<305> jsonBuffer;

  JsonObject Roott = jsonBuffer.to<JsonObject>();
  Roott["mac"] = String(WiFi.macAddress());
  
  JsonObject Sensors = Roott.createNestedObject("sensors");
  Sensors["energy"]       = energy;
  Sensors["voltage"]      = voltage;               
  Sensors["powerfactor"]  = PF;
  Sensors["THDi"]         = THD_current;
  Sensors["THDv"]         = THD_voltage;             
  Sensors["current"]      = current;
  Sensors["ver"]          = "1";
  
  char JsonMessageBuffer[305] = "";
  serializeJsonPretty(jsonBuffer, JsonMessageBuffer);
  // Serial.println(JsonMessageBuffer);

  char buff[305] = "";
  int k = 0;
  size_t nullTerm = strlen(JsonMessageBuffer);                                                      // Get current length
  for (size_t i = 0; i < nullTerm; ++i) 
  {
    if ((JsonMessageBuffer[i] != '\n') && (JsonMessageBuffer[i] != '\b') && (JsonMessageBuffer[i] != '\f') && (JsonMessageBuffer[i] != '\r') && (JsonMessageBuffer[i] != '\t') && (JsonMessageBuffer[i] != '\\') && (JsonMessageBuffer[i] != ' ')) 
    {
      buff[k] = JsonMessageBuffer[i];                                                               // Copy character 'up'
      k = k + 1;
    }
  }

  for (size_t i = k; i < 304; ++i) 
  {
    buff[i] = ' ';
  }

  Serial.println("Original JSON:(Plain Text):");
  Serial.println(JsonMessageBuffer);
  Serial.println(strlen(JsonMessageBuffer));
  Serial.println("Compress JSON:(Plain Text):");
  Serial.println(buff);
  Serial.println(strlen(buff));

  // Encrypting JSON:
  
  for (int i = 0; i < 19; i++) {
    for (int j = 0; j < 16; j++)  {
      EM_plainText[j] = buff[i * 16 + j];
    }
    EM_encrypt(EM_plainText, key, EM_cipherTextOutput);
    for (int k = i * 16; k < (i + 1) * 16; k++)
      EM_finalCipheredText[k] = EM_cipherTextOutput[k % 16];
  }

  // base64 encrypting:
  // src=JSONmessageBuffer;

  char * src = (char*)EM_finalCipheredText;
  size_t  olen;
  int result = mbedtls_base64_encode((unsigned char*)EM_DSTSend, EM_dlen, &olen, (const unsigned char*)src, EM_slen);

  Serial.println("result encode:");
  Serial.println(result);
  Serial.println("slen:");
  Serial.println(EM_slen);
  Serial.println("olen:");
  Serial.println( olen);

  Serial.println("base64 encrypted:");
  Serial.print((char *)EM_DSTSend);
  Serial.println();
}

// Send User Name And EM_Pass Word To Server To Get APITOKEN.
void EM_loginToServer(void)
{
  Serial.println("----------------LOGIN----------------");
  StaticJsonDocument<305> LoginJsonBuffer;
  LoginJsonBuffer["email"] = UserName_EM;
  LoginJsonBuffer["password"] = UserPass_EM;

  char JsonMessageBuffer[304] = "";
  serializeJsonPretty(LoginJsonBuffer, JsonMessageBuffer);
  Serial.println(JsonMessageBuffer);
  
  String SiteAddrBuff = HttpString + SiteIP + LoginString;
  EM_sendpkt(1, SiteAddrBuff, JsonMessageBuffer);
}

void EM_registerToServer(void)
{
  Serial.println("----------------َAPI TOKEN----------------");
  Serial.println(ApiToken_EM);
  
  StaticJsonDocument<305> LoginJsonBuffer;
  LoginJsonBuffer["mac"] = String(WiFi.macAddress());
  LoginJsonBuffer["ip"] = String(WiFi.localIP());
  LoginJsonBuffer["type"] = "2";

  char JsonMessageBuffer[304] = "";
  serializeJsonPretty(LoginJsonBuffer, JsonMessageBuffer);
  Serial.println(JsonMessageBuffer);
  String dest = "http://" + EM_serverURL + "device?token=" + ApiToken_EM;
  Serial.println(dest);

  String SiteAddrBuff = HttpString + SiteIP + RegisterString;
  EM_sendpkt(2,SiteAddrBuff + ApiToken_EM, JsonMessageBuffer);
}

String GetMAcAdd(void)
{
  return String(WiFi.macAddress());
}

String GetLast6Mac(void)
{
  String BUFF = GetMAcAdd();
  String OutPutData;

  for (int i = 9; i <= 16; i++)
  {
    OutPutData += BUFF[i];
  }

  return OutPutData;
}

void EM_decrypt(unsigned char * chipherText, char * key, unsigned char * outputBuffer) 
{
  mbedtls_aes_context aes;

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec( &aes, (const unsigned char*) key, strlen(key) * 8 );
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)chipherText, outputBuffer);
  mbedtls_aes_free(&aes);
}

// Encrypting Base On base64
void EM_encrypt(char * plainText, char * key, unsigned char * outputBuffer) 
{

  mbedtls_aes_context aes;

  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, strlen(key) * 8 );
  mbedtls_aes_crypt_ecb( &aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)plainText, outputBuffer);
  mbedtls_aes_free( &aes );
}


/*  *** GateWay Function Defenition ***  */
void GW_PinInit(void)
{
  pinMode(GW_TRIGGER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(GW_TRIGGER_PIN), GW_WiFiInterrupt, FALLING);
}

void GW_WiFiInterrupt(void)
{
  if (!GW_Pressed)
  {
    GW_Pressed = true;
    GW_WiFiInterruptFlag = true;
  }
}

void httpPOSTrequest(const char* serverName, String postdata)
{
  HTTPClient http;   
  String response;

  http.begin(serverName);                                   //Specify destination for HTTP request
  http.addHeader("Content-Type", "text/plain");             //Specify content-type header
  
  int httpResponseCode = http.POST(postdata);   //Send the actual POST request
  
  if(httpResponseCode>0)
  {
    response = http.getString();                       //Get the response to the request
  
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
  }else
  {
    Serial.println("[A]: Error on sending POST: ");
    Serial.print(httpResponseCode);
   }
  
   http.end();  //Free resources
   // return response;
}

String httpGETRequest(const char* serverName)
{
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = ""; 
  
  if ((httpResponseCode == 200) || (httpResponseCode == 1)) 
  {
    //delay(5000);
    payload = http.getString();

    Serial.println("[A]: HTTP Response code: ");
    Serial.println(httpResponseCode);
  }
  else 
  {
    Serial.println("[A]: Error code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();

  return payload;
}

void GW_loginToServer(void)
{
  Serial.println("----------------LOGIN----------------");
  StaticJsonDocument<305> LoginJsonBuffer;
  LoginJsonBuffer["email"] = GW_Email;
  LoginJsonBuffer["password"] = GW_EmailPass;

  char JsonMessageBuffer[304] = "";
  serializeJsonPretty(LoginJsonBuffer, JsonMessageBuffer);
  Serial.println(JsonMessageBuffer);
  String xx;
  
  String SiteAddrBuff = HttpString + SiteIP + LoginString;
  GW_sendpkt(1, SiteAddrBuff, JsonMessageBuffer ,xx);
}

void GW_registerToServer(void)
{
  Serial.println("----------------API TOKEN----------------");
  Serial.println(GW_apiToken);
  
  StaticJsonDocument<305> LoginJsonBuffer;
  LoginJsonBuffer["mac"] = GW_MacAdd;
  LoginJsonBuffer["ip"] = GW_IP;
  LoginJsonBuffer["type"] = "2";

  char JsonMessageBuffer[304] = "";
  serializeJsonPretty(LoginJsonBuffer, JsonMessageBuffer);
  Serial.println(JsonMessageBuffer);
  String dest = "http://" + EM_serverURL + "device?token=" + GW_apiToken;
  String xx;

  String SiteAddrBuff = HttpString + SiteIP + RegisterString;
  GW_sendpkt(2,SiteAddrBuff + GW_apiToken, JsonMessageBuffer ,xx);
}

void GW_sendpkt(int type, String dest, char * payload ,String sendData)
{
  //int PrimaryKeySize = 0;
  int httpCode = 0;
  char EM_SHA256key[17];
  unsigned long EM_StartMillis = millis(); 
  char *key = (char*) EM_SHA256key;

  while (1)
  {
    EM_SetConnection(dest);

    Serial.println("** Before HTTP Post **");

    if (type == 1) 
    {
      Serial.println("\n\nSend Login PKT");
      httpCode = EM_HTTP_Handle.POST((char *) payload);   
    }

    if (type == 2) 
    {
      Serial.println("\n\nSend Register PKT");
      httpCode = EM_HTTP_Handle.POST((char *) payload);
    }

    if (type == 4)
    {
      Serial.println("Send Data PKT");
      Serial.println(sendData);
      httpCode = EM_HTTP_Handle.POST(sendData);
    }

    Serial.print("\n\n[A]: HTTP Response Delay=");
    Serial.print(millis()-EM_StartMillis);
    Serial.println("ms");
    Serial.print("[A]: HTTP Code Is = ");
    Serial.println(httpCode);

    StaticJsonDocument<300>  ServerResponseJsonBuffer;

    if ( (httpCode == 200) || (httpCode == 1) )
    {
      String Str = EM_HTTP_Handle.getString();
      Serial.println("\n\n[A]: Send Data : " + Str);

      if (type == 1)
      {
        deserializeJson(ServerResponseJsonBuffer, Str);
        GW_code = ServerResponseJsonBuffer["success"].as<String>();
        GW_apiToken = ServerResponseJsonBuffer["token"].as<String>();
      }

      if (type == 2)
      {
        deserializeJson(ServerResponseJsonBuffer, Str);
        GW_code = ServerResponseJsonBuffer["success"].as<String>();
        GW_PER = ServerResponseJsonBuffer["period"].as<String>();
        GW_last_version = ServerResponseJsonBuffer["version"].as<String>();
      }

      if (type == 4)
      {

        Serial.println("\n\n[A]: Data Was Sent.");
        EM_HTTP_Handle.end();                                                      //close http socket
        EM_Client_Handle.stop();                                                   //Close connection
        break;
        // char DSTRecive[409];
        // Str.toCharArray(DSTRecive, 409);

        // unsigned char EM_plainText[17] = "" ;
        // unsigned char EM_cipherTextOutput[17] = "";
        // //unsigned char decipheredTextOutput[17] = "";
        // //unsigned char EM_finalCipheredText[305] = "";
        // unsigned char finalDecipheredText[305] = "";

        // size_t dlen1 = 304;
        // unsigned char  base64_decoded[dlen1];
        // size_t  olen1;
        // size_t EM_dlen = 409;
        // size_t slen1 = EM_dlen - 1;

        // mbedtls_base64_decode((unsigned char*)base64_decoded, dlen1, &olen1, ( unsigned char*)DSTRecive, slen1);
        
        // //Serial.println("result encode:");
        // //Serial.println(result);
        // //Serial.println("EM_slen:");
        // //Serial.println(EM_slen);
        // //Serial.println("olen:");
        // //Serial.println( olen);
        // //Serial.println("base64 encrypted:");
        // //for (int i=0; i<EM_dlen; i++)
        // // Serial.print((char)DSTRecive[i]);
        // //Serial.println();
        
        // //Decrypting base64 encoded:

        // for (int i = 0; i < 19; i++) 
        // {
        //   for (int j = 0; j < 16; j++) 
        //   {
        //     EM_cipherTextOutput[j] = base64_decoded[i * 16 + j];
        //   }

        //   EM_decrypt(EM_cipherTextOutput, key, (unsigned char*)EM_plainText);

        //   for (int k = i * 16; k < (i + 1) * 16; k++)
        //   {
        //     finalDecipheredText[k] = EM_plainText[k % 16];
        //   }   
        // }

        // String s((const __FlashStringHelper*) finalDecipheredText);
        // Serial.println("S is :");
        // Serial.println(s);
        // deserializeJson(ServerResponseJsonBuffer, s);
        // GW_code = ServerResponseJsonBuffer["success"].as<String>();
        // GW_StatusRelay = ServerResponseJsonBuffer["status"].as<String>();
      }

      if (GW_code == "true")
      {
        Serial.println("\n\n[A]: True Recieved From Site.");
        EM_HTTP_Handle.end();                                                      //close http socket
        EM_Client_Handle.stop();                                                   //Close connection
        break;                                                           // break from while (1)
      }
    }
    
    if (millis() - EM_StartMillis > EM_SendPktTimeOut)
    {
      break;
    }

    Serial.print("\n\nVariable Code Is :");
    Serial.println(GW_code);
    Serial.println("http 200ok or true not recieved");
    EM_HTTP_Handle.end();
    EM_Client_Handle.stop();
  }
}

void GW_senddata(String data ,String api)
{
  Serial.println("----------------DATA----------------");
  // String dest = "http://" + EM_serverURL + "device/data" + "?" + "token=" + api;
  char NO[2];
  Serial.println(api);

  String SiteAddrBuff = HttpString + SiteIP + DataString;
  GW_sendpkt(4, SiteAddrBuff + api, NO ,data);
}

void GW_TimerInit(void)
{
  if (ITimer2.attachInterruptInterval(TIMER2_INTERVAL_MS * 1000, TimerHandler2))
      Serial.println("Starting  ITimer2 OK");
    else
      Serial.println("Can't set ITimer2. Select another freq. or timer");
}

void IRAM_ATTR TimerHandler2(void)
{
  GW_StartSending = true;
}

void SaveGateWayParameters(int Type)
{
  if (Type == 1)
  {
    Serial.println("\n\n");
    Serial.println("[A]: Save GateWay Parameters -->");
    Serial.print("[A]: SSID Smart Plug : ");
    Serial.println(SSIDsg);
    Serial.print("[A]: Pass Smart Plug : ");
    Serial.println(Passsg);

    Serial.print("[A]: UserName Smart Plug : ");
    Serial.println(UserNamesg);
    Serial.print("[A]: UserPass Smart Plug : ");
    Serial.println(UserPasssg);

    Serial.print("[A]: SiteIP Smart Plug : ");
    Serial.println(SiteIP);
    Serial.println("\n\n");

    NVS.putString(addrSSIDsg, SSIDsg);
    NVS.putString(addrPasssg, Passsg);
    NVS.putString(addrUserNameSG, UserNamesg);
    NVS.putString(addrUserPassSG, UserPasssg);
    NVS.putString(addr_SiteIP ,SiteIP);
  }
}

void GetGateWayParameters(int Type)
{
  if (Type == 1)
  {
    SSIDsg     = NVS.getString(addrSSIDsg);
    Passsg     = NVS.getString(addrPasssg);
    UserNamesg = NVS.getString(addrUserNameSG);
    UserPasssg = NVS.getString(addrUserPassSG);
    SiteIP     = NVS.getString(addr_SiteIP);
    
    Serial.println("[A]: Get GateWay Parameters (type 1) -->");
    Serial.println("\n\n");
    Serial.print("[A]: SSID GateWay : ");
    Serial.println(SSIDsg);
    Serial.print("[A]: Pass GateWay : ");
    Serial.println(Passsg);

    Serial.print("[A]: UserName GateWay : ");
    Serial.println(UserNamesg);
    Serial.print("[A]: UserPass GateWay : ");
    Serial.println(UserPasssg);

    Serial.print("[A]: SiteIP GateWay : ");
    Serial.println(SiteIP);
    Serial.println("\n\n");
  }

  if (Type == 2)
  {
    SSIDsg     = NVS.getString(addrSSIDsg);
    Passsg     = NVS.getString(addrPasssg);

    Serial.println("[A]: Get GateWay Parameters (type 2) -->");
    Serial.println("\n\n");
    Serial.print("[A]: SSID GateWay : ");
    Serial.println(SSIDsg);
    Serial.print("[A]: Pass GateWay : ");
    Serial.println(Passsg);
    Serial.println("\n\n");
  }
}

void SaveDeviceParameter(void)
{
  Serial.println("\n\n");
  Serial.println("[A]: Save Gateway Device -->");
  Serial.println(GatewayMacadd.Device1Mac);
  Serial.println(GatewayMacadd.Device1Status);
  Serial.println(GatewayMacadd.Device2Mac);
  Serial.println(GatewayMacadd.Device2Status);
  Serial.println(GatewayMacadd.Device3Mac);
  Serial.println(GatewayMacadd.Device3Status);
  Serial.println(GatewayMacadd.Device4Mac);
  Serial.println(GatewayMacadd.Device4Status);
  Serial.println(GatewayMacadd.Device5Mac);
  Serial.println(GatewayMacadd.Device5Status);
  Serial.println(GatewayMacadd.Device6Mac);
  Serial.println(GatewayMacadd.Device6Status);
  Serial.println(GatewayMacadd.Device7Mac);
  Serial.println(GatewayMacadd.Device7Status);
  Serial.println(GatewayMacadd.Device8Mac);
  Serial.println(GatewayMacadd.Device8Status);
  Serial.println("\n\n");

  NVS.putString(addrDevice1       ,GatewayMacadd.Device1Mac);
  NVS.putInt(addrDeviceStatus1 ,GatewayMacadd.Device1Status);
  NVS.putString(addrDevice2       ,GatewayMacadd.Device2Mac);
  NVS.putInt(addrDeviceStatus2 ,GatewayMacadd.Device2Status);
  NVS.putString(addrDevice3       ,GatewayMacadd.Device3Mac);
  NVS.putInt(addrDeviceStatus3 ,GatewayMacadd.Device3Status);
  NVS.putString(addrDevice4       ,GatewayMacadd.Device4Mac);
  NVS.putInt(addrDeviceStatus4 ,GatewayMacadd.Device4Status);
  NVS.putString(addrDevice5       ,GatewayMacadd.Device5Mac);
  NVS.putInt(addrDeviceStatus5 ,GatewayMacadd.Device5Status);
  NVS.putString(addrDevice6       ,GatewayMacadd.Device6Mac);
  NVS.putInt(addrDeviceStatus6 ,GatewayMacadd.Device6Status);
  NVS.putString(addrDevice7       ,GatewayMacadd.Device7Mac);
  NVS.putInt(addrDeviceStatus7 ,GatewayMacadd.Device7Status);
  NVS.putString(addrDevice8       ,GatewayMacadd.Device8Mac);
  NVS.putInt(addrDeviceStatus8 ,GatewayMacadd.Device8Status);
}

void GetDeviceParameter(void)
{
  GatewayMacadd.Device1Mac    = NVS.getString(addrDevice1);
  GatewayMacadd.Device1Status = NVS.getInt(addrDeviceStatus1);
  GatewayMacadd.Device2Mac    = NVS.getString(addrDevice2);
  GatewayMacadd.Device2Status = NVS.getInt(addrDeviceStatus2);
  GatewayMacadd.Device3Mac    = NVS.getString(addrDevice3);
  GatewayMacadd.Device3Status = NVS.getInt(addrDeviceStatus3);
  GatewayMacadd.Device4Mac    = NVS.getString(addrDevice4);
  GatewayMacadd.Device4Status = NVS.getInt(addrDeviceStatus4);
  GatewayMacadd.Device5Mac    = NVS.getString(addrDevice5);
  GatewayMacadd.Device5Status = NVS.getInt(addrDeviceStatus5);
  GatewayMacadd.Device6Mac    = NVS.getString(addrDevice6);
  GatewayMacadd.Device6Status = NVS.getInt(addrDeviceStatus6);
  GatewayMacadd.Device7Mac    = NVS.getString(addrDevice7);
  GatewayMacadd.Device7Status = NVS.getInt(addrDeviceStatus7);
  GatewayMacadd.Device8Mac    = NVS.getString(addrDevice8);
  GatewayMacadd.Device8Status = NVS.getInt(addrDeviceStatus8);

  Serial.println("\n\n");
  Serial.println("[A]: Get Gateway Device -->");
  Serial.println(GatewayMacadd.Device1Mac);
  Serial.println(GatewayMacadd.Device1Status);
  Serial.println(GatewayMacadd.Device2Mac);
  Serial.println(GatewayMacadd.Device2Status);
  Serial.println(GatewayMacadd.Device3Mac);
  Serial.println(GatewayMacadd.Device3Status);
  Serial.println(GatewayMacadd.Device4Mac);
  Serial.println(GatewayMacadd.Device4Status);
  Serial.println(GatewayMacadd.Device5Mac);
  Serial.println(GatewayMacadd.Device5Status);
  Serial.println(GatewayMacadd.Device6Mac);
  Serial.println(GatewayMacadd.Device6Status);
  Serial.println(GatewayMacadd.Device7Mac);
  Serial.println(GatewayMacadd.Device7Status);
  Serial.println(GatewayMacadd.Device8Mac);
  Serial.println(GatewayMacadd.Device8Status);
  Serial.println("\n\n");
}


/*  *** Smart Plug Function Defenition ***  */
void SP_data(void)
{
  StaticJsonDocument<305> jsonBuffer;

  JsonObject Roott = jsonBuffer.to<JsonObject>();
  Roott["mac"] = String(WiFi.macAddress());
  
  JsonObject Sensors = Roott.createNestedObject("sensors");
  Sensors["energy"]       = energy;
  Sensors["voltage"]      = voltage;               
  Sensors["powerfactor"]  = PF;
  Sensors["THDi"]         = EM_THD_current;
  Sensors["THDv"]         = EM_THD_voltage;             
  Sensors["current"]      = current;
  Sensors["ver"]          = "1";
  
  
  serializeJsonPretty(jsonBuffer, SP_EncryptedData);
}

void SP_login(void)
{
  Serial.println("----------------LOGIN----------------");
  StaticJsonDocument<305> LoginJsonBuffer;
  LoginJsonBuffer["email"] = TestUser;
  LoginJsonBuffer["password"] = UserPasscg;

  
  serializeJsonPretty(LoginJsonBuffer, UserAccont);
  Serial.println(UserAccont);
}

void SM_PinInit(void)
{
  pinMode(inPinCurrent, INPUT);
  pinMode(inPinVoltage, INPUT);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  
  pinMode(SM_TRIGGER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SM_TRIGGER_PIN), SM_WiFiInterrupt, FALLING);
}

void SM_ServerInit(void)
{
  // Get.
  AcyncWeb.on("/email", 
              HTTP_GET, 
              [](AsyncWebServerRequest *request){
                request->send_P(200, "text/plain", UserAccont);
              });

  AcyncWeb.on("/apitoken", 
              HTTP_GET, 
              [](AsyncWebServerRequest *request){
                request->send_P(200, "text/plain", SM_ApiToken.c_str());
              });

  AcyncWeb.on("/encryptdata", 
              HTTP_GET, 
              [](AsyncWebServerRequest *request){
                request->send_P(200, "text/plain", (char *) EM_DSTSend);
              });

  AcyncWeb.on("/data", 
              HTTP_GET, 
              [](AsyncWebServerRequest *request){
                request->send_P(200, "text/plain", (char *) SP_EncryptedData);
              });

  // Post.
  AcyncWeb.on(
              "/api",
              HTTP_POST,
              [](AsyncWebServerRequest * request){},
              NULL,
              [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
                  
                SM_ApiToken = String((char*) data);
                NVS.putString(addrSP_ApiToken ,SM_ApiToken);
                request->send(200);
              });

  AcyncWeb.begin();

  Serial.println("[A]: Server Was Initialized.");
}

void SM_TimerInit(void)
{
  if (ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS * 1000, SM_Timer1Interrupt))
    Serial.println("[A]: Starting  ITimer0 OK");
  else
    Serial.println("[A]: Can't set ITimer0. Select another freq. or timer");
}

void IRAM_ATTR SM_Timer1Interrupt(void)
{
  SM_CalculateFlag = true;
}

void SM_WiFiInterrupt(void)
{
  if (!SM_Pressed)
  {
    SM_Pressed = true;
    SM_WiFiInterruptFlag = true;
  }
}

void SP_makeJSONdata(void)
{
  char SP_SHA256key[17];
  String HashOut;

  HashOut = NVS.getString(addrSP_ShaKey);
  HashOut.toCharArray(SP_SHA256key, 17);

  char *key = (char*) SP_SHA256key;
  StaticJsonDocument<305> jsonBuffer;

  JsonObject Roott = jsonBuffer.to<JsonObject>();
  Roott["mac"] = String(WiFi.macAddress());
  
  JsonObject Sensors = Roott.createNestedObject("sensors");
  Sensors["energy"]       = energy;
  Sensors["voltage"]      = voltage;               
  Sensors["powerfactor"]  = PF;
  Sensors["THDi"]         = THD_current;
  Sensors["THDv"]         = THD_voltage;             
  Sensors["current"]      = current;
  Sensors["ver"]          = "1";
  
  char JsonMessageBuffer[305] = "";
  serializeJsonPretty(jsonBuffer, JsonMessageBuffer);
  Serial.println(JsonMessageBuffer);

  char buff[305] = "";
  int k = 0;
  size_t nullTerm = strlen(JsonMessageBuffer);                                                      // Get current length
  for (size_t i = 0; i < nullTerm; ++i) 
  {
    if ((JsonMessageBuffer[i] != '\n') && (JsonMessageBuffer[i] != '\b') && (JsonMessageBuffer[i] != '\f') && (JsonMessageBuffer[i] != '\r') && (JsonMessageBuffer[i] != '\t') && (JsonMessageBuffer[i] != '\\') && (JsonMessageBuffer[i] != ' ')) 
    {
      buff[k] = JsonMessageBuffer[i];                                                               // Copy character 'up'
      k = k + 1;
    }
  }

  for (size_t i = k; i < 304; ++i) 
  {
    buff[i] = ' ';
  }

  // Serial.print("\n[A]: Original JSON (Plain Text) : ");
  // Serial.println(JsonMessageBuffer);
  // Serial.println(strlen(JsonMessageBuffer));
  // Serial.print("[A]: Compress JSON (Plain Text) : ");
  // Serial.println(buff);
  // Serial.println(strlen(buff));

  // Encrypting JSON:
  
  for (int i = 0; i < 19; i++) {
    for (int j = 0; j < 16; j++)  {
      EM_plainText[j] = buff[i * 16 + j];
    }
    EM_encrypt(EM_plainText, key, EM_cipherTextOutput);
    for (int k = i * 16; k < (i + 1) * 16; k++)
      EM_finalCipheredText[k] = EM_cipherTextOutput[k % 16];
  }

  // base64 encrypting:
  // src=JSONmessageBuffer;

  char * src = (char*)EM_finalCipheredText;
  size_t  olen;
  mbedtls_base64_encode((unsigned char*)SP_DSTSend, EM_dlen, &olen, (const unsigned char*)src, EM_slen);

  // Serial.println("result encode:");
  // Serial.println(result);
  // Serial.println("EM_slen:");
  // Serial.println(EM_slen);
  // Serial.println("olen:");
  // Serial.println( olen);

  // Serial.print("[A]: base64 encrypted:");
  // Serial.println((char *)SP_DSTSend);
}

void SP_SHA256_key(void)
{
  char SP_SHA256key[17];
  char payload[100];
  byte shaResult[32];

  String str = TestUser + UserPasscg;
  str.toCharArray(payload, 100);

  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

  const size_t payloadLength = strlen(payload);

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char *) payload, payloadLength);
  mbedtls_md_finish(&ctx, shaResult);
  mbedtls_md_free(&ctx);
  
  String HashOut = "";

  for (int i = 0; i < 16; i++)
  {
    char str[3];
    sprintf(str, "%02x", (int)shaResult[i]);
    HashOut = HashOut + str;
  }

  HashOut.toCharArray(SP_SHA256key, 17);
  Serial.println("[A]: Shakey Is : ");
  Serial.println(HashOut);
  NVS.putString(addrSP_ShaKey ,HashOut);
}

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
  while ( (millis() - startTime) < Time) {

    volt = analogRead(inPinVoltage);
    voltage_fft = volt - dc_voltage;
    v[No] = Voltage_Scale_FFT* ((voltage_fft * (3.3)) / 4095);
    current_fft = analogRead(inPinCurrent);
    current_fft = current_fft - dc_current;
    i[No] = Current_Scale_FFT * ((current_fft * (3.3)) / 4095);
    No++;
  }

  No--;
  for (int h = 1; h <= K; h++) {
    for (int k = 0; k < No ; k++) {
      //    for (int k = 1; k < 850; k++) {
      xv[h] += Complex(v[k], 0.0) * (Complex(cos(2 * PI * h * k / No), 0.0)    + j * Complex(sin(2 * PI * h * k / No), 0.0));
      xi[h] += Complex(i[k], 0.0) * (Complex(cos(2 * PI * h * k / No), 0.0) + j * Complex(sin(2 * PI * h * k / No), 0.0));
    }
  }


  for (int i = 1; i <= K; i++) {
    tv[i] = atan(xv[i].imag() / xv[i].real());
    ti[i] = atan(xi[i].imag() / xi[i].real());
  }

  for (int i = 1; i <= K; i++) {
    abs_xv[i] = (2 * sqrt(xv[i].real() * xv[i].real() + xv[i].imag() * xv[i].imag())) / No;
    abs_xi[i] = (2 * sqrt(xi[i].real() * xi[i].real() + xi[i].imag() * xi[i].imag())) / No;
  }

  for (int h = 1; h <= K; h++) {
    p[h] = 0.5 * abs_xv[h] * abs_xi[h] * cos(tv[h] - ti[h]);
    xp += p[h];
    q[h] = 0.5 * abs_xv[h] * abs_xi[h] * sin(tv[h] - ti[h]);
    xq += q[h];
    XV = (abs_xv[h] / sqrt(2)) * (abs_xv[h] / sqrt(2)) + XV;
    XI =  (abs_xi[h] / sqrt(2)) * (abs_xi[h] / sqrt(2)) + XI;
  }
  for (int h = 2; h <= K; h++) {
    hV = abs_xv[h] * abs_xv[h] + hV;
    hI = abs_xi[h] * abs_xi[h] + hI;
  }

  Vrms_fft = sqrt(XV);
  Irms_fft = sqrt(XI);
  S  = Vrms_fft * Irms_fft;

  P  = xp;

  Q  = xq;
  D  = sqrt(S * S - P * P - Q * Q);
  PF_fft = P / S;
  THDv = sqrt(hV) / (abs_xv[1]);
  THDi = sqrt(hI) / (abs_xi[1]);

  /////////////////////////////////CONFIGUING WITH FLUK/////////////////////////////////
  THDv = THDv / 2;
  THDi = THDi * 3 / 4;
  Q  = xq / 10;
  
  THD_voltage = THDv;
  THD_current = THDi;
  voltage = Vrms_fft;
  current = Irms_fft;
  PF =   PF_fft;                         // Must be corrected
  if (PF < 0 )PF = -PF;
  if (P < 0 )P = -P;
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
  calculate(20, 0, inPinVoltage, inPinCurrent);
}

void SP_CalculateEnergy()
{
  double sum = 0;

  energy = (((SM_SendDataPeriod) * power * 0.001) / 3600); //Wh{
  sum = sum + energy;

  if (sum < 0)  sum = 0;
}

void calculateANDwritenergy()
{
  CalculateVI();
  SP_CalculateEnergy();
}

void SaveSmartPlugParameters(int Type)
{
  if (Type == 1)
  {
    Serial.println("\n\n");
    Serial.println("[A]: Save Smart Plug Parameters -->");
    Serial.print("[A]: SSID Smart Plug : ");
    Serial.println(SSIDcg);
    Serial.print("[A]: Pass Smart Plug : ");
    Serial.println(Passcg);

    Serial.print("[A]: UserName Smart Plug : ");
    Serial.println(TestUser);
    Serial.print("[A]: UserPass Smart Plug : ");
    Serial.println(UserPasscg);

    Serial.print("[A]: SiteIP Smart Plug : ");
    Serial.println(SiteIP);
    Serial.println("\n\n");

    NVS.putString(addrSSIDcg, SSIDcg);
    NVS.putString(addrPasscg, Passcg);
    NVS.putString(addrUserNameCG, TestUser);
    NVS.putString(addrUserPassCG, UserPasscg);
    NVS.putString(addr_SiteIP ,SiteIP);
  }
}

void GetSmartPlugParameters(int Type)
{
  if (Type == 1)
  {
    SSIDcg     = NVS.getString(addrSSIDcg);
    Passcg     = NVS.getString(addrPasscg);
    TestUser   = NVS.getString(addrUserNameCG);
    UserPasscg = NVS.getString(addrUserPassCG);
    SiteIP     = NVS.getString(addr_SiteIP);
    
    Serial.println("[A]: Get Smart Plug Parameters (type 1) -->");
    Serial.println("\n\n");
    Serial.print("[A]: SSID Smart Plug : ");
    Serial.println(SSIDcg);
    Serial.print("[A]: Pass Smart Plug : ");
    Serial.println(Passcg);

    Serial.print("[A]: UserName Smart Plug : ");
    Serial.println(TestUser);
    Serial.print("[A]: UserPass Smart Plug : ");
    Serial.println(UserPasscg);

    Serial.print("[A]: SiteIP Smart Plug : ");
    Serial.println(SiteIP);
    Serial.println("\n\n");
  }

  if (Type == 2)
  {
    SSIDcg = NVS.getString(addrSSIDcg);
    Passcg = NVS.getString(addrPasscg);

    Serial.println("\n\n");
    Serial.println("[A]: Get Smart Plug Parameters (type 2) -->");
    Serial.print("[A]: SSID Smart Plug : ");
    Serial.println(SSIDcg);
    Serial.print("[A]: Pass Smart Plug : ");
    Serial.println(Passcg);
    Serial.println("\n\n");
  }
}


// Global Function.
// Creat ConfigPortal Page For User Interface.
void ConfigPortal(void)
{
  switch (SelectedBoard)
  {
    // Energy Monitor.
    case EnergyMonitor:
    { 
      GetEnergyMonitorParameters(1);

      ssid = "EnergyMonitor_" + GetLast6Mac();
      
      Serial.println("\n\n[A]: Starting ConfigPortal On ->" + ssid);

      // Local intialization. Once its business is done, there is no need to keep it around
      // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
      // ESP_WiFiManager ESP_wifiManager;
      // Use this to personalize DHCP hostname (RFC952 conformed)
      ESP_WiFiManager ESP_wifiManager(ssid.c_str());

      ESP_wifiManager.setDebugOutput(true);

      // Use only to erase stored WiFi Credentials
      // resetSettings();
      // ESP_wifiManager.resetSettings();

      //set custom ip for portal
      //ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

      ESP_wifiManager.setMinimumSignalQuality(-1);

      #if !USE_DHCP_IP    
        // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
        ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);   
      #endif                         

      Serial.println("[A]: Stored: SSID = " + SSID_EM + ", Pass = " + Pass_EM);

      ssid.toUpperCase();

      if (SSID_EM == "")
      {
        Serial.println("[A]: Open Config Portal Without Timeout: No Stored Credentials.");
    
        // It Starts An Access Point.
        // And Goes Into A Blocking loop Awaiting Configuration.
        if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str())) {
          Serial.println("[A]: Not Connected To WiFi But Continuing Anyway.");
        } else {
          Serial.println("[A]: WiFi Connected...yeey :)");
        }

        SelectedBoard = ESP_wifiManager.getWitchCode();
        NVS.putInt(addrSelectedBoard, SelectedBoard);

        Serial.print("[A]: SelectedBoard Is : ");
        Serial.println(SelectedBoard);
        
        if (SelectedBoard == EnergyMonitor) 
        {
          EM_ConfigPortalRun = true;
        }
      }
      
      
      if (SelectedBoard == EnergyMonitor) 
      {
        if (EM_ConfigPortalRun) 
        {
          UserName_EM = ESP_wifiManager.getUserName();
          UserPass_EM = ESP_wifiManager.getUserNamePass();

          SSID_EM = ESP_wifiManager.getmSSID();
          Pass_EM = ESP_wifiManager.getmPASS();

          SiteIP = ESP_wifiManager.getSiteAddress1();

          SaveEnergyMonitorParameters(1);

          SM_ConfigPortalRun = false;
        }

        unsigned long startedAt = millis();

        while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
        {
          WiFi.mode(WIFI_STA);
          WiFi.persistent (true);

          // We start by connecting to a WiFi network

          Serial.print("[A]: ConnectingTo ");
          Serial.println(SSID_EM);

          WiFi.config(stationIP, gatewayIP, netMask);
          //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);

          WiFi.begin(SSID_EM.c_str(), Pass_EM.c_str());

          int i = 0;
          while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
          {
            delay(WHILE_LOOP_DELAY);
          }
        }

        Serial.print("[A]: After Waiting ");
        Serial.print((millis() - startedAt) / 1000);
        Serial.print("Secs More In Setup(), Connection Result Is ");
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
          Serial.print("Connected. Local IP: ");
          Serial.println(WiFi.localIP());

          EM_loginToServer();
          EM_registerToServer();
          EM_SHA256_key();
        } else {
          Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
        }
      }
     
      if (SelectedBoard == SmartPlug || SelectedBoard == GateWay)
      {
        SelectedBoard = 0;
      }

      break;
    }
    
    // GateWay.
    case GateWay:
    {
      GetGateWayParameters(1);

      ssid = "GateWay_" + GetLast6Mac();
      
      Serial.println("\n\n[A]: Starting ConfigPortal On ->" + ssid);

      // Local intialization. Once its business is done, there is no need to keep it around
      // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
      // ESP_WiFiManager ESP_wifiManager;
      // Use this to personalize DHCP hostname (RFC952 conformed)
      ESP_WiFiManager ESP_wifiManager(ssid.c_str());
  
      ESP_wifiManager.setDebugOutput(true);

      // Use only to erase stored WiFi Credentials
      //resetSettings();
      //ESP_wifiManager.resetSettings();

      //set custom ip for portal
      //ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

      ESP_wifiManager.setMinimumSignalQuality(-1);

      #if !USE_DHCP_IP    
        // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
        ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);   
      #endif                         

      //Remove this line if you do not want to see WiFi password printed
      Serial.println("[A]: Stored: SSID = " + SSIDsg + ", Pass = " + Passsg);

      // SSID to uppercase
      ssid.toUpperCase();

      if (SSIDsg == "")
      {
        Serial.println("[A]: Open Config Portal Without Timeout: No Stored Credentials.");
    
        // It Starts An Access Point.
        // And Goes Into A Blocking loop Awaiting Configuration.
        if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str())) {
          Serial.println("[A]: Not Connected To WiFi But Continuing Anyway.");
        } else {
          Serial.println("[A]: WiFi Connected...yeey :)");
        }

        SelectedBoard = ESP_wifiManager.getWitchCode();
        NVS.putInt(addrSelectedBoard, SelectedBoard); 

        Serial.print("[A]: SelectedBoard Is : ");
        Serial.println(SelectedBoard);

        if (SelectedBoard == GateWay) 
        {
          GW_ConfigPortalRun = true;
        }
      }

      if (SelectedBoard == GateWay) 
      {
        if (GW_ConfigPortalRun) 
        {
          SSIDsg = ESP_wifiManager.getSSIDsg();
          Passsg = ESP_wifiManager.getPasswordsg();
    
          UserNamesg = ESP_wifiManager.getUserNamesg();
          UserPasssg = ESP_wifiManager.getUserPasssg();

          SiteIP = ESP_wifiManager.getSiteAddress2();

          SaveGateWayParameters(1);

          Serial.println("[A]: GateWay Parameters Was Saved.");

          GW_ConfigPortalRun = false;
        }
        
        WiFi.disconnect();
        delay(1000);

        unsigned long startedAt = millis();

        while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
        {
          // Setting the ESP as an access point
          // Remove the password parameter, if you want the AP (Access Point) to be open
          WiFi.mode(WIFI_MODE_APSTA);

          Serial.print("Setting AP (Access Point) On -> SSID : ");
          Serial.print(UserNamesg.c_str());
          Serial.print("  PASS : ");
          Serial.println(UserPasssg.c_str());

          WiFi.softAP(UserNamesg.c_str(), UserPasssg.c_str(), 13);

          IPAddress IP = WiFi.softAPIP();
          Serial.print("AP IP address: ");
          Serial.println(IP);
          WiFi.persistent (true);

          // We start by connecting to a WiFi network

          Serial.print("[A]: ConnectingTo ");
          Serial.println(SSIDsg);

          WiFi.config(stationIP, gatewayIP, netMask);
          //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);

          WiFi.begin(SSIDsg.c_str(), Passsg.c_str());

          int i = 0;
          while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
          {
            delay(WHILE_LOOP_DELAY);
          }
        }

        Serial.print("[A]: After Waiting ");
        Serial.print((millis() - startedAt) / 1000);
        Serial.print("Secs More In Setup(), Connection Result Is ");
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
          Serial.print("Connected. Local IP: ");
          Serial.println(WiFi.localIP());
        } else {
          Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
        }
      }
   
      if (SelectedBoard == EnergyMonitor || SelectedBoard == SmartPlug)
      {
        SelectedBoard = 0;
      }
      
      break;
    }
    
    // Smart Plug.
    case SmartPlug:
    {
      GetSmartPlugParameters(1);

      ssid = "Smart Plug_" + GetLast6Mac();
      
      Serial.println("\n\n[A]: Starting ConfigPortal On ->" + ssid);

      // Local intialization. Once its business is done, there is no need to keep it around
      // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
      // ESP_WiFiManager ESP_wifiManager;
      // Use this to personalize DHCP hostname (RFC952 conformed)
      ESP_WiFiManager ESP_wifiManager(ssid.c_str());
  
      ESP_wifiManager.setDebugOutput(true);

      // Use only to erase stored WiFi Credentials
      // resetSettings();
      // ESP_wifiManager.resetSettings();

      //set custom ip for portal
      //ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

      ESP_wifiManager.setMinimumSignalQuality(-1);

      #if !USE_DHCP_IP    
        // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
        ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);   
      #endif                         

      Serial.println("[A]: Stored: SSID = " + SSIDcg + ", Pass = " + Passcg);

      ssid.toUpperCase();

      if (SSIDcg == "")
      {
        Serial.println("[A]: Open Config Portal Without Timeout: No Stored Credentials.");
    
        // It Starts An Access Point.
        // And Goes Into A Blocking loop Awaiting Configuration.
        if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str())) {
          Serial.println("[A]: Not Connected To WiFi But Continuing Anyway.");
        } else {
          Serial.println("[A]: WiFi Connected...yeey :)");
        }

        SelectedBoard = ESP_wifiManager.getWitchCode();
        NVS.putInt(addrSelectedBoard, SelectedBoard);

        Serial.print("[A]: SelectedBoard Is : ");
        Serial.println(SelectedBoard);
        
        if (SelectedBoard == SmartPlug) 
        {
          SM_ConfigPortalRun = true;
        }
      }

      if (SelectedBoard == SmartPlug) 
      {
        if (SM_ConfigPortalRun) 
        {
          SSIDcg = ESP_wifiManager.getSSIDcg();
          Passcg = ESP_wifiManager.getPasswordcg();

          TestUser = ESP_wifiManager.getUsernamecg();
          UserPasscg = ESP_wifiManager.getuPasswordcg();

          SiteIP = ESP_wifiManager.getSiteAddress3();

          SaveSmartPlugParameters(1);

          SM_ConfigPortalRun = false;
        }

        unsigned long startedAt = millis();

        while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
        {
          WiFi.mode(WIFI_STA);
          WiFi.persistent (true);

          // We start by connecting to a WiFi network

          Serial.print("[A]: ConnectingTo ");
          Serial.println(SSIDcg);

          WiFi.config(stationIP, gatewayIP, netMask);
          //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);

          WiFi.begin(SSIDcg.c_str(), Passcg.c_str());

          int i = 0;
          while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
          {
            delay(WHILE_LOOP_DELAY);
          }
        }

        Serial.print("[A]: After Waiting ");
        Serial.print((millis() - startedAt) / 1000);
        Serial.print("Secs More In Setup(), Connection Result Is ");
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
          Serial.print("Connected. Local IP: ");
          Serial.println(WiFi.localIP());

          SP_SHA256_key();
          SP_login();
        } else {
          Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
        }
      }
    
      if (SelectedBoard == EnergyMonitor || SelectedBoard == GateWay)
      {
        SelectedBoard = 0;
      }
        
      break;
    }

    default:
    {
      ssid = "ConfigPortal" + GetLast6Mac();

      Serial.println("\n\n[A]: Starting ConfigPortal On ->" + ssid);
     
      // Local intialization. Once its business is done, there is no need to keep it around
      // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
      // ESP_WiFiManager ESP_wifiManager;
      // Use this to personalize DHCP hostname (RFC952 conformed)
      ESP_WiFiManager ESP_wifiManager(ssid.c_str());

      ESP_wifiManager.setDebugOutput(true);

      // Use only to erase stored WiFi Credentials
      // resetSettings();
      // ESP_wifiManager.resetSettings();

      // set custom ip for portal
      // ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

      ESP_wifiManager.setMinimumSignalQuality(-1);

      #if !USE_DHCP_IP    
        // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
        ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);   
      #endif                         

      //Remove this line if you do not want to see WiFi password printed
      // Serial.println("[A]: Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

      // SSID to uppercase
      ssid.toUpperCase();

      Serial.println("[A]: Open Config Portal Without Timeout: No Stored Credentials.");

      // It Starts An Access Point.
      // And Goes Into A Blocking loop Awaiting Configuration.
      if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str())) {
        Serial.println("[A]: Not Connected To WiFi But Continuing Anyway.");
      } else {
        Serial.println("[A]: WiFi Connected...yeey :)");
      }

      SelectedBoard = ESP_wifiManager.getWitchCode();
      NVS.putInt(addrSelectedBoard, SelectedBoard);

      Serial.print("[A]: SelectedBoard Is : ");
      Serial.println(SelectedBoard);

      if (SelectedBoard == EnergyMonitor) 
      {
        UserName_EM = ESP_wifiManager.getUserName();
        UserPass_EM = ESP_wifiManager.getUserNamePass();

        SSID_EM = ESP_wifiManager.getmSSID();
        Pass_EM = ESP_wifiManager.getmPASS();

        SiteIP = ESP_wifiManager.getSiteAddress1();

        SaveEnergyMonitorParameters(1);

        SM_ConfigPortalRun = false;

        unsigned long startedAt = millis();

        while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
        {
          WiFi.mode(WIFI_STA);
          WiFi.persistent (true);

          // We start by connecting to a WiFi network

          Serial.print("[A]: Connecting To ");
          Serial.println(SSID_EM);

          WiFi.config(stationIP, gatewayIP, netMask);
          //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);

          WiFi.begin(SSID_EM.c_str(), Pass_EM.c_str());

          int i = 0;
          while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
          {
            delay(WHILE_LOOP_DELAY);
          }
        }

        Serial.print("[A]: After Waiting ");
        Serial.print((millis() - startedAt) / 1000);
        Serial.print("Secs More In Setup(), Connection Result Is ");
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
          Serial.print("Connected. Local IP: ");
          Serial.println(WiFi.localIP());

          EM_loginToServer();
          EM_registerToServer();
          EM_SHA256_key();
        } else {
          Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
        }
      }
      
      if (SelectedBoard == GateWay) 
      {
        SSIDsg = ESP_wifiManager.getSSIDsg();
        Passsg = ESP_wifiManager.getPasswordsg();
  
        UserNamesg = ESP_wifiManager.getUserNamesg();
        UserPasssg = ESP_wifiManager.getUserPasssg();

        SiteIP = ESP_wifiManager.getSiteAddress2();

        SaveGateWayParameters(1);

        Serial.println("[A]: GateWay Parameters Was Saved.");

        WiFi.disconnect();
        delay(1000);
        unsigned long startedAt = millis();

        while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
        {
          // Setting the ESP as an access point
          // Remove the password parameter, if you want the AP (Access Point) to be open
          WiFi.mode(WIFI_MODE_APSTA);

          Serial.print("Setting AP (Access Point) On -> SSID : ");
          Serial.print(UserNamesg.c_str());
          Serial.print("  PASS : ");
          Serial.println(UserPasssg.c_str());
          
          WiFi.softAP(UserNamesg.c_str(), UserPasssg.c_str(), 13);

          IPAddress IP = WiFi.softAPIP();
          Serial.print("AP IP address: ");
          Serial.println(IP);
          WiFi.persistent (true);

          // We start by connecting to a WiFi network

          Serial.print("[A]: ConnectingTo ");
          Serial.println(SSIDsg);

          WiFi.config(stationIP, gatewayIP, netMask);
          //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);

          WiFi.begin(SSIDsg.c_str(), Passsg.c_str());

          int i = 0;
          while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
          {
            delay(WHILE_LOOP_DELAY);
          }
        }

        Serial.print("[A]: After Waiting ");
        Serial.print((millis() - startedAt) / 1000);
        Serial.print("Secs More In Setup(), Connection Result Is ");
        Serial.println();

        if (WiFi.status() == WL_CONNECTED)
        {
          Serial.print("Connected. Local IP: ");
          Serial.println(WiFi.localIP());
        } else {
          Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
        }
      }  
      
      if (SelectedBoard == SmartPlug) 
      {
        SSIDcg = ESP_wifiManager.getSSIDcg();
        Passcg = ESP_wifiManager.getPasswordcg();

        TestUser = ESP_wifiManager.getUsernamecg();
        UserPasscg = ESP_wifiManager.getuPasswordcg();

        SiteIP = ESP_wifiManager.getSiteAddress3();

        SaveSmartPlugParameters(1);
        
        unsigned long startedAt = millis();

        while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
        {
          WiFi.mode(WIFI_STA);
          WiFi.persistent (true);

          // We start by connecting to a WiFi network

          Serial.print("[A]: ConnectingTo ");
          Serial.println(SSIDcg);

          WiFi.config(stationIP, gatewayIP, netMask);
          //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);

          WiFi.begin(SSIDcg.c_str(), Passcg.c_str());

          int i = 0;
          while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
          {
            delay(WHILE_LOOP_DELAY);
          }
        }

        Serial.print("[A]: After Waiting ");
        Serial.print((millis() - startedAt) / 1000);
        Serial.print("Secs More In Setup(), Connection Result Is ");
        Serial.println();

        if (WiFi.status() == WL_CONNECTED)
        {
          Serial.print("Connected. Local IP: ");
          Serial.println(WiFi.localIP());

          SP_SHA256_key();
          SP_login();
        } else {
          Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
        }
      }
      
      break;
    }
  }
}

// Creat ConfigPortal Page For User Interface.
void ReConfigPortal(void)
{
  AcyncWeb.end();

  if (SelectedBoard == EnergyMonitor)
  {
    ssid = "EnergyMonitor_" + GetLast6Mac();
  } else if (SelectedBoard == GateWay)
  {
    ssid = "Gateway_" + GetLast6Mac();
  } else if (SelectedBoard == SmartPlug)
  {
    ssid = "SmartPlug_" + GetLast6Mac();
  } else
  {
    ssid = "ConfigPortal_" + GetLast6Mac();
  }

  // Local intialization. Once its business is done, there is no need to keep it around
  // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
  // ESP_WiFiManager ESP_wifiManager;
  // Use this to personalize DHCP hostname (RFC952 conformed)
  ESP_WiFiManager ESP_wifiManager(ssid.c_str());

  ESP_wifiManager.setDebugOutput(true);

  // set custom ip for portal
  // ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

  ESP_wifiManager.setMinimumSignalQuality(-1);

  #if !USE_DHCP_IP    
    // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
    ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);   
  #endif                         

  // SSID to uppercase
  ssid.toUpperCase();

  Serial.println("[A]: Open Config Portal Without Timeout.");

  // It Starts An Access Point.
  // And Goes Into A Blocking loop Awaiting Configuration.
  if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str())) 
  {
    Serial.println("[A]: Not Connected To WiFi But Continuing Anyway.");
  } else 
  {
    Serial.println("[A]: WiFi Connected...yeey :)");
  }

  SelectedBoard = ESP_wifiManager.getWitchCode();
  NVS.putInt(addrSelectedBoard, SelectedBoard);

  Serial.print("[A]: SelectedBoard Is : ");
  Serial.println(SelectedBoard);

  if (SelectedBoard == EnergyMonitor) 
  {
    UserName_EM = ESP_wifiManager.getUserName();
    UserPass_EM = ESP_wifiManager.getUserNamePass();

    SSID_EM = ESP_wifiManager.getmSSID();
    Pass_EM = ESP_wifiManager.getmPASS();

    SiteIP = ESP_wifiManager.getSiteAddress1();

    SaveEnergyMonitorParameters(1);

    unsigned long startedAt = millis();

    while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
    {
      WiFi.mode(WIFI_STA);
      WiFi.persistent (true);

      // We start by connecting to a WiFi network

      Serial.print("[A]: ConnectingTo ");
      Serial.println(SSID_EM);

       WiFi.config(stationIP, gatewayIP, netMask);
       //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);

      WiFi.begin(SSID_EM.c_str(), Pass_EM.c_str());

      int i = 0;
      while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
      {
        delay(WHILE_LOOP_DELAY);
      }
    }

    Serial.print("[A]: After Waiting ");
    Serial.print((millis() - startedAt) / 1000);
    Serial.print("Secs More In Setup(), Connection Result Is ");
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.print("Connected. Local IP: ");
      Serial.println(WiFi.localIP());

      EM_loginToServer();
      EM_registerToServer();
      EM_SHA256_key();
    } else {
      Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
    }
  }
  
  if (SelectedBoard == GateWay) 
  {
    SSIDsg = ESP_wifiManager.getSSIDsg();
    Passsg = ESP_wifiManager.getPasswordsg();
  
    UserNamesg = ESP_wifiManager.getUserNamesg();
    UserPasssg = ESP_wifiManager.getUserPasssg();

    SiteIP = ESP_wifiManager.getSiteAddress2();

    SaveGateWayParameters(1);

    WiFi.disconnect();
    delay(1000);
    unsigned long startedAt = millis();

    while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
    {
      // Setting the ESP as an access point
      // Remove the password parameter, if you want the AP (Access Point) to be open
      WiFi.mode(WIFI_MODE_APSTA);

      Serial.print("Setting AP (Access Point) On -> SSID : ");
      Serial.print(UserNamesg.c_str());
      Serial.print("  PASS : ");
      Serial.println(UserPasssg.c_str());
          
      WiFi.softAP(UserNamesg.c_str(), UserPasssg.c_str(), 13);

      IPAddress IP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(IP);
      WiFi.persistent (true);

      // We start by connecting to a WiFi network

      Serial.print("[A]: ConnectingTo ");
      Serial.println(SSIDsg);

      WiFi.config(stationIP, gatewayIP, netMask);
      //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);

      WiFi.begin(SSIDsg.c_str(), Passsg.c_str());

      int i = 0;
      while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
      {
        delay(WHILE_LOOP_DELAY);
      }
    }

    Serial.print("[A]: After Waiting ");
    Serial.print((millis() - startedAt) / 1000);
    Serial.print("Secs More In Setup(), Connection Result Is ");
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Connected. Local IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
    }
  }     
  
  if (SelectedBoard == SmartPlug) 
  {
    SSIDcg = ESP_wifiManager.getSSIDcg();
    Passcg = ESP_wifiManager.getPasswordcg();

    TestUser = ESP_wifiManager.getUsernamecg();
    UserPasscg = ESP_wifiManager.getuPasswordcg();
    
    SiteIP = ESP_wifiManager.getSiteAddress3();

    SaveSmartPlugParameters(1);

    unsigned long startedAt = millis();

    while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
    {
      WiFi.mode(WIFI_STA);
      WiFi.persistent (true);

      // We start by connecting to a WiFi network

      Serial.print("[A]: ConnectingTo ");
      Serial.println(SSIDcg);

       WiFi.config(stationIP, gatewayIP, netMask);
       //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);

      WiFi.begin(SSIDcg.c_str(), Passcg.c_str());

      int i = 0;
      while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
      {
        delay(WHILE_LOOP_DELAY);
      }
    }

    Serial.print("[A]: After Waiting ");
    Serial.print((millis() - startedAt) / 1000);
    Serial.print("Secs More In Setup(), Connection Result Is ");
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.print("Connected. Local IP: ");
      Serial.println(WiFi.localIP());

      SP_SHA256_key();
      SP_login();
    } else {
      Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
    }

    AcyncWeb.begin();
  }
}
