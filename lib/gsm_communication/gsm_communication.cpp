#define TINY_GSM_MODEM_SIM800

#include "gsm_communication.h"
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <pin_definition.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <SD.h>
#include <pin_definition.h>
#include <sd_card.h>
#include <globalVariables.h>

// String apiKey = getConfigValue("DEVICE_WRITE_API_KEY"); // Get the API key from EEPROM
// String channelId = getConfigValue("DEVICE_CHANEL_ID"); // Get the channel ID from EEPROM

void gsmTurnOn() {
    pinMode(GSM_POWER_SWITCH_PIN, OUTPUT);
    digitalWrite(GSM_POWER_SWITCH_PIN, HIGH);
    delay(1000);
}

void gsmTurnOff() {
    pinMode(GSM_POWER_SWITCH_PIN, OUTPUT);
    digitalWrite(GSM_POWER_SWITCH_PIN, LOW);
    delay(1000);
}

void checkNetwork() {
    Serial1.println("AT+CREG?");
    delay(1000);
    Serial1.println("AT+CGATT?");
    delay(1000);
}

int8_t gsmHealthCheck() {
    Serial1.println("AT");
    delay(1000);
    String response = "";
    while (Serial1.available()) {
        char c = Serial1.read();
        if (isDigit(c)) { // Only keep numeric characters
            response += c;
        }
    }
    if (response.length() > 0) {
        return 1;
    } else {
        return 0;
    }
}

String getGSMData(){
  delay(2000);
  String response = "";
  while (Serial1.available()) {
    char c = Serial1.read();
    if (isDigit(c)) { // Only keep numeric characters
      response += c;
    }
  }

  if (response.length() > 0) {
    response.trim();
  } else {
    Serial.println("Failed to get IMSI.");
  }
  return response;
}

String getIMSI() {
  Serial1.println("AT+CIMI");
  String response = getGSMData();
  return response;
}

String getCCID() {
  Serial1.println("AT+CCID");
  String response = getGSMData();
  return response;
}

String getIMEI() {
  Serial1.println("AT+GSN");
  String response = getGSMData();
  return response;
}

String getSignalQuality() {
  Serial1.println("AT+CSQ");
  String response = getGSMData();
  return response;
}

void postData(String url) {
    Serial.print("Requesting URL: ");
    Serial.println(url);

    http.get(url);

    int statusCode = http.responseStatusCode();
    String response = http.responseBody();

    Serial.print("Response Code: ");
    Serial.println(statusCode);
    Serial.print("Response Body: ");
    Serial.println(response);

    if (statusCode == 200) {
        Serial.println("Data sent successfully!");
        // updateEEPROMFromResponse(response); // Update EEPROM with response data
    } else {
        Serial.println("Failed to send data.");
    }

    modem.gprsDisconnect();
    Serial.println("Disconnected from GPRS.");
}

void getConfigDetails(String ccid) {
  String url = "/device/"+ccid+"/selfconfig"; //"/";
  postData(url);
}

void postParameters(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8, String field9, String field10, String field11, String field12) {
  String url = "/update?api_key=" + apiKey + "&field1=" + field1 + "&field2=" + field2 + "&field3=" + field3 + "&field4=" + field4 + "&field5=" + field5 + "&field6=" + field6 + "&field7=" + field7 + "&field8=" + field8 + "&field9=" + field9 + "&field10=" + field10 + "&field11=" + field11 + "&field12=" + field12;
  postData(url);
}

void connectGPRS() {
  Serial.println("Initializing modem...");
  if (!modem.restart()) {
      Serial.println("Failed to restart modem!");
      return;
  } 
  Serial.println("Waiting for network...");
  if (!modem.waitForNetwork()) {
      Serial.println("Network connection failed!");
      return;
  }
  Serial.println("Connected to network!");
  
  Serial.println("Connecting to GPRS...");
  if (!modem.gprsConnect(apn, user, pass)) {
      Serial.println("Failed to connect to GPRS!");
      return;
  }
  Serial.println("GPRS connected!");
}

