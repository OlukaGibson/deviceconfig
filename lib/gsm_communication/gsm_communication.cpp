// #define TINY_GSM_MODEM_SIM800
#include "gsm_communication.h"

#include <SD.h>
#include <SHT2x.h>
#include <Arduino.h>
#include <sd_card.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
#include <pin_definition.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>


void disconnectGPRS(){
  modem.gprsDisconnect();
  Serial.println(F("Disconnected from GPRS."));
}

void powerGSM(int state) {
  pinMode(GSM_POWER_SWITCH_PIN, OUTPUT);
  digitalWrite(GSM_POWER_SWITCH_PIN, state ? HIGH : LOW);
  // Serial.println("GSM power state: " + String(state ? "ON" : "OFF"));
  delay(2000);
}

bool checkNetwork() {
  Serial.println(F("Waiting for network..."));
  
  // Set a timeout for network registration
  unsigned long startTime = millis();
  const unsigned long timeout = 60000; // 60 seconds timeout
  
  while (millis() - startTime < timeout) {
    if (modem.isNetworkConnected()) {
      Serial.println(F("Connected to network!"));
      return true;
    }
    delay(1000);
    Serial.print(F("."));
  }
  
  Serial.println(F("Network connection failed!"));
  return false;
}

int gsmHealthCheck() {
  // Serial.println("Testing GSM modem...");
  
  // Use TinyGSM's init/restart function to test if modem is responsive
  if (modem.testAT(1000)) {  // Test with 1000ms timeout
      // Serial.println("GSM modem is responding");
      
      // You can perform additional checks if needed
      // String modemInfo = modem.getModemInfo();
      // Serial.print("Modem info: ");
      // Serial.println(modemInfo);
      
      return 0; // GSM module is responding
  } else {
      // Serial.println("GSM modem not responding");
      return 1; // GSM module is not responding
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
    Serial.println(F("Failed to get IMSI."));
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
    Serial.print(F("Requesting URL: "));
    Serial.println(url);

    http.get(url);

    int statusCode = http.responseStatusCode();
    responseData = http.responseBody();

    Serial.print(F("Response Code: "));
    Serial.println(statusCode);
    Serial.print(F("Response Body: "));
    Serial.println(responseData);

    if (statusCode == 200) {
        Serial.println(F("Data sent successfully!"));
        // return response;
        // updateEEPROMFromResponse(response); // Update EEPROM with response data
    } else {
        Serial.println(F("Failed to send data."));
        // return "null";
    }

    // modem.gprsDisconnect();
    // Serial.println("Disconnected from GPRS.");
}

void getEEPROMData(String ccid) {
  String url =  "/device/"+ccid+"/selfconfig"; //"/";
  postData(url);
  // return postData(url);
}

void getConfigData(String deviceID) {
  String url = "/device/"+deviceID+"/getconfig";
  
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
    Serial.println("Config data received successfully!");
    
    // Parse the JSON response
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);
    
    if (error) {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
    } else {
      // Update global variables with configuration values
      String deploymentMode = doc["configs"][" Deployment Mode"] | "0";
      String batteryMonitoring = doc["configs"]["Chip Based battery monitoring"] | "0";
      String debugEnable = doc["configs"]["Debug Enable"] | "0";
      String pmSampleEntries = doc["configs"]["PM sample entries"] | "0";
      String spv = doc["configs"]["SPV"] | "0";
      String sdCardPin = doc["configs"]["Sd card pin"] | "0";
      String transmissionMode = doc["configs"]["Transmission Mode"] | "0";
      String firmwareVersion = String(doc["firmwareVersion"] | "null");
      fileDownloadState = doc["fileDownloadState"] | false;
      // deviceID = String(doc["deviceID"] | "0");
      String firmwareCRC32 = doc["firmwareCRC32"] | "null";
      
      // Log retrieved values
      Serial.print(F("Deployment Mode: "));
      Serial.println(deploymentMode);
      Serial.print(F("Battery Monitoring: "));
      Serial.println(batteryMonitoring);
      Serial.print(F("Debug Enable: "));
      Serial.println(debugEnable);
      Serial.print(F("PM Sample Entries: "));
      Serial.println(pmSampleEntries);
      Serial.print(F("SPV: "));
      Serial.println(spv);
      Serial.print(F("SD Card Pin: "));
      Serial.println(sdCardPin);
      Serial.print(F("Transmission Mode: "));
      Serial.println(transmissionMode);
      Serial.print(F("Firmware Version: "));
      Serial.println(firmwareVersion);
      Serial.print(F("File Download State: "));
      Serial.println(fileDownloadState ? "true" : "false");
      Serial.print(F("Device ID: "));
      Serial.println(deviceID);
      Serial.print(F("Firmware CRC32: "));
      Serial.println(firmwareCRC32);
    }
  } else {
    Serial.println("Failed to get configuration data.");
  }

  // modem.gprsDisconnect();
  // Serial.println("Disconnected from GPRS.");
}

