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

void powerGSM(bool state) {
    pinMode(GSM_POWER_SWITCH_PIN, OUTPUT);
    digitalWrite(GSM_POWER_SWITCH_PIN, state);
    // Serial.println("GSM power state: " + String(state ? "ON" : "OFF"));
    delay(2000);
}

void checkNetwork() {
    Serial1.println("AT+CREG?");
    delay(1000);
    Serial1.println("AT+CGATT?");
    delay(1000);
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
  // String url = "/device/"+deviceID+"/getconfig";
  
  // Serial.print("Requesting URL: ");
  // Serial.println(url);

  // http.get(url);

  // int statusCode = http.responseStatusCode();
  // String response = http.responseBody();

  // Serial.print("Response Code: ");
  // Serial.println(statusCode);
  // Serial.print("Response Body: ");
  // Serial.println(response);

  // if (statusCode == 200) {
  //   Serial.println("Config data received successfully!");
    
  //   // Parse the JSON response
  //   DynamicJsonDocument doc(1024);
  //   DeserializationError error = deserializeJson(doc, response);
    
  //   if (error) {
  //     Serial.print("JSON parsing failed: ");
  //     Serial.println(error.c_str());
  //   } else {
  //     // Update global variables with configuration values
  //     deploymentMode = doc["configs"][" Deployment Mode"] | "null";
  //     batteryMonitoring = doc["configs"]["Chip Based battery monitoring"] | "null";
  //     debugEnable = doc["configs"]["Debug Enable"] | "null";
  //     pmSampleEntries = doc["configs"]["PM sample entries"] | "null";
  //     spv = doc["configs"]["SPV"] | "null";
  //     sdCardPin = doc["configs"]["Sd card pin"] | "null";
  //     transmissionMode = doc["configs"]["Transmission Mode"] | "null";
  //     firmwareVersion = String(doc["firmwareVersion"] | "null");
  //     fileDownloadState = doc["fileDownloadState"] | false;
  //     deviceID = String(doc["deviceID"] | "null");
  //     firmwareCRC32 = doc["firmwareCRC32"] | "null";
      
  //     // Log retrieved values
  //     Serial.println("Deployment Mode: " + deploymentMode);
  //     Serial.println("Battery Monitoring: " + batteryMonitoring);
  //     Serial.println("Debug Enable: " + debugEnable);
  //     Serial.println("PM Sample Entries: " + pmSampleEntries);
  //     Serial.println("SPV: " + spv);
  //     Serial.println("SD Card Pin: " + sdCardPin);
  //     Serial.println("Transmission Mode: " + transmissionMode);
  //     Serial.println("Firmware Version: " + firmwareVersion);
  //     Serial.println("File Download State: " + String(fileDownloadState ? "true" : "false"));
  //     Serial.println("Device ID: " + deviceID);
  //     Serial.println("Firmware CRC32: " + firmwareCRC32);
  //   }
  // } else {
  //   Serial.println("Failed to get configuration data.");
  // }

  // // modem.gprsDisconnect();
  // // Serial.println("Disconnected from GPRS.");
}

void postMetaData(String metadata1, String metadata2, String metadata3, String metadata4) {
  String url = "/metadataupdate?api_key=" + loadDataFromEEPROM("DEVICE_WRITE_API_KEY") + "&metadata1=" + metadata1 + "&metadata2=" + metadata2 + "&metadata3=" + metadata3 + "&metadata4=" + metadata4;
  postData(url);
}

void postDeviceData(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8) {
  String url = "/update?api_key=" + loadDataFromEEPROM("DEVICE_WRITE_API_KEY") + "&field1=" + field1 + "&field2=" + field2 + "&field3=" + field3 + "&field4=" + field4 + "&field5=" + field5 + "&field6=" + field6 + "&field7=" + field7 + "&field8=" + field8;
  postData(url);
}

void connectGPRS() {
  Serial.println(F("Initializing modem..."));
  if (!modem.restart()) {
      Serial.println(F("Failed to restart modem!"));
      return;
  } 
  Serial.println(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
      Serial.println(F("Network connection failed!"));
      return;
  }
  Serial.println(F("Connected to network!"));
  
  Serial.println(F("Connecting to GPRS..."));
  if (!modem.gprsConnect(apn, user, pass)) {
      Serial.println(F("Failed to connect to GPRS!"));
      return;
  }
  Serial.println(F("GPRS connected!"));

  client.setTimeout(30000); // 30 seconds timeout
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

void firmwareUpdate(String resource) {
  // if (fileDownloadState){
    Serial.println("Resource: " + resource);
    if(fileState == -1){
      Serial.println(F("Aborting firmware update."));
      firmwareDelete();
      fileState = 0;
      return;
    }else if(fileState == 0){
      firmwareDelete();
      fileState = firmwareDownload(resource);
    }else if(fileState == 1){
      pass;
    }else{
      Serial.println("fileState: " + String(fileState) + "Aborting firmware update.");
      firmwareDelete();
      fileState = 0;
      return;
    }
    for(int i = 0; i < 5; i++){
      if(fileState == 1){
        Serial.println(F("Retrying firmware download..."));
        delay(1000); 
        fileState = resumeFirmwareDownload(resource);
      } else {
        break;
      }
    }
    if(fileState == 0){
      if (SD.exists(FIRMWARE_NAME)) {
        verifyFirmware(FIRMWARE_NAME, loadDataFromEEPROM("DEVICE_FIRMWARE_CRC32"));
      } else {
        Serial.println(F("Firmware file not found after download attempt"));
      }
    }else{
      Serial.println(F("Firmware download failed"));
      return;
    }
  // }
}
