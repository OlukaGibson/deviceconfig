#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

#define SerialAT Serial1

void sendToThingSpeak(int field1);

// Define your APN credentials
const char apn[]  = "TM";     // Replace with your APN
const char user[] = "";             // APN username (if required)
const char pass[] = "";             // APN password (if required)

// ThingSpeak API
const char server[] = "api.thingspeak.com";
const String apiKey = "3WN7JU712JH0D99F";  // Your ThingSpeak API Key

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, server, 80);

void setup() {
    Serial.begin(115200);
    pinMode(A0, OUTPUT);
    digitalWrite(A0, HIGH);
    SerialAT.begin(115200);

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

    sendToThingSpeak(25);  // Example: Send field1=25
}

void loop() {
    // No loop logic required
}

void sendToThingSpeak(int field1) {
    String url = "/update?api_key=" + apiKey + "&field1=" + String(field1);
    
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
    } else {
        Serial.println("Failed to send data.");
    }

    modem.gprsDisconnect();
    Serial.println("Disconnected from GPRS.");
}