void postMetaData(String metadata1, String metadata2, String metadata3, String metadata4) {
  String url = "/metadataupdate?api_key=" + loadDataFromEEPROM("DEVICE_WRITE_API_KEY") + "&metadata1=" + metadata1 + "&metadata2=" + metadata2 + "&metadata3=" + metadata3 + "&metadata4=" + metadata4;
  postData(url);
}

void postDeviceData(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8) {
  String url = "/update?api_key=" + loadDataFromEEPROM("DEVICE_WRITE_API_KEY") + "&field1=" + field1 + "&field2=" + field2 + "&field3=" + field3 + "&field4=" + field4 + "&field5=" + field5 + "&field6=" + field6 + "&field7=" + field7 + "&field8=" + field8;
  postData(url);
}

bool connectGPRS() {
  Serial.println(F("Initializing modem..."));
  if (!modem.restart()) {
      Serial.println(F("Failed to restart modem!"));
      return false;
  } 
  Serial.println(F("Modem initialized successfully."));
  
  Serial.println(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
      Serial.println(F("Network connection failed!"));
      return false;
  }
  Serial.println(F("Connected to network!"));
  
  Serial.println(F("Connecting to GPRS..."));
  if (!modem.gprsConnect(apn, user, pass)) {
      Serial.println(F("Failed to connect to GPRS!"));
      return false;
  }
  Serial.println(F("GPRS connected!"));

  client.setTimeout(30000); // 30 seconds timeout
  return true;
}

int8_t firmwareDownload(String resource) {
  Serial.println(F("Starting firmware download..."));
  http.beginRequest();
  http.get(resource);
  http.sendHeader("Accept", "application/octet-stream");
  http.endRequest();

  // Check response status
  int statusCode = http.responseStatusCode();
  Serial.print(F("Status code: "));
  Serial.println(statusCode);

  if (statusCode != 200 && statusCode != 206) {
    Serial.print(F("Failed to get file. Status code: "));
    Serial.println(statusCode);
    return -1;  // Return error code
  }

  // Get content length
  uint32_t length = http.contentLength();
  Serial.print(F("Content-Length: "));
  Serial.println(length);
  
  // Use expected size if content length is invalid
  if (length <= 0) {
    Serial.println(F("Invalid content length. Using expected size instead."));
    length = EXPECTED_SIZE;
  }

  // Allocate buffer only when needed
  byte* buff = (byte*)malloc(chunkSize);
  if (!buff) {
    Serial.println(F("Failed to allocate memory for download buffer"));
    return -1;
  }

  // Prepare file for writing
  File file = SD.open(FIRMWARE_NAME, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to open file on SD card."));
    free(buff);  // Free the buffer before returning
    return -1;
  }

  // Reset tracking variables
  downloaded = 0;
  receivingData = false;
  lastDataTime = millis();
  downloadStartTime = millis();
  lastProgressTime = millis();

  while (true) {
    // Check for available data
    if (http.available()) {
      lastDataTime = millis();
      receivingData = true;
      
      size_t available = http.available();
      size_t toRead = min(available, chunkSize);
      int bytesRead = http.read(buff, toRead);
      
      // Process received data
      if (bytesRead > 0) {
        size_t bytesWritten = file.write(buff, bytesRead);
        if (bytesWritten != (size_t)bytesRead) {
          Serial.println(F("Write error!"));
          break;
        }
        downloaded += bytesRead;
        
        // Progress indicators remain the same
        if (downloaded % 4096 == 0) {
          Serial.print(F("."));
        }
        
        if (millis() - lastProgressTime > 5000) {
          Serial.println();
          Serial.print(F("Downloaded: "));
          Serial.print(downloaded / 1024);
          Serial.print(F("KB ("));
          if (length > 0) {
            Serial.print((downloaded * 100) / length);
            Serial.print(F("%"));
          }
          Serial.println(F(")"));
          lastProgressTime = millis();
        }
      }
    } 
    else {
      if (!http.connected()) {
        if (receivingData) {
          Serial.println(F("\nServer disconnected. Download may be complete."));
          break;
        }
      }
      
      if (receivingData && (millis() - lastDataTime > 20000)) {
        Serial.println(F("\nData reception timeout."));
        break;
      }
      
      delay(100);
    }
    
    if (millis() - downloadStartTime > 300000) {
      Serial.println(F("\nDownload timeout - maximum time exceeded!"));
      break;
    }
  }
  
  file.close();
  
  // Verify downloaded file
  File readFile = SD.open(FIRMWARE_NAME);
  if (readFile) {
    unsigned long fileSize = readFile.size();
    readFile.close();
    
    Serial.print(F("\nDownload completed. Total bytes: "));
    Serial.println(downloaded);
    Serial.print(F("Saved file size: "));
    Serial.println(fileSize);

    // Free the buffer before verification
    free(buff);

    // Verify file size
    if (fileSize < EXPECTED_SIZE) {
      Serial.println(F("WARNING: File appears incomplete!"));
      return 1;
    } else {
      Serial.println(F("File download appears successful."));
      return 0;
    }
  } else {
    Serial.println(F("Error opening file for verification."));
    free(buff);  // Don't forget to free the buffer on this path too
    return -1;
  }
}

