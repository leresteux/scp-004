// basé sur crypto-ticker pour la programmation du crypto-ticker https://github.com/openhardwarelabs/bitcoin-ticker
//info PWN pins esp8266 1 to 8 and 12
// Libraries SP for ESp8266
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>// version 5
#include <Wire.h>
//for ultraS
#include <HCSR04.h>// https://github.com/Martinsos/arduino-lib-hc-sr04
//for arduino
#include <Servo.h>
#include <Chrono.h>

/////////const & var for crypto ticker
// WiFi settings
const char* ssid     = "juju";
const char* password = "superjuju";
// API server
const char* host = "api.coindesk.com";
//price
int price, oldprice;

/////////const & var for UltraS.
const byte triggerPin = 13; //<to change
const byte echoPin = 12; //<to change
UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);
unsigned int distance;

/////////const & var for servo RGBled and others.
// servo

const byte startPos = 90;
const byte nbreServo = 1;
const byte miniPosLimite = 10;
const byte maxPosLimite = 170;
const byte servoPins[] = {3}; //<to change

int posLimitesTemp[2];
bool directionAngle[nbreServo];
float pos[nbreServo];
float nbreDegreVariation;

// RVBled
int LEDPower = 0;
int LEDPowerLimitesTemp[2];
int colorVariation;
int lightVariation;

const byte RGBpins[3] = {11, 5, 6}; //<to change
const byte maxPowerLimite = 220;
const byte miniPowerLimite = 20;

byte etatNuage = 0;

Servo servo[nbreServo];
Chrono chronoTempete;

void setup() {

  // Serial
  Serial.begin(115200);
  delay(10);
  ////wifi
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  ////UltraS.
  float distance = distanceSensor.measureDistanceCm();
  Serial.println(distance);
  
  ////servo
  for (byte i=0; i<nbreServo;i++){
  servo[i].attach(servoPins[i]);
  }
  ////RGB
  for (byte i = 0; i < 3; i++) {
    pinMode(RGBpins[i], OUTPUT);
  }
  for (byte i = 0; i < nbreServo; i++) {
    pos[i] = startPos;
    servo[i].write(90);
  }
}

void loop() {

  cryptoTicker();
  delay(1);
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

void cryptoTicker() { // Connect to API
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/v1/bpi/currentprice.json";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(100);

  // Read all the lines of the reply from server and print them to Serial
  String answer;
  while (client.available()) {
    String line = client.readStringUntil('\r');
    answer += line;
  }

  client.stop();
  Serial.println();
  Serial.println("closing connection");

  // Process answer
  Serial.println();
  Serial.println("Answer: ");
  Serial.println(answer);

  // Convert to JSON
  String jsonAnswer;
  int jsonIndex;

  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == '{') {
      jsonIndex = i;
      break;
    }
  }

  // Get JSON data
  jsonAnswer = answer.substring(jsonIndex);
  Serial.println();
  Serial.println("JSON answer: ");
  Serial.println(jsonAnswer);
  jsonAnswer.trim();

  // Get rate as float
  int rateIndex = jsonAnswer.indexOf("rate_float");
  String priceString = jsonAnswer.substring(rateIndex + 12, rateIndex + 18);
  priceString.trim();
  price = priceString.toFloat();
  Serial.println();
  Serial.println("Bitcoin price: ");
  Serial.print(price);
  Serial.print("S VS ");
  Serial.print(oldprice);
  Serial.println("S 10sec before");
  // Print price

  if (price < oldprice) {
    Serial.println(":-( <( OH NO!)");
  } else if  (price > oldprice) {
    Serial.println(":-) <( EH! EH!) ");
  } else {
    Serial.println(":-o <(nothink to say) ");
  }
  oldprice = price;
  delay(10000);
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
  } else if (distance < 40) {
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
      float R = random(2, 4);
      nbreDegreVariation = R / 10;
      colorVariation = 0;
      break;
  }

}
