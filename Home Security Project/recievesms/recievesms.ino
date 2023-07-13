#include <SoftwareSerial.h>

SoftwareSerial mySerial(3,2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Initializing");
  delay(5000);
  mySerial.println("AT");
  updateSerial();

  mySerial.println("AT+CSQ");
  updateSerial();

  mySerial.println("AT+CNMI=1,2,0,0,0");
  updateSerial();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  updateSerial();
  reader();
}

void updateSerial(){
  while (Serial.available())
  {
    mySerial.write(Serial.read());
    
  }
  while (mySerial.available()){
    Serial.write(mySerial.read());
    
  }
}

void reader(){
  while (mySerial.available()){
    String command;
    command = (String)mySerial.read();
    if (command == "B_ON"){
      digitalWrite(8,HIGH);
    }
    else if(command == "B_OFF"){
      digitalWrite(8,LOW);
    }
  }
}
