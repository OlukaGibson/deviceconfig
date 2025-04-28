#define TINY_GSM_MODEM_SIM800
#include "eeprom_config.h"

#include <EEPROM.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>

// EEPROM_CONFIGURATION_STRUCT eeprom_configuration_struct;

void saveConfigurationToEEPROM() {
  // Write a signature first
  EEPROM.put(SIGNATURE_ADDRESS, WRITTEN_SIGNATURE);
  // Write the structure after the signature
  EEPROM.put(SIGNATURE_ADDRESS + sizeof(WRITTEN_SIGNATURE), eeprom_configuration_struct);
  Serial.println("Configuration saved to EEPROM.");
  // EEPROM.commit(); // Only needed if you are on ESP32/ESP8266, not for Arduino Mega 2560
}

void deviceSelfConfig(String configJson) {
    StaticJsonDocument<1024> doc; // Enough space for your JSON
  
    DeserializationError error = deserializeJson(doc, configJson);
  
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    Serial.println("JSON parsed successfully!");
  
    // Read and fill the struct
    strlcpy(eeprom_configuration_struct.DEVICE_NAME, doc["name"] | "", sizeof(eeprom_configuration_struct.DEVICE_NAME));
    strlcpy(eeprom_configuration_struct.DEVICE_WRITE_API_KEY, doc["writekey"] | "", sizeof(eeprom_configuration_struct.DEVICE_WRITE_API_KEY));
    strlcpy(eeprom_configuration_struct.DEVICE_READ_API_KEY, doc["readkey"] | "", sizeof(eeprom_configuration_struct.DEVICE_READ_API_KEY));
    strlcpy(eeprom_configuration_struct.DEVICE_CHANEL_ID, doc["deviceID"] | "", sizeof(eeprom_configuration_struct.DEVICE_CHANEL_ID));
  
    strlcpy(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION, "0.0", sizeof(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION)); // Default for now
    strlcpy(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32, "00000000", sizeof(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32)); // Default for now
  
    JsonObject configs = doc["configs"];
  
    eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES = configs["Sample Interval"] | 15;
    eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES = configs["Upload Interval"] | 30;
    eeprom_configuration_struct.DEPLOYMENT_MODE = configs["Deployment Mode"] | 0;
    eeprom_configuration_struct.BATTERY_MONITORING = configs["Chip Based battery monitoring"] | 0;
    eeprom_configuration_struct.DEBUG_ENABLE = configs["Debug Enable"] | 0;
    eeprom_configuration_struct.PM_SAMPLE_ENTRIES = configs["PM sample entries"] | 60;
    eeprom_configuration_struct.SPV = configs["SPV"] | 100;
    eeprom_configuration_struct.SD_CARD_PIN = configs["Sd card pin"] | 28;
    eeprom_configuration_struct.TRANSMISSION_MODE = configs["Transmission Mode"] | 0;
    eeprom_configuration_struct.SAMPLE_ENTRIES = eeprom_configuration_struct.PM_SAMPLE_ENTRIES; // Assuming you want same
  
    // Finally, save to EEPROM
    saveConfigurationToEEPROM();
  }

