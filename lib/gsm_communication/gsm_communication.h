#ifndef GSM_COMMUNICATION_H
#define GSM_COMMUNICATION_H

#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <Arduino.h>
#include <pin_definition.h>

extern const char apn[];
extern const char user[];
extern const char pass[];
extern const char serverurl1[];
extern const char serverurl2[];

extern TinyGsm modem; // Declare as extern
extern TinyGsmClient client; // Declare as extern
extern HttpClient http; // Declare as extern

void checkNetwork();
void connectGPRS();
String getIMSI();
String getCCID();
String getIMEI();
String getSignalQuality();
void getConfigDetails(String ccid);
void postParameters(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8, String field9, String field10, String field11, String field12);

#endif