int8_t resumeFirmwareDownload(String resource) {
  // Check already downloaded size
  File file = SD.open(FIRMWARE_NAME, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to open file for resuming."));
    return -1;
  }

  uint32_t alreadyDownloaded = file.size();
  Serial.print(F("Resuming from byte: "));
  Serial.println(alreadyDownloaded);

  // Allocate buffer only when needed
  byte* buff = (byte*)malloc(chunkSize);
  if (!buff) {
    Serial.println(F("Failed to allocate memory for download buffer"));
    file.close();
    return -1;
  }

  // Setup HTTP with Range header
  http.beginRequest();
  http.sendHeader("Range", "bytes=" + String(alreadyDownloaded) + "-");
  Serial.print(F("Range: bytes="));
  Serial.print(alreadyDownloaded);
  Serial.println("-");
  http.get(resource);
  http.sendHeader("Accept", "application/octet-stream");
  http.endRequest();

  int statusCode = http.responseStatusCode();
  Serial.print(F("Status code: "));
  Serial.println(statusCode);

  if (statusCode != 206 && statusCode != 200) {
    Serial.println(F("Server did not support partial content or failed."));
    file.close();
    free(buff);
    return 1; // retry later
  }

  uint32_t contentLength = http.contentLength();
  if (contentLength <= 0) {
    Serial.println(F("Invalid or missing content length."));
    file.close();
    free(buff);
    return 1;
  }

  uint32_t totalDownloaded = alreadyDownloaded;
  lastDataTime = millis();
  downloadStartTime = millis();
  receivingData = false;

  while (true) {
    if (http.available()) {
      receivingData = true;
      lastDataTime = millis();

      size_t available = http.available();
      size_t toRead = min(available, chunkSize);
      int bytesRead = http.read(buff, toRead);

      if (bytesRead > 0) {
        size_t written = file.write(buff, bytesRead);
        if (written != (size_t)bytesRead) {
          Serial.println(F("Write error during resume."));
          file.close();
          free(buff);
          return -1;
        }
        totalDownloaded += bytesRead;

        if (totalDownloaded % 4096 == 0) {
          Serial.print(".");
        }

        if (millis() - lastProgressTime > 5000) {
          Serial.println();
          Serial.print(F("Downloaded: "));
          Serial.print(totalDownloaded / 1024);
          Serial.print(F("KB"));
          Serial.println();
          lastProgressTime = millis();
        }
      }
    } else {
      if (!http.connected()) {
        Serial.println(F("\nDisconnected from server."));
        break;
      }

      if (receivingData && (millis() - lastDataTime > 20000)) {
        Serial.println(F("\nTimeout: No data for 20 seconds."));
        break;
      }

      delay(100);
    }

    if (millis() - downloadStartTime > 300000) {
      Serial.println(F("\nDownload timeout (5 minutes)."));
      break;
    }
  }

  file.close();
  
  // Don't forget to free the buffer before returning
  free(buff);

  File finalFile = SD.open(FIRMWARE_NAME);
  if (!finalFile) {
    Serial.println(F("Failed to reopen file for verification."));
    return -1;
  }

  uint32_t finalSize = finalFile.size();
  finalFile.close();

  Serial.print(F("Final file size: "));
  Serial.println(finalSize);
  Serial.print(F("EXPECTED_SIZE: "));
  Serial.println(EXPECTED_SIZE);

  if (finalSize < EXPECTED_SIZE) {
    Serial.println(F("Download not complete. Can resume later."));
    return 1;
  } else if (finalSize == EXPECTED_SIZE) {
    Serial.println(F("Firmware download complete!"));
    disconnectGPRS();
    return 0;
  } else {
    Serial.println(F("File size mismatch. Possible error during download."));
    return -1;
  }
}

