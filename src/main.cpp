#include <Arduino.h>
#include <gsm_communication.h>
#include "configuration/configuration.h"
#include <pin_definition.h>
#include <sd_card.h>
#include <globalVariables.h>
#include <ArduinoJson.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <eeprom_config.h>
#include <SD.h>


void setup() {
  Serial.begin(115200);
  powerGSM(1);
  powerSD(1);
  delay(3000);

  Serial1.begin(115200);

  // Test the SD card first 
  // testSDCard();
  // delay(1000);

  // checkNetwork();
  // delay(3000);
  // String imei = getIMEI();
  // String imsi = getIMSI();
  // String ccid = getCCID();
  // String signalQuality = getSignalQuality();

  // Serial.println("CCID: " + ccid);
  // Serial.println("IMEI: " + imei);
  // Serial.println("IMSI: " + imsi);
  // Serial.println("Signal Quality: " + signalQuality);

  // connectGPRS();
  // Initialize SD card first, to ensure it's ready for firmware storage
  // Serial.println("Initializing SD card...");
  // if (sdHealthCheck() != 0) {
  //   Serial.println("SD card initialization failed! Cannot proceed with firmware update.");
  //   return;
  // }
  // deviceSelfConfig(ccid);
  // getConfigData("1");
  // postMetaData("1", "2", "3", "4");
  // postDeviceData("1", "2", "3", "4", "5", "6", "7", "8");
  // String resource = "/firmware/" + firmwareVersion + "/download/firmwarebin";
  // Serial.println("Resource: " + resource);
  // firmwareUpdate(FIRMWARE_NAME, resource);
  // getEEPROMData(ccid);
  // disconnectGPRS();
  // powerGSM(0);
  // delay(2000);
  // // Serial.println("EEPROM Data: " + responseData);
  // if (responseData == "null") {
  //   Serial.println("Failed to get EEPROM data.");
  //   return;
  // }
  // Serial.println("EEPROM Data: " + responseData);
  // deviceSelfConfig(responseData);
  String channelID = loadDataFromEEPROM("DEVICE_CHANEL_ID");
  Serial.println("Channel ID: " + channelID);
  // Serial.println("Device Name: " + String(eeprom_configuration_struct.DEVICE_NAME));
  // Serial.println("Device Write API Key: " + String(eeprom_configuration_struct.DEVICE_WRITE_API_KEY));
  // Serial.println("Device Read API Key: " + String(eeprom_configuration_struct.DEVICE_READ_API_KEY));  
  // Serial.println("Device Channel ID: " + String(eeprom_configuration_struct.DEVICE_CHANEL_ID));
  // Serial.println("Device Firmware Version: " + String(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION));
  // delay(1000);
}

void loop() {
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
}