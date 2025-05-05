#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "pitches.h"

// WiFi credentials
#define WIFI_SSID "TLU"
#define WIFI_PASSWORD ""

// Firebase credentials
#define API_KEY "AIzaSyCYFHx-Sq1v4dl9Ncqa4Hnq6IoaUL7IdDM"
#define DATABASE_URL "https://familybear-ab556-default-rtdb.europe-west1.firebasedatabase.app"
#define USER_EMAIL "estere12@tlu.ee"
#define USER_PASSWORD "Bear2025"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Buzzer pin
#define BUZZER_PIN 19

// LEDC constants
#define LEDC_CHANNEL 0   
#define LEDC_TIMER    // 8-bit resolution
#define LEDC_FREQ_HZ 1000             // Frequency for the PWM signal
            // LEDC channel

// Harry Potter melody
int potterMelody[] = {
  REST, NOTE_D4, NOTE_G4, NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_D5,
  NOTE_C5, NOTE_A4, NOTE_G4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_GS4,
  NOTE_D4, NOTE_D4
};

// Durations for the melody
int potterDurations[] = {
  2, 4, 4, 8, 4, 2, 4, 2, 2, 4, 8, 4, 2, 4, 1, 4
};

// WiFi connection function
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

// Setup PWM for the buzzer
void setupPWM() {
  ledcSetup(LEDC_CHANNEL, LEDC_FREQ_HZ, LEDC_TIMER);  // Set frequency and resolution
  ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL);            // Attach pin to PWM channel
}

// Song playback function
void playSong(int melody[], int durations[], int length) {
  unsigned long startTime = millis();
  for (int i = 0; i < length; i++) {
    int duration = (1000 / durations[i]);
    int pause = duration * 1.30;

    if (melody[i] != REST) {
      ledcWriteTone(BUZZER_PIN, melody[i]);  // Generate tone using PWM
    }
    delay(pause);
    ledcWriteTone(BUZZER_PIN, 0);  // Stop the tone

    if (millis() - startTime > 120000) {
      break;  // Stop after 2 minutes
    }
  }

  Serial.println("Song finished.");
}

// Firebase stream callback
void FirebaseStreamCallback(FirebaseStream data) {
  if (data.dataType() == "json") {
    FirebaseJson json = data.to<FirebaseJson>();
    FirebaseJsonData result;
    json.get(result, "pattern");

    String pattern = result.stringValue;
    Serial.println("Pattern received: " + pattern);

    if (pattern == "potter1") {
      playSong(potterMelody, potterDurations, sizeof(potterDurations) / sizeof(int));
    } else {
      Serial.println("Unknown pattern received.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  connectToWiFi();

  // Firebase setup
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  fbdo.setResponseSize(4096);
  config.timeout.serverResponse = 10000;

  if (!Firebase.RTDB.beginStream(&fbdo, "/commands/sound")) {
    Serial.println("Failed to begin Firebase stream.");
  } else {
    Serial.println("Firebase stream started.");
  }

  Firebase.RTDB.setStreamCallback(&fbdo, FirebaseStreamCallback, nullptr);

  setupPWM();  // Set up PWM for buzzer
}

void loop() {
  delay(1000);  // Callback handles everything
}

