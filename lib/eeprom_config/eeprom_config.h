#ifndef EEPROM_CONFIG_H
#define EEPROM_CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

// Define the struct for EEPROM configuration
typedef struct {
    char DEVICE_NAME[32];
    char DEVICE_WRITE_API_KEY[32];
    char DEVICE_READ_API_KEY[32];
    char DEVICE_CHANEL_ID[32];
    char DEVICE_FIRMWARE_VERSION[32];
    int DEVICE_UPLOAD_INTERVAL_MINUTES;
} EEPROM_CONFIGURATION_FILE;

// Function prototypes
void saveConfigurationToEEPROM(const EEPROM_CONFIGURATION_FILE &config);
EEPROM_CONFIGURATION_FILE loadConfigurationFromEEPROM();
String getConfigValue(const char *key);
String getAllConfigValues();
void updateEEPROMFromResponse(String response);

#endif