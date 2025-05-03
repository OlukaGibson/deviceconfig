// #define TINY_GSM_MODEM_SIM800

#include <EEPROM.h>
#include <Arduino.h>
#include <sd_card.h>
#include <pm_sensor.h>
#include <ArduinoJson.h>
#include <gps_tracker.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <temp_humidity.h>
#include <SoftwareSerial.h>
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

// float field1, field2, field3, field4, field5, field6, field7, field8, field9, field10, field11;
// String field1Str, field2Str, field3Str, field4Str, field5Str, field6Str, field7Str, field8Str, field9Str, field10Str, field11Str;
// String extra_data = "null";

int statusValue() {
  int possibleValues[] = {0, 1, 2, 4, 8, 16, 32, 64, 128}; 
  int randomIndex = random(0, 9);
  return possibleValues[randomIndex];
}

void collectDeviceData(float* dataBuffer) {
  // Local variables for calculations - not stored globally
  int pmSampleEntries = 5;
  Serial.print(F("PM Sample Entries: "));
  Serial.println(pmSampleEntries);

  // Simulate PM sensor data collection with random values
  // Generate base values for PM2.5 between 10-45
  float pm1_2_5_base = random(1000, 4500) / 100.0;
  
  // Generate PM2.5 for second sensor within 7 units of first sensor
  float pm2_2_5_base = pm1_2_5_base + random(-700, 700) / 100.0;
  pm2_2_5_base = constrain(pm2_2_5_base, 10.0, 45.0);
  
  // PM10 is typically higher than PM2.5
  float pm1_10_base = pm1_2_5_base + random(300, 1500) / 100.0;
  pm1_10_base = constrain(pm1_10_base, pm1_2_5_base, 45.0);
  
  // PM10 for second sensor within 7 units of first sensor
  float pm2_10_base = pm1_10_base + random(-700, 700) / 100.0;
  pm2_10_base = constrain(pm2_10_base, 10.0, 45.0);
  
  float pms1_2_5_sum = 0, pms1_10_sum = 0;
  float pms2_2_5_sum = 0, pms2_10_sum = 0;
  int validSamples1 = 0, validSamples2 = 0;
  
  Serial.println(F("Collecting PM sensor data (simulated)..."));
  for (int i = 0; i < pmSampleEntries; i++) {
    Serial.print(F("PM Sample #"));
    Serial.print(i + 1);
    Serial.print(F("/"));
    Serial.println(pmSampleEntries);
    
    // Add small variation to each reading
    float variation1 = random(-50, 50) / 100.0; // ±0.50
    float variation2 = random(-50, 50) / 100.0; // ±0.50
    
    // Simulate readings for Sensor 1
    float pm1_2_5_reading = pm1_2_5_base + variation1;
    float pm1_10_reading = pm1_10_base + variation1;
    
    pms1_2_5_sum += pm1_2_5_reading;
    pms1_10_sum += pm1_10_reading;
    validSamples1++;
    
    Serial.print(F("PM Sensor 1 - PM2.5: "));
    Serial.print(pm1_2_5_reading);
    Serial.print(F(", PM10: "));
    Serial.println(pm1_10_reading);
    
    // Simulate readings for Sensor 2
    float pm2_2_5_reading = pm2_2_5_base + variation2;
    float pm2_10_reading = pm2_10_base + variation2;
    
    pms2_2_5_sum += pm2_2_5_reading;
    pms2_10_sum += pm2_10_reading;
    validSamples2++;
    
    Serial.print(F("PM Sensor 2 - PM2.5: "));
    Serial.print(pm2_2_5_reading);
    Serial.print(F(", PM10: "));
    Serial.println(pm2_10_reading);
    
    delay(500); // Simulated delay between samples
  }

  // Calculate PM averages
  dataBuffer[0] = pms1_2_5_sum / validSamples1; // PM Sensor 1 PM2.5
  dataBuffer[1] = pms1_10_sum / validSamples1;  // PM Sensor 1 PM10
  dataBuffer[2] = pms2_2_5_sum / validSamples2; // PM Sensor 2 PM2.5
  dataBuffer[3] = pms2_10_sum / validSamples2;  // PM Sensor 2 PM10
  
  // Simulate GPS data near 0.332082, 32.570481
  // Add very small variation to GPS coordinates
  dataBuffer[4] = 0.332082 + random(-1000, 1000) / 1000000.0; // Latitude with tiny variation
  dataBuffer[5] = 32.570481 + random(-1000, 1000) / 1000000.0; // Longitude with tiny variation
  
  // Battery voltage between 4.15 and 4.20
  dataBuffer[6] = 4.15 + random(0, 50) / 1000.0;
  
  // Temperature and humidity sensors (typical room values)
  // DHT sensor (typically external)
  dataBuffer[7] = 22.0 + random(-300, 300) / 100.0;  // Temperature 19-25°C
  dataBuffer[8] = 50.0 + random(-1500, 1500) / 100.0; // Humidity 35-65%
  
  // SHT sensor (typically internal)
  dataBuffer[9] = 23.0 + random(-200, 200) / 100.0; // Temperature 21-25°C
  dataBuffer[10] = 45.0 + random(-1000, 1000) / 100.0; // Humidity 35-55%
  
  // Print summary of collected data
  Serial.println("\nData Collection Summary:");
  Serial.print(F("PM1 2.5 (avg): ")); Serial.println(dataBuffer[0]);
  Serial.print(F("PM1 10 (avg): ")); Serial.println(dataBuffer[1]);
  Serial.print(F("PM2 2.5 (avg): ")); Serial.println(dataBuffer[2]);
  Serial.print(F("PM2 10 (avg): ")); Serial.println(dataBuffer[3]);
  Serial.print(F("Latitude: ")); Serial.println(dataBuffer[4], 6);
  Serial.print(F("Longitude: ")); Serial.println(dataBuffer[5], 6);
  Serial.print(F("Battery Voltage: ")); Serial.println(dataBuffer[6]);
  Serial.print(F("DHT Temperature: ")); Serial.println(dataBuffer[7]);
  Serial.print(F("DHT Humidity: ")); Serial.println(dataBuffer[8]);
  Serial.print(F("SHT Temperature: ")); Serial.println(dataBuffer[9]);
  Serial.print(F("SHT Humidity: ")); Serial.println(dataBuffer[10]);
  
  // Check differences between sensors to confirm they meet requirements
  Serial.print(F("PM2.5 difference: ")); Serial.println(abs(dataBuffer[0] - dataBuffer[2]));
  Serial.print(F("PM10 difference: ")); Serial.println(abs(dataBuffer[1] - dataBuffer[3]));
}

