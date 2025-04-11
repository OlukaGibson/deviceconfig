#include "eeprom_config.h"
#include <EEPROM.h>
#include <ArduinoJson.h>

// Save the configuration struct to EEPROM
void saveConfigurationToEEPROM(const EEPROM_CONFIGURATION_FILE &config) {
    EEPROM.put(0, config);
    // EEPROM.commit(); // Ensure changes are written to EEPROM
}

// Load the configuration struct from EEPROM
EEPROM_CONFIGURATION_FILE loadConfigurationFromEEPROM() {
    EEPROM_CONFIGURATION_FILE config;
    EEPROM.get(0, config);
    return config;
}

// Retrieve a specific value from the configuration struct
String getConfigValue(const char *key) {
    EEPROM_CONFIGURATION_FILE config = loadConfigurationFromEEPROM();

    if (strcmp(key, "DEVICE_NAME") == 0) return String(config.DEVICE_NAME);
    if (strcmp(key, "DEVICE_WRITE_API_KEY") == 0) return String(config.DEVICE_WRITE_API_KEY);
    if (strcmp(key, "DEVICE_READ_API_KEY") == 0) return String(config.DEVICE_READ_API_KEY);
    if (strcmp(key, "DEVICE_CHANEL_ID") == 0) return String(config.DEVICE_CHANEL_ID);
    if (strcmp(key, "DEVICE_FIRMWARE_VERSION") == 0) return String(config.DEVICE_FIRMWARE_VERSION);
    if (strcmp(key, "DEVICE_UPLOAD_INTERVAL_MINUTES") == 0) return String(config.DEVICE_UPLOAD_INTERVAL_MINUTES);

    return String("");
}

// Retrieve all configuration values as a JSON string
String getAllConfigValues() {
    EEPROM_CONFIGURATION_FILE config = loadConfigurationFromEEPROM();
    StaticJsonDocument<256> jsonDoc;

    jsonDoc["DEVICE_NAME"] = config.DEVICE_NAME;
    jsonDoc["DEVICE_WRITE_API_KEY"] = config.DEVICE_WRITE_API_KEY;
    jsonDoc["DEVICE_READ_API_KEY"] = config.DEVICE_READ_API_KEY;
    jsonDoc["DEVICE_CHANEL_ID"] = config.DEVICE_CHANEL_ID;
    jsonDoc["DEVICE_FIRMWARE_VERSION"] = config.DEVICE_FIRMWARE_VERSION;
    jsonDoc["DEVICE_UPLOAD_INTERVAL_MINUTES"] = config.DEVICE_UPLOAD_INTERVAL_MINUTES;

    String jsonString;
    serializeJson(jsonDoc, jsonString);
    return jsonString;
}

void updateEEPROMFromResponse(String response) {
    StaticJsonDocument<256> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, response);

    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
    }

    EEPROM_CONFIGURATION_FILE config = loadConfigurationFromEEPROM();

    if (jsonDoc.containsKey("name")) {
        strncpy(config.DEVICE_NAME, jsonDoc["name"], sizeof(config.DEVICE_NAME) - 1);
    }
    if (jsonDoc.containsKey("writekey")) {
        strncpy(config.DEVICE_WRITE_API_KEY, jsonDoc["writekey"], sizeof(config.DEVICE_WRITE_API_KEY) - 1);
    }
    if (jsonDoc.containsKey("deviceID")){
        strncpy(config.DEVICE_CHANEL_ID, jsonDoc["deviceID"], sizeof(config.DEVICE_CHANEL_ID) - 1);
    }
    // if (jsonDoc.containsKey("imsi")) {
    //     strncpy(config.DEVICE_CHANEL_ID, jsonDoc["imsi"], sizeof(config.DEVICE_CHANEL_ID) - 1);
    // }

    saveConfigurationToEEPROM(config);
    Serial.println("EEPROM updated with new configuration.");
}