https://github.com/acrobotic/Ai_Demos_ESP8266/blob/master/crypto_ticker/simple_btc_coinmarketcap/simple_btc_coinmarketcap.ino


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "RED";
const char* password = "lemot2passeestlemot2passe";

const char* site = "http://jsonplaceholder.typicode.com/users/1";

String texte; // Variable qui enregistrera le texte reçu

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Démmarrage");

  // On commence par se connecter au réseau WiFi
    
  Serial.print("Connexion au réseau WiFi ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connexion au WiFi réussie !");
  Serial.print("Adresse IP locale de l'ESP : ");
  Serial.println(WiFi.localIP());
}
void loop() {
  // Conexion au serveur et envoi de la requète
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(site, httpPort)) {
      Serial.println("Connection au serveur echouée.");
      return;
    }
    // Maintenant on envoie la requete au serveur
    client.print(String("GET https://www.google.com/finance/quote/BTC-USD"));
    client.println(String("HTTP/1.1"));
    client.print(String("Host: "));
    client.println(String(site));
    client.println(String("Connection: close"));
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 1000) {  // Si timeout
        Serial.println("Timeout du client !");
        Serial.println("");
        client.stop();
        return;
      }
    }

    // On lit les lignes que le serveur nous envoie
    while(client.available()) {
      texte += client.readStringUntil('\r');
    }

    Serial.println("");
    Serial.println("Connection fermée.");
    Serial.println("");
}
