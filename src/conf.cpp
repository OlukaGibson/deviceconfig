#include <SD.h>
#include <sd_card.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>
#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <pin_definition.h>
#include <globalVariables.h>
#include <ArduinoHttpClient.h>
#include <gsm_communication.h>

// Global variables
String DevName;
long channelId;
char writeAPIKey[16];
int DeploymentMode;
int SDCS;
int spvalue;
int DebugEnable;
int TransMode;
int chipBasedBatteryOps;
int pmSamples;

// Function prototypes
void configureFromJSON(String jsonData);
void printCurrentConfiguration();
void jsonWriteString(char add, String data);
String read_String(char add);
String read_EE(char add);
void writeString(char add, String data);

// bool attemptGPRSConnection(int maxRetries = 3);
// void configureExample1();
// void configureExample2();
// void jsonWriteBuffer(char add, char* Buffer);
// void writeBuffer(char add, char* Buffer);
// void getEEPROMData(String ccid);

void setup(){
    Serial.begin(115200);
    delay(1000);

    Serial1.begin(115200);
    delay(1000);

    clearEEPROM();
}

void loop(){
    Serial.println(F("__________________________STARTING device__________________________"));
    Serial.println(F("Initializing GSM modem..."));
    powerGSM(1);
    delay(1000);
    checkNetwork();
    delay(1000);
    String imei = getIMEI();
    delay(10);
    String imsi = getIMSI();
    delay(10);
    String signalQuality = getSignalQuality();
    delay(10);
    String ccid = getCCID();
    delay(10);
    Serial.print(F("CCID: "));
    Serial.println(ccid);

    bool connected = connectGPRS();
    if (connected) {
        delay(100);
        getEEPROMData(ccid);
        disconnectGPRS();
    } else {
        Serial.println(F("Failed to connect to GPRS for configuration."));
        disconnectGPRS();
    }
    powerGSM(0);
    delay(2000);

    if (responseData != "null") {
        Serial.println(F("Configuration data received successfully."));
        Serial.println(responseData);
        configureFromJSON(responseData);
        printCurrentConfiguration();
        powerGSM(1);
        delay(1000);
        if (!SD.begin(SD_CS_PIN)) {
          Serial.println(F("SD card initialization failed!"));
          return;
        }else {
          Serial.println(F("SD card initialization successful."));
          firmwareFlash();
        }  
    } else {
        Serial.println(F("Failed to get EEPROM data."));
        return;
    }


}

void configureFromJSON(String jsonData) {
  Serial.println(F("##################### JSON DEVICE CONFIGURATION #######################"));
  
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, jsonData);
  
  if (error) {
    Serial.print(F("JSON parsing failed: "));
    Serial.println(error.c_str());
    return;
  }
  
  Serial.println(F("Configuring device from JSON data..."));
  
  // Clear EEPROM first
  clearEEPROM();
  
  // Extract and set device name using fixed write function
  if (doc.containsKey("name")) {
    String deviceName = doc["name"];
    // deviceName.trim();
    DevName = deviceName;
    jsonWriteString(1, DevName);  // Use the fixed function
    Serial.print(F("Device Name set to: "));
    Serial.println(DevName);
  }
  
  // Extract and set channel ID
  if (doc.containsKey("deviceID")) {
    channelId = doc["deviceID"];
    writeString(31, String(channelId));  // Numbers should work fine
    Serial.print(F("Channel ID set to: "));
    Serial.println(channelId);
  }
  
  // Extract and set write API key using fixed function
  if (doc.containsKey("writekey")) {
    String writeKey = doc["writekey"];
    // writeKey.trim();
    jsonWriteString(61, writeKey);  // Use the fixed function
    Serial.print(F("Write API Key set to: "));
    Serial.println(writeKey);
  }
  
  // Rest of the configurations (these are mostly numbers so should work)
  if (doc.containsKey("configs")) {
    JsonObject configs = doc["configs"];
    
    if (configs.containsKey("Deployment Mode")) {
      DeploymentMode = configs["Deployment Mode"].as<String>().toInt();
      writeString(100, String(DeploymentMode));
    }
    
    if (configs.containsKey("SD card pin")) {
      SDCS = configs["SD card pin"].as<String>().toInt();
      writeString(103, String(SDCS));
    }
    
    if (configs.containsKey("Sleep Percentile Value")) {
      spvalue = configs["Sleep Percentile Value"].as<String>().toInt();
      writeString(110, String(spvalue));
    }
    
    if (configs.containsKey("Debug Enable")) {
      DebugEnable = configs["Debug Enable"].as<String>().toInt();
      writeString(120, String(DebugEnable));
    }
    
    if (configs.containsKey("Transmission Mode")) {
      TransMode = configs["Transmission Mode"].as<String>().toInt();
      writeString(130, String(TransMode));
    }
    
    if (configs.containsKey("Chip Based battery monitoring")) {
      chipBasedBatteryOps = configs["Chip Based battery monitoring"].as<String>().toInt();
      writeString(135, String(chipBasedBatteryOps));
    }
    
    if (configs.containsKey("PM sample entries")) {
      pmSamples = configs["PM sample entries"].as<String>().toInt();
      writeString(140, String(pmSamples));
    }
  }
  
  // Set device config mode
  writeString(145, String(2));
  
  Serial.println(F("JSON Configuration Complete!"));
  printCurrentConfiguration();
}

