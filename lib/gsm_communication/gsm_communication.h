#ifndef GSM_COMMUNICATION_H
#define GSM_COMMUNICATION_H

#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <Arduino.h>
#include <pin_definition.h>
#include <globalVariables.h>


void gsmTurnOn();
void gsmTurnOff();
int8_t gsmHealthCheck();
void checkNetwork();
void connectGPRS();
String getIMSI();
String getCCID();
String getIMEI();
String getSignalQuality();

void deviceSelfConfig(String ccid);
void getConfigData(String deviceID);
void postParameters(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8, String field9, String field10, String field11, String field12, String field13, String field14);

bool downloadFirmware(const String& version);

#endif