#ifndef SD_CARD_H
#define SD_CARD_H

#define TINY_GSM_MODEM_SIM800

#include <SD.h>
#include <SPI.h>
#include <CRC32.h>
#include <EEPROM.h>
#include <TinyGsm.h>
#include <Arduino.h>
#include <Arduino.h>
#include <avr/wdt.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <gsm_communication.h>
#include <ArduinoHttpClient.h>

void powerSD(bool state);
int16_t sdHealthCheck();
void firmwareDelete(String fileName);
void verifyFirmware(String fileName, String firmwareCRC32);
void firmwareFlash();

#endif