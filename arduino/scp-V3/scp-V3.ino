#include <Servo.h>
#include <Chrono.h>

const byte startPos = 90;
const byte nbreServo = 1;
const byte miniPosLimite = 10;
const byte maxPosLimite = 170;


const byte RGBpins[3] = {5, 6, 11};
const byte maxPowerLimite = 220;
const byte miniPowerLimite = 20;

int whitePower = 0;
int whitePowerLimitesTemp[2];
int colorVariation;

int posLimitesTemp[2];

bool directionAngle[nbreServo];
float pos[nbreServo];
float nbreDegreVariation;
bool tempeteWIP;
byte etatNuage = 0;

Servo servo[nbreServo];
Chrono chronoTempete;


void setup() {

  Serial.begin(9600);

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
  ledRVB();
  moveMode(0);

  Serial.print("nbreDegreVariation ");
  Serial.println(nbreDegreVariation);
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
      break;
    case 2:
    Rpins
      lightVariationMode();
      analogWrite(RGBpins[2], lightVariation );

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
  int lightVariation = random(
                         (whitePowerLimitesTemp[0] + random((0 - colorVariation / 2), colorVariation)),  //minimun dans la variation de cooleur,
                         (whitePowerLimitesTemp[1] + random((0 - colorVariation / 2), colorVariation))); //maxi
}

void tempete() {

  switch (etatNuage) {
    case 1:
      if (chronoTempete.hasPassed(1000 * random(3, 4))) {
        chronoTempete.restart();
        if (random(10)) {
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
  switch (etatNuage) {
    case 1:
      whitePowerLimitesTemp[0] = 100;
      whitePowerLimitesTemp[1] = 150;
      posLimitesTemp[0] = startPos - 4 0;
      posLimitesTemp[1] = startPos + 40;
      nbreDegreVariation = 3;
      colorVariation = 50;
      break;
    case 2:
      whitePowerLimitesTemp[0] = 200;
      whitePowerLimitesTemp[1] = maxPowerLimite;
      posLimitesTemp[0] = startPos - 20;
      posLimitesTemp[1] = startPos + 20;
      nbreDegreVariation = 5 + random(6);
      colorVariation = 100;
      break;
    default:
      whitePowerLimitesTemp[0] = miniPowerLimite;
      whitePowerLimitesTemp[1] = miniPowerLimite + 2;
      posLimitesTemp[0] = startPos - 50;
      posLimitesTemp[1] = startPos + 50;
      nbreDegreVariation = 1;
      colorVariation = 0;
      break;
  }
}
