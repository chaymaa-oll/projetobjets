#include <BLEDevice.h>
#include <BLEScan.h>
#include <Wire.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Wi-Fi Configuration
#define WIFI_SSID "Galaxy S24 Ultra 463F"
#define WIFI_PASSWORD "kaoutarsoud03"

// Firebase Configuration
#define API_KEY "AIzaSyCgh9hmbPhc4yYaQrPWYO9fOzOJzvqHHDw"
#define DATABASE_URL "https://projetobjets-default-rtdb.europe-west1.firebasedatabase.app/"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// BLE objects
BLEScan* pBLEScan;
int scanTime = 3; // BLE scan time in seconds
const int MAX_DEVICES = 20; // Max devices to process

void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    Serial.println("Initialisation...");

    // Initialize BLE
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);

    Serial.println("BLE Scanner prêt !");

    // Connect to Wi-Fi
    Serial.println("Connexion au Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnecté au Wi-Fi !");
    Serial.println(WiFi.localIP());

    // Configure Firebase
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.token_status_callback = tokenStatusCallback;

    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("Authentification Firebase réussie !");
    } else {
        Serial.printf("Erreur Firebase : %s\n", config.signer.signupError.message.c_str());
    }

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // Scan BLE devices and send to Firebase
    scanAndSendToFirebase();

    // Halt the program after scan
    Serial.println("Scan terminé, programme terminé.");
    while (true) {
        delay(1000); // Stop the program
    }
}

void scanAndSendToFirebase() {
    // Verify BLEScan is initialized
    if (!pBLEScan) {
        Serial.println("Erreur : BLEScan non initialisé !");
        return;
    }

    // Scan BLE devices
    Serial.println("\nDébut du scan...");
    BLEScanResults* scanResults = pBLEScan->start(scanTime, false); // Pointeur pour les résultats
    int deviceCount = scanResults->getCount();

    // Limit the number of devices detected
    if (deviceCount > MAX_DEVICES) {
        deviceCount = MAX_DEVICES;
    }

    Serial.printf("Nombre de périphériques détectés : %d\n", deviceCount);

    // Send data to Firebase
    if (Firebase.ready()) {
        String path = "/BLEScan/LastScan";
        FirebaseJson json;
        json.set("deviceCount", deviceCount);

        FirebaseJsonArray devices;
        for (int i = 0; i < deviceCount; i++) {
            BLEAdvertisedDevice device = scanResults->getDevice(i);
            if (device.getAddress().toString().length() > 0) { // Ensure valid data
                FirebaseJson deviceJson;
                deviceJson.set("address", device.getAddress().toString().c_str());
                deviceJson.set("rssi", device.getRSSI());
                devices.add(deviceJson);
            }
        }
        json.set("devices", devices);

        // Send data to Firebase
        if (Firebase.RTDB.setJSON(&fbdo, path, &json)) {
            Serial.println("Données envoyées avec succès à Firebase !");
        } else {
            Serial.println("Erreur d'envoi des données à Firebase !");
            Serial.println(fbdo.errorReason());
        }
    } else {
        Serial.println("Firebase non prêt !");
    }

    // Release BLE resources
    pBLEScan->clearResults();
    BLEDevice::deinit();
}

void loop() {
    // Empty loop since everything is done in setup()
}
