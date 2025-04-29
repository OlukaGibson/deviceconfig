// #define TINY_GSM_MODEM_SIM800

#include <EEPROM.h>
#include <Arduino.h>
#include <sd_card.h>
#include <pm_sensor.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <gps_tracker.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <temp_humidity.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>
#include <gsm_communication.h>

extern SoftwareSerial pmsSensor2;

// field1 --> pms sensor1 2.5
// field2 --> pms sensor1 10
// field3 --> pms sensor2 2.5
// field4 --> pms sensor2 10
// field5 --> latitude
// field6 --> longitude
// field7 --> battery voltage
// field8 --> dht temperature
// field9 --> dht humidity
// field10 --> sht temperature
// field11 --> sht humidity

float field1, field2, field3, field4, field5, field6, field7, field8, field9, field10, field11;
// String field1Str, field2Str, field3Str, field4Str, field5Str, field6Str, field7Str, field8Str, field9Str, field10Str, field11Str;
// String extra_data = "null";

int statusValue(){
    int status = 0;
    powerGSM(1);
    delay(10);
    powerSD(1);
    delay(10);
    powerTHSensor(1);
    delay(100);
    powerPMS(1);
    delay(10000);
    powerGPS(1);
    delay(10);
    status = status /*+ gsmHealthCheck() + sdHealthCheck() + pmSensorHealthCheck("PMS_X3") + pmSensorHealthCheck("PMS_SX") + gpsHealthCheck()*/ + thSensorHealthCheck("DHT") + thSensorHealthCheck("SHT")/**/;

    powerGSM(0);
    delay(10);
    powerSD(0);
    delay(10);
    powerTHSensor(0);
    delay(100);
    powerPMS(0);
    delay(10000);
    powerGPS(0);
    delay(10);

    return status;
}

void collectData() {
    Serial.println("Starting data collection...");
    
    // Define variables for all fields
    float field1 = 0, field2 = 0, field3 = 0, field4 = 0;
    float field5 = 0, field6 = 0, field7 = 0, field8 = 0, field9 = 0, field10 = 0, field11 = 0;
    String extra_data = "null";
  
    // Get PM sample entries from EEPROM
    int pmSampleEntries;// = loadDataFromEEPROM("PM_SAMPLE_ENTRIES").toInt();
    // if (pmSampleEntries <= 0) {
      pmSampleEntries = 5; // Default value if EEPROM value is invalid
    // }
    Serial.print("PM Sample Entries: ");
    Serial.println(pmSampleEntries);
  
    // Variables for PM sensors averages
    float pms1_2_5_sum = 0, pms1_10_sum = 0; // Sensor 1 (PMS_RX3_PIN/PMS_TX3_PIN)
    float pms2_2_5_sum = 0, pms2_10_sum = 0; // Sensor 2 (PMS_SSRX_PIN/PMS_SSTX_PIN)
    int validSamples1 = 0, validSamples2 = 0;
  
    // Power on the PM sensors
    powerPMS(true);
    delay(30000); // Wait for PM sensors to stabilize (30 seconds)
  
    // Collect PM sensor data (multiple samples)
    Serial.println("Collecting PM sensor data...");
    for (int i = 0; i < pmSampleEntries; i++) {
      Serial.print("PM Sample #");
      Serial.print(i + 1);
      Serial.print("/");
      Serial.println(pmSampleEntries);
      
      // Read from PMS Sensor 1 (Using Hardware Serial3)
      if (readPMSdata(&Serial3)) {
        // Add values to sum for averaging
        pms1_2_5_sum += pmdata.pm25_standard;
        pms1_10_sum += pmdata.pm100_standard;
        validSamples1++;
        
        Serial.print("PM Sensor 1 - PM2.5: ");
        Serial.print(pmdata.pm25_standard);
        Serial.print(", PM10: ");
        Serial.println(pmdata.pm100_standard);
      } else {
        Serial.println("Failed to read from PM Sensor 1");
      }
      
      delay(500); // Short delay between sensor readings
      
      // Read from PMS Sensor 2 (Using SoftwareSerial)
      if (readPMSdata(&pmsSensor2)) {
        // Add values to sum for averaging
        pms2_2_5_sum += pmdata.pm25_standard;
        pms2_10_sum += pmdata.pm100_standard;
        validSamples2++;
        
        Serial.print("PM Sensor 2 - PM2.5: ");
        Serial.print(pmdata.pm25_standard);
        Serial.print(", PM10: ");
        Serial.println(pmdata.pm100_standard);
      } else {
        Serial.println("Failed to read from PM Sensor 2");
      }
      
      delay(1000); // Wait between samples
    }
  
    // Calculate PM averages
    if (validSamples1 > 0) {
      field1 = pms1_2_5_sum / validSamples1; // field1 = PM Sensor 1 PM2.5
      field2 = pms1_10_sum / validSamples1;  // field2 = PM Sensor 1 PM10
    }
    
    if (validSamples2 > 0) {
      field3 = pms2_2_5_sum / validSamples2; // field3 = PM Sensor 2 PM2.5
      field4 = pms2_10_sum / validSamples2;  // field4 = PM Sensor 2 PM10
    }
  
    // Power off PM sensors to save energy
    powerPMS(false);
    
    // Get GPS data
    Serial.println("Getting GPS data...");
    powerGPS(true);
    delay(5000);
    GPSData gpsData = getGPSData();
    field5 = gpsData.latitude;
    field6 = gpsData.longitude;
    powerGPS(false);
    
    // Get battery voltage
    Serial.println("Reading battery voltage...");
    field7 = battery_voltage; // You may need to implement battery voltage reading
    
    // Read temperature and humidity sensors
    Serial.println("Reading temperature and humidity sensors...");
    powerTHSensor(true);
    delay(2000); // Wait for sensors to stabilize
    THData thData = readTempHumiditySensors();
    field8 = thData.dht_temperature;
    field9 = thData.dht_humidity;
    field10 = thData.sht_temperature;
    field11 = thData.sht_humidity;
    powerTHSensor(false);
    
    // Save the collected data to global variables
    pms1_2_5 = field1;
    pms1_10 = field2;
    pms2_2_5 = field3;
    pms2_10 = field4;
    latitude = field5;
    longitude = field6;
    battery_voltage = field7;
    external_temperature = field8;
    external_humidity = field9;
    internal_temperature = field10;
    internal_humidity = field11;
    
    // Print summary of collected data
    Serial.println("\nData Collection Summary:");
    Serial.print("PM1 2.5 (avg): "); Serial.println(field1);
    Serial.print("PM1 10 (avg): "); Serial.println(field2);
    Serial.print("PM2 2.5 (avg): "); Serial.println(field3);
    Serial.print("PM2 10 (avg): "); Serial.println(field4);
    Serial.print("Latitude: "); Serial.println(field5, 6);
    Serial.print("Longitude: "); Serial.println(field6, 6);
    Serial.print("Battery Voltage: "); Serial.println(field7);
    Serial.print("DHT Temperature: "); Serial.println(field8);
    Serial.print("DHT Humidity: "); Serial.println(field9);
    Serial.print("SHT Temperature: "); Serial.println(field10);
    Serial.print("SHT Humidity: "); Serial.println(field11);
    
    Serial.println("Data collection completed");
  }