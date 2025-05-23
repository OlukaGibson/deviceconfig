#ifndef GSM_COMMUNICATION_H
#define GSM_COMMUNICATION_H

// #define TINY_GSM_MODEM_SIM800

#include <SD.h>
#include <SHT2x.h>
#include <Arduino.h>
#include <sd_card.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>

// // Config structure to hold the configuration data
// typedef struct {
//     String deviceID;
//     String apiKey;
//     String batteryMonitoring;
//     String debugEnable;
//     String pmSampleEntries;
//     String spv;
//     String sdCardPin;
//     String transmissionMode;
//     String deploymentMode;
//     String firmwareVersion;
//     String firmwareCRC32;
//     bool fileDownloadState;
// } ConfigData;

// // Global instance of the configuration data
// extern ConfigData configData;

String getIMSI();
String getCCID();
String getIMEI();
bool connectGPRS();
bool checkNetwork();
int gsmHealthCheck();
void disconnectGPRS();
String getSignalQuality();
void powerGSM(int state);
void getEEPROMData(String ccid);
void getConfigData(String deviceID);
int8_t firmwareDownload(String resource);
int8_t resumeFirmwareDownload(String resource);
void firmwareUpdate(String resource);
void postMetaData(String metadata1, String metadata2, String metadata3, String metadata4);
void postDeviceData(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8);


#endif