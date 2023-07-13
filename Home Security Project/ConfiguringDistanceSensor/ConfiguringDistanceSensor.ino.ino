#define echoPin 10
#define trigPin 9
#define buzzerPin 8
#include <SoftwareSerial.h>

long duration;
float distance;
float trigDistance = 2.80f;
int count;
SoftwareSerial mySerial(3,2);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode( trigPin , OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin,OUTPUT);
   mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, i t will back to OK
  updateSerial();
  

}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(200);
  digitalWrite(trigPin , HIGH);
  delayMicroseconds(100);
  digitalWrite(trigPin,LOW);
  float duration = pulseIn(echoPin, HIGH);
  float distance = (340*duration)/2000000;
  Serial.print(distance);
  Serial.println("m");
  delay(200);

  if (distance <= trigDistance){
    Serial.println("There is someone in between ");
    count+=1;
    delay(200);
    mySerial.println("ATD+ +919711674182;"); //  change ZZ with country code and xxxxxxxxxxx with phone number to dial
    updateSerial();
    delay(20000); // wait for 20 seconds...
    mySerial.println("ATH"); //hang up
    updateSerial();
  }
  

}
void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
