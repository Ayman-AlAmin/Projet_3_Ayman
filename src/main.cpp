#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Information de connexion WiFi
const char* ssid = "IOT-6220";
const char* password = "6220@Maisonneuve"; // PAS SUR A REVOIR!!!!!!!!!!!!!!

// API auquel il faut se connecter
const char* apiURL = "http://api.open-notify.org/iss-now.json";




void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Connect to WiFi
  Serial.println();
  Serial.print("Connexion au WiFi: ");
  Serial.println(ssid);
  
  // Début de la connexion au WiFi de l'école
  WiFi.begin(ssid, password);

  // Attente de connection avec timeout simple
  unsigned long startAttemptTime = millis();
  const unsigned long timeoutMs = 15000; // 15 seconds max

  while (WiFi.status() != WL_CONNECTED && (millis() - startAttemptTime) < timeoutMs) {
    delay(500);
    Serial.print(".");
  }

    // Affichage de l'IP quand connexion réussite 
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Erreur: impossible de se connecter au WiFi. Verifiez le reseau ou le mot de passe.");
    return;
  }

  // Affichage de l'IP quand connexion réussite 
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi deconnecte, tentative de reconnexion...");
    WiFi.reconnect();
    delay(2000);
    return;
  }

  HTTPClient http;
  http.begin(apiURL);
  int httpCode = http.GET();

  Serial.print("Code HTTP: ");
  Serial.println(httpCode);

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err) {
      Serial.print("Erreur JSON: ");
      Serial.println(err.c_str());
    } else {
      const char* latitude = doc["iss_position"]["latitude"];
      const char* longitude = doc["iss_position"]["longitude"];
      Serial.print("ISS position -> Lat: ");
      Serial.print(latitude);
      Serial.print(" | Lon: ");
      Serial.println(longitude);
    }
  } else if (httpCode >= 400 && httpCode < 600) {
    Serial.println("Erreur HTTP: code indique une erreur cote client ou serveur.");
  } else if (httpCode < 0) {
    Serial.println("Erreur HTTP: requete non envoyee (verifier la connexion).");
  } else {
    Serial.println("Reponse HTTP inattendue.");
  }

  http.end();
  delay(5000); // pause avant la prochaine requete
}