void collectMetaData(int* metaBuffer) {
  // Directly fill the provided buffer
  
  // Simulate metadata collection with random values
  metaBuffer[0] = statusValue();
  metaBuffer[1] = random(23, 26);
  metaBuffer[2] = random(0, 100);
  metaBuffer[3] = random(0, 2);

  // Print summary of collected metadata
  Serial.println(F("\nMetadata Collection Summary:"));
  Serial.print(F("Metadata 1: ")); Serial.println(metaBuffer[0]);
  Serial.print(F("Metadata 2: ")); Serial.println(metaBuffer[1]);
  Serial.print(F("Metadata 3: ")); Serial.println(metaBuffer[2]);
  Serial.print(F("Metadata 4: ")); Serial.println(metaBuffer[3]);
}

// int statusValue(){
//     int status = 0;
//     powerGSM(1);
//     delay(10);
//     powerSD(1);
//     delay(10);
//     powerTHSensor(1);
//     delay(100);
//     powerPMS(1);
//     delay(10000);
//     powerGPS(1);
//     delay(10);
//     status = status + gsmHealthCheck() + sdHealthCheck() + pmSensorHealthCheck("PMS_X3") + pmSensorHealthCheck("PMS_SX") + gpsHealthCheck() + thSensorHealthCheck("DHT") + thSensorHealthCheck("SHT")/**/;

//     powerGSM(0);
//     delay(10);
//     powerSD(0);
//     delay(10);
//     powerTHSensor(0);
//     delay(100);
//     powerPMS(0);
//     delay(10000);
//     powerGPS(0);
//     delay(10);

//     return status;
// }

// void collectData() {
//     Serial.println("Starting data collection...");
    
//     // Define variables for all fields
//     float field1 = 0, field2 = 0, field3 = 0, field4 = 0;
//     float field5 = 0, field6 = 0, field7 = 0, field8 = 0, field9 = 0, field10 = 0, field11 = 0;
//     String extra_data = "null";
  
//     // Get PM sample entries from EEPROM
//     int pmSampleEntries;// = loadDataFromEEPROM("PM_SAMPLE_ENTRIES").toInt();
//     // if (pmSampleEntries <= 0) {
//       pmSampleEntries = 5; // Default value if EEPROM value is invalid
//     // }
//     Serial.print("PM Sample Entries: ");
//     Serial.println(pmSampleEntries);
  
//     // Variables for PM sensors averages
//     float pms1_2_5_sum = 0, pms1_10_sum = 0; // Sensor 1 (PMS_RX3_PIN/PMS_TX3_PIN)
//     float pms2_2_5_sum = 0, pms2_10_sum = 0; // Sensor 2 (PMS_SSRX_PIN/PMS_SSTX_PIN)
//     int validSamples1 = 0, validSamples2 = 0;
  
