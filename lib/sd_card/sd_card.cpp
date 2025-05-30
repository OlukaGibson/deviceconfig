// #define TINY_GSM_MODEM_SIM800
#include "sd_card.h"

#include <SD.h>
#include <SPI.h>
#include <SHT2x.h>
#include <CRC32.h>
#include <EEPROM.h>
#include <TinyGsm.h>
#include <Arduino.h>
#include <Arduino.h>
#include <avr/wdt.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <gsm_communication.h>
#include <ArduinoHttpClient.h>

CRC32 crc;

void powerSD(bool state) {
  pinMode(SD_POWER_SWITCH_PIN, OUTPUT);
  digitalWrite(SD_POWER_SWITCH_PIN, state);
  // Serial.println("SD power state: " + String(state ? "ON" : "OFF"));
  delay(1000);
}

int sdHealthCheck(){
  SPI.begin();
  if (!SD.begin(SD_CS_PIN)){
    // Serial.println("SD card init failed!");
    return 2;
  }else{
    // Serial.println("SD card initialized.");
    return 0;
  }
}

void firmwareDelete() {
  if (SD.exists(FIRMWARE_NAME)) {
    if (SD.remove(FIRMWARE_NAME)) {
      Serial.println(F("File deleted successfully."));
    } else {
      Serial.println(F("Failed to delete file."));
    }
  } else {
    Serial.println(F("File does not exist. No action taken."));
  }
  delay(1000);
}

void firmwareFlash(){
  Serial.println(F("Initiating firmware flash from SD card"));
  
  // Check if firmware file exists first
  if (!SD.exists(FIRMWARE_NAME)) {
    Serial.println(F("Firmware file not found! Aborting flash."));
    return;
  }
  
  Serial.println(F("Setting EEPROM flag and triggering watchdog reset"));
  
  // Set the EEPROM flag to 0xF0 at address 0x1FF
  // This will be read by the bootloader to begin flashing from SD
  EEPROM.write(0x1FF, 0xF0);
  
  // Enable watchdog timer with 500ms timeout
  wdt_enable(WDTO_500MS);
  
  // Wait for watchdog to timeout and reset the device
  // This will trigger the bootloader which will check the EEPROM flag
  delay(600);
  
  // Code should never reach here due to watchdog reset
  Serial.println(F("Error: Watchdog reset failed"));
  return;
}


void verifyFirmware(String fileName, String firmwareCRC32){
  File file = SD.open(fileName);
  if (!file) {
    Serial.println(F("Failed to open file!"));
    return;
  }

  // Process the file in chunks for better performance
  const size_t bufferSize = 64;
  uint8_t buffer[bufferSize];
  
  while (file.available()) {
    // Read up to bufferSize bytes at once
    size_t bytesRead = file.read(buffer, bufferSize);
    
    // Update CRC with the buffer
    for (size_t i = 0; i < bytesRead; i++) {
      crc.update(buffer[i]);
    }
  }
  file.close();
  
  // Get the final CRC value
  uint32_t checksum = crc.finalize();

  Serial.print(F("CRC32: "));
  Serial.println(checksum, HEX);
  if (checksum == strtoul(firmwareCRC32.c_str(), NULL, 16)) {
    Serial.println(F("CRC32 successful!"));
    firmwareFlash();
  } else {
    Serial.println(F("CRC32 failed!"));
    return;
  }
}