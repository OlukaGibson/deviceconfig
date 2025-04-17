#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <Arduino.h>
#include "globalVariables.h"
#include <gsm_communication.h>
#include <eeprom_config.h>

// variables for control
String created_at;
int8_t delta_t;
String deviceName = getConfigValue("DEVICE_NAME");
String apiKey = "L1FU10HZOQ745OCM"; //getConfigValue("DEVICE_WRITE_API_KEY");
String channelId = getConfigValue("DEVICE_CHANEL_ID");
extern float pms1_2_5;
extern float pms1_10;
extern float pms2_2_5;
extern float pms2_10;
float latitude;
float longitude;
float battery_voltage;
String extra_data;
float discharge_rate;
float soc;
float battery_voltage1;
float external_pressure;
float altitude;
float external_temperature;
float external_humidity;
float internal_temperature;
float internal_humidity;
uint16_t status = 0;
struct pms5003data pmdata;

// Gsm related variables
const char apn[] = "TM";
const char user[] = "";
const char pass[] = "";
const char serverurl1[] = "34.132.108.60";
const char serverurl2[] = "api.thingspeak.com";

TinyGsm modem(Serial1);
TinyGsmClient client(modem);
HttpClient http(client, serverurl1, 80);

// FOTA related variables
const char FILE_NAME[] = "/file.bin";
const unsigned long EXPECTED_SIZE = 128400;
const size_t chunkSize = 1024; // 1KB chunks
byte buff[chunkSize];
unsigned long downloaded = 0;
unsigned long lastProgressTime = millis();
unsigned long downloadStartTime = millis();
unsigned long lastDataTime = millis();
boolean receivingData = false;