// #include <Arduino.h>
// /*
//   Blink
//   Turns on an LED on for one second, then off for one second, repeatedly.
 
//   This example code is in the public domain.
//  */
 
// // Pin 13 has an LED connected on most Arduino boards.
// // give it a name:
// int led = 13;
// #include "EEPROM.h"
// #include <avr/wdt.h>
// // the setup routine runs once when you press reset:
// void setup() {                
//   Serial.begin(115200);
//   Serial.println("Press 'F' and 'enter' to set flagbit in eeprom 0x1FF to 0xF0 ");
//   Serial.println("also to induce watchdog timeout which triggers the bootloader ");
//   Serial.println("and flashes the new firmware on the sd card");

// }

// // the loop routine runs over and over again forever:
// void loop() {
//  char inChar = '\0';

//  while (Serial.available() > 0) {
//    inChar = Serial.read();
//  }
//    wdt_enable(WDTO_500MS); // have the wdt reset the chip
//   // if 500ms passes without a reset
//  if(inChar == 'F'){
//     Serial.println("");
//     Serial.println("rebooting and flashing with firmware.bin on sdcard"); 
//     EEPROM.write(0x1FF,0xF0);
//     wdt_reset();
//     delay(600); // wait 600ms to timeout wdt
//  }
// }

// // #define TINY_GSM_MODEM_SIM800

// // #include <TinyGsmClient.h>
// // #include <ArduinoHttpClient.h>
// // #include <SD.h>
// // #include <SPI.h>

// // // GSM module settings
// // #define GSM_POWER_SWITCH_PIN    A0
// // #define RX1_PIN                 19
// // #define TX1_PIN                 18
// // #define GSM_RESET_PIN           10

// // // SD card settings
// // #define SD_POWER_SWITCH_PIN     30
// // #define SD_CS_PIN               28
// // #define SD_MOSI_PIN             51
// // #define SD_MISO_PIN             50
// // #define SD_SCK_PIN              52

// // #define FILE_NAME "/firmware.bin"

// // // APN config
// // const char apn[]  = "TM"; // Replace with your SIM card APN
// // const char user[] = "";
// // const char pass[] = "";

// // // URL to download
// // const char server[] = "34.132.108.60";
// // const int port = 80;
// // const char resource[] = "/firmware/42.74/download/firwmwarebin";

// // // Firmware expected size
// // const unsigned long EXPECTED_SIZE = 131481; // ~128.4KB

// // // Globals
// // TinyGsm modem(Serial1);
// // TinyGsmClient client(modem);
// // HttpClient http(client, server, port);

// // void powerGSM(bool state) {
// //   digitalWrite(GSM_POWER_SWITCH_PIN, state);
// // }

// // void resetGSM() {
// //   digitalWrite(GSM_RESET_PIN, LOW);
// //   delay(100);
// //   digitalWrite(GSM_RESET_PIN, HIGH);
// //   delay(1000);
// // }

// // void powerSD(bool state) {
// //   digitalWrite(SD_POWER_SWITCH_PIN, state);
// // }

// // void setup() {
// //   Serial.begin(115200);
// //   delay(10);

// //   // Power and reset control pins
// //   pinMode(GSM_POWER_SWITCH_PIN, OUTPUT);
// //   pinMode(GSM_RESET_PIN, OUTPUT);
// //   pinMode(SD_POWER_SWITCH_PIN, OUTPUT);

// //   powerGSM(true);
// //   resetGSM();

// //   // Begin Serial1 for GSM
// //   Serial1.begin(9600);

// //   Serial.println("Initializing modem...");
// //   modem.restart();

// //   Serial.print("Connecting to GPRS...");
// //   if (!modem.gprsConnect(apn, user, pass)) {
// //     Serial.println(" fail");
// //     while (true);
// //   }
// //   Serial.println(" success");
  
// //   // Set client timeout
// //   client.setTimeout(30000); // 30 seconds timeout

// //   // Power on SD card
// //   powerSD(true);
// //   delay(100);

// //   SPI.begin();
// //   if (!SD.begin(SD_CS_PIN)) {
// //     Serial.println("SD card init failed!");
// //     while (true);
// //   }
// //   Serial.println("SD card initialized.");

// //   // If old file exists, delete it
// //   if (SD.exists(FILE_NAME)) {
// //     SD.remove(FILE_NAME);
// //     Serial.println("Removed old firmware file.");
// //   }

