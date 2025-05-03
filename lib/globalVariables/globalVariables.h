#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

// Define the modem type before including the TinyGSM library
// #define TINY_GSM_MODEM_SIM800

// Standard libraries first
#include <Arduino.h>
#include <SD.h>
#include <EEPROM.h>
#include <SPI.h>

// Third-party libraries
#include <SHT2x.h>
#include <TinyGsm.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

// Project-specific includes
#include <pin_definition.h>


// variables for control

// extern String deviceName;
// extern String deviceID; //uint32_t
// extern String apiKey;
// extern String deploymentMode;
// extern String batteryMonitoring;
// extern String debugEnable;
// extern String pmSampleEntries;
// extern String spv;
// extern String sdCardPin;
// extern String transmissionMode;
// extern String firmwareVersion;

// extern String firmwareCRC32;
// // extern int deviceID;

// sht sht25 obect and gps object
extern SHT2x sht25;
extern TinyGPSPlus gps;

extern String created_at;
extern int8_t delta_t;
extern bool fileDownloadState;
extern int fileState;
extern String responseData;
extern const uint32_t WRITTEN_SIGNATURE;
extern const int SIGNATURE_ADDRESS;

typedef struct
{
    char DEVICE_NAME[16];
    char DEVICE_WRITE_API_KEY[20];
    char DEVICE_READ_API_KEY[20];
    char DEVICE_CHANEL_ID[16];
    char DEVICE_FIRMWARE_VERSION[10];
    char DEVICE_FIRMWARE_CRC32[16];
    uint16_t DEVICE_SAMPLE_INTERVAL_MINUTES;
    uint16_t DEVICE_UPLOAD_INTERVAL_MINUTES;
    uint16_t DEPLOYMENT_MODE;
    uint16_t BATTERY_MONITORING;
    uint16_t DEBUG_ENABLE;
    uint16_t PM_SAMPLE_ENTRIES;
    uint16_t SPV;
    uint16_t SD_CARD_PIN;
    uint16_t TRANSMISSION_MODE;
    uint16_t SAMPLE_ENTRIES;
} EEPROM_CONFIGURATION_STRUCT;
extern EEPROM_CONFIGURATION_STRUCT eeprom_configuration_struct;


// extern float pms1_2_5;
// extern float pms1_10;
// extern float pms2_2_5;
// extern float pms2_10;
// extern float latitude;
// extern float longitude;
// extern float battery_voltage;
// extern String extra_data;
// extern float discharge_rate;
// extern float soc;
// extern float battery_voltage1;
// extern float external_pressure;
// extern float altitude;
// extern float external_temperature;
// extern float external_humidity;
// extern float internal_temperature;
// extern float internal_humidity;
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
// extern byte buff[];
extern unsigned long downloaded;
extern unsigned long lastProgressTime;
extern unsigned long downloadStartTime;
extern unsigned long lastDataTime;
extern boolean receivingData;

#endif