//     // Power on the PM sensors
//     powerPMS(true);
//     delay(30000); // Wait for PM sensors to stabilize (30 seconds)
  
//     // Collect PM sensor data (multiple samples)
//     Serial.println("Collecting PM sensor data...");
//     for (int i = 0; i < pmSampleEntries; i++) {
//       Serial.print("PM Sample #");
//       Serial.print(i + 1);
//       Serial.print("/");
//       Serial.println(pmSampleEntries);
      
//       // Read from PMS Sensor 1 (Using Hardware Serial3)
//       if (readPMSdata(&Serial3)) {
//         // Add values to sum for averaging
//         pms1_2_5_sum += pmdata.pm25_standard;
//         pms1_10_sum += pmdata.pm100_standard;
//         validSamples1++;
        
//         Serial.print("PM Sensor 1 - PM2.5: ");
//         Serial.print(pmdata.pm25_standard);
//         Serial.print(", PM10: ");
//         Serial.println(pmdata.pm100_standard);
//       } else {
//         Serial.println("Failed to read from PM Sensor 1");
//       }
      
//       delay(500); // Short delay between sensor readings
      
//       // Read from PMS Sensor 2 (Using SoftwareSerial)
//       if (readPMSdata(&pmsSensor2)) {
//         // Add values to sum for averaging
//         pms2_2_5_sum += pmdata.pm25_standard;
//         pms2_10_sum += pmdata.pm100_standard;
//         validSamples2++;
        
//         Serial.print("PM Sensor 2 - PM2.5: ");
//         Serial.print(pmdata.pm25_standard);
//         Serial.print(", PM10: ");
//         Serial.println(pmdata.pm100_standard);
//       } else {
//         Serial.println("Failed to read from PM Sensor 2");
//       }
      
//       delay(1000); // Wait between samples
//     }
  
//     // Calculate PM averages
//     if (validSamples1 > 0) {
//       field1 = pms1_2_5_sum / validSamples1; // field1 = PM Sensor 1 PM2.5
//       field2 = pms1_10_sum / validSamples1;  // field2 = PM Sensor 1 PM10
//     }
    
//     if (validSamples2 > 0) {
//       field3 = pms2_2_5_sum / validSamples2; // field3 = PM Sensor 2 PM2.5
//       field4 = pms2_10_sum / validSamples2;  // field4 = PM Sensor 2 PM10
//     }
  
//     // Power off PM sensors to save energy
//     powerPMS(false);
    
//     // Get GPS data
//     Serial.println("Getting GPS data...");
//     powerGPS(true);
//     delay(5000);
//     GPSData gpsData = getGPSData();
//     field5 = gpsData.latitude;
//     field6 = gpsData.longitude;
//     powerGPS(false);
    
//     // Get battery voltage
//     Serial.println("Reading battery voltage...");
//     field7 = battery_voltage; // You may need to implement battery voltage reading
    
//     // Read temperature and humidity sensors
//     Serial.println("Reading temperature and humidity sensors...");
//     powerTHSensor(true);
//     delay(2000); // Wait for sensors to stabilize
//     THData thData = readTempHumiditySensors();
//     field8 = thData.dht_temperature;
//     field9 = thData.dht_humidity;
//     field10 = thData.sht_temperature;
//     field11 = thData.sht_humidity;
//     powerTHSensor(false);
    
//     // Save the collected data to global variables
//     pms1_2_5 = field1;
//     pms1_10 = field2;
//     pms2_2_5 = field3;
//     pms2_10 = field4;
//     latitude = field5;
//     longitude = field6;
//     battery_voltage = field7;
//     external_temperature = field8;
//     external_humidity = field9;
//     internal_temperature = field10;
//     internal_humidity = field11;
    
//     // Print summary of collected data
//     Serial.println("\nData Collection Summary:");
//     Serial.print("PM1 2.5 (avg): "); Serial.println(field1);
//     Serial.print("PM1 10 (avg): "); Serial.println(field2);
//     Serial.print("PM2 2.5 (avg): "); Serial.println(field3);
//     Serial.print("PM2 10 (avg): "); Serial.println(field4);
//     Serial.print("Latitude: "); Serial.println(field5, 6);
//     Serial.print("Longitude: "); Serial.println(field6, 6);
//     Serial.print("Battery Voltage: "); Serial.println(field7);
//     Serial.print("DHT Temperature: "); Serial.println(field8);
//     Serial.print("DHT Humidity: "); Serial.println(field9);
//     Serial.print("SHT Temperature: "); Serial.println(field10);
//     Serial.print("SHT Humidity: "); Serial.println(field11);
    
//     Serial.println("Data collection completed");
//   }