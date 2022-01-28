//https://www.tutorialspoint.com/arduino/arduino_multi_dimensional_arrays.htm
#include <Servo.h>

const byte startAngle = 90;
const byte nbreServo = 6;
const byte posMini = 10;
const byte posMax = 170;
const byte nbrededegreMax = 20;
const byte nbredevibrationMax = 20;

bool directionAngle[nbreServo];
int pos[nbreServo];
byte nbrededegre[nbreServo] = {1,1,1,1,1,1};
byte colere[nbreServo] = { };
Servo servo[nbreServo];

void setup() {
  servo[0].attach(3);
  servo[1].attach(5);
  servo[2].attach(6);
  servo[3].attach(9);
  servo[4].attach(10);
  servo[5].attach(11);

  Serial.begin(9600);

  for (byte i = 0; i < nbreServo; i++) {

    pos[i] = startAngle;
    servo[i].write(90);
  }
}

void loop() {
  if (random(10)==0) {
    colere[random(nbreServo)] = 1;
  }
  for (byte i = 0; i < nbreServo; i++) {
 
    moveMode(i);
    safelimites(i);
    servo[i].write(pos[i]);

    Serial.print("Servo ");
    Serial.print(i);
    Serial.print(" - var en degre = ");
    Serial.println(nbrededegre[i]);
  }
}


void moveMode(byte whatServoInfo) {
  // on change de nbrededegre
  if (pos[whatServoInfo] <= posMini or pos[whatServoInfo] >= posMax) {
    directionAngle[whatServoInfo] = !directionAngle[whatServoInfo];
  }
  if (colere[whatServoInfo] != 0) {
    nbrededegre[whatServoInfo] = nbrededegre[whatServoInfo] + random(-2, 5);
  }

  // remise au minimun la nbrededegre de tous si un servo arrive au max et coleremode off
  if ( nbrededegre[whatServoInfo] > nbrededegreMax) {
    for (byte j = 0; j < nbreServo; j++) {
      nbrededegre[j] = 1;
      colere[j] = 0;
    }
  }
}

void safelimites(byte whatServoInfo) {

  // on modifie la direction en bout de limite

  if (directionAngle[whatServoInfo]) {
    pos[whatServoInfo] = pos[whatServoInfo] + nbrededegre[whatServoInfo];
  } else {
    pos[whatServoInfo] = pos[whatServoInfo] - nbrededegre[whatServoInfo];
  }
  // on reste dans les limite d'angle
  if (pos[whatServoInfo] > posMax) {
    pos[whatServoInfo] = posMax;
  }
  if (pos[whatServoInfo] < posMini) {
    pos[whatServoInfo] = posMini;
  }
  if (nbrededegre[whatServoInfo] < 1) {
    nbrededegre[whatServoInfo] = 1;
  }
}