bool downloadFirmware(const String& version) {
  Serial.println("Starting firmware download...");
  
  // Make sure we're connected to GPRS
  if (!modem.isGprsConnected()) {
    Serial.println("GPRS not connected. Reconnecting...");
    if (!modem.gprsConnect(apn, user, pass)) {
      Serial.println("GPRS connection failed");
      return false;
    }
  }
  
  // Prepare the endpoint
  String endpoint = "/firmware/" + version + "/download/firwmwarebin";
  Serial.print("Requesting firmware from URL: ");
  Serial.println(serverurl1 + endpoint);
  
  // Initialize SD card with better error handling
  Serial.println("Enabling SD card power...");
  pinMode(SD_POWER_SWITCH_PIN, OUTPUT);
  digitalWrite(SD_POWER_SWITCH_PIN, HIGH);
  delay(1000); // Give more time to power up
  
  Serial.println("Setting up SD card pins...");
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH); // Default state for CS pin
  
  // Initialize SPI pins
  pinMode(SD_MOSI_PIN, OUTPUT);
  pinMode(SD_MISO_PIN, INPUT);
  pinMode(SD_SCK_PIN, OUTPUT);
  
  Serial.println("Initializing SD card library...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed! Possible causes:");
    Serial.println("- Card not inserted properly");
    Serial.println("- Card not compatible or damaged");
    Serial.println("- Wiring issue (check connections)");
    Serial.println("- Power issue (check voltage)");
    return false;
  }
  
  Serial.println("SD Card initialized successfully!");
  
  // Create or open file to save firmware
  String fileName = "/firmware_" + version + ".bin";
  if (SD.exists(fileName)) {
    Serial.println("Removing existing firmware file...");
    SD.remove(fileName);
  }
  
  Serial.println("Creating firmware file: " + fileName);
  File firmwareFile = SD.open(fileName, FILE_WRITE);
  if (!firmwareFile) {
    Serial.println("Failed to create firmware file on SD card. Checking card status:");
    
    // Try to diagnose file creation issue
    Serial.println("- Checking root directory access...");
    File root = SD.open("/");
    if (!root) {
      Serial.println("- Unable to open root directory");
    } else {
      Serial.println("- Root directory contents:");
      int fileCount = 0;
      while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        fileCount++;
        Serial.print("  * ");
        Serial.print(entry.name());
        Serial.print(" - ");
        Serial.print(entry.size());
        Serial.println(" bytes");
        entry.close();
      }
      if (fileCount == 0) {
        Serial.println("  (No files found)");
      }
      root.close();
    }
    
    // Try creating a simple test file to diagnose write issues
    Serial.println("- Testing if we can create any file...");
    File testFile = SD.open("/test.txt", FILE_WRITE);
    if (testFile) {
      testFile.println("Test");
      testFile.close();
      Serial.println("- Test file created successfully, but firmware file creation failed");
    } else {
      Serial.println("- Cannot create any files on the SD card");
    }
    
    return false;
  }
  
  // Make the HTTP request
  Serial.println("Connecting to server...");
  http.connectionKeepAlive();
  int err = http.get(endpoint);
  
  if (err != 0) {
    Serial.print("HTTP request failed with error: ");
    Serial.println(err);
    firmwareFile.close();
    return false;
  }
  
  int statusCode = http.responseStatusCode();
  Serial.print("HTTP Response code: ");
  Serial.println(statusCode);
  
  if (statusCode != 200) {
    Serial.println("Failed to download firmware");
    firmwareFile.close();
    return false;
  }
  
  // Get the content length if available
  long contentLength = http.contentLength();
  long bytesWritten = 0;
  
  if (contentLength > 0) {
    Serial.print("Content Length: ");
    Serial.println(contentLength);
  }
  
  // Download and save the file
  Serial.println("Downloading firmware...");
  long timeoutStart = millis();
  const long timeout = 60000; // 60 seconds timeout (increased from 30s)
  const int bufferSize = 128;
  uint8_t buffer[bufferSize];
  int progressPercent = 0;
  int lastPercentReport = -1;
  
  while (http.connected() && (millis() - timeoutStart < timeout)) {
    // Get available data
    int bytesAvailable = http.available();
    
    if (bytesAvailable > 0) {
      timeoutStart = millis(); // Reset timeout
      
      // Read up to buffer size
      int bytesToRead = min(bytesAvailable, bufferSize);
      int bytesRead = http.readBytes(buffer, bytesToRead);
      
      // Write to file
      int bytesWrittenNow = firmwareFile.write(buffer, bytesRead);
      
      if (bytesWrittenNow != bytesRead) {
        Serial.println("Warning: Not all bytes were written to SD card");
        Serial.print("Attempted: ");
        Serial.print(bytesRead);
        Serial.print(", Written: ");
        Serial.println(bytesWrittenNow);
      }
      
      bytesWritten += bytesWrittenNow;
      
      // Show download progress, but only when percentage changes significantly
      if (contentLength > 0) {
        progressPercent = (bytesWritten * 100) / contentLength;
        if (progressPercent != lastPercentReport && (progressPercent % 5 == 0)) {
          lastPercentReport = progressPercent;
          Serial.print("Download progress: ");
          Serial.print(progressPercent);
          Serial.println("%");
        }
      } else {
        // If content length is unknown, show progress every 10KB
        if (bytesWritten % 10240 == 0) {
          Serial.print("Downloaded: ");
          Serial.print(bytesWritten / 1024);
          Serial.println(" KB");
        }
      }
      
      // Flush to SD card every 32KB to reduce the risk of data loss
      if (bytesWritten % 32768 == 0) {
        firmwareFile.flush();
      }
    } else {
      // No data available, wait a bit
      delay(20);
    }
    
    // Check if we've received all the data
    if (contentLength > 0 && bytesWritten >= contentLength) {
      break;
    }
  }
  
  // Ensure all data is written to the card
  firmwareFile.flush();
  
  // Close the file
  firmwareFile.close();
  
  // Stop the HTTP connection
  http.stop();
  
  if (bytesWritten == 0) {
    Serial.println("No data received");
    return false;
  }
  
  Serial.print("Firmware download complete. Saved ");
  Serial.print(bytesWritten);
  Serial.print(" bytes (");
  Serial.print(bytesWritten / 1024);
  Serial.println(" KB) to SD card.");
  
  // Verify the downloaded file
  File checkFile = SD.open(fileName);
  if (!checkFile) {
    Serial.println("Error: Could not open the downloaded firmware file for verification");
    return false;
  }
  
  long fileSize = checkFile.size();
  checkFile.close();
  
  Serial.print("Verified file size: ");
  Serial.print(fileSize);
  Serial.println(" bytes");
  
  if (fileSize != bytesWritten) {
    Serial.println("Warning: File size does not match bytes written");
  }
  
  return true;
}