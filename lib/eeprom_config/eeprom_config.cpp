// #define TINY_GSM_MODEM_SIM800
#include "eeprom_config.h"

#include <SHT2x.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <TinyGPSPlus.h>
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
  Serial.println(F("Configuration saved to EEPROM."));
  // EEPROM.commit(); // Only needed if you are on ESP32/ESP8266, not for Arduino Mega 2560
}

bool deviceSelfConfig(String configJson) {
  DynamicJsonDocument doc(1024);
  
  DeserializationError error = deserializeJson(doc, configJson);
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return false;
  }
  Serial.println(F("JSON parsed successfully!"));

  // Read and fill the struct
  strlcpy(eeprom_configuration_struct.DEVICE_NAME, doc["name"] | "", sizeof(eeprom_configuration_struct.DEVICE_NAME));
  strlcpy(eeprom_configuration_struct.DEVICE_WRITE_API_KEY, doc["writekey"] | "", sizeof(eeprom_configuration_struct.DEVICE_WRITE_API_KEY));
  strlcpy(eeprom_configuration_struct.DEVICE_READ_API_KEY, doc["readkey"] | "", sizeof(eeprom_configuration_struct.DEVICE_READ_API_KEY));
  
  // Convert deviceID from number to string before storing
  String deviceIDStr = String(doc["deviceID"].as<int>());
  strlcpy(eeprom_configuration_struct.DEVICE_CHANEL_ID, deviceIDStr.c_str(), sizeof(eeprom_configuration_struct.DEVICE_CHANEL_ID));
  
  strlcpy(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION, "42.74", sizeof(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION));
  strlcpy(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32, "103848AF", sizeof(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32));
  
  JsonObject configs = doc["configs"];
  
  // Convert string values to integers
  eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES = configs["Sample Interval"].as<String>().toInt();
  if (eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES == 0) eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES = 10; // Default if conversion fails
  
  eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES = configs["Upload Interval"].as<String>().toInt();
  if (eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES == 0) eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES = 20;
  
  eeprom_configuration_struct.DEPLOYMENT_MODE = configs["Deployment Mode"].as<String>().toInt();
  if (eeprom_configuration_struct.DEPLOYMENT_MODE > 1) eeprom_configuration_struct.DEPLOYMENT_MODE = 1;
  
  eeprom_configuration_struct.BATTERY_MONITORING = configs["Chip Based battery monitoring"].as<String>().toInt();
  if (eeprom_configuration_struct.BATTERY_MONITORING > 1) eeprom_configuration_struct.BATTERY_MONITORING = 1;
  
  eeprom_configuration_struct.DEBUG_ENABLE = configs["Debug Enable"].as<String>().toInt();
  if (eeprom_configuration_struct.DEBUG_ENABLE > 1) eeprom_configuration_struct.DEBUG_ENABLE = 1;
  
  eeprom_configuration_struct.PM_SAMPLE_ENTRIES = configs["PM sample entries"].as<String>().toInt();
  if (eeprom_configuration_struct.PM_SAMPLE_ENTRIES == 0) eeprom_configuration_struct.PM_SAMPLE_ENTRIES = 5;
  
  eeprom_configuration_struct.SPV = configs["SPV"].as<String>().toInt();
  if (eeprom_configuration_struct.SPV == 0) eeprom_configuration_struct.SPV = 50;
  
  eeprom_configuration_struct.SD_CARD_PIN = configs["Sd card pin"].as<String>().toInt();
  if (eeprom_configuration_struct.SD_CARD_PIN == 0) eeprom_configuration_struct.SD_CARD_PIN = 20;
  
  eeprom_configuration_struct.TRANSMISSION_MODE = configs["Transmission Mode"].as<String>().toInt();
  if (eeprom_configuration_struct.TRANSMISSION_MODE > 1) eeprom_configuration_struct.TRANSMISSION_MODE = 1;
  
  // Print parsed values for debugging
  Serial.println(F("Configuration values from server:"));
  Serial.print(F("Sample Interval: ")); Serial.println(eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES);
  Serial.print(F("Upload Interval: ")); Serial.println(eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES);
  Serial.print(F("Deployment Mode: ")); Serial.println(eeprom_configuration_struct.DEPLOYMENT_MODE);
  Serial.print(F("Battery Monitoring: ")); Serial.println(eeprom_configuration_struct.BATTERY_MONITORING);
  Serial.print(F("Debug Enable: ")); Serial.println(eeprom_configuration_struct.DEBUG_ENABLE);
  Serial.print(F("PM Sample Entries: ")); Serial.println(eeprom_configuration_struct.PM_SAMPLE_ENTRIES);
  Serial.print(F("SPV: ")); Serial.println(eeprom_configuration_struct.SPV);
  Serial.print(F("SD Card Pin: ")); Serial.println(eeprom_configuration_struct.SD_CARD_PIN);
  Serial.print(F("Transmission Mode: ")); Serial.println(eeprom_configuration_struct.TRANSMISSION_MODE);
  
  // Finally, save to EEPROM
  saveConfigurationToEEPROM();
  return true;
}

