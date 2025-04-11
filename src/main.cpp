#include <Arduino.h>
#include <gsm_communication.h>
#include "configuration/configuration.h"
#include <pin_definition.h>
#include <sd_card.h>

void setup() {
  Serial.begin(115200);
  pinMode(GSM_POWER_SWITCH_PIN, OUTPUT);
  digitalWrite(GSM_POWER_SWITCH_PIN, HIGH);

  Serial1.begin(115200);

  // Test the SD card first
  testSDCard();
  delay(1000);

  checkNetwork();
  delay(3000);
  String imei = getIMEI();
  String imsi = getIMSI();
  String ccid = getCCID();
  String signalQuality = getSignalQuality();

  Serial.println("CCID: " + ccid);
  Serial.println("IMEI: " + imei);
  Serial.println("IMSI: " + imsi);
  Serial.println("Signal Quality: " + signalQuality);

  connectGPRS();
  
  // Now try downloading the firmware
  // if (downloadFirmware("hi")) {
  //   Serial.println("Firmware downloaded successfully");
  // } else {
  //   Serial.println("Firmware download failed");
  // }
  
  getConfigDetails(ccid);
}

void loop() {
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
}