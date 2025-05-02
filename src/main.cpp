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
float* collectDeviceData();
int* collectMetaData();
int statusValue();

void setup(){
  Serial.begin(115200);
  delay(1000);
  Serial.println("__________________________STARTING device__________________________");
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
  Serial.println("__________________________STARTING configuration__________________________");
  checkEEPROMConfigData();
  Serial.println("__________________________configuration DONE__________________________");
  delay(30000);

  Serial.println("__________________________STARTING device data collection__________________________");
  float* data = collectDeviceData();
  // String extra_data = String(data[7]) + "," + String(data[8]) + "," + String(data[9]) + "," + String(data[10]);
  // Serial.println("Extra data: " + extra_data);
  powerGSM(1);
  delay(1000);
  checkNetwork();
  delay(3000);
  connectGPRS();
  delay(3000);
  postDeviceData(String(data[0]), String(data[1]), String(data[2]), String(data[3]), String(data[4]), String(data[5]), String(data[6]), String(data[7]));
  Serial.println("__________________________Device data collection DONE__________________________");
  disconnectGPRS();
  delay(2000);
  powerGSM(0);
  delay(30000);
  
  Serial.println("__________________________STARTING meta data collection__________________________");
  int* metadata = collectMetaData();
  powerGSM(1);
  delay(1000);
  checkNetwork();
  delay(3000);
  connectGPRS();
  delay(3000);
  postMetaData(String(metadata[0]), String(metadata[1]), String(metadata[2]), String(metadata[3]));
  Serial.println("__________________________Meta data collection DONE__________________________");
  disconnectGPRS();
  delay(2000);
  powerGSM(0);
  delay(30000); 

  if (fileDownloadState){
    Serial.println("__________________________STARTING F.O.T.A. update__________________________");
    powerGSM(1);
    delay(1000);
    checkNetwork();
    delay(3000);
    connectGPRS();
    delay(3000);
    String resource = "/firmware/" + loadDataFromEEPROM("DEVICE_FIRMWARE_VERSION") + "/download/firmwarebin";
    Serial.println("Resource: " + resource);
    firmwareUpdate(FIRMWARE_NAME, resource);
    Serial.println("__________________________F.O.T.A. update DONE__________________________");
    disconnectGPRS();
    delay(2000);
    powerGSM(0);
    delay(30000);
    }
}