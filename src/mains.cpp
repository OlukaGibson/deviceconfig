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
char writeAPIKey[16];  // Buffer size for API key
int DeploymentMode;
int SDCS;
int spvalue;
int DebugEnable;
int TransMode;
int chipBasedBatteryOps;
int pmSamples;

// Function prototypes
void configureExample1();
void configureExample2();
void configureFromJSON(String jsonData);
void printCurrentConfiguration();
void writeString(char add, String data);
String read_String(char add);
String read_EE(char add);
void jsonWriteBuffer(char add, char* Buffer);
void getEEPROMData(String ccid);

void setup(){
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("__________________________STARTING device__________________________"));
  Serial1.begin(115200);
  delay(1000);

  // Serial.println(F("Initializing GSM modem..."));
  // powerGSM(1);
  delay(2000);

  
  clearEEPROM();
  // printCurrentConfiguration();

  delay(3000);
  configureExample1();
    // configureExample2();
}

void loop() {}

void configureExample2() {
  String jsonConfig = R"({
    "configs": {
      "Chip Based battery monitoring": "0",
      "Debug Enable": "1",
      "Deployment Mode": "0",
      "PM sample entries": "5",
      "SD card pin": "28",
      "Sleep Percentile Value": "100",
      "Transmission Mode": "0"
    },
    "deviceID": 1,
    "fileDownloadState": false
  })";
  
  configureFromJSON(jsonConfig);
}

