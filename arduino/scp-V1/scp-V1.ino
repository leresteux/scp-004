//https://www.tutorialspoint.com/arduino/arduino_multi_dimensional_arrays.htm
#include <Servo.h>

byte startAngle = 90;
bool directionAngle[4]={1,1,1,1};
int pos[4];
int redpin = 9;
int bluepin = 10;
int greenpin = 11;
int whitePower = 200;
int maxPower = 240;
int miniPower = 100;

Servo servo[4];

void setup() {
  pos[0]=startAngle;
  pinMode(redpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  servo[0].attach(3);
  pos[0] = startAngle;
  servo[0].write(pos[0]);
}


void loop() {

 // move(0);
  ledRVB();

}

void move(byte whatServoInfo) {
  if (pos[whatServoInfo] < 10 or pos[whatServoInfo] > 170) {
    directionAngle[whatServoInfo] = !directionAngle[whatServoInfo];
  }
  if (directionAngle[whatServoInfo]) {
    pos[whatServoInfo]++;
  } else {
    pos[whatServoInfo]--;
  }
  servo[whatServoInfo].write(pos[whatServoInfo]);
}

void ledRVB() {

  whitePower = whitePower + random(-1, 3);
  if (whitePower < miniPower) {
    whitePower = miniPower;
  }
  if (whitePower > maxPower) {
    whitePower = maxPower;
  }
  analogWrite(redpin, whitePower);
  analogWrite(bluepin, whitePower);
  analogWrite(greenpin, whitePower);
}
