#include <SoftwareSerial.h>
SoftwareSerial mySerial(3,2);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);

  for (int i = 0; i<=5;i++){
    Serial.print(".");
    delay(1000);
  }
  mySerial.println("AT");// Handshake test
  updateSerial();
  mySerial.println("AT+CSQ");//Signal quailty test
  updateSerial();
  mySerial.println("AT+CCID");//Read sim if it is plugged
  updateSerial();
  mySerial.println("AT+CREG?");//Check whether it has registered in the network
  updateSerial();
}

void loop() {
  // put your main code here, to run repeatedly:
  updateSerial();
}

void updateSerial(){
  delay(500);
  while (Serial.available()){
    mySerial.write(Serial.read());
  }
  while(mySerial.available()){
    Serial.write(mySerial.read());
  }  
  
}
