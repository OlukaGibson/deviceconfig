#include <Arduino.h>
#include <SoftwareSerial.h>

String apn = "TM";
String apn_u = "";
String apn_p = "";
String url = "https://api.thingspeak.com/update?api_key=3WN7JU712JH0D99F&field1=11";

void setup() {
  Serial.begin(115200);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
  Serial1.begin(115200);
  
  // Initialize modem
  Serial1.println("AT");
  delay(1000);
  Serial1.println("AT+CREG?");
  delay(1000);
  Serial1.println("AT+CGATT?");
  delay(1000);
  Serial1.println("AT+CSTT=\"" + apn + "\",\"" + apn_u + "\",\"" + apn_p + "\"");
  delay(1000);
  Serial1.println("AT+CIICR");
  delay(3000);
  Serial1.println("AT+CIFSR");
  delay(1000);
  
  // Start HTTP POST
  Serial1.println("AT+HTTPINIT");
  delay(1000);
  Serial1.println("AT+HTTPPARA=\"CID\",1");
  delay(1000);
  Serial1.println("AT+HTTPPARA=\"URL\",\"" + url + "\"");
  delay(1000);
  Serial1.println("AT+HTTPACTION=0"); // 1 for POST
  delay(5000); // Wait for the response
  
  // Read the response
  if (Serial1.available()) {
    while (Serial1.available()) {
      char c = Serial1.read();
      Serial.print(c);
    }
  }
  
  Serial1.println("AT+HTTPTERM");
}

void loop() {
  // Nothing to do here
}

// #include <Arduino.h>
// #include <SoftwareSerial.h>

// String imsi = "";

// void checkNetwork();
// String getIMSI();

// void setup() {
//   Serial.begin(115200);
//   pinMode(A0, OUTPUT);
//   digitalWrite(A0, HIGH);

//   Serial1.begin(115200);
//   Serial.println("Initializing modem...");
//   delay(3000);

//   checkNetwork();
//   imsi = getIMSI();

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
//       char c = Serial1.read();
//       if (c == '\n' || c == '\r') continue;
//       response += c;
//   }
  
//   response.trim();
//   Serial.print("IMSI: ");
//   Serial.println(response);
  
//   return response;
// }

//  The  SoftwareSerial  library is used to create a software serial port on pins 2 and 3. The  setup()  function initializes the serial port at 9600 baud and prints a message to the serial port. The  loop()  function prints a message to the serial port every second. 
//  To compile and upload the code to the ESP32, click on the  Upload  button on the Arduino IDE. 
//  After uploading the code, open the serial monitor by clicking on the  Tools  menu, then  Serial Monitor . Set the baud rate to 9600 and you should see the message “Hello, world!” printed every second. 
//  Conclusion 
//  In this tutorial, you learned how to use the SoftwareSerial library to create a software serial port on the ESP32. You also learned how to use the SoftwareSerial library to communicate with other devices using the software serial port. 
//  If you have any questions or feedback, feel free to leave a comment. 
//  Thanks for reading! 
//  You may also like: 
 
//  ESP32 Deep Sleep with Periodic Wake Up
//  ESP32 Web Server with SPIFFS (SPI Flash File System)
//  ESP32 Web Server with Arduino IDE
//  ESP32 Static/Fixed IP Address
//  ESP32 Data Logging Temperature to MicroSD Card