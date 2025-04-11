#include <Arduino.h>
#include <EEPROM.h>

void checkEEPROM() {
    bool isBlank = true;
    for (int i = 0; i < EEPROM.length(); i++) {
        if (EEPROM.read(i) != 0xFF) { // Assuming blank EEPROM cells are 0xFF
            isBlank = false;
            break;
        }
    }

    if (isBlank) {
        Serial.println("EEPROM is blank.");
    } else {
        Serial.println("EEPROM contains data:");
        for (int i = 0; i < EEPROM.length(); i++) {
            Serial.print("Address ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(EEPROM.read(i), HEX);
        }
    }
}

