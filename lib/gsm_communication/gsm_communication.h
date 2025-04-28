#ifndef GSM_COMMUNICATION_H
#define GSM_COMMUNICATION_H

#define TINY_GSM_MODEM_SIM800

#include <SD.h>
#include <Arduino.h>
#include <sd_card.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>

// Config structure to hold the configuration data
typedef struct {
    String deviceID;
    String apiKey;
    String batteryMonitoring;
    String debugEnable;
    String pmSampleEntries;
    String spv;
    String sdCardPin;
    String transmissionMode;
    String deploymentMode;
    String firmwareVersion;
    String firmwareCRC32;
    bool fileDownloadState;
} ConfigData;

// Global instance of the configuration data
extern ConfigData configData;

void powerGSM(bool state);
int8_t gsmHealthCheck();
void disconnectGPRS();
void checkNetwork();
void connectGPRS();
String getIMSI();
String getCCID();
String getIMEI();
String getSignalQuality();

void getEEPROMData(String ccid);
void getConfigData(String deviceID);
void postMetaData(String metadata1, String metadata2, String metadata3, String metadata4);
void postDeviceData(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8);
int8_t firmwareDownload(String resource);
int8_t resumeFirmwareDownload(String resource);
void firmwareUpdate(String fileName, String resource);

#endif