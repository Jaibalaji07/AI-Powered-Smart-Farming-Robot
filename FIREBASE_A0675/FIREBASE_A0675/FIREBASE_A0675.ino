// WORKING CODE 
#include <Arduino.h>

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>
#include <SoftwareSerial.h>

SoftwareSerial ss(D3, D4);

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// -------- WiFi Credentials --------
#define WIFI_SSID "A0675"
#define WIFI_PASSWORD "12345678"

// -------- Firebase Credentials --------
#define API_KEY "AIzaSyBhtR_iqEQxVsw6xC6dL7RiRmLrmD61QBY"
#define DATABASE_URL "https://fir-d280e-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup() {

  Serial.begin(9600);
  ss.begin(9600);

  connectToWiFi();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signup successful");
    signupOK = true;
  } else {
    Serial.printf("Firebase signup error: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {

  if (Firebase.ready() && signupOK) {

    if (Firebase.RTDB.getString(&fbdo, "/Monitoring/id")) {

      if (fbdo.dataType() == "string") {

        String firebaseValue = fbdo.stringData();
        Serial.println("Firebase data: " + firebaseValue);

        if (firebaseValue == "A") {

          Serial.println("A received from Firebase");
          ss.write('A');   

          delay(4000);    
          // Change value to X
          if (Firebase.RTDB.setString(&fbdo, "/Monitoring/id", "X")) {
            Serial.println("Firebase updated to X");
          } else {
            Serial.println("Update failed:");
            Serial.println(fbdo.errorReason());
          }
        }
      }
    } else {
      Serial.println("Read failed:");
      Serial.println(fbdo.errorReason());
    }
  }

  delay(2000);  // Small delay to avoid too many requests
}

void connectToWiFi() {

  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED) {

    if (millis() - startTime > 15000) {
      Serial.println("\nFailed to connect to WiFi");
      return;
    }

    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
}