void printCurrentConfiguration() {
  Serial.println(F("Current Configuration:"));
  Serial.print(F("Device Name: ")); Serial.println(read_String(1));
  Serial.print(F("Channel ID: ")); Serial.println(read_EE(31));   
  Serial.print(F("Write Key: ")); Serial.println(read_String(61)); 
  Serial.print(F("SDCS PIN no.: ")); Serial.println(read_EE(103)); 
  Serial.print(F("Deployment Mode: ")); Serial.println(read_EE(100)); 
  Serial.print(F("Sleep Percentile Value: ")); Serial.println(read_EE(110));  
  Serial.print(F("Debug Enable Value: ")); Serial.println(read_EE(120));  
  Serial.print(F("Transmission Mode Value: ")); Serial.println(read_EE(130));  
  Serial.print(F("Chip Based battery monitoring: ")); Serial.println(read_EE(135));   
  Serial.print(F("PM sample entries: ")); Serial.println(read_EE(140));   
}

void jsonWriteString(char add, String data) {
  int _size = data.length();
  
  EEPROM.write(add, data[0]);
  
  // Write the rest of the string starting from add+1
  for(int i = 0; i < _size; i++){
    EEPROM.write(add + 1 + i, data[i]);
  }
  
  // Add carriage return before null terminator
  EEPROM.write(add + 1 + _size, '\r');
  EEPROM.write(add + 1 + _size + 1, '\0'); 
}

String read_String(char add){
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500){    
    k=EEPROM.read(add+1+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

String read_EE(char add){
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

void writeString(char add,String data){
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++){
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  //EEPROM.commit();
}

// bool attemptGPRSConnection(int maxRetries = 3) {
//     int retryCount = 0;
//     bool connected = false;
    
//     while (!connected && retryCount < maxRetries) {
//       retryCount++;
//       Serial.print(F("GPRS connection attempt "));
//       Serial.print(retryCount);
//       Serial.print(F(" of "));
//       Serial.println(maxRetries);
      
//       // Power cycle modem for each attempt
//       powerGSM(0);
//       delay(3000);
//       powerGSM(1);
//       delay(5000);
      
//       if (checkNetwork()) {
//         delay(3000);
//         connected = connectGPRS();
        
//         if (connected) {
//           Serial.println(F("Successfully connected to GPRS"));
//           return true;
//         }
//       }
      
//       Serial.println(F("Connection attempt failed, retrying..."));
//       delay(5000);
//     }
    
//     Serial.println(F("Failed to connect after maximum retries"));
//     return false;
// }

