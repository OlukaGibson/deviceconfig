// #define TINY_GSM_MODEM_SIM800

#include <EEPROM.h>
#include <Arduino.h>
#include <sd_card.h>
#include <pm_sensor.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>
#include <gsm_communication.h>


void checkConfigData(){
    if (isEEPROMEmpty()) {
        Serial.println("EEPROM is empty. Please configure the device.");
        
        // get CCID
        powerGSM(1);
        delay(1000);
        checkNetwork();
        delay(1000);
        String ccid = getCCID();
        Serial.println("CCID: " + ccid);
        
        // get configuration data
        connectGPRS();
        delay(100);
        getEEPROMData(ccid);
        disconnectGPRS();
        powerGSM(0);
        delay(2000);

        // check if data is null
        if (responseData == "null") {
            Serial.println("Failed to get EEPROM data.");
            return;
        }
        // Serial.println("EEPROM Data: " + responseData);
        
        // configure device
        deviceSelfConfig(responseData);
        String all = loadDataFromEEPROM("ALL");
        Serial.println(all);
        // return;
    } else {
        Serial.println("EEPROM is not empty. Proceeding with configuration.");
    }
}