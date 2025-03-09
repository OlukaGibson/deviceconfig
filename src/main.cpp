#include <Arduino.h>
#include <SoftwareSerial.h>

String imsi = "";

void checkNetwork();
String getIMSI();

void setup() {
  Serial.begin(115200);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);

  Serial1.begin(115200);
  Serial.println("Initializing modem...");
  delay(3000);

  checkNetwork();
  imsi = getIMSI();

}

void loop() {
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
}
 
void checkNetwork() {
  Serial1.println("AT+CREG?");
  delay(1000);
  Serial1.println("AT+CGATT?");
  delay(1000);
}

String getIMSI() {
  Serial1.println("AT+CIMI");
  delay(2000);
  
  String response = "";
  while (Serial1.available()) {
      char c = Serial1.read();
      if (c == '\n' || c == '\r') continue;
      response += c;
  }
  
  response.trim();
  Serial.print("IMSI: ");
  Serial.println(response);
  
  return response;
}

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