void configureExample1() {
  String jsonConfig = R"({
    "configs": {
      "Chip Based battery monitoring": "0",
      "Debug Enable": "1",
      "Deployment Mode": "0",
      "PM sample entries": "60",
      "SD card pin": "28",
      "Sleep Percentile Value": "500",
      "Transmission Mode": "0"
    },
    "deviceID": 2653387,
    "name": " Airqo-G5377",
    "networkID": "8944501905220513027",
    "readkey": " NC2TG9T8745FIABA",
    "writekey": " PJ526POTDD9SZIX9"
  })";
  
  configureFromJSON(jsonConfig); //2653387
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
  
  // Extract and set device name
  if (doc.containsKey("name")) {
    String deviceName = doc["name"];
    if (deviceName.length() <= 22) {
      DevName = deviceName;
      writeString(1, DevName);
      Serial.print(F("Device Name set to: "));
      Serial.println(DevName);
    } else {
      Serial.println(F("Warning: Device name too long, skipping"));
    }
  }
  
  // Extract and set channel ID
  if (doc.containsKey("deviceID")) {
    channelId = doc["deviceID"];
    writeString(31, String(channelId));
    Serial.print(F("Channel ID set to: "));
    Serial.println(channelId);
  }
  
  // Extract and set write API key
  if (doc.containsKey("writekey")) {
    String writeKey = doc["writekey"];
    strcpy(writeAPIKey, writeKey.c_str());
    jsonWriteBuffer(61, writeAPIKey);
    Serial.print(F("Write API Key set to: "));
    Serial.println(writeKey);
  }
  
  // Extract and set configurations
  if (doc.containsKey("configs")) {
    JsonObject configs = doc["configs"];
    
    // Deployment Mode
    if (configs.containsKey("Deployment Mode")) {
      DeploymentMode = configs["Deployment Mode"].as<String>().toInt();
      writeString(100, String(DeploymentMode));
      Serial.print(F("Deployment Mode set to: "));
      Serial.println(DeploymentMode);
    }
    
    // SD Card CS Pin
    if (configs.containsKey("SD card pin")) {
      SDCS = configs["SD card pin"].as<String>().toInt();
      writeString(103, String(SDCS));
      Serial.print(F("SD Card CS Pin set to: "));
      Serial.println(SDCS);
    }
    
    // Sleep Percentile Value
    if (configs.containsKey("Sleep Percentile Value")) {
      spvalue = configs["Sleep Percentile Value"].as<String>().toInt();
      writeString(110, String(spvalue));
      Serial.print(F("Sleep Percentile Value set to: "));
      Serial.println(spvalue);
    }
    
    // Debug Enable
    if (configs.containsKey("Debug Enable")) {
      DebugEnable = configs["Debug Enable"].as<String>().toInt();
      writeString(120, String(DebugEnable));
      Serial.print(F("Debug Enable set to: "));
      Serial.println(DebugEnable);
    }
    
    // Transmission Mode
    if (configs.containsKey("Transmission Mode")) {
      TransMode = configs["Transmission Mode"].as<String>().toInt();
      writeString(130, String(TransMode));
      Serial.print(F("Transmission Mode set to: "));
      Serial.println(TransMode);
    }
    
    // Chip Based Battery Monitoring
    if (configs.containsKey("Chip Based battery monitoring")) {
      chipBasedBatteryOps = configs["Chip Based battery monitoring"].as<String>().toInt();
      writeString(135, String(chipBasedBatteryOps));
      Serial.print(F("Chip Based Battery Monitoring set to: "));
      Serial.println(chipBasedBatteryOps);
    }
    
    // PM Sample Entries
    if (configs.containsKey("PM sample entries")) {
      pmSamples = configs["PM sample entries"].as<String>().toInt();
      writeString(140, String(pmSamples));
      Serial.print(F("PM Sample Entries set to: "));
      Serial.println(pmSamples);
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

void writeString(char add,String data){
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++){
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  //EEPROM.commit();
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

void jsonWriteBuffer(char add, char* Buffer){
  int _size = strlen(Buffer);
  int i;
  for(i=0;i<_size;i++){
    EEPROM.write(add+i,Buffer[i]);
  }
  EEPROM.write(add+_size,'\0');  // Add null terminator
}

void writeBuffer(char add, char* Buffer){
  int _size = strlen(Buffer);
  int i;
  for(i=0;i<_size;i++){
    EEPROM.write(add+i,Buffer[i]);
  }
}

// void postData(String url) {
//     Serial.print(F("Requesting URL: "));
//     Serial.println(url);

//     http.get(url);

//     int statusCode = http.responseStatusCode();
//     responseData = http.responseBody();

//     Serial.print(F("Response Code: "));
//     Serial.println(statusCode);
//     Serial.print(F("Response Body: "));
//     Serial.println(responseData);

//     if (statusCode == 200) {
//         Serial.println(F("Data sent successfully!"));
//         // return response;
//         // updateEEPROMFromResponse(response); // Update EEPROM with response data
//     } else {
//         Serial.println(F("Failed to send data."));
//         // return "null";
//     }

//     // modem.gprsDisconnect();
//     // Serial.println("Disconnected from GPRS.");
// }

// void getEEPROMData(String ccid) {
//   String url =  "/device/"+ccid+"/selfconfig"; //"/";
//   postData(url);
//   // return postData(url);
// }

// String getGSMData(){
//   delay(2000);
//   String response = "";
//   while (Serial1.available()) {
//     char c = Serial1.read();
//     if (isDigit(c)) { // Only keep numeric characters
//       response += c;
//     }
//   }

//   if (response.length() > 0) {
//     response.trim();
//   } else {
//     Serial.println(F("Failed to get IMSI."));
//   }
//   return response;
// }

// String getIMSI() {
//   Serial1.println("AT+CIMI");
//   String response = getGSMData();
//   return response;
// }

// String getCCID() {
//   Serial1.println("AT+CCID");
//   String response = getGSMData();
//   return response;
// }

// String getIMEI() {
//   Serial1.println("AT+GSN");
//   String response = getGSMData();
//   return response;
// }

// String getSignalQuality() {
//   Serial1.println("AT+CSQ");
//   String response = getGSMData();
//   return response;
// }

// void powerGSM(int state) {
//   pinMode(GSM_POWER_SWITCH_PIN, OUTPUT);
//   digitalWrite(GSM_POWER_SWITCH_PIN, state ? HIGH : LOW);
//   // Serial.println("GSM power state: " + String(state ? "ON" : "OFF"));
//   delay(2000);
// }

// #include <SD.h>
// #include <sd_card.h>
// #include <Arduino.h>
// #include <ArduinoJson.h>
// #include <eeprom_config.h>
// #include <TinyGsmClient.h>
// #include <pin_definition.h>
// #include <globalVariables.h>
// #include <ArduinoHttpClient.h>
// #include <gsm_communication.h>


// void setup() {
//   Serial.begin(115200);
//   powerGSM(1);
//   powerSD(1);
//   delay(3000);

//   Serial1.begin(115200);

//   // Test the SD card first 
//   // testSDCard();
//   // delay(1000);

//   // checkNetwork();
//   // delay(3000);
//   // String imei = getIMEI();
//   // String imsi = getIMSI();
//   // String ccid = getCCID();
//   // String signalQuality = getSignalQuality();

//   // Serial.println("CCID: " + ccid);
//   // Serial.println("IMEI: " + imei);
//   // Serial.println("IMSI: " + imsi);
//   // Serial.println("Signal Quality: " + signalQuality);

//   // connectGPRS();
//   // Initialize SD card first, to ensure it's ready for firmware storage
//   // Serial.println("Initializing SD card...");
//   // if (sdHealthCheck() != 0) {
//   //   Serial.println("SD card initialization failed! Cannot proceed with firmware update.");
//   //   return;
//   // }
//   // deviceSelfConfig(ccid);
//   // getConfigData("1");
//   // postMetaData("1", "2", "3", "4");
//   // postDeviceData("1", "2", "3", "4", "5", "6", "7", "8");
//   // String resource = "/firmware/" + firmwareVersion + "/download/firmwarebin";
//   // Serial.println("Resource: " + resource);
//   // firmwareUpdate(FIRMWARE_NAME, resource);
//   // getEEPROMData(ccid);
//   // disconnectGPRS();
//   // powerGSM(0);
//   // delay(2000);
//   // // Serial.println("EEPROM Data: " + responseData);
//   // if (responseData == "null") {
//   //   Serial.println("Failed to get EEPROM data.");
//   //   return;
//   // }
//   // Serial.println("EEPROM Data: " + responseData);
//   // deviceSelfConfig(responseData);
//   String channelID = loadDataFromEEPROM("DEVICE_CHANEL_ID");
//   Serial.println("Channel ID: " + channelID);
//   // Serial.println("Device Name: " + String(eeprom_configuration_struct.DEVICE_NAME));
//   // Serial.println("Device Write API Key: " + String(eeprom_configuration_struct.DEVICE_WRITE_API_KEY));
//   // Serial.println("Device Read API Key: " + String(eeprom_configuration_struct.DEVICE_READ_API_KEY));  
//   // Serial.println("Device Channel ID: " + String(eeprom_configuration_struct.DEVICE_CHANEL_ID));
//   // Serial.println("Device Firmware Version: " + String(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION));
//   // delay(1000);
// }

// void loop() {
//   while (Serial1.available()) {
//     Serial.write(Serial1.read());
//   }
// }

// #include <Arduino.h>
// #include <EEPROM.h>
// #include <ArduinoJson.h>


// const uint32_t WRITTEN_SIGNATURE = 0xBEEFDEED;
// const int SIGNATURE_ADDRESS = 0;

// typedef struct
// {
//     char DEVICE_NAME[32];
//     char DEVICE_WRITE_API_KEY[32];
//     char DEVICE_READ_API_KEY[32];
//     char DEVICE_CHANEL_ID[32];
//     char DEVICE_FIRMWARE_VERSION[32];
//     char DEVICE_FIRMWARE_CRC32[32];
//     int DEVICE_SAMPLE_INTERVAL_MINUTES;
//     int DEVICE_UPLOAD_INTERVAL_MINUTES;
//     int DEPLOYMENT_MODE;
//     int BATTERY_MONITORING;
//     int DEBUG_ENABLE;
//     int PM_SAMPLE_ENTRIES;
//     int SPV;
//     int SD_CARD_PIN;
//     int TRANSMISSION_MODE;
//     int SAMPLE_ENTRIES;
// } EEPROM_CONFIGURATION_STRUCT;

// EEPROM_CONFIGURATION_STRUCT eeprom_configuration_struct;

// void saveConfigurationToEEPROM() {
//     // Write a signature first
//     EEPROM.put(SIGNATURE_ADDRESS, WRITTEN_SIGNATURE);
  
//     // Write the structure after the signature
//     EEPROM.put(SIGNATURE_ADDRESS + sizeof(WRITTEN_SIGNATURE), eeprom_configuration_struct);
  
//     // EEPROM.commit(); // Only needed if you are on ESP32/ESP8266, not for Arduino Mega 2560
//   }

//   void loadFromJSONString(const String& jsonString) {
//     StaticJsonDocument<1024> doc; // Enough space for your JSON
  
//     DeserializationError error = deserializeJson(doc, jsonString);
  
//     if (error) {
//       Serial.print("deserializeJson() failed: ");
//       Serial.println(error.c_str());
//       return;
//     }
  
//     // Read and fill the struct
//     strlcpy(eeprom_configuration_struct.DEVICE_NAME, doc["name"] | "", sizeof(eeprom_configuration_struct.DEVICE_NAME));
//     strlcpy(eeprom_configuration_struct.DEVICE_WRITE_API_KEY, doc["writekey"] | "", sizeof(eeprom_configuration_struct.DEVICE_WRITE_API_KEY));
//     strlcpy(eeprom_configuration_struct.DEVICE_READ_API_KEY, doc["readkey"] | "", sizeof(eeprom_configuration_struct.DEVICE_READ_API_KEY));
//     strlcpy(eeprom_configuration_struct.DEVICE_CHANEL_ID, doc["networkID"] | "", sizeof(eeprom_configuration_struct.DEVICE_CHANEL_ID));
  
//     strlcpy(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION, "1.0.0", sizeof(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION)); // Default for now
//     strlcpy(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32, "00000000", sizeof(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32)); // Default for now
  
//     JsonObject configs = doc["configs"];
  
//     eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES = configs["Sample Interval"] | 15;
//     eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES = configs["Upload Interval"] | 30;
//     eeprom_configuration_struct.DEPLOYMENT_MODE = configs["Deployment Mode"] | 0;
//     eeprom_configuration_struct.BATTERY_MONITORING = configs["Chip Based battery monitoring"] | 0;
//     eeprom_configuration_struct.DEBUG_ENABLE = configs["Debug Enable"] | 0;
//     eeprom_configuration_struct.PM_SAMPLE_ENTRIES = configs["PM sample entries"] | 60;
//     eeprom_configuration_struct.SPV = configs["SPV"] | 100;
//     eeprom_configuration_struct.SD_CARD_PIN = configs["Sd card pin"] | 28;
//     eeprom_configuration_struct.TRANSMISSION_MODE = configs["Transmission Mode"] | 0;
//     eeprom_configuration_struct.SAMPLE_ENTRIES = eeprom_configuration_struct.PM_SAMPLE_ENTRIES; // Assuming you want same
  
//     // Finally, save to EEPROM
//     saveConfigurationToEEPROM();
//   }
  
  
//   void setup() {
//     Serial.begin(115200);
  
//     // if (!loadConfigurationFromEEPROM()) {
//     //   Serial.println("Setting up default configuration...");
  
//     //   // Set default values
//     //   strcpy(eeprom_configuration_struct.DEVICE_NAME, "Airqo-G5377");
//     //   strcpy(eeprom_configuration_struct.DEVICE_WRITE_API_KEY, "L1FU10HZOQ745OCM");
//     //   strcpy(eeprom_configuration_struct.DEVICE_READ_API_KEY, "DIEICCW0EUBWJQCY");
//     //   strcpy(eeprom_configuration_struct.DEVICE_CHANEL_ID, "8944501905220513027");
  
//     //   strcpy(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION, "1.0.0");
//     //   strcpy(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32, "00000000");
  
//     //   eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES = 15;
//     //   eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES = 30;
//     //   eeprom_configuration_struct.DEPLOYMENT_MODE = 0;
//     //   eeprom_configuration_struct.BATTERY_MONITORING = 0;
//     //   eeprom_configuration_struct.DEBUG_ENABLE = 1;
//     //   eeprom_configuration_struct.PM_SAMPLE_ENTRIES = 60;
//     //   eeprom_configuration_struct.SPV = 100;
//     //   eeprom_configuration_struct.SD_CARD_PIN = 28;
//     //   eeprom_configuration_struct.TRANSMISSION_MODE = 0;
//     //   eeprom_configuration_struct.SAMPLE_ENTRIES = 60;
  
//     //   saveConfigurationToEEPROM();
//     // }
    
//     // Load the configuration from EEPROM
//     String incomingJson = R"rawliteral(
//         {
//           "configs": {
//             "Chip Based battery monitoring": "0",
//             "Debug Enable": "1",
//             "Deployment Mode": "0",
//             "PM sample entries": "70",
//             "SPV": "100",
//             "Sample Interval": "15",
//             "Sd card pin": "28",
//             "Transmission Mode": "0",
//             "Upload Interval": "30"
//           },
//           "deviceID": 1,
//           "name": "Airqo-G5378",
//           "networkID": "8944501905220513027",
//           "readkey": "DIEICCW0EUBWJQCY",
//           "writekey": "L1FU10HZOQ745OCM"
//         }
//         )rawliteral";
      
//     loadFromJSONString(incomingJson);
    
//     // Show what we loaded
//     Serial.print("Device Name: ");
//     Serial.println(eeprom_configuration_struct.DEVICE_NAME);
//     Serial.print("Device Write API Key: ");
//     Serial.println(eeprom_configuration_struct.DEVICE_WRITE_API_KEY);
//     Serial.print("Device Read API Key: ");
//     Serial.println(eeprom_configuration_struct.DEVICE_READ_API_KEY);
//     Serial.print("Device Channel ID: ");
//     Serial.println(eeprom_configuration_struct.DEVICE_CHANEL_ID);
//     Serial.print("Device Firmware Version: ");
//     Serial.println(eeprom_configuration_struct.DEVICE_FIRMWARE_VERSION);
//     Serial.print("Device Firmware CRC32: ");
//     Serial.println(eeprom_configuration_struct.DEVICE_FIRMWARE_CRC32);
//     Serial.print("Sample Interval: ");
//     Serial.println(eeprom_configuration_struct.DEVICE_SAMPLE_INTERVAL_MINUTES);
//     Serial.print("Upload Interval: ");
//     Serial.println(eeprom_configuration_struct.DEVICE_UPLOAD_INTERVAL_MINUTES);
//     Serial.print("Deployment Mode: ");
//     Serial.println(eeprom_configuration_struct.DEPLOYMENT_MODE);
//     Serial.print("Battery Monitoring: ");
//     Serial.println(eeprom_configuration_struct.BATTERY_MONITORING);
//     Serial.print("Debug Enable: ");
//     Serial.println(eeprom_configuration_struct.DEBUG_ENABLE);
//     Serial.print("PM Sample Entries: ");
//     Serial.println(eeprom_configuration_struct.PM_SAMPLE_ENTRIES);
//     Serial.print("SPV: ");
//     Serial.println(eeprom_configuration_struct.SPV);
//     Serial.print("SD Card Pin: ");
//     Serial.println(eeprom_configuration_struct.SD_CARD_PIN);
//     Serial.print("Transmission Mode: ");
//     Serial.println(eeprom_configuration_struct.TRANSMISSION_MODE);

//   }
  
//   void loop() {
//     // Nothing here
//   }
  

  

// // #include <CRC32.h>
// // #include <SD.h>

// // void setup() {
// //   Serial.begin(115200);
// //   pinMode(30, OUTPUT);
// //   digitalWrite(30, HIGH);
// //   delay(1000);
// //   if (!SD.begin(28)) {
// //     Serial.println("SD init failed!");
// //     return;
// //   }

// //   Serial.println("SD card initialized.");
// //   File file = SD.open("/firmware.bin");
// //   if (!file) {
// //     Serial.println("Failed to open file!");
// //     return;
// //   }

// //   // Create CRC32 object
// //   CRC32 crc;
  
// //   // Process the file in chunks for better performance
// //   const size_t bufferSize = 64;
// //   uint8_t buffer[bufferSize];
  
// //   while (file.available()) {
// //     // Read up to bufferSize bytes at once
// //     size_t bytesRead = file.read(buffer, bufferSize);
    
// //     // Update CRC with the buffer
// //     for (size_t i = 0; i < bytesRead; i++) {
// //       crc.update(buffer[i]);
// //     }
// //   }
// //   file.close();
  
// //   // Get the final CRC value
// //   uint32_t checksum = crc.finalize();

// //   Serial.print("CRC32: 0x");
// //   Serial.println(checksum, HEX);
// // }

// // void loop() {
// //   // No loop logic needed
// // }

// // // #include <Arduino.h>
// // // /*
// // //   Blink
// // //   Turns on an LED on for one second, then off for one second, repeatedly.
 
// // //   This example code is in the public domain.
// // //  */
 
// // // // Pin 13 has an LED connected on most Arduino boards.
// // // // give it a name:
// // // int led = 13;
// // // #include "EEPROM.h"
// // // #include <avr/wdt.h>
// // // // the setup routine runs once when you press reset:
// // // void setup() {                
// // //   Serial.begin(115200);
// // //   Serial.println("Press 'F' and 'enter' to set flagbit in eeprom 0x1FF to 0xF0 ");
// // //   Serial.println("also to induce watchdog timeout which triggers the bootloader ");
// // //   Serial.println("and flashes the new firmware on the sd card");

// // // }

// // // // the loop routine runs over and over again forever:
// // // void loop() {
// // //  char inChar = '\0';

// // //  while (Serial.available() > 0) {
// // //    inChar = Serial.read();
// // //  }
// // //    wdt_enable(WDTO_500MS); // have the wdt reset the chip
// // //   // if 500ms passes without a reset
// // //  if(inChar == 'F'){
// // //     Serial.println("");
// // //     Serial.println("rebooting and flashing with firmware.bin on sdcard"); 
// // //     EEPROM.write(0x1FF,0xF0);
// // //     wdt_reset();
// // //     delay(600); // wait 600ms to timeout wdt
// // //  }
// // // }

// // // // #define TINY_GSM_MODEM_SIM800

// // // // #include <TinyGsmClient.h>
// // // // #include <ArduinoHttpClient.h>
// // // // #include <SD.h>
// // // // #include <SPI.h>

// // // // // GSM module settings
// // // // #define GSM_POWER_SWITCH_PIN    A0
// // // // #define RX1_PIN                 19
// // // // #define TX1_PIN                 18
// // // // #define GSM_RESET_PIN           10

// // // // // SD card settings
// // // // #define SD_POWER_SWITCH_PIN     30
// // // // #define SD_CS_PIN               28
// // // // #define SD_MOSI_PIN             51
// // // // #define SD_MISO_PIN             50
// // // // #define SD_SCK_PIN              52

// // // // #define FILE_NAME "/firmware.bin"

// // // // // APN config
// // // // const char apn[]  = "TM"; // Replace with your SIM card APN
// // // // const char user[] = "";
// // // // const char pass[] = "";

// // // // // URL to download
// // // // const char server[] = "34.132.108.60";
// // // // const int port = 80;
// // // // const char resource[] = "/firmware/42.74/download/firwmwarebin";

// // // // // Firmware expected size
// // // // const unsigned long EXPECTED_SIZE = 131481; // ~128.4KB

// // // // // Globals
// // // // TinyGsm modem(Serial1);
// // // // TinyGsmClient client(modem);
// // // // HttpClient http(client, server, port);

// // // // void powerGSM(bool state) {
// // // //   digitalWrite(GSM_POWER_SWITCH_PIN, state);
// // // // }

// // // // void resetGSM() {
// // // //   digitalWrite(GSM_RESET_PIN, LOW);
// // // //   delay(100);
// // // //   digitalWrite(GSM_RESET_PIN, HIGH);
// // // //   delay(1000);
// // // // }

// // // // void powerSD(bool state) {
// // // //   digitalWrite(SD_POWER_SWITCH_PIN, state);
// // // // }

// // // // void setup() {
// // // //   Serial.begin(115200);
// // // //   delay(10);

// // // //   // Power and reset control pins
// // // //   pinMode(GSM_POWER_SWITCH_PIN, OUTPUT);
// // // //   pinMode(GSM_RESET_PIN, OUTPUT);
// // // //   pinMode(SD_POWER_SWITCH_PIN, OUTPUT);

// // // //   powerGSM(true);
// // // //   resetGSM();

// // // //   // Begin Serial1 for GSM
// // // //   Serial1.begin(9600);

// // // //   Serial.println("Initializing modem...");
// // // //   modem.restart();

// // // //   Serial.print("Connecting to GPRS...");
// // // //   if (!modem.gprsConnect(apn, user, pass)) {
// // // //     Serial.println(" fail");
// // // //     while (true);
// // // //   }
// // // //   Serial.println(" success");
  
// // // //   // Set client timeout
// // // //   client.setTimeout(30000); // 30 seconds timeout

// // // //   // Power on SD card
// // // //   powerSD(true);
// // // //   delay(100);

// // // //   SPI.begin();
// // // //   if (!SD.begin(SD_CS_PIN)) {
// // // //     Serial.println("SD card init failed!");
// // // //     while (true);
// // // //   }
// // // //   Serial.println("SD card initialized.");

// // // //   // If old file exists, delete it
// // // //   if (SD.exists(FILE_NAME)) {
// // // //     SD.remove(FILE_NAME);
// // // //     Serial.println("Removed old firmware file.");
// // // //   }

// // // //   // Start HTTP GET request
// // // //   Serial.println("Sending GET request...");
// // // //   http.beginRequest();
// // // //   http.get(resource);
// // // //   http.sendHeader("Accept", "application/octet-stream");
// // // //   http.endRequest();

// // // //   // Check response status
// // // //   int statusCode = http.responseStatusCode();
// // // //   Serial.print("Status code: ");
// // // //   Serial.println(statusCode);
  
// // // //   if (statusCode != 200) {
// // // //     Serial.print("Failed to get file. Status code: ");
// // // //     Serial.println(statusCode);
// // // //     return;
// // // //   }
  
// // // //   // Get content length
// // // //   int length = http.contentLength();
// // // //   Serial.print("Content-Length: ");
// // // //   Serial.println(length);
  
// // // //   // Use expected size if content length is invalid
// // // //   if (length <= 0) {
// // // //     Serial.println("Invalid content length. Using expected size instead.");
// // // //     length = EXPECTED_SIZE;
// // // //   }
  
// // // //   // Prepare file for writing
// // // //   File file = SD.open(FILE_NAME, FILE_WRITE);
// // // //   if (!file) {
// // // //     Serial.println("Failed to open file on SD card.");
// // // //     return;
// // // //   }

// // // //   // Download variables
// // // //   const size_t chunkSize = 1024; // 1KB chunks
// // // //   byte buff[chunkSize];
// // // //   unsigned long downloaded = 0;
// // // //   unsigned long lastProgressTime = millis();
// // // //   unsigned long downloadStartTime = millis();
// // // //   unsigned long lastDataTime = millis();
// // // //   boolean receivingData = false;
  
// // // //   Serial.println("Starting download...");
  
// // // //   // Main download loop
// // // //   while (true) {
// // // //     // Check for available data
// // // //     if (http.available()) {
// // // //       lastDataTime = millis();
// // // //       receivingData = true;
      
// // // //       size_t available = http.available();
// // // //       size_t toRead = min(available, chunkSize);
// // // //       int bytesRead = http.read(buff, toRead);
      
// // // //       // Process received data
// // // //       if (bytesRead > 0) {
// // // //         size_t bytesWritten = file.write(buff, bytesRead);
// // // //         if (bytesWritten != (size_t)bytesRead) {
// // // //           Serial.println("Write error!");
// // // //           break;
// // // //         }
// // // //         downloaded += bytesRead;
        
// // // //         // Print progress indicators
// // // //         if (downloaded % 4096 == 0) { // Every 4KB
// // // //           Serial.print(".");
// // // //         }
        
// // // //         // Print progress percentage
// // // //         if (millis() - lastProgressTime > 5000) {  // Every 5 seconds
// // // //           Serial.println();
// // // //           Serial.print("Downloaded: ");
// // // //           Serial.print(downloaded / 1024);
// // // //           Serial.print("KB (");
// // // //           if (length > 0) {
// // // //             Serial.print((downloaded * 100) / length);
// // // //             Serial.print("%");
// // // //           }
// // // //           Serial.println(")");
// // // //           lastProgressTime = millis();
// // // //         }
// // // //       }
// // // //     } 
// // // //     else {
// // // //       // Check if we're done or disconnected
// // // //       if (!http.connected()) {
// // // //         if (receivingData) {
// // // //           Serial.println("\nServer disconnected. Download may be complete.");
// // // //           break;
// // // //         }
// // // //       }
      
// // // //       // Check for data timeout (10 seconds without data)
// // // //       if (receivingData && (millis() - lastDataTime > 10000)) {
// // // //         Serial.println("\nData reception timeout.");
// // // //         break;
// // // //       }
      
// // // //       // Small delay when no data is available
// // // //       delay(100);
// // // //     }
    
// // // //     // Safety timeout - abort if downloading takes too long (5 minutes)
// // // //     if (millis() - downloadStartTime > 300000) {
// // // //       Serial.println("\nDownload timeout - maximum time exceeded!");
// // // //       break;
// // // //     }
// // // //   }

// // // //   file.close();
  
// // // //   // Verify downloaded file
// // // //   File readFile = SD.open(FILE_NAME);
// // // //   if (readFile) {
// // // //     unsigned long fileSize = readFile.size();
// // // //     readFile.close();
    
// // // //     Serial.print("\nDownload completed. Total bytes: ");
// // // //     Serial.println(downloaded);
// // // //     Serial.print("Saved file size: ");
// // // //     Serial.println(fileSize);
    
// // // //     // Verify file size
// // // //     if (fileSize < 10000) { // If smaller than 10KB
// // // //       Serial.println("WARNING: File appears incomplete!");
// // // //     } else {
// // // //       Serial.println("File download appears successful.");
// // // //     }
// // // //   } else {
// // // //     Serial.println("Error opening file for verification.");
// // // //   }
  
// // // //   // Disconnect from network
// // // //   modem.gprsDisconnect();
// // // //   Serial.println("GPRS disconnected");
// // // // }

// // // // void loop() {
// // // //   // Nothing here
// // // // }


// // // // // int8_t binDownload(String fileName, String resource) {
// // // // //     Serial.println("Starting binary download: " + fileName);
// // // // //     Serial.println("From resource: " + resource);
// // //   
// // // // //     // Initialize download tracking variables
// // // // //     bool downloadComplete = false;
// // // // //     int8_t downloadResult = 0;
// // // // //     uint32_t downloaded = 0;
// // // // //     bool receivingData = false;
// // // // //     unsigned long lastDataTime = 0;
// // // // //     unsigned long downloadStartTime = millis();
// // // // //     unsigned long lastProgressTime = 0;
// // //     
// // // // //     // Check if file exists to determine if we should resume
// // // // //     if (SD.exists(fileName)) {
// // // // //       File existingFile = SD.open(fileName);
// // // // //       if (existingFile) {
// // // // //         downloaded = existingFile.size();
// // // // //         existingFile.close();
// // //         
// // // // //         if (downloaded > 0) {
// // // // //           Serial.print("Existing file found with size: ");
// // // // //           Serial.println(downloaded);
// // // // //           Serial.println("Attempting to resume download...");
// // // // //           return resumeBinDownload(fileName, resource);
// // // // //         } else {
// // // // //           // File exists but is empty, delete and restart
// // // // //           SD.remove(fileName);
// // // // //         }
// // // // //       }
// // // // //     }
// // //     
// // // // //     // Start fresh download
// // // // //     http.beginRequest();
// // // // //     http.get(resource);
// // // // //     http.sendHeader("Accept", "application/octet-stream");
// // // // //     http.endRequest();
// // //   
// // // // //     // Check response status
// // // // //     int statusCode = http.responseStatusCode();
// // // // //     Serial.print("Status code: ");
// // // // //     Serial.println(statusCode);
// // //   
// // // // //     if (statusCode != 200) {
// // // // //       Serial.print("Failed to get file. Status code: ");
// // // // //       Serial.println(statusCode);
// // // // //       return -1;
// // // // //     }
// // //   
// // // // //     // Get content length
// // // // //     uint32_t length = http.contentLength();
// // // // //     Serial.print("Content-Length: ");
// // // // //     Serial.println(length);
// // //   
// // // // //     // Prepare file for writing
// // // // //     File file = SD.open(fileName, FILE_WRITE);
// // // // //     if (!file) {
// // // // //       Serial.println("Failed to open file on SD card.");
// // // // //       return -1;
// // // // //     }
// // //   
// // // // //     while (true) {
// // // // //       // Check for available data
// // // // //       if (http.available()) {
// // // // //         lastDataTime = millis();
// // // // //         receivingData = true;
// // //         
// // // // //         size_t available = http.available();
// // // // //         size_t toRead = min(available, chunkSize);
// // // // //         int bytesRead = http.read(buff, toRead);
// // //         
// // // // //         // Process received data
// // // // //         if (bytesRead > 0) {
// // // // //           size_t bytesWritten = file.write(buff, bytesRead);
// // // // //           if (bytesWritten != (size_t)bytesRead) {
// // // // //             Serial.println("Write error!");
// // // // //             file.close();
// // // // //             return -1;
// // // // //           }
// // // // //           downloaded += bytesRead;
// // //           
// // // // //           // Print progress indicators
// // // // //           if (downloaded % 4096 == 0) { // Every 4KB
// // // // //             Serial.print(".");
// // // // //           }
// // //           
// // // // //           // Print progress percentage
// // // // //           if (millis() - lastProgressTime > 5000) {  // Every 5 seconds
// // // // //             Serial.println();
// // // // //             Serial.print("Downloaded: ");
// // // // //             Serial.print(downloaded / 1024);
// // // // //             Serial.print("KB");
// // // // //             if (length > 0) {
// // // // //               Serial.print(" (");
// // // // //               Serial.print((downloaded * 100) / length);
// // // // //               Serial.print("%)");
// // // // //             }
// // // // //             Serial.println();
// // // // //             lastProgressTime = millis();
// // // // //           }
// // // // //         }
// // // // //       } 
// // // // //       else {
// // // // //         // Check if we're done or disconnected
// // // // //         if (!http.connected()) {
// // // // //           if (receivingData) {
// // // // //             Serial.println("\nServer disconnected. Download may be complete or interrupted.");
// // // // //             break;
// // // // //           }
// // // // //         }
// // //         
// // // // //         // Check for data timeout (20 seconds without data)
// // // // //         if (receivingData && (millis() - lastDataTime > 20000)) {
// // // // //           Serial.println("\nData reception timeout.");
// // // // //           break;
// // // // //         }
// // //         
// // // // //         // Small delay when no data is available
// // // // //         delay(100);
// // // // //       }
// // //       
// // // // //       // Safety timeout - abort if downloading takes too long (5 minutes)
// // // // //       if (millis() - downloadStartTime > 300000) {
// // // // //         Serial.println("\nDownload timeout - maximum time exceeded!");
// // // // //         break;
// // // // //       }
// // // // //     }
// // //     
// // // // //     file.close();
// // //     
// // // // //     // Verify downloaded file
// // // // //     File readFile = SD.open(fileName);
// // // // //     if (readFile) {
// // // // //       uint32_t fileSize = readFile.size();
// // // // //       readFile.close();
// // //       
// // // // //       Serial.print("\nDownload attempt completed. Total bytes: ");
// // // // //       Serial.println(downloaded);
// // // // //       Serial.print("Saved file size: ");
// // // // //       Serial.println(fileSize);
// // //   
// // // // //       // Verify against content length if available
// // // // //       if (length > 0 && fileSize < length) {
// // // // //         Serial.println("File appears incomplete. Can be resumed later.");
// // // // //         return 1; // Resumable state
// // // // //       } else {
// // // // //         Serial.println("File download appears successful.");
// // // // //         return 0; // Success
// // // // //       }
// // // // //     } else {
// // // // //       Serial.println("Error opening file for verification.");
// // // // //       return -1; // Error
// // // // //     }
// // // // //   }
// // //   
// // // // //   int8_t resumeBinDownload(String fileName, String resource) {
// // // // //     // Check already downloaded size
// // // // //     File file = SD.open(fileName, FILE_WRITE);
// // // // //     if (!file) {
// // // // //       Serial.println("Failed to open file for resuming.");
// // // // //       return -1;
// // // // //     }
// // //   
// // // // //     uint32_t alreadyDownloaded = file.size();
// // // // //     file.close(); // Close and reopen in append mode
// // // // //     file = SD.open(fileName, FILE_WRITE);
// // // // //     if (!file) {
// // // // //       Serial.println("Failed to reopen file in append mode.");
// // // // //       return -1;
// // // // //     }
// // //     
// // // // //     if (!file.seek(alreadyDownloaded)) {
// // // // //       Serial.println("Failed to seek to end of file.");
// // // // //       file.close();
// // // // //       return -1;
// // // // //     }
// // //   
// // // // //     Serial.print("Resuming download from byte: ");
// // // // //     Serial.println(alreadyDownloaded);
// // //   
// // // // //     // Setup HTTP with Range header
// // // // //     http.beginRequest();
// // // // //     http.get(resource);
// // // // //     http.sendHeader("Range", "bytes=" + String(alreadyDownloaded) + "-");
// // // // //     http.sendHeader("Accept", "application/octet-stream");
// // // // //     http.endRequest();
// // //   
// // // // //     int statusCode = http.responseStatusCode();
// // // // //     Serial.print("Status code: ");
// // // // //     Serial.println(statusCode);
// // //   
// // // // //     if (statusCode != 206 && statusCode != 200) {
// // // // //       Serial.println("Server did not support partial content or failed.");
// // // // //       file.close();
// // // // //       return -1;
// // // // //     }
// // //   
// // // // //     uint32_t contentLength = http.contentLength();
// // // // //     Serial.print("Content-Length: ");
// // // // //     Serial.println(contentLength);
// // //   
// // // // //     uint32_t totalDownloaded = alreadyDownloaded;
// // // // //     unsigned long lastDataTime = millis();
// // // // //     unsigned long downloadStartTime = millis();
// // // // //     unsigned long lastProgressTime = 0;
// // // // //     bool receivingData = false;
// // //   
// // // // //     while (true) {
// // // // //       if (http.available()) {
// // // // //         receivingData = true;
// // // // //         lastDataTime = millis();
// // //   
// // // // //         size_t available = http.available();
// // // // //         size_t toRead = min(available, chunkSize);
// // // // //         int bytesRead = http.read(buff, toRead);
// // //   
// // // // //         if (bytesRead > 0) {
// // // // //           size_t written = file.write(buff, bytesRead);
// // // // //           if (written != (size_t)bytesRead) {
// // // // //             Serial.println("Write error during resume.");
// // // // //             file.close();
// // // // //             return -1;
// // // // //           }

// // // // //           totalDownloaded += bytesRead;
// // //   
// // // // //           if (totalDownloaded % 4096 == 0) {
// // // // //             Serial.print(".");
// // // // //           }
// // //   
// // // // //           if (millis() - lastProgressTime > 5000) {
// // // // //             Serial.println();
// // // // //             Serial.print("Downloaded: ");
// // // // //             Serial.print(totalDownloaded / 1024);
// // // // //             Serial.print("KB");
// // // // //             if (contentLength > 0) {
// // // // //               uint32_t totalSize = alreadyDownloaded + contentLength;
// // // // //               Serial.print(" (");
// // // // //               Serial.print((totalDownloaded * 100) / totalSize);
// // // // //               Serial.print("%)");
// // // // //             }
// // // // //             Serial.println();
// // // // //             lastProgressTime = millis();
// // // // //           }
// // // // //         }
// // // // //       } else {
// // // // //         if (!http.connected()) {
// // // // //           Serial.println("\nDisconnected from server.");
// // // // //           break;
// // // // //         }
// // //   
// // // // //         if (receivingData && (millis() - lastDataTime > 20000)) {
// // // // //           Serial.println("\nTimeout: No data for 20 seconds.");
// // // // //           break;
// // // // //         }
// // //   
// // // // //         delay(100);
// // // // //       }
// // //   
// // // // //       if (millis() - downloadStartTime > 300000) {
// // // // //         Serial.println("\nDownload timeout (5 minutes).");
// // // // //         break;
// // // // //       }
// // // // //     }
// // //   
// // // // //     file.close();
// // //   
// // // // //     // Check file size after download attempt
// // // // //     File finalFile = SD.open(fileName);
// // // // //     if (!finalFile) {
// // // // //       Serial.println("Failed to reopen file for verification.");
// // // // //       return -1;
// // // // //     }
// // //   
// // // // //     uint32_t finalSize = finalFile.size();
// // // // //     finalFile.close();
// // //   
// // // // //     Serial.print("Final file size: ");
// // // // //     Serial.println(finalSize);
// // //   
// // // // //     // Determine if the download is complete based on content-length
// // // // //     if (contentLength > 0) {
// // // // //       uint32_t expectedTotalSize = alreadyDownloaded + contentLength;
// // //       
// // // // //       if (finalSize < expectedTotalSize) {
// // // // //         Serial.println("Download not complete. Can resume later.");
// // // // //         return 1;
// // // // //       } else {
// // // // //         Serial.println("Binary download complete!");
// // // // //         return 0;
// // // // //       }
// // // // //     } else {
// // // // //       // If content-length is not available, we can't verify completion
// // // // //       Serial.println("Download progress cannot be verified without content length.");
// // // // //       return 0;  // Assume success
// // // // //     }
// // // // //   }
// // //   
// // // // //   void downloadBinaryFile(String fileName, String resource) {
// // // // //     // Delete existing file if it exists (unless we want to resume)
// // // // //     int8_t downloadState = binDownload(fileName, resource);
// // //   
// // // // //     // Keep retrying if needed
// // // // //     int maxRetries = 3;
// // // // //     int retryCount = 0;
// // //     
// // // // //     while (downloadState == 1 && retryCount < maxRetries) {
// // // // //       Serial.print("Retrying binary download (attempt ");
// // // // //       Serial.print(retryCount + 1);
// // // // //       Serial.print("/");
// // // // //       Serial.print(maxRetries);
// // // // //       Serial.println(")...");
// // //       
// // // // //       downloadState = resumeBinDownload(fileName, resource);
// // // // //       retryCount++;
// // //       
// // // // //       if (downloadState == 1) {
// // // // //         delay(5000); // Wait before retrying
// // // // //       }
// // // // //     }
// // //   
// // // // //     if (downloadState == 0) {
// // // // //       Serial.println("Binary download completed successfully!");
// // // // //     } else if (downloadState == 1) {
// // // // //       Serial.println("Binary download is incomplete after maximum retries.");
// // // // //     } else {
// // // // //       Serial.println("Binary download failed with error.");
// // // // //     }
// // // // //   }