String loadDataFromEEPROM(String data) {
    EEPROM.get(SIGNATURE_ADDRESS + sizeof(WRITTEN_SIGNATURE), eeprom_configuration_struct);
    Serial.println("Configuration loaded from EEPROM.");
    // Print the loaded configuration for debugging
    if (data == "DEVICE_NAME") {
      // Serial.print(F("Device Name: "));
      // Serial.println(eeprom_configuration_struct.DEVICE_NAME);
      return eeprom_configuration_struct.DEVICE_NAME;
    } else if (data == "DEVICE_WRITE_API_KEY") {
      // Serial.print(F("Device Write API Key: "));
      // Serial.println(eeprom_configuration_struct.DEVICE_WRITE_API_KEY);
      return eeprom_configuration_struct.DEVICE_WRITE_API_KEY;
    } else if (data == "DEVICE_READ_API_KEY") {
      // Serial.print(F("Device Read API Key: "));
      // Serial.println(eeprom_configuration_struct.DEVICE_READ_API_KEY);
      return eeprom_configuration_struct.DEVICE_READ_API_KEY;
    } else if (data == "DEVICE_CHANEL_ID") {
      // Serial.print(F("Device Channel ID: "));
      // Serial.println(eeprom_configuration_struct.DEVICE_CHANEL_ID);
      return eeprom_configuration_struct.DEVICE_CHANEL_ID;
    } else if (data == "DEVICE_FIRMWARE_VERSION") {
      // Serial.print(F("Device Firmware Version: "));
      // Serial.println(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION);
      return eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION;
    } else if (data == "DEVICE_FIRMWARE_CRC32") {
      // Serial.print(F("Device Firmware CRC32: "));
      // Serial.println(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32);
      return eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32;
    } else if (data == "DEVICE_SAMPLE_INTERVAL_MINUTES") {
      // Serial.print(F("Sample Interval: "));
      // Serial.println(eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES);
      return String(eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES);
    } else if (data == "DEVICE_UPLOAD_INTERVAL_MINUTES") {
      // Serial.print(F("Upload Interval: "));
      // Serial.println(eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES);
      return String(eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES);
    } else if (data == "DEPLOYMENT_MODE") {
      // Serial.print(F("Deployment Mode: "));
      // Serial.println(eeprom_configuration_struct.DEPLOYMENT_MODE);
      return String(eeprom_configuration_struct.DEPLOYMENT_MODE);
    } else if (data == "BATTERY_MONITORING") {
      // Serial.print(F("Battery Monitoring: "));
      // Serial.println(eeprom_configuration_struct.BATTERY_MONITORING);
      return String(eeprom_configuration_struct.BATTERY_MONITORING);
    } else if (data == "DEBUG_ENABLE") {
      // Serial.print(F("Debug Enable: "));
      // Serial.println(eeprom_configuration_struct.DEBUG_ENABLE);
      return String(eeprom_configuration_struct.DEBUG_ENABLE);
    } else if (data == "PM_SAMPLE_ENTRIES") {
      // Serial.print(F("PM Sample Entries: "));
      // Serial.println(eeprom_configuration_struct.PM_SAMPLE_ENTRIES);
      return String(eeprom_configuration_struct.PM_SAMPLE_ENTRIES);
    } else if (data == "SPV") {
      // Serial.print(F("SPV: "));
      // Serial.println(eeprom_configuration_struct.SPV);
      return String(eeprom_configuration_struct.SPV);
    } else if (data == "SD_CARD_PIN") {
      // Serial.print(F("SD Card Pin: "));
      // Serial.println(eeprom_configuration_struct.SD_CARD_PIN);
      return String(eeprom_configuration_struct.SD_CARD_PIN);
    } else if (data == "TRANSMISSION_MODE") {
      // Serial.print(F("Transmission Mode: "));
      // Serial.println(eeprom_configuration_struct.TRANSMISSION_MODE);
      return String(eeprom_configuration_struct.TRANSMISSION_MODE);
    } else if (data == "ALL") {
      Serial.print(F("Device Name: "));
      Serial.println(eeprom_configuration_struct.DEVICE_NAME);
      Serial.print(F("Device Write API Key: "));
      Serial.println(eeprom_configuration_struct.DEVICE_WRITE_API_KEY);
      Serial.print(F("Device Read API Key: "));
      Serial.println(eeprom_configuration_struct.DEVICE_READ_API_KEY);
      Serial.print(F("Device Channel ID: "));
      Serial.println(eeprom_configuration_struct.DEVICE_CHANEL_ID);
      Serial.print(F("Device Firmware Version: "));
      Serial.println(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION);
      Serial.print(F("Device Firmware CRC32: "));
      Serial.println(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32);
      Serial.print(F("Sample Interval: "));
      Serial.println(eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES);
      Serial.print(F("Upload Interval: "));
      Serial.println(eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES);
      Serial.print(F("Deployment Mode: "));
      Serial.println(eeprom_configuration_struct.DEPLOYMENT_MODE);
      Serial.print(F("Battery Monitoring: "));
      Serial.println(eeprom_configuration_struct.BATTERY_MONITORING);
      Serial.print(F("Debug Enable: "));
      Serial.println(eeprom_configuration_struct.DEBUG_ENABLE);
      Serial.print(F("PM Sample Entries: "));
      Serial.println(eeprom_configuration_struct.PM_SAMPLE_ENTRIES);
      Serial.print(F("SPV: "));
      Serial.println(eeprom_configuration_struct.SPV);
      Serial.print(F("SD Card Pin: "));
      Serial.println(eeprom_configuration_struct.SD_CARD_PIN);
      Serial.print(F("Transmission Mode: "));
      Serial.println(eeprom_configuration_struct.TRANSMISSION_MODE);
      return "done";
    } else if (data == "SIGNATURE") {
      uint32_t signature;
      EEPROM.get(SIGNATURE_ADDRESS, signature);
      // Serial.print(F("Signature: "));
      // Serial.println(signature, HEX);
      return String(signature, HEX);
    } else if (data == "SIGNATURE_ADDRESS") {
      uint32_t signatureAddress;
      EEPROM.get(SIGNATURE_ADDRESS, signatureAddress);
      // Serial.print(F("Signature Address: "));
      // Serial.println(signatureAddress, HEX);
      return String(signatureAddress, HEX);
    } else {
      // Serial.println("Invalid data requested from EEPROM.");
      return "Invalid data requested from EEPROM.";
    }
}

