#include "gsm_communication.h"
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <pin_definition.h>
#include <ArduinoJson.h>
#include <eeprom_config.h>

const char apn[] = "TM";
const char user[] = "";
const char pass[] = "";
const char serverurl1[] = "34.132.108.60";
const char serverurl2[] = "api.thingspeak.com";

TinyGsm modem(Serial1); // Define the modem object
TinyGsmClient client(modem); // Define the client object
HttpClient http(client, serverurl1, 80); // Define the http object

String apiKey = getConfigValue("DEVICE_WRITE_API_KEY"); // Get the API key from EEPROM
String channelId = getConfigValue("DEVICE_CHANEL_ID"); // Get the channel ID from EEPROM

void checkNetwork() {
    Serial1.println("AT+CREG?");
    delay(1000);
    Serial1.println("AT+CGATT?");
    delay(1000);
}

String getGSMData(){
  delay(2000);
  String response = "";
  while (Serial1.available()) {
    char c = Serial1.read();
    if (isDigit(c)) { // Only keep numeric characters
      response += c;
    }
  }

  if (response.length() > 0) {
    response.trim();
  } else {
    Serial.println("Failed to get IMSI.");
  }
  return response;
}

String getIMSI() {
  Serial1.println("AT+CIMI");
  String response = getGSMData();
  return response;
}

String getCCID() {
  Serial1.println("AT+CCID");
  String response = getGSMData();
  return response;
}

String getIMEI() {
  Serial1.println("AT+GSN");
  String response = getGSMData();
  return response;
}

String getSignalQuality() {
  Serial1.println("AT+CSQ");
  String response = getGSMData();
  return response;
}

void postData(String url) {
    Serial.print("Requesting URL: ");
    Serial.println(url);

    http.get(url);

    int statusCode = http.responseStatusCode();
    String response = http.responseBody();

    Serial.print("Response Code: ");
    Serial.println(statusCode);
    Serial.print("Response Body: ");
    Serial.println(response);

    if (statusCode == 200) {
        Serial.println("Data sent successfully!");
        // updateEEPROMFromResponse(response); // Update EEPROM with response data
    } else {
        Serial.println("Failed to send data.");
    }

    modem.gprsDisconnect();
    Serial.println("Disconnected from GPRS.");
}

void getConfigDetails(String ccid) {
  String url = "/device/"+ccid+"/selfconfig"; //"/";
  postData(url);
}

void postParameters(String field1, String field2, String field3, String field4, String field5, String field6, String field7, String field8, String field9, String field10, String field11, String field12) {
  String url = "/update?api_key=" + apiKey + "&field1=" + field1 + "&field2=" + field2 + "&field3=" + field3 + "&field4=" + field4 + "&field5=" + field5 + "&field6=" + field6 + "&field7=" + field7 + "&field8=" + field8 + "&field9=" + field9 + "&field10=" + field10 + "&field11=" + field11 + "&field12=" + field12;
  postData(url);
}

void connectGPRS() {
  Serial.println("Initializing modem...");
  if (!modem.restart()) {
      Serial.println("Failed to restart modem!");
      return;
  } 
  Serial.println("Waiting for network...");
  if (!modem.waitForNetwork()) {
      Serial.println("Network connection failed!");
      return;
  }
  Serial.println("Connected to network!");
  
  Serial.println("Connecting to GPRS...");
  if (!modem.gprsConnect(apn, user, pass)) {
      Serial.println("Failed to connect to GPRS!");
      return;
  }
  Serial.println("GPRS connected!");
}