// //   // Start HTTP GET request
// //   Serial.println("Sending GET request...");
// //   http.beginRequest();
// //   http.get(resource);
// //   http.sendHeader("Accept", "application/octet-stream");
// //   http.endRequest();

// //   // Check response status
// //   int statusCode = http.responseStatusCode();
// //   Serial.print("Status code: ");
// //   Serial.println(statusCode);
  
// //   if (statusCode != 200) {
// //     Serial.print("Failed to get file. Status code: ");
// //     Serial.println(statusCode);
// //     return;
// //   }
  
// //   // Get content length
// //   int length = http.contentLength();
// //   Serial.print("Content-Length: ");
// //   Serial.println(length);
  
// //   // Use expected size if content length is invalid
// //   if (length <= 0) {
// //     Serial.println("Invalid content length. Using expected size instead.");
// //     length = EXPECTED_SIZE;
// //   }
  
// //   // Prepare file for writing
// //   File file = SD.open(FILE_NAME, FILE_WRITE);
// //   if (!file) {
// //     Serial.println("Failed to open file on SD card.");
// //     return;
// //   }

// //   // Download variables
// //   const size_t chunkSize = 1024; // 1KB chunks
// //   byte buff[chunkSize];
// //   unsigned long downloaded = 0;
// //   unsigned long lastProgressTime = millis();
// //   unsigned long downloadStartTime = millis();
// //   unsigned long lastDataTime = millis();
// //   boolean receivingData = false;
  
// //   Serial.println("Starting download...");
  
// //   // Main download loop
// //   while (true) {
// //     // Check for available data
// //     if (http.available()) {
// //       lastDataTime = millis();
// //       receivingData = true;
      
// //       size_t available = http.available();
// //       size_t toRead = min(available, chunkSize);
// //       int bytesRead = http.read(buff, toRead);
      
// //       // Process received data
// //       if (bytesRead > 0) {
// //         size_t bytesWritten = file.write(buff, bytesRead);
// //         if (bytesWritten != (size_t)bytesRead) {
// //           Serial.println("Write error!");
// //           break;
// //         }
// //         downloaded += bytesRead;
        
// //         // Print progress indicators
// //         if (downloaded % 4096 == 0) { // Every 4KB
// //           Serial.print(".");
// //         }
        
// //         // Print progress percentage
// //         if (millis() - lastProgressTime > 5000) {  // Every 5 seconds
// //           Serial.println();
// //           Serial.print("Downloaded: ");
// //           Serial.print(downloaded / 1024);
// //           Serial.print("KB (");
// //           if (length > 0) {
// //             Serial.print((downloaded * 100) / length);
// //             Serial.print("%");
// //           }
// //           Serial.println(")");
// //           lastProgressTime = millis();
// //         }
// //       }
// //     } 
// //     else {
// //       // Check if we're done or disconnected
// //       if (!http.connected()) {
// //         if (receivingData) {
// //           Serial.println("\nServer disconnected. Download may be complete.");
// //           break;
// //         }
// //       }
      
// //       // Check for data timeout (10 seconds without data)
// //       if (receivingData && (millis() - lastDataTime > 10000)) {
// //         Serial.println("\nData reception timeout.");
// //         break;
// //       }
      
// //       // Small delay when no data is available
// //       delay(100);
// //     }
    
// //     // Safety timeout - abort if downloading takes too long (5 minutes)
// //     if (millis() - downloadStartTime > 300000) {
// //       Serial.println("\nDownload timeout - maximum time exceeded!");
// //       break;
// //     }
// //   }

// //   file.close();
  
// //   // Verify downloaded file
// //   File readFile = SD.open(FILE_NAME);
// //   if (readFile) {
// //     unsigned long fileSize = readFile.size();
// //     readFile.close();
    
// //     Serial.print("\nDownload completed. Total bytes: ");
// //     Serial.println(downloaded);
// //     Serial.print("Saved file size: ");
// //     Serial.println(fileSize);
    
// //     // Verify file size
// //     if (fileSize < 10000) { // If smaller than 10KB
// //       Serial.println("WARNING: File appears incomplete!");
// //     } else {
// //       Serial.println("File download appears successful.");
// //     }
// //   } else {
// //     Serial.println("Error opening file for verification.");
// //   }
  
// //   // Disconnect from network
// //   modem.gprsDisconnect();
// //   Serial.println("GPRS disconnected");
// // }

// // void loop() {
// //   // Nothing here
// // }