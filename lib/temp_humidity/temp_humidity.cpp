#include <temp_humidity.h>

#include <DHT.h>
#include <Wire.h>
#include <SHT2x.h>
#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <pin_definition.h>
#include <globalVariables.h>

void powerTHSensor(bool state){
    pinMode(DHT_SHT_POWER_SWITCH_PIN, OUTPUT);
    digitalWrite(DHT_SHT_POWER_SWITCH_PIN, state);
    // Serial.println("Temperature and Humidity Sensor power state: " + String(state ? "ON" : "OFF"));
    delay(1000);
}

int thSensorHealthCheck(String sensor){
    if (sensor == "DHT"){
        DHT dht(DHT_DATA_PIN, DHT_TYPE);
        dht.begin();
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        if (isnan(h) || isnan(t)) {
            // Serial.println("Failed to read from DHT sensor!");
            return 64; // Error reading DHT sensor
        } else {
            // Serial.print("Humidity: ");
            // Serial.print(h);
            // Serial.print("%  Temperature: ");
            // Serial.print(t);
            // Serial.println("°C");
            return 0; // Success
        }
    } else if (sensor == "SHT"){
        Wire.begin();
        // sht25;
        if (!sht25.begin()) {
            // Serial.println("Failed to initialize SHT sensor!");
            return 32; // Error initializing SHT sensor
        } else {
            float h = sht25.getHumidity();
            float t = sht25.getTemperature();
            if (isnan(h) || isnan(t)) {
                // Serial.println(F("Failed to read from SHT sensor!"));
                return 32; // Error reading SHT sensor
            } else {
                // Serial.print("Humidity: ");
                // Serial.print(h);
                // Serial.print("%  Temperature: ");
                // Serial.print(t);
                // Serial.println("°C");
                return 0; // Success
            }
        }
    } else {
        // Serial.println("Invalid sensor type.");
        return 32; // Invalid sensor type
    }
}

THData readTempHumiditySensors() {
    THData result;
    result.dht_success = false;
    result.sht_success = false;
    
    // Power on the sensors
    // // powerTHSensor(true);
    // delay(2000); // Give sensors time to stabilize
    
    // Read DHT sensor
    DHT dht(DHT_DATA_PIN, DHT_TYPE);
    dht.begin();
    result.dht_humidity = dht.readHumidity();
    result.dht_temperature = dht.readTemperature();
    
    if (!isnan(result.dht_humidity) && !isnan(result.dht_temperature)) {
        result.dht_success = true;
        Serial.print(F("DHT Temperature: "));
        Serial.print(result.dht_temperature);
        Serial.print(F("°C, DHT Humidity: "));
        Serial.print(result.dht_humidity);
        Serial.println(F("%"));
    } else {
        Serial.println(F("DHT error"));
        result.dht_temperature = 0.0;
        result.dht_humidity = 0.0;
    }
    
    // Read SHT sensor
    Wire.begin();
    SHT2x sht25;
    if (sht25.begin()) {
        result.sht_humidity = sht25.getHumidity();
        result.sht_temperature = sht25.getTemperature();
        
        if (!isnan(result.sht_humidity) && !isnan(result.sht_temperature)) {
            result.sht_success = true;
            // Serial.println(F("SHT Reading successful:"));
            Serial.print(F("SHT Temperature: "));
            Serial.print(result.sht_temperature);
            Serial.print(F("°C, SHT Humidity: "));
            Serial.print(result.sht_humidity);
            Serial.println(F("%"));
        } else {
            Serial.println(F("SHT error!"));
            result.sht_temperature = 0.0;
            result.sht_humidity = 0.0;
        }
    } else {
        Serial.println(F("initialize SHT error!"));
        result.sht_temperature = 0.0;
        result.sht_humidity = 0.0;
    }

    
    return result;
}