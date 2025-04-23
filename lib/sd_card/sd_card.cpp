#include "sd_card.h"
#include <pin_definition.h>
#include <SD.h>
#include <SPI.h>
#include <globalVariables.h>
#include <gsm_communication.h>
#include <Arduino.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include <CRC32.h>

CRC32 crc;

void powerSD(bool state) {
  pinMode(SD_POWER_SWITCH_PIN, OUTPUT);
  digitalWrite(SD_POWER_SWITCH_PIN, state);
  Serial.println("SD power state: " + String(state ? "ON" : "OFF"));
  delay(1000);
}

int16_t sdHealthCheck(){
  SPI.begin();
  if (!SD.begin(SD_CS_PIN)){
    Serial.println("SD card init failed!");
    return 2;
  }else{
    Serial.println("SD card initialized.");
    return 0;
  }
}

void firmwareDelete(String fileName) {
  if (SD.exists(fileName)) {
    if (SD.remove(fileName)) {
      Serial.println("File deleted successfully.");
    } else {
      Serial.println("Failed to delete file.");
    }
  } else {
    Serial.println("File does not exist. No action taken.");
  }
  delay(1000);
}

void firmwareFlash(){
  Serial.println("Initiating firmware flash from SD card");
  
  // Check if firmware file exists first
  if (!SD.exists(FIRMWARE_NAME)) {
    Serial.println("Firmware file not found! Aborting flash.");
    return;
  }
  
  Serial.println("Setting EEPROM flag and triggering watchdog reset");
  
  // Set the EEPROM flag to 0xF0 at address 0x1FF
  // This will be read by the bootloader to begin flashing from SD
  EEPROM.write(0x1FF, 0xF0);
  
  // Enable watchdog timer with 500ms timeout
  wdt_enable(WDTO_500MS);
  
  // Wait for watchdog to timeout and reset the device
  // This will trigger the bootloader which will check the EEPROM flag
  delay(600);
  
  // Code should never reach here due to watchdog reset
  Serial.println("Error: Watchdog reset failed");
  return;
}


void verifyFirmware(String fileName, String firmwareCRC32){
  File file = SD.open(fileName);
  if (!file) {
    Serial.println("Failed to open file!");
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

  Serial.print("CRC32: ");
  Serial.println(checksum, HEX);
  if (checksum == strtoul(firmwareCRC32.c_str(), NULL, 16)) {
    Serial.println("Firmware CRC32 verification successful!");
    firmwareFlash();
  } else {
    Serial.println("Firmware CRC32 verification failed!");
    return;
  }
}