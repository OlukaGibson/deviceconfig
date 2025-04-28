#ifndef EEPROM_CONFIG_H
#define EEPROM_CONFIG_H

#define TINY_GSM_MODEM_SIM800

#include <EEPROM.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>

void saveConfigurationToEEPROM();
void deviceSelfConfig(String configJson);
String loadDataFromEEPROM(String data);
void clearEEPROM();

#endif