//https://www.tutorialspoint.com/arduino/arduino_multi_dimensional_arrays.htm
#include <Servo.h>

byte startAngle = 90;
bool directionAngle[4];
int pos[4];

Servo servo[4];

void setup() {
  
  for (byte i = 0; i < 4; i++) {
    servo[i].attach(i);
    pos[i] = startAngle;
    servo[i].write(pos[i]);
  }
}

void loop() {
  for (byte i = 0; i < 4; i++) {
    move(i);
  }
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