void clearEEPROM() {
  Serial.println("Clearing EEPROM...");
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0xFF); // Write 0xFF to each byte
  }
    Serial.println("EEPROM cleared.");
}

// void load_configuration_from_eeprom(){
//     uint32_t signature;
//     EEPROM.get(SIGNATURE_ADDRESS, signature);
//     if(signature == WRITTEN_SIGNATURE) {
//         // Change this line - read from the same address where you stored it
//         EEPROM.get(sizeof(uint32_t), eeprom_configuration_struct);
//         Serial.println(F("EEPROM configuration loaded successfully."));
        
//         //Device Name
//         Serial.print(F("Device Name: "));
//         Serial.println(eeprom_configuration_struct.DEVICE_NAME);
        
//         // Device Write API Key
//         Serial.print(F("Device Write API Key: "));
//         for (int i = 0; i < 3; i++) {
//             Serial.print(eeprom_configuration_struct.DEVICE_WRITE_API_KEY[i]);
//         }
//         for (int i = 3; i < strlen(eeprom_configuration_struct.DEVICE_WRITE_API_KEY)-3; i++) {
//             Serial.print(F("*"));
//         }
//         for (int i = strlen(eeprom_configuration_struct.DEVICE_WRITE_API_KEY)-3; i < strlen(eeprom_configuration_struct.DEVICE_WRITE_API_KEY); i++) {
//             Serial.print(eeprom_configuration_struct.DEVICE_WRITE_API_KEY[i]);
//         }
//         Serial.println();

//         // Device Read API Key
//         Serial.print(F("Device Read API Key: "));
//         for (int i = 0; i < 3; i++) {
//             Serial.print(eeprom_configuration_struct.DEVICE_READ_API_KEY[i]);
//         }
//         for (int i = 3; i < strlen(eeprom_configuration_struct.DEVICE_READ_API_KEY)-3; i++) {
//             Serial.print(F("*"));
//         }
//         for (int i = strlen(eeprom_configuration_struct.DEVICE_READ_API_KEY)-3; i < strlen(eeprom_configuration_struct.DEVICE_READ_API_KEY); i++) {
//             Serial.print(eeprom_configuration_struct.DEVICE_READ_API_KEY[i]);
//         }
//         Serial.println();

//         // Device Channel ID
//         Serial.print(F("Device Channel ID: "));
//         Serial.println(eeprom_configuration_struct.DEVICE_CHANEL_ID);

//         // Device Firmware Version
//         Serial.print(F("Device Firmware Version: "));
//         Serial.println(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION);

//         // Device Firmware CRC32
//         Serial.print(F("Device Firmware CRC32: "));
//         Serial.println(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32);

//         // Sample Interval
//         Serial.print(F("Sample Interval: "));
//         Serial.println(eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES);

//         // Upload Interval
//         Serial.print(F("Upload Interval: "));
//         Serial.println(eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES);

//         // Deployment Mode
//         Serial.print(F("Deployment Mode: "));
//         Serial.println(eeprom_configuration_struct.DEPLOYMENT_MODE);

//         // Battery Monitoring
//         Serial.print(F("Battery Monitoring: "));
//         Serial.println(eeprom_configuration_struct.BATTERY_MONITORING);

//         // Debug Enable
//         Serial.print(F("Debug Enable: "));
//         Serial.println(eeprom_configuration_struct.DEBUG_ENABLE);

//         // PM Sample Entries
//         Serial.print(F("PM Sample Entries: "));
//         Serial.println(eeprom_configuration_struct.PM_SAMPLE_ENTRIES);

//         // SPV
//         Serial.print(F("SPV: "));
//         Serial.println(eeprom_configuration_struct.SPV);

