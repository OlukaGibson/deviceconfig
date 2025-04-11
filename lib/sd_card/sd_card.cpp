#include "sd_card.h"
#include <pin_definition.h>

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