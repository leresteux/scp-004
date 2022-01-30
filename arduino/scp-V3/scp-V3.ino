#include <Servo.h>


const byte startAngle = 90;
const byte nbreServo = 1;
const byte miniPos = 10;
const byte maxPos = 170;
const byte nbrededegreMax = 20;
const byte nbredevibrationMax = 20;

const byte redpin = 5;
const byte bluepin = 6;
const byte greenpin = 11;
const byte maxPower = 220;
const byte miniPower = 0;

int whitePower = 0;
int poslinkedwhite=whitePower;

bool directionAngle[nbreServo];
byte pos[nbreServo];
byte nbrededegre[nbreServo] = {};

Servo servo[nbreServo];

void setup() {
  servo[0].attach(3);

  pinMode(redpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  pinMode(greenpin, OUTPUT);

 
  Serial.begin(9600);

  for (byte i = 0; i < nbreServo; i++) {

    pos[i] = startAngle;
    servo[i].write(90);
  }
}

void loop() {

  ledRVB();

  nbrededegre[0] = map (whitePower,miniPower,maxPower,0, nbrededegreMax);


    moveMode(0);
    safelimites(0);
    

    Serial.print("nbrededegre ");
    Serial.println(nbrededegre[0]);
 
}


void moveMode(byte whatServoInfo) {
  // on change de nbrededegre
  if (pos[whatServoInfo] <= miniPos or pos[whatServoInfo] >= maxPos) {
    directionAngle[whatServoInfo] = !directionAngle[whatServoInfo];
  }

  // remise au minimun la nbrededegre de tous si un servo arrive au max et coleremode off
  if ( nbrededegre[whatServoInfo] > nbrededegreMax) {
    for (byte j = 0; j < nbreServo; j++) {
      nbrededegre[j] = 1;
    }
  }


  if (directionAngle[whatServoInfo]) {
    pos[whatServoInfo] = pos[whatServoInfo] + nbrededegre[whatServoInfo];
  } else {
    pos[whatServoInfo] = pos[whatServoInfo] - nbrededegre[whatServoInfo];
  }
  
  safelimites(whatServoInfo);
  servo[whatServoInfo].write(pos[whatServoInfo]);
}

void safelimites(byte whatServoInfo) {

  // on modifie la direction en bout de limite

  // on reste dans les limite d'angle
  if (pos[whatServoInfo] > maxPos) {
    pos[whatServoInfo] = maxPos;
  }
  if (pos[whatServoInfo] < miniPos) {
    pos[whatServoInfo] = miniPos;
  }
  if (nbrededegre[whatServoInfo] < 1) {
    nbrededegre[whatServoInfo] = 1;
  }

}

void ledRVB() {

  whitePower = whitePower +  random(-2,3);
  if (whitePower < miniPower) {
    whitePower = miniPower;
  }
  if (whitePower > maxPower) {
    whitePower = maxPower;
  }
  analogWrite(redpin, whitePower);
  analogWrite(bluepin, whitePower);
  analogWrite(greenpin, whitePower);

  Serial.print("whitePower ");
  Serial.println( whitePower);
}
