#ifndef GSM_COMMUNICATION_H
#define GSM_COMMUNICATION_H

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
void getConfigDetails(String ccid);
void postParameters(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8, String field9, String field10, String field11, String field12);
bool downloadFirmware(const String& version);

#endif