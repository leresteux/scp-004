#include <Servo.h>
#include <Chrono.h>
#include <HCSR04.h>// https://github.com/Martinsos/arduino-lib-hc-sr04

const byte triggerPin = 13;
const byte echoPin = 12;
UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);
unsigned int distance;

const byte startPos = 90;
const byte nbreServo = 1;
const byte miniPosLimite = 10;
const byte maxPosLimite = 170;

const byte RGBpins[3] = {11, 5, 6};
const byte maxPowerLimite = 220;
const byte miniPowerLimite = 20;

int LEDPower = 0;
int LEDPowerLimitesTemp[2];
int colorVariation;
int lightVariation;

int posLimitesTemp[2];
bool directionAngle[nbreServo];
float pos[nbreServo];
float nbreDegreVariation;

byte etatNuage = 0;

Servo servo[nbreServo];
Chrono chronoTempete;


void setup() {

  Serial.begin(9600);


  float distance = distanceSensor.measureDistanceCm();
  Serial.println(distance);

  servo[0].attach(3);
  for (byte i = 0; i < 3; i++) {
    pinMode(RGBpins[i], OUTPUT);
  }
  for (byte i = 0; i < nbreServo; i++) {
    pos[i] = startPos;
    servo[i].write(90);
  }
}

void loop() {

  tempete();
  delay(1);
  tempeteWithUltraS();
  delay(1);
  etatNuageUpDate();
  delay(1);
  ledRVB();
  delay(1);
  moveMode(0);
  delay(1);
}


void moveMode(byte whatServoInfo) {

  // on change de nbreDegreVariation
  if (pos[whatServoInfo] <= posLimitesTemp[0] ) {
    directionAngle[whatServoInfo] = !directionAngle[whatServoInfo];
    pos[whatServoInfo] = posLimitesTemp[0];
  } else if ( pos[whatServoInfo] >= posLimitesTemp[1]) {
    directionAngle[whatServoInfo] = !directionAngle[whatServoInfo];
    pos[whatServoInfo] = posLimitesTemp[1];

  }
  if (directionAngle[whatServoInfo]) {
    pos[whatServoInfo] = pos[whatServoInfo] + nbreDegreVariation;
  } else {
    pos[whatServoInfo] = pos[whatServoInfo] - nbreDegreVariation;
  }
  safeServoLimites(whatServoInfo);
  servo[whatServoInfo].write(pos[whatServoInfo]);
}

void safeServoLimites(byte whatServoInfo) {

  // on reste dans les limite d'angle
  if (pos[whatServoInfo] > maxPosLimite) {
    pos[whatServoInfo] = maxPosLimite;
  }
  if (pos[whatServoInfo] < miniPosLimite) {
    pos[whatServoInfo] = miniPosLimite;
  }

}

void ledRVB() {

  switch (etatNuage) {
    case 1:
      //RBpins actives
      for (byte i = 0; i < 2; i++) {
        lightVariationMode();
        analogWrite(RGBpins[i], lightVariation );
      }
      analogWrite(RGBpins[2], random(0, 20) );
      break;
    case 2:
      //    Rpins
      lightVariationMode();
      analogWrite(RGBpins[1], lightVariation );
      analogWrite(RGBpins[0], random(0, 15) );
      analogWrite(RGBpins[2], 0 );
      break;
    default:
      //RGBpins actives
      for (byte i = 0; i < 3; i++) {
        lightVariationMode();
        analogWrite(RGBpins[i], lightVariation );
      }
      break;
  }
}

void lightVariationMode() {
  lightVariation = random(
                     (LEDPowerLimitesTemp[0] + random((0 - colorVariation / 2), colorVariation)),  //minimun dans la variation de cooleur,
                     (LEDPowerLimitesTemp[1] + random((0 - colorVariation / 2), colorVariation))); //maxi
}

void tempete() {

  switch (etatNuage) {
    case 1:
      if (chronoTempete.hasPassed(1000 * random(2, 4))) {
        chronoTempete.restart();
        if (random(10) == 0) {
          etatNuage = 2;
        } else {
          etatNuage = 0;
        }
      }
      break;
    case 2:
      if (chronoTempete.hasPassed(1000 * random(1, 2))) {
        chronoTempete.restart();
        etatNuage = 0;
      }
      break;
    default:
      if (chronoTempete.hasPassed(10000)) {
        etatNuage = random(2);
        chronoTempete.restart();
        break;
      }
  }

}


void tempeteWithUltraS() {



  distance = distanceSensor.measureDistanceCm();


  if (distance < 15) {
    etatNuage = 2;
  } else if (distance < 30) {
    etatNuage = 1;
  } else {
    etatNuage = 0;
  }

}
void etatNuageUpDate() {
  switch (etatNuage) {
    case 1:
      LEDPowerLimitesTemp[0] = 100;
      LEDPowerLimitesTemp[1] = 150;
      posLimitesTemp[0] = startPos - 40;
      posLimitesTemp[1] = startPos + 40;
      nbreDegreVariation = random(1, 3);
      colorVariation = 50;
      break;
    case 2:
      LEDPowerLimitesTemp[0] = 200;
      LEDPowerLimitesTemp[1] = maxPowerLimite;
      posLimitesTemp[0] = startPos - 20;
      posLimitesTemp[1] = startPos + 20;
      nbreDegreVariation = 3 + random(3);
      colorVariation = 100;
      break;
    default:
      LEDPowerLimitesTemp[0] = miniPowerLimite;
      LEDPowerLimitesTemp[1] = miniPowerLimite + 2;
      posLimitesTemp[0] = startPos - 50;
      posLimitesTemp[1] = startPos + 50;
      float R = random(2,4);
      nbreDegreVariation = R/10;
      colorVariation = 0;
      break;
  }

}
