#include <Arduino.h>
#include <gsm_communication.h>
#include "configuration/configuration.h"
#include <pin_definition.h>

void setup() {
  Serial.begin(115200);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);

  Serial1.begin(115200);

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
  getConfigDetails(ccid);
}

void loop() {
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
}