//         // SD Card Pin
//         Serial.print(F("SD Card Pin: "));
//         Serial.println(eeprom_configuration_struct.SD_CARD_PIN);

//         // Transmission Mode
//         Serial.print(F("Transmission Mode: "));
//         Serial.println(eeprom_configuration_struct.TRANSMISSION_MODE);
//     } else {
//         Serial.println(F("EEPROM configuration not found!"));
//     }
// }




// // Save the configuration struct to EEPROM
// void saveConfigurationToEEPROM(const EEPROM_CONFIGURATION_FILE &config) {
//     EEPROM.put(0, config);
//     // EEPROM.commit(); // Ensure changes are written to EEPROM
// }

// // Load the configuration struct from EEPROM
// EEPROM_CONFIGURATION_FILE loadConfigurationFromEEPROM() {
//     EEPROM_CONFIGURATION_FILE config;
//     EEPROM.get(0, config);
//     return config;
// }

// // Retrieve a specific value from the configuration struct
// String getConfigValue(const char *key) {
//     EEPROM_CONFIGURATION_FILE config = loadConfigurationFromEEPROM();

//     if (strcmp(key, "DEVICE_NAME") == 0) return String(config.DEVICE_NAME);
//     if (strcmp(key, "DEVICE_WRITE_API_KEY") == 0) return String(config.DEVICE_WRITE_API_KEY);
//     if (strcmp(key, "DEVICE_READ_API_KEY") == 0) return String(config.DEVICE_READ_API_KEY);
//     if (strcmp(key, "DEVICE_CHANEL_ID") == 0) return String(config.DEVICE_CHANEL_ID);
//     if (strcmp(key, "DEVICE_FIRMWARE_VERSION") == 0) return String(config.DEVICE_FIRMWARE_VERSION);
//     if (strcmp(key, "DEVICE_UPLOAD_INTERVAL_MINUTES") == 0) return String(config.DEVICE_UPLOAD_INTERVAL_MINUTES);

//     return String("");
// }

// // Retrieve all configuration values as a JSON string
// String getAllConfigValues() {
//     EEPROM_CONFIGURATION_FILE config = loadConfigurationFromEEPROM();
//     StaticJsonDocument<256> jsonDoc;

//     jsonDoc["DEVICE_NAME"] = config.DEVICE_NAME;
//     jsonDoc["DEVICE_WRITE_API_KEY"] = config.DEVICE_WRITE_API_KEY;
//     jsonDoc["DEVICE_READ_API_KEY"] = config.DEVICE_READ_API_KEY;
//     jsonDoc["DEVICE_CHANEL_ID"] = config.DEVICE_CHANEL_ID;
//     jsonDoc["DEVICE_FIRMWARE_VERSION"] = config.DEVICE_FIRMWARE_VERSION;
//     jsonDoc["DEVICE_UPLOAD_INTERVAL_MINUTES"] = config.DEVICE_UPLOAD_INTERVAL_MINUTES;

//     String jsonString;
//     serializeJson(jsonDoc, jsonString);
//     return jsonString;
// }

// void updateEEPROMFromResponse(String response) {
//     StaticJsonDocument<256> jsonDoc;
//     DeserializationError error = deserializeJson(jsonDoc, response);

//     if (error) {
//         Serial.print("Failed to parse JSON: ");
//         Serial.println(error.c_str());
//         return;
//     }

//     EEPROM_CONFIGURATION_FILE config = loadConfigurationFromEEPROM();

//     if (jsonDoc.containsKey("name")) {
//         strncpy(config.DEVICE_NAME, jsonDoc["name"], sizeof(config.DEVICE_NAME) - 1);
//     }
//     if (jsonDoc.containsKey("writekey")) {
//         strncpy(config.DEVICE_WRITE_API_KEY, jsonDoc["writekey"], sizeof(config.DEVICE_WRITE_API_KEY) - 1);
//     }
//     if (jsonDoc.containsKey("deviceID")){
//         strncpy(config.DEVICE_CHANEL_ID, jsonDoc["deviceID"], sizeof(config.DEVICE_CHANEL_ID) - 1);
//     }
//     // if (jsonDoc.containsKey("imsi")) {
//     //     strncpy(config.DEVICE_CHANEL_ID, jsonDoc["imsi"], sizeof(config.DEVICE_CHANEL_ID) - 1);
//     // }

//     saveConfigurationToEEPROM(config);
//     Serial.println("EEPROM updated with new configuration.");
// }