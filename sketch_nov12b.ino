#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Définir le pin de la LED
#define LED_PIN 2

// Déclaration de la classe pour gérer les événements du serveur BLE
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Un appareil s'est connecté !");
    digitalWrite(LED_PIN, HIGH); // Allumer la LED quand quelqu'un se connecte
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Un appareil s'est déconnecté !");
    digitalWrite(LED_PIN, LOW); // Éteindre la LED quand la connexion est coupée
  }
};

void setup() {
  // Initialisation de la LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Assurez-vous que la LED est éteinte au démarrage

  // Initialisation du Moniteur Série
  Serial.begin(115200);
  Serial.println("Initialisation de l'ESP32 comme beacon BLE...");

  // Initialisation du périphérique BLE
  BLEDevice::init("ESP32 Beacon Test");

  // Création du serveur BLE
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks()); // Définir les callbacks pour gérer les connexions

  // Création de la publicité BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  BLEAdvertisementData oAdvertisementData;
  String beaconData = "ESP32Beacon123"; // Données du beacon
  oAdvertisementData.setManufacturerData(beaconData);
  pAdvertising->setAdvertisementData(oAdvertisementData);

  // Démarrer la publicité BLE
  pAdvertising->start();
  Serial.println("Beacon BLE démarré et en cours de diffusion !");
}

void loop() {
  // Affiche un message toutes les 5 secondes pour confirmer que le code est actif
  Serial.println("Beacon BLE en cours de diffusion...");
  delay(5000);
}