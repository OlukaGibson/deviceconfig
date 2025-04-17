#include "sd_card.h"
#include <pin_definition.h>
#include <SD.h>
#include <SPI.h>
#include <globalVariables.h>
#include <gsm_communication.h>
#include <Arduino.h>

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












bool initSDCard() {
  Serial.println("Initializing SD card...");
  
  // Power up the SD card
  pinMode(SD_POWER_SWITCH_PIN, OUTPUT);
  digitalWrite(SD_POWER_SWITCH_PIN, HIGH);
  delay(500); // Give it time to power up
  
  // Set up SPI pins (may be needed depending on your hardware)
  pinMode(SD_CS_PIN, OUTPUT);
  
  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed!");
    return false;
  }
  
  Serial.println("SD Card initialized successfully!");
  return true;
}

void testSDCard() {
  if (!initSDCard()) {
    Serial.println("SD Card test failed - could not initialize card.");
    return;
  }
  
  // Try to create a test file
  const char* testFileName = "/sdtest.txt";
  
  // Remove the file if it exists
  if (SD.exists(testFileName)) {
    Serial.println("Removing existing test file...");
    SD.remove(testFileName);
  }
  
  // Create and open the file
  File testFile = SD.open(testFileName, FILE_WRITE);
  if (!testFile) {
    Serial.println("SD Card test failed - could not create file!");
    return;
  }
  
  // Write to the file
  if (testFile.println("This is a test file.")) {
    Serial.println("Successfully wrote to test file!");
  } else {
    Serial.println("SD Card test failed - write error!");
    testFile.close();
    return;
  }
  
  testFile.close();
  
  // Verify by reading the file
  testFile = SD.open(testFileName);
  if (!testFile) {
    Serial.println("SD Card test failed - could not open file for reading!");
    return;
  }
  
  Serial.println("Reading from test file:");
  while (testFile.available()) {
    Serial.write(testFile.read());
  }
  
  testFile.close();
  Serial.println("\nSD Card test completed successfully!");
  // Remove the cardSize function as it's not available in this SD library
  Serial.println("SD Card is working properly");
}