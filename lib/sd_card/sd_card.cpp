#include "sd_card.h"
#include <pin_definition.h>
#include <SD.h>
#include <SPI.h>
#include <globalVariables.h>
#include <gsm_communication.h>
#include <Arduino.h>
#include <avr/wdt.h>
#include <EEPROM.h>

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

void firmwareRename(String fileName) {
  // If old file exists, rename it to backup
  if (SD.exists(fileName)) {
    Serial.println("Old firmware file exists. Renaming to backup...");
    File oldFile = SD.open(fileName);
    if (oldFile) {
        File newFile = SD.open("/backup_firmware.bin", FILE_WRITE);
        if (newFile) {
            while (oldFile.available()) {
                newFile.write(oldFile.read());
            }
            newFile.close();
        }
        oldFile.close();
        SD.remove(fileName);
    } else {
        Serial.println("Failed to open old firmware file for renaming.");
    }
  } else {
    Serial.println("No old firmware file found.");
  }
}

void firmwareFlash(){
  Serial.println("Initiating firmware flash from SD card");
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
}