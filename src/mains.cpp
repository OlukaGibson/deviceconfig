// #include <Arduino.h>
// #include <SoftwareSerial.h>

// String getIMSI();
// String getCCID();
// String getIMEI();

// void checkNetwork();

// void setup() {
//   Serial.begin(115200);
//   pinMode(A0, OUTPUT);
//   digitalWrite(A0, HIGH);

//   Serial1.begin(115200);
//   Serial.println("Initializing modem...");
//   delay(3000);

//   checkNetwork();

//   String ccid = getCCID();
//   String imei = getIMEI();
//   String imsi = getIMSI();

//   Serial.println("CCID: " + ccid);
//   Serial.println("IMEI: " + imei);
//   Serial.println("IMSI: " + imsi);
// }

// void loop() {
//   while (Serial1.available()) {
//     Serial.write(Serial1.read());
//   }
// }

// void checkNetwork() {
//   Serial1.println("AT+CREG?");
//   delay(1000);
//   Serial1.println("AT+CGATT?");
//   delay(1000);
// }

// String getIMSI() {
//   Serial1.println("AT+CIMI");
//   delay(2000);

//   String response = "";
//   while (Serial1.available()) {
//     char c = Serial1.read();
//     if (isDigit(c)) { // Only keep numeric characters
//       response += c;
//     }
//   }

//   response.trim();
//   return response;
// }

// String getCCID() {
//   Serial1.println("AT+CCID");
//   delay(2000);

//   String response = "";
//   while (Serial1.available()) {
//     char c = Serial1.read();
//     if (isDigit(c)) { // Only keep numeric characters
//       response += c;
//     }
//   }

//   response.trim();
//   return response;
// }

// String getIMEI() {
//   Serial1.println("AT+GSN");
//   delay(2000);

//   String response = "";
//   while (Serial1.available()) {
//     char c = Serial1.read();
//     if (isDigit(c)) { // Only keep numeric characters
//       response += c;
//     }
//   }

//   response.trim();
//   return response;
// }


// // 12:46:27.524 -> CCID: 8944501905220513
// // 12:46:27.524 -> IMEI: 862749051555539
// // 12:46:27.524 -> IMSI: 234502106051302


// //  The  SoftwareSerial  library is used to create a software serial port on pins 2 and 3. The  setup()  function initializes the serial port at 9600 baud and prints a message to the serial port. The  loop()  function prints a message to the serial port every second. 
// //  To compile and upload the code to the ESP32, click on the  Upload  button on the Arduino IDE. 
// //  After uploading the code, open the serial monitor by clicking on the  Tools  menu, then  Serial Monitor . Set the baud rate to 9600 and you should see the message “Hello, world!” printed every second. 
// //  Conclusion 
// //  In this tutorial, you learned how to use the SoftwareSerial library to create a software serial port on the ESP32. You also learned how to use the SoftwareSerial library to communicate with other devices using the software serial port. 
// //  If you have any questions or feedback, feel free to leave a comment. 
// //  Thanks for reading! 
// //  You may also like: 
 
// //  ESP32 Deep Sleep with Periodic Wake Up
// //  ESP32 Web Server with SPIFFS (SPI Flash File System)
// //  ESP32 Web Server with Arduino IDE
// //  ESP32 Static/Fixed IP Address
// //  ESP32 Data Logging Temperature to MicroSD Card



// #define TINY_GSM_MODEM_SIM800

// #include <TinyGsmClient.h>
// #include <ArduinoHttpClient.h>

// #define SerialAT Serial1

// void sendToThingSpeak(String field1);

// // Define your APN credentials
// const char apn[]  = "TM";     // Replace with your APN
// const char user[] = "";             // APN username (if required)
// const char pass[] = "";             // APN password (if required)

// // ThingSpeak API
// const char server[] = "34.132.108.60";//"api.thingspeak.com";
// // const String apiKey = "3WN7JU712JH0D99F";  // Your ThingSpeak API Key

// String imsi = "";

// void checkNetwork();
// String getIMSI();

// TinyGsm modem(SerialAT);
// TinyGsmClient client(modem);
// HttpClient http(client, server, 80);

// void setup() {
//     Serial.begin(115200);
//     pinMode(A0, OUTPUT);
//     digitalWrite(A0, HIGH);
//     SerialAT.begin(115200);

//     checkNetwork();
//     imsi = getIMSI();
//     Serial.print("IMSI: ");
//     Serial.println(imsi);

//     Serial.println("Initializing modem...");
//     if (!modem.restart()) {
//         Serial.println("Failed to restart modem!");
//         return;
//     }

//     Serial.println("Waiting for network...");
//     if (!modem.waitForNetwork()) {
//         Serial.println("Network connection failed!");
//         return;
//     }
//     Serial.println("Connected to network!");

//     Serial.println("Connecting to GPRS...");
//     if (!modem.gprsConnect(apn, user, pass)) {
//         Serial.println("Failed to connect to GPRS!");
//         return;
//     }
//     Serial.println("GPRS connected!");

//     sendToThingSpeak(imsi);  // Example: Send field1=25
// }

// void loop() {
//     // No loop logic required
// }


// void checkNetwork() {
//     Serial1.println("AT+CREG?");
//     delay(1000);
//     Serial1.println("AT+CGATT?");
//     delay(1000);
//   }

//   String getIMSI() {
//     Serial1.println("AT+CIMI");
//     delay(2000);
    
//     String response = "";
//     while (Serial1.available()) {
//         char c = Serial1.read();
//         if (c == '\n' || c == '\r') continue;
//         response += c;
//     }
    
//     response.trim();
    
//     String imsi = "";
//     for (char c : response) {
//       if (isDigit(c)) {
//         imsi += c;
//       }
//     }
    
//     return imsi;
//   }

// void sendToThingSpeak(String imsi) {
//     String url = "/device/234502106051372/selfconfig";//"/"+imsi+"/selfconfig";//"/update?api_key=" + apiKey + "&field1=" + String(field1);
    
//     Serial.print("Requesting URL: ");
//     Serial.println(url);
    
//     http.get(url);

//     int statusCode = http.responseStatusCode();
//     String response = http.responseBody();

//     Serial.print("Response Code: ");
//     Serial.println(statusCode);
//     Serial.print("Response Body: ");
//     Serial.println(response);

//     if (statusCode == 200) {
//         Serial.println("Data sent successfully!");
//     } else {
//         Serial.println("Failed to send data.");
//     }

//     modem.gprsDisconnect();
//     Serial.println("Disconnected from GPRS.");
// }