String loadDataFromEEPROM(String data) {
    EEPROM.get(SIGNATURE_ADDRESS + sizeof(WRITTEN_SIGNATURE), eeprom_configuration_struct);
    Serial.println(F("Configuration loaded from EEPROM."));
    // Print the loaded configuration for debugging
    if (data == "DEVICE_NAME") {
      return eeprom_configuration_struct.DEVICE_NAME;
    } else if (data == "DEVICE_WRITE_API_KEY") {
      return eeprom_configuration_struct.DEVICE_WRITE_API_KEY;
    } else if (data == "DEVICE_READ_API_KEY") {
      return eeprom_configuration_struct.DEVICE_READ_API_KEY;
    } else if (data == "DEVICE_CHANEL_ID") {
      return eeprom_configuration_struct.DEVICE_CHANEL_ID;
    } else if (data == "DEVICE_FIRMWARE_VERSION") {
      return eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION;
    } else if (data == "DEVICE_FIRMWARE_CRC32") {
      return eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32;
    } else if (data == "DEVICE_SAMPLE_INTERVAL_MINUTES") {
      return String(eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES);
    } else if (data == "DEVICE_UPLOAD_INTERVAL_MINUTES") {
      return String(eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES);
    } else if (data == "DEPLOYMENT_MODE") {
      return String(eeprom_configuration_struct.DEPLOYMENT_MODE);
    } else if (data == "BATTERY_MONITORING") {
      return String(eeprom_configuration_struct.BATTERY_MONITORING);
    } else if (data == "DEBUG_ENABLE") {
      return String(eeprom_configuration_struct.DEBUG_ENABLE);
    } else if (data == "PM_SAMPLE_ENTRIES") {
      return String(eeprom_configuration_struct.PM_SAMPLE_ENTRIES);
    } else if (data == "SPV") {
      return String(eeprom_configuration_struct.SPV);
    } else if (data == "SD_CARD_PIN") {
      return String(eeprom_configuration_struct.SD_CARD_PIN);
    } else if (data == "TRANSMISSION_MODE") {
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
      return String(signature, HEX);
    } else if (data == "SIGNATURE_ADDRESS") {
      uint32_t signatureAddress;
      EEPROM.get(SIGNATURE_ADDRESS, signatureAddress);
      return String(signatureAddress, HEX);
    } else {
      return "Invalid data requested from EEPROM.";
    }
}

void clearEEPROM() {
  Serial.println(F("Clearing EEPROM..."));
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0xFF); // Write 0xFF to each byte
  }
    Serial.println(F("EEPROM cleared."));
}

bool isEEPROMEmpty() {
  for (int i = 0; i < EEPROM.length(); i++) {
    if (EEPROM.read(i) != 0xFF) {
      return false; // EEPROM is not empty
    }
  }
  return true; // EEPROM is empty
}


