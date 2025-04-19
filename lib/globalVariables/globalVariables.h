#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

// Define the modem type before including the TinyGSM library
#define TINY_GSM_MODEM_SIM800

#include <TinyGsm.h>
#include <ArduinoHttpClient.h>
#include <Arduino.h>
#include <pin_definition.h>


// variables for control
extern String created_at;
extern int8_t delta_t;
extern String deviceName;
extern String channelID; //uint32_t
extern String apiKey;
extern float pms1_2_5;
extern float pms1_10;
extern float pms2_2_5;
extern float pms2_10;
extern float latitude;
extern float longitude;
extern float battery_voltage;
extern String extra_data;
extern float discharge_rate;
extern float soc;
extern float battery_voltage1;
extern float external_pressure;
extern float altitude;
extern float external_temperature;
extern float external_humidity;
extern float internal_temperature;
extern float internal_humidity;
extern uint16_t status;
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
extern struct pms5003data pmdata;

// Gsm related variables
extern const char apn[];
extern const char user[];
extern const char pass[];
extern const char serverurl1[];
extern const char serverurl2[];
extern TinyGsm modem;
extern TinyGsmClient client;
extern HttpClient http;

// FOTA related variables
extern const char FIRMWARE_NAME[];
extern const unsigned long EXPECTED_SIZE;
extern const size_t chunkSize;
extern byte buff[];
extern unsigned long downloaded;
extern unsigned long lastProgressTime;
extern unsigned long downloadStartTime;
extern unsigned long lastDataTime;
extern boolean receivingData;

#endif