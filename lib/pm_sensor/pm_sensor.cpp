#include <Arduino.h>
#include <Wire.h>
#include <globalVariables.h>
#include <pm_sensor.h>
#include <pin_definition.h>

void pmSensorTurnOn() {
    pinMode(PMS_POWER_SWITCH_PIN, OUTPUT);
    digitalWrite(PMS_POWER_SWITCH_PIN, HIGH);
    delay(1000); // Wait for the sensor to power up
    // Serial.println("PM Sensor powered on.");
}

void pmSensorTurnOff() {
    pinMode(PMS_POWER_SWITCH_PIN, OUTPUT);
    digitalWrite(PMS_POWER_SWITCH_PIN, LOW);
    delay(1000); // Wait for the sensor to power down
    // Serial.println("PM Sensor powered off.");
}

bool readPMSdata(Stream *s){
    if (! s->available()){
      return false ;
    }
    
    // Read a byte at a time until we get to the special '0x42' start-byte
    if (s->peek() != 0x42) {
      s->read();
      return false;
    }
  
    // Now read all 32 bytes
    if (s->available() < 32) {
      return false;
    }
  
    uint8_t buffer[32];
    uint16_t sum = 0;
    s->readBytes(buffer, 32);
  
    // get checksum ready
    for (uint8_t i = 0; i < 30; i++) {
      sum += buffer[i];
    }
  
    // The data comes in endian'd, this solves it so it works on all platforms
    uint16_t buffer_u16[15];
    for (uint8_t i = 0; i < 15; i++) {
      buffer_u16[i] = buffer[2 + i * 2 + 1];
      buffer_u16[i] += (buffer[2 + i * 2] << 8);
    }
  
    // put it into a nice struct :)
    memcpy((void *)&pmdata, (void *)buffer_u16, 30);
  
    if (sum != pmdata.checksum) {
      Serial.println("Checksum failure");
      return false;
    }
  
    // success!
    return true;
  
  }

  int8_t pmSensorHealthCheck() {
    if 
}