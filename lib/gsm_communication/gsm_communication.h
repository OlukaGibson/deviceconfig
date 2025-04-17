#ifndef GSM_COMMUNICATION_H
#define GSM_COMMUNICATION_H

#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <Arduino.h>
#include <pin_definition.h>
#include <globalVariables.h>


void powerGSM(bool state);
int8_t gsmHealthCheck();
void checkNetwork();
void connectGPRS();
String getIMSI();
String getCCID();
String getIMEI();
String getSignalQuality();

void deviceSelfConfig(String ccid);
void getConfigData(String deviceID);
void postMetaData(String metadata1, String metadata2, String metadata3, String metadata4);
void postDeviceData(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8);
void firmwareUpdate(String fileName, String resource);

#endif