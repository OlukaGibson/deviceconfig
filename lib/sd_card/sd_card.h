#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <SD.h>

void powerSD(bool state);
int16_t sdHealthCheck();
void firmwareDelete(String fileName);
void verifyFirmware(String fileName, String firmwareCRC32);
void firmwareFlash();

#endif