#include <Arduino.h>
#include <gsm_communication.h>
#include "configuration/configuration.h"
#include <pin_definition.h>
#include <sd_card.h>

const char resource[] = "/firmware/42.74/download/firwmwarebin";

void setup() {
  Serial.begin(115200);
  powerGSM(1);
  powerSD(1);
  delay(3000);

  Serial1.begin(115200);

  // Test the SD card first
  // testSDCard();
  // delay(1000);

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
  // Initialize SD card first, to ensure it's ready for firmware storage
  Serial.println("Initializing SD card...");
  if (sdHealthCheck() != 0) {
    Serial.println("SD card initialization failed! Cannot proceed with firmware update.");
    return;
  }
  // deviceSelfConfig(ccid);
  // getConfigData("1");
  // postMetaData("1", "2", "3", "4");
  // postDeviceData("1", "2", "3", "4", "5", "6", "7", "8");
  firmwareUpdate("/firmware.bin", resource);
}

void loop() {
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
}