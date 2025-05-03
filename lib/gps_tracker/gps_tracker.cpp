#include <gps_tracker.h>

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>
#include <gsm_communication.h>


// gps related pins
// #define GPS_POWER_SWITCH_PIN    A11
// #define RX2_PIN                 17
// #define TX2_PIN                 16

void powerGPS(bool state){
    pinMode(GPS_POWER_SWITCH_PIN, OUTPUT);
    digitalWrite(GPS_POWER_SWITCH_PIN, state);
    // Serial.println(F("GPS power state: "));+ String(state ? "ON" : "OFF"));
    delay(1000);
}

int gpsHealthCheck() {
    unsigned long startTime = millis();
    const unsigned long timeout = 5000; // 5-second timeout
    
    // Clear any existing GPS data
    while (Serial2.available())
        Serial2.read();

    // Send the power on command if using a cellular+GPS module
    Serial2.println("AT+CGNSPWR=1");
    delay(100);
    
    // Try to get valid GPS data
    bool validData = false;
    Serial.println(F("Checking GPS health..."));
    
    while (millis() - startTime < timeout) {
        while (Serial2.available() > 0) {
            char c = Serial2.read();
            if (gps.encode(c)) {
                // Check if we have valid data
                if (gps.location.isValid() || 
                    gps.date.isValid() || 
                    gps.time.isValid() ||
                    gps.satellites.isValid()) {
                    validData = true;
                    // Serial.println("GPS module is responding with valid data");
                    return 0; // GPS is healthy
                }
            }
        }
        
        // Print a dot every second to show progress
        static unsigned long lastDotTime = 0;
        if (millis() - lastDotTime > 1000) {
            // Serial.print(".");
            lastDotTime = millis();
        }
    }
    
    // Serial.println("\nNo valid GPS data received");
    return 16; // GPS module is not responding with valid data
}

GPSData getGPSData(unsigned long timeout) {
    GPSData data;
    
    // Initialize all values and validity flags
    data.latitude = 0.0;
    data.longitude = 0.0;
    data.altitude = 0.0;
    data.speed = 0.0;
    data.course = 0.0;
    data.satellites = 0;
    data.hdop = 0;
    data.locationValid = false;
    data.altitudeValid = false;
    data.speedValid = false;
    data.courseValid = false;
    data.satellitesValid = false;
    data.hdopValid = false;
    data.date = "";
    data.time = "";
    data.dateValid = false;
    data.timeValid = false;
    
    // Power on GPS if it's not already
    powerGPS(true);
    delay(1000);
    
    // Initialize Serial2 for GPS communication
    Serial2.begin(115200);
    delay(1000);
    
    // Turn on GNSS power
    Serial2.println("AT+CGNSPWR=1");
    delay(1000);
    
    // Clear any pending serial data
    while (Serial2.available()) {
        Serial2.read();
    }
    
    // Start time for timeout
    unsigned long startTime = millis();
    
    Serial.println(F("Waiting for GPS data..."));
    
    // Read GPS data until timeout
    while (millis() - startTime < timeout) {
        // Process data from GPS
        while (Serial2.available() > 0) {
            char c = Serial2.read();
            // Feed the character into the GPS parser
            if (gps.encode(c)) {
                // If new location data is available
                if (gps.location.isUpdated()) {
                    data.latitude = gps.location.lat();
                    data.longitude = gps.location.lng();
                    data.locationValid = gps.location.isValid();
                    
                    data.altitude = gps.altitude.meters();
                    data.altitudeValid = gps.altitude.isValid();
                    
                    data.speed = gps.speed.kmph();
                    data.speedValid = gps.speed.isValid();
                    
                    data.course = gps.course.deg();
                    data.courseValid = gps.course.isValid();
                    
                    data.satellites = gps.satellites.value();
                    data.satellitesValid = gps.satellites.isValid();
                    
                    data.hdop = gps.hdop.value();
                    data.hdopValid = gps.hdop.isValid();
                    
                    if (gps.date.isValid()) {
                        char dateStr[11];
                        sprintf(dateStr, "%02d/%02d/%04d", gps.date.day(), gps.date.month(), gps.date.year());
                        data.date = String(dateStr);
                        data.dateValid = true;
                    }
                    
                    if (gps.time.isValid()) {
                        char timeStr[9];
                        sprintf(timeStr, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
                        data.time = String(timeStr);
                        data.timeValid = true;
                    }
                    
                    // If we have a valid location, we can stop looking
                    if (data.locationValid) {
                        Serial.println(F("Valid GPS location obtained"));
                        break;
                    }
                }
            }
        }
        
        // If we have valid location data, break out of the waiting loop
        if (data.locationValid) {
            break;
        }
        
        // Print a dot every second to show we're still waiting
        static unsigned long lastDotTime = 0;
        if (millis() - lastDotTime > 1000) {
            Serial.print(".");
            lastDotTime = millis();
        }
    }
    
    Serial.println();  // End the dots line
    
    // Print the results
    if (data.locationValid) {
        Serial.print(F("GPS Location: "));
        Serial.print(data.latitude, 6);
        Serial.print(F(", "));
        Serial.println(data.longitude, 6);
    } else {
        Serial.println(F("Could not get valid GPS location data"));
    }
    
    if (data.altitudeValid) {
        Serial.print(F("Altitude: "));
        Serial.println(data.altitude);
    }
    
    if (data.speedValid) {
        Serial.print(F("Speed (km/h): "));
        Serial.println(data.speed);
    }
    
    if (data.courseValid) {
        Serial.print(F("Course (degrees): "));
        Serial.println(data.course);
    }
    
    if (data.satellitesValid) {
        Serial.print(F("Satellites: "));
        Serial.println(data.satellites);
    }
    
    if (data.hdopValid) {
        Serial.print(F("HDOP: "));
        Serial.println(data.hdop);
    }
    
    if (data.dateValid && data.timeValid) {
        Serial.print(F("Date/Time: "));
        Serial.print(data.date);
        Serial.print(F(" "));
        Serial.println(data.time);
    }
    
    // Optionally power off GPS to save energy
    // powerGPS(false);
    
    return data;
}