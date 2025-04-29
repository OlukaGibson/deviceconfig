#include <SD.h>
#include <sd_card.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>
#include <gsm_communication.h>

// configuration methods
void checkConfigData();


void setup(){
  Serial.begin(115200);
  delay(10);
  Serial1.begin(115200);
  delay(10);

}

void loop(){
  checkConfigData();
  delay(1000);
}