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

void checkEEPROMConfigData();
void updateEEPROMConfigData();

void checkEEPROMConfigData(){
    if (isEEPROMEmpty()) {
        // get CCID
        powerGSM(1);
        delay(1000);
        checkNetwork();
        delay(1000);
        String imei = getIMEI();
        String imsi = getIMSI();
        String ccid = getCCID();
        String signalQuality = getSignalQuality();
        // String ccid = getCCID();
        Serial.print(F("CCID: "));
        Serial.println(ccid);
        
        // get configuration data
        connectGPRS();
        delay(100);
        getEEPROMData(ccid);
        disconnectGPRS();
        powerGSM(0);
        delay(2000);

        // check if data is null
        if (responseData == "null") {
            Serial.println(F("Failed to get EEPROM data."));
            return;
        }
        // Serial.println("EEPROM Data: " + responseData);
        
        // configure device
        deviceSelfConfig(responseData);
        String all = loadDataFromEEPROM("ALL");
        Serial.println(all);
        // return;
    } else {
        updateEEPROMConfigData();
    }
}

void updateEEPROMConfigData() {
    Serial.println(F("Checking for configuration updates from server..."));
    
    // Store current configuration values for comparison
    String currentDeviceName = loadDataFromEEPROM("DEVICE_NAME");
    String currentWriteApiKey = loadDataFromEEPROM("DEVICE_WRITE_API_KEY");
    String currentReadApiKey = loadDataFromEEPROM("DEVICE_READ_API_KEY");
    String currentChannelId = loadDataFromEEPROM("DEVICE_CHANEL_ID");
    String currentSampleInterval = loadDataFromEEPROM("DEVICE_SAMPLE_INTERVAL_MINUTES");
    String currentUploadInterval = loadDataFromEEPROM("DEVICE_UPLOAD_INTERVAL_MINUTES");
    String currentDeploymentMode = loadDataFromEEPROM("DEPLOYMENT_MODE");
    String currentBatteryMonitoring = loadDataFromEEPROM("BATTERY_MONITORING");
    String currentDebugEnable = loadDataFromEEPROM("DEBUG_ENABLE");
    String currentPMSampleEntries = loadDataFromEEPROM("PM_SAMPLE_ENTRIES");
    String currentSPV = loadDataFromEEPROM("SPV");
    String currentSDCardPin = loadDataFromEEPROM("SD_CARD_PIN");
    String currentTransmissionMode = loadDataFromEEPROM("TRANSMISSION_MODE");
    String currentFirmwareVersion = loadDataFromEEPROM("DEVICE_FIRMWARE_VERSION");
    String currentFirmwareCRC32 = loadDataFromEEPROM("DEVICE_FIRMWARE_CRC32");
    
    Serial.println(F("Current configuration loaded from EEPROM"));
    
    // Power on GSM and establish connection
    powerGSM(1);
    delay(1000);
    
    // bool networkReady = checkNetwork();
    // if (!networkReady) {
    //     Serial.println("Network connection failed. Cannot update configuration.");
    //     powerGSM(0);
    //     return;
    // }
    checkNetwork();
    delay(3000);

    // Connect to GPRS to fetch data from server
    // bool connected = connectGPRS();
    // if (!connected) {
    //     Serial.println("GPRS connection failed. Cannot update configuration.");
    //     powerGSM(0);
    //     return;
    // }
    connectGPRS();
    delay(5000);
    
    // Get configuration data from server using the device channel ID
    responseData = "null"; // Reset response data
    getConfigData(currentChannelId);
    
    // Disconnect from GPRS and power off GSM
    disconnectGPRS();
    powerGSM(0);
    
    // Check if data was received successfully
    if (responseData == "null") {
        Serial.println(F("Failed to get configuration data from server."));
        return;
    }
    
    // Parse the JSON response
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, responseData);
    
    if (error) {
        Serial.print(F("JSON parsing failed: "));
        Serial.println(error.c_str());
        return;
    }
    
    // Check each configuration value and update if different
    bool configUpdated = false;
    
    // Compare device ID (may be a number in the JSON)
    if (doc.containsKey("deviceID")) {
        String newDeviceID = String(doc["deviceID"].as<int>());
        if (newDeviceID != currentChannelId) {
            strlcpy(eeprom_configuration_struct.DEVICE_CHANEL_ID, newDeviceID.c_str(), sizeof(eeprom_configuration_struct.DEVICE_CHANEL_ID));
            configUpdated = true;
            // Serial.println(F("Device ID updated"));
        }
    }
    
    // Handle the optional firmware-related fields - only update if provided
    if (doc.containsKey("firmwareVersion")) {
        String newFirmwareVersion = doc["firmwareVersion"].as<String>();
        if (newFirmwareVersion != currentFirmwareVersion) {
            strlcpy(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION, newFirmwareVersion.c_str(), sizeof(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION));
            configUpdated = true;
            // Serial.println(F("Firmware version updated"));
        }
    }
    
    if (doc.containsKey("firmwareCRC32")) {
        String newFirmwareCRC32 = doc["firmwareCRC32"].as<String>();
        if (newFirmwareCRC32 != currentFirmwareCRC32) {
            strlcpy(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32, newFirmwareCRC32.c_str(), sizeof(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32));
            configUpdated = true;
            // Serial.println(F("Firmware CRC32 updated"));
        }
    }
    
    if (doc.containsKey("fileDownloadState")) {
        fileDownloadState = doc["fileDownloadState"].as<bool>();
        // Serial.print(F("File download state updated to: "));
        // Serial.println(fileDownloadState ? "true" : "false");
    }
    
    // Handle configs object if it exists
    if (doc.containsKey("configs")) {
        JsonObject configs = doc["configs"];
        
        // Compare sample interval
        if (configs.containsKey("Sample Interval") && 
            configs["Sample Interval"].as<String>().toInt() != currentSampleInterval.toInt()) {
            eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES = configs["Sample Interval"].as<String>().toInt();
            configUpdated = true;
            // Serial.println(F("Sample interval updated"));
        }
        
        // Compare upload interval
        if (configs.containsKey("Upload Interval") && 
            configs["Upload Interval"].as<String>().toInt() != currentUploadInterval.toInt()) {
            eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES = configs["Upload Interval"].as<String>().toInt();
            configUpdated = true;
            // Serial.println(F("Upload interval updated"));
        }
        
        // Compare deployment mode
        if (configs.containsKey("Deployment Mode") && 
            configs["Deployment Mode"].as<String>().toInt() != currentDeploymentMode.toInt()) {
            eeprom_configuration_struct.DEPLOYMENT_MODE = configs["Deployment Mode"].as<String>().toInt();
            configUpdated = true;
            // Serial.println(F("Deployment mode updated"));
        }
        
        // Compare battery monitoring
        if (configs.containsKey("Chip Based battery monitoring") && 
            configs["Chip Based battery monitoring"].as<String>().toInt() != currentBatteryMonitoring.toInt()) {
            eeprom_configuration_struct.BATTERY_MONITORING = configs["Chip Based battery monitoring"].as<String>().toInt();
            configUpdated = true;
            // Serial.println(F("Battery monitoring updated"));
        }
        
        // Compare debug enable
        if (configs.containsKey("Debug Enable") && 
            configs["Debug Enable"].as<String>().toInt() != currentDebugEnable.toInt()) {
            eeprom_configuration_struct.DEBUG_ENABLE = configs["Debug Enable"].as<String>().toInt();
            configUpdated = true;
            // Serial.println(F("Debug enable updated"));
        }
        
        // Compare PM sample entries
        if (configs.containsKey("PM sample entries") && 
            configs["PM sample entries"].as<String>().toInt() != currentPMSampleEntries.toInt()) {
            eeprom_configuration_struct.PM_SAMPLE_ENTRIES = configs["PM sample entries"].as<String>().toInt();
            configUpdated = true;
            // Serial.println(F("PM sample entries updated"));
        }
        
        // Compare SPV
        if (configs.containsKey("SPV") && 
            configs["SPV"].as<String>().toInt() != currentSPV.toInt()) {
            eeprom_configuration_struct.SPV = configs["SPV"].as<String>().toInt();
            configUpdated = true;
            // Serial.println(F("SPV updated"));
        }
        
        // Compare SD card pin
        if (configs.containsKey("Sd card pin") && 
            configs["Sd card pin"].as<String>().toInt() != currentSDCardPin.toInt()) {
            eeprom_configuration_struct.SD_CARD_PIN = configs["Sd card pin"].as<String>().toInt();
            configUpdated = true;
            // Serial.println(F("SD card pin updated"));
        }
        
        // Compare transmission mode
        if (configs.containsKey("Transmission Mode") && 
            configs["Transmission Mode"].as<String>().toInt() != currentTransmissionMode.toInt()) {
            eeprom_configuration_struct.TRANSMISSION_MODE = configs["Transmission Mode"].as<String>().toInt();
            configUpdated = true;
            // Serial.println(F("Transmission mode updated"));
        }
    }
    
    // Save to EEPROM if any values were updated
    if (configUpdated) {
        saveConfigurationToEEPROM();
        Serial.println(F("Configuration updated and saved to EEPROM"));
    } else {
        Serial.println(F("No configuration changes detected"));
    }
}