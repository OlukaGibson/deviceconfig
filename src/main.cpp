#include <SD.h>
#include <sd_card.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>
#include <gsm_communication.h>

// Create separate SoftwareSerial for PMS Sensor 2
SoftwareSerial pmsSensor2(PMS_SSRX_PIN, PMS_SSTX_PIN);

// configuration methods
void checkEEPROMConfigData();
void collectDeviceData(float* dataBuffer);
void collectMetaData(int* metaBuffer);
int statusValue();

// Single shared data buffer - only declared once
float dataBuffer[11];
int metaBuffer[4];

void setup(){
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("__________________________STARTING device__________________________"));
  Serial1.begin(115200);
  delay(1000);
  
  // Use Serial3 for PMS Sensor 1 (hardware serial)
  Serial3.begin(9600); // SENSOR 1
  delay(1000);
  
  // Use SoftwareSerial for PMS Sensor 2
  pmsSensor2.begin(9600);
  delay(1000);
  
  Serial2.begin(9600); // For GPS
  delay(1000);
}

void loop(){
  Serial.println(F("__________________________STARTING configuration__________________________"));
  checkEEPROMConfigData();
  Serial.println(F("__________________________configuration DONE__________________________"));
  delay(30000);

  Serial.println(F("__________________________STARTING device data collection__________________________"));
  collectDeviceData(dataBuffer); // Use shared buffer instead of allocating new memory
  
  powerGSM(1);
  delay(1000);
  checkNetwork();
  delay(3000);
  connectGPRS();
  delay(3000);
  // Pass individual values from buffer instead of creating new strings each time
  postDeviceData(
    String(dataBuffer[0]), 
    String(dataBuffer[1]), 
    String(dataBuffer[2]), 
    String(dataBuffer[3]), 
    String(dataBuffer[4]), 
    String(dataBuffer[5]), 
    String(dataBuffer[6]), 
    String(dataBuffer[7])
  );
  Serial.println(F("__________________________Device data collection DONE__________________________"));
  disconnectGPRS();
  delay(2000);
  powerGSM(0);
  delay(30000);
  
  Serial.println(F("__________________________STARTING meta data collection__________________________"));
  collectMetaData(metaBuffer);
  postMetaData(
  String(metaBuffer[0]),
  String(metaBuffer[1]),
  String(metaBuffer[2]),
  String(metaBuffer[3])
  );
  Serial.println(F("__________________________Meta data collection DONE__________________________"));
  disconnectGPRS();
  delay(2000);
  powerGSM(0);
  delay(30000); 

  if (fileDownloadState){
    Serial.println(F("__________________________STARTING F.O.T.A. update__________________________"));
    powerGSM(1);
    delay(1000);
    checkNetwork();
    delay(3000);
    connectGPRS();
    delay(3000);
    String resource = "/firmware/" + loadDataFromEEPROM("DEVICE_FIRMWARE_VERSION") + "/download/firmwarebin";
    Serial.println("Resource: " + resource);
    firmwareUpdate(resource);
    Serial.println(F("__________________________F.O.T.A. update DONE__________________________"));
    disconnectGPRS();
    delay(2000);
    powerGSM(0);
    delay(30000);
    }
}