// First, add these missing functions
void firmwareDelete() {
  // Delete firmware file if it exists
  if (SD.exists(FIRMWARE_NAME)) {
    Serial.println(F("Deleting existing firmware file..."));
    SD.remove(FIRMWARE_NAME);
  }
}

bool verifyFirmware(const char* firmwareFile, String expectedCRC) {
  File file = SD.open(firmwareFile);
  if (!file) {
    Serial.println(F("Failed to open firmware file for verification"));
    return false;
  }
  
  // Simple size verification for now
  unsigned long fileSize = file.size();
  file.close();
  
  Serial.print(F("Firmware file size: "));
  Serial.println(fileSize);
  
  if (fileSize == EXPECTED_SIZE) {
    Serial.println(F("Firmware size verification passed"));
    // You would add actual CRC checking here
    return true;
  } else {
    Serial.println(F("Firmware size verification failed"));
    return false;
  }
}

void firmwareUpdate(String resource){
  // Initialize SD card first
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("SD card initialization failed!"));
    return;
  }
  Serial.println(F("SD card initialized successfully."));

  firmwareDelete();
  Serial.println(F("Starting firmware update..."));
  int8_t fileState = firmwareDownload(resource);
  // int8_t fileState = 1;
  while (fileState == 1) {
    Serial.println(F("Resuming firmware download..."));
    fileState = resumeFirmwareDownload(resource);
  }
  verifyFirmware(FIRMWARE_NAME, "103848AF");
}

// Now the improved firmwareUpdate function
// void firmwareUpdate(String resource) {
//   Serial.println("Resource: " + resource);
  
//   // Check current fileState and handle accordingly
//   if (fileState == -1) {
//     Serial.println(F("Previous error state. Aborting firmware update."));
//     firmwareDelete();
//     fileState = 0;
//     return;
//   }
  
//   // If fileState is 1 (incomplete download), check if file exists
//   if (fileState == 1) {
//     if (SD.exists(FIRMWARE_NAME)) {
//       Serial.println(F("Incomplete download found. Resuming..."));
//       // File exists, attempt to resume download
//     } else {
//       Serial.println(F("Incomplete download marked but file missing. Starting fresh download."));
//       fileState = 0; // Reset to start fresh download
//     }
//   }
  
//   // If fileState is 0 or reset above, start fresh download
//   if (fileState == 0) {
//     firmwareDelete(); // Start with a clean slate
//     fileState = firmwareDownload(resource);
//   }
  
//   // Retry logic - try to resume download up to 5 times if incomplete
//   int retryCount = 0;
//   while (fileState == 1 && retryCount < 5) {
//     Serial.print(F("Retry #"));
//     Serial.print(retryCount + 1);
//     Serial.println(F(" resuming firmware download..."));
//     delay(1000);
//     fileState = resumeFirmwareDownload(resource);
//     retryCount++;
//   }
  
//   // Process final state
//   if (fileState == 0) {
//     // Success - verify the download
//     if (SD.exists(FIRMWARE_NAME)) {
//       File file = SD.open(FIRMWARE_NAME);
//       if (file) {
//         unsigned long fileSize = file.size();
//         file.close();
        
//         if (fileSize == EXPECTED_SIZE) {
//           // File size matches expected, verify CRC
//           Serial.println(F("File size matches expected. Verifying firmware..."));
//           if (verifyFirmware(FIRMWARE_NAME, loadDataFromEEPROM("DEVICE_FIRMWARE_CRC32"))) {
//             Serial.println(F("Firmware verified successfully!"));
//             // Here you would apply the firmware update
//           } else {
//             Serial.println(F("Firmware verification failed."));
//             fileState = -1;
//             firmwareDelete();
//           }
//         } else if (fileSize > EXPECTED_SIZE) {
//           // File is larger than expected - error
//           Serial.println(F("File size exceeds expected size. Possible corruption."));
//           fileState = -1;
//           firmwareDelete();
//         } else {
//           // File is smaller than expected - still incomplete
//           Serial.println(F("File size smaller than expected. Download incomplete."));
//           fileState = 1; // Mark for future resumption
//         }
//       }
//     } else {
//       Serial.println(F("Firmware file not found after reported successful download."));
//       fileState = -1;
//       firmwareDelete();
//     }
//   } else if (fileState == 1) {
//     // Still incomplete after retries
//     Serial.println(F("Firmware download still incomplete after maximum retries."));
//     Serial.println(F("Will attempt again on next update cycle."));
//   } else {
//     // Error state
//     Serial.println(F("Firmware download failed with errors."));
//     fileState = -1;
//     firmwareDelete();
//   }
// }

