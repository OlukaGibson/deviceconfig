#ifndef TEMP_HUMIDITY_H
#define TEMP_HUMIDITY_H

#include <DHT.h>
#include <Wire.h>
#include <SHT2x.h>
#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <pin_definition.h>
#include <globalVariables.h>

struct THData {
  float dht_temperature;
  float dht_humidity;
  float sht_temperature;
  float sht_humidity;
  bool dht_success;
  bool sht_success;
};

void powerTHSensor(bool state);
int thSensorHealthCheck(String sensor);
THData readTempHumiditySensors();

#endif