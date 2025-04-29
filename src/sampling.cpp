// #define TINY_GSM_MODEM_SIM800

#include <EEPROM.h>
#include <Arduino.h>
#include <sd_card.h>
#include <pm_sensor.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <temp_humidity.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>
#include <gsm_communication.h>
#include <gps_tracker.h>

int statusValue(){
    int status = 0;
    powerGSM(1);
    delay(10);
    powerSD(1);
    delay(10);
    powerTHSensor(1);
    delay(10);
    powerPMS(1);
    delay(10);
    powerGPS(1);
    delay(10);
    status = status + gsmHealthCheck() + sdHealthCheck() + pmSensorHealthCheck("PMS_X3") + pmSensorHealthCheck("PMS_SX") + gpsHealthCheck() + thSensorHealthCheck("DHT") + thSensorHealthCheck("SHT");

    return status;
}

