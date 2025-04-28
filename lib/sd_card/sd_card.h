#ifndef SD_CARD_H
#define SD_CARD_H

#define TINY_GSM_MODEM_SIM800

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <globalVariables.h>
#include <gsm_communication.h>
#include <Arduino.h>
#include <ArduinoHttpClient.h>
#include <EEPROM.h>
#include <CRC32.h>
#include <pin_definition.h>
#include <TinyGsmClient.h>
#include <TinyGsm.h>
#include <ArduinoJson.h>
#include <avr/wdt.h>

void powerSD(bool state);
int16_t sdHealthCheck();
void firmwareDelete(String fileName);
void verifyFirmware(String fileName, String firmwareCRC32);
void firmwareFlash();

#endif