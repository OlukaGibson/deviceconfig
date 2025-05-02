// #define TINY_GSM_MODEM_SIM800
#include "globalVariables.h"

#include <SD.h>
#include <SHT2x.h>
#include <Arduino.h>
#include <TinyGsm.h>
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <ArduinoHttpClient.h>
#include <gsm_communication.h>

// variables for control

// // String deviceName = getConfigValue("DEVICE_NAME");
// String apiKey = "L1FU10HZOQ745OCM"; //getConfigValue("DEVICE_WRITE_API_KEY");
// String deviceID = "null";//getConfigValue("DEVICE_CHANEL_ID");
// String deploymentMode = "null";
// String batteryMonitoring = "null";
// String debugEnable = "null";
// String pmSampleEntries = "null";
// String spv = "null";
// String sdCardPin = "null";
// String transmissionMode = "null";
// String firmwareVersion = "42.74";

// String firmwareCRC32 = "103848AF";
// // int deviceID = 0;

// sht sht25 obect and gps object
SHT2x sht25;
TinyGPSPlus gps;

String created_at;
int8_t delta_t;
bool fileDownloadState = false;
int fileState = 0;
String responseData = "null";
const uint32_t WRITTEN_SIGNATURE = 0xBEEFDEED;
const int SIGNATURE_ADDRESS = 0;
EEPROM_CONFIGURATION_STRUCT eeprom_configuration_struct;


//data variables
// float pms1_2_5;
// float pms1_10;
// float pms2_2_5;
// float pms2_10;
// float latitude;
// float longitude;
// float battery_voltage;
// String extra_data;
// float discharge_rate;
// float soc;
// float battery_voltage1;
// float external_pressure;
// float altitude;
// float external_temperature;
// float external_humidity;
// float internal_temperature;
// float internal_humidity;
uint16_t status = 0;
struct pms5003data pmdata;

// Gsm related variables
const char apn[] = "TM";
const char user[] = "";
const char pass[] = "";
const char serverurl1[] = "34.135.156.106";//"34.132.108.60";
const char serverurl2[] = "api.thingspeak.com";

TinyGsm modem(Serial1);
TinyGsmClient client(modem);
HttpClient http(client, serverurl1, 80);

// FOTA related variables
const char FIRMWARE_NAME[] = "/firmware.bin";
const unsigned long EXPECTED_SIZE = 128406;
const size_t chunkSize = 1024; // 1KB chunks
byte buff[chunkSize];
unsigned long downloaded = 0;
unsigned long lastProgressTime = millis();
unsigned long downloadStartTime = millis();
unsigned long lastDataTime = millis();
boolean receivingData = false;

//configuration variables