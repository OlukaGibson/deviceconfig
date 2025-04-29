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
void checkConfigData();
void collectData();
int statusValue();

void setup(){
  Serial.begin(115200);
  delay(10);
  Serial.println("Starting setup...");
  Serial1.begin(115200);
  delay(10);
  
  // Use Serial3 for PMS Sensor 1 (hardware serial)
  Serial3.begin(9600); // SENSOR 1
  delay(10);
  
  // Use SoftwareSerial for PMS Sensor 2
  pmsSensor2.begin(9600);
  delay(10);
  
  Serial2.begin(9600); // For GPS
}

void loop(){
  // // checkConfigData();
  // // delay(1000);
  // Serial.println("Collecting data...");
  // collectData();
  // delay(1000);
  // Serial.println("Working instruments");
  // Serial.println("Status: " + String(statusValue()));
  // delay(3000);
}