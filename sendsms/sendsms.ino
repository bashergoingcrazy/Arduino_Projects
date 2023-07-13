#include <SoftwareSerial.h>

SoftwareSerial mySerial(3,2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("initializing");
  delay(5000);
  mySerial.println("AT");
  updateSerial();

  mySerial.println("AT+CMGF=1");
  updateSerial();

  mySerial.println("AT+CMGS=\"+919711674182\"");
  updateSerial();
  mySerial.print("HELLO I am arduino, How can I help you today?");
  updateSerial();
  mySerial.write(26);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

void updateSerial(){
  delay(500);
  while(Serial.available()){
    mySerial.write(Serial.read());
  }
  while(mySerial.available()){
    Serial.write(mySerial.read());
  }
}
  
