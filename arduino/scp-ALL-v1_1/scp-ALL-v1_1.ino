// board http://arduino.esp8266.com/stable/package_esp8266com_index.json
// basé sur crypto-ticker pour la programmation du crypto-ticker https://github.com/openhardwarelabs/bitcoin-ticker
//info PWN pins esp8266 1 to 8 and 12
// plan LOLIN V3 https://store.mectronica.it/3800-large_default/wireless-iot-development-board-nodemcu-v3-lolin-wifi-esp8266-with-pcb-antenna-ch340.jpg

// Libraries SP for wifi
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
//for ultraS
#include <HCSR04.h>// https://github.com/Martinsos/arduino-lib-hc-sr04
//for arduino
#include <Servo.h>
#include <Chrono.h>

/////////const & var for crypto ticker
// WiFi settings

const char* ssid     = "RED";
const char* password = "lemot2passeestlemot2passe";

// API server
const char* host = "api.coindesk.com";

int price, oldPrice;

int howmanysecond = 5;
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
const byte servoPins[nbreServo] = {4}; // D2

int posLimitesTemp[2];
bool directionAngle[nbreServo];
float pos[nbreServo];
float nbreDegreVariation;

// RVBled
int LEDPower = 0;
int LEDPowerLimitesTemp[2];
int colorVariation;
int lightVariation;

const byte RGBpins[3] = {12, 14, 15};
const byte maxPowerLimite = 220;
const byte miniPowerLimite = 20;

byte etatNuage = 0;

Servo servo[nbreServo];
Chrono chronoTempete;
Chrono chronoTicker;

void setup() {

  // Serial
  Serial.begin(115200);
  delay(10);

  for (byte i = 0; i < nbreServo; i++) {
    servo[i].attach(servoPins[i]);
    pos[i] = startPos;
    servo[i].write(10);
    delay(100);
    servo[i].write(170);
    delay(100);
    servo[i].write(startPos);
  }


  ////RGB
  for (byte i = 0; i < 3; i++) {
    pinMode(RGBpins[i], OUTPUT);
  }

  delay(1000);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  delay (10);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1000);
//test servo 
}
void loop() {
  cryptoTicker();


  // tempeteWithChrono();
  delay(1);
  //  tempeteWithUltraS();
  delay(1);
  etatNuageUpDate();
  delay(1);
  ledRVB();
  delay(1);
  moveMode(0);
  delay(1);
  Serial.print("etat : "); Serial.println(etatNuage);
  Serial.println();
}

void cryptoTicker() {
  if (
    chronoTicker.hasPassed(3000)
   // now%howmanysecond==0
  ) {
    chronoTicker.restart();
    // Connect to API
    // Serial.print("connecting to ");
    // Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }

    // We now create a URI for the request
    String url = "/v1/bpi/currentprice.json";

    //Serial.print("Requesting URL: ");
    //Serial.println(url);

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
    //Serial.println();
    //Serial.println("closing connection");

    // Process answer
    //Serial.println();
    //Serial.println("Answer: ");
    //Serial.println(answer);

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
    //    Serial.println();
    //    Serial.println("JSON answer: ");
    //    Serial.println(jsonAnswer);
    jsonAnswer.trim();

    // Get rate as float
    int rateIndex = jsonAnswer.indexOf("rate_float");
    String priceString = jsonAnswer.substring(rateIndex + 12, rateIndex + 18);
    priceString.trim();
    price = priceString.toFloat();

    // Print price
    Serial.println();
    Serial.print("Bitcoin price: ");
    Serial.print(price);
    Serial.print(" vs oldprice :  ");
    Serial.print(oldPrice);
    if (price > oldPrice) {
      etatNuage = 1;
    } else if (price < oldPrice) {
      etatNuage = 2;
    } else {
      etatNuage = 0;
    }
  }
  oldPrice = price;
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

void tempeteWithChrono() {

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
