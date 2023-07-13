#include <AFMotor.h>

AF_DCMotor motor2(2);
AF_DCMotor motor1(1);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

void setup() 
{
	//Set initial speed of the motor & stop
	motor2.setSpeed(200);
  motor1.setSpeed(225);
  motor3.setSpeed(255);
  motor4.setSpeed(255);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void loop() 
{
	uint8_t i;

	// Turn on motor
	motor2.run(FORWARD);
  motor1.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  motor1.setSpeed(255);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(255);
	// delay(1000);
}