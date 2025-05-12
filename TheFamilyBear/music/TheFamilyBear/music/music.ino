#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "pitches.h"
#include <time.h>
#include <ArduinoJson.h>
#include "ForceSensor.h"  // Include ForceSensor header

// WiFi credentials
#define WIFI_SSID "TLU"
#define WIFI_PASSWORD ""  // Add your WiFi password here

// Firebase host and path
#define FIREBASE_HOST "familybear-ab556-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_PATH "/commands/sound.json"

// Buzzer pin
#define BUZZER_PIN 19

// Force sensor pin
#define FSR_PIN 5  // Set the force sensor pin to 5

// Track last played timestamp
long long lastPlayedTimestamp = 0;

// Force Sensor instance
ForceSensor forceSensor(FSR_PIN); // Instantiate the force sensor object with pin 5

// Full Melody and durations for different songs

// Full "Harry Potter" theme (Potter1)
long melody1[] = {
 REST, NOTE_D4,
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_C5, 
  NOTE_A4,
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_F4, NOTE_GS4,
  NOTE_D4, 
  NOTE_D4,
  
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_F5, NOTE_E5,
  NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_B4,
  NOTE_G4,
  NOTE_AS4,
   
  NOTE_D5, NOTE_AS4,
  NOTE_D5, NOTE_AS4,
  NOTE_DS5, NOTE_D5,
  NOTE_CS5, NOTE_A4,
  NOTE_AS4, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_D4,
  NOTE_D5, 
  REST, NOTE_AS4,  
  
  NOTE_D5, NOTE_AS4,
  NOTE_D5, NOTE_AS4,
  NOTE_F5, NOTE_E5,
  NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_AS4,
  NOTE_G4
};
long durations1[] = {
    2, 4,
  4, 8, 4,
  2, 4,
  2, 
  2,
  4, 8, 4,
  2, 4,
  1, 
  4,
  
  4, 8, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1,
  4,
   
  2, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1, 
  4, 4,  
  
  2, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1
};

// Full "Merry Christmas" song
long melody2[] = {
   NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
  NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5
};
long durations2[] = {
    8, 8, 4,
  8, 8, 4,
  8, 8, 8, 8,
  2,
  8, 8, 8, 8,
  8, 8, 8, 16, 16,
  8, 8, 8, 8,
  4, 4
};

// Full "Pink Panther" theme
long melody3[] = {
  REST, REST, REST, NOTE_DS4, 
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_E4, NOTE_G4, NOTE_B4,   
  NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, 
  NOTE_E4, REST, REST, NOTE_DS4,
  
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_G4, NOTE_B4, NOTE_E5,
  NOTE_DS5,   
  NOTE_D5, REST, REST, NOTE_DS4, 
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_E4, NOTE_G4, NOTE_B4,   
  
  NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, 
  NOTE_E4, REST,
  REST, NOTE_E5, NOTE_D5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4,
  NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4,   
  NOTE_G4, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4
};
long durations3[] = {
    2, 4, 8, 8, 
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,   
  2, 16, 16, 16, 16, 
  2, 4, 8, 4,
  
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,
  1,   
  2, 4, 8, 8, 
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,   
  
  2, 16, 16, 16, 16, 
  4, 4,
  4, 8, 8, 8, 8, 8, 8,
  16, 8, 16, 8, 16, 8, 16, 8,   
  16, 16, 16, 16, 16, 2
};

// WiFi connection
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("...");
  }
  Serial.println("\n✅ WiFi connected");
  Serial.println("IP: " + WiFi.localIP().toString());
}

// Get current timestamp in milliseconds
long long getRealTimestamp() {
  time_t now = time(nullptr);
  return now > 0 ? (long long)now * 1000 : 0;
}

// Send song pattern and timestamp to Firebase
void sendFirebaseCommand(String pattern) {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();  // ⚠️ Accept all certificates (for testing)

  HTTPClient http;
  String url = "https://" + String(FIREBASE_HOST) + FIREBASE_PATH;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  long long timestamp = getRealTimestamp();
  String payload = "{\"pattern\":\"" + pattern + "\",\"timestamp\":" + String(timestamp) + "}" ;

  Serial.println("📤 Sending to Firebase: " + payload);
  int httpCode = http.PUT(payload);

  if (httpCode > 0) {
    Serial.println("✅ Command sent. HTTP code: " + String(httpCode));
    Serial.println("Response: " + http.getString());
  } else {
    Serial.println("❌ Failed to send command: " + http.errorToString(httpCode));
  }

  http.end();
}

// Play song based on the pattern
void playSong(long melody[], long durations[], int length) {
  Serial.println("🎵 Playing song...");
  unsigned long startTime = millis();
  const unsigned long maxDuration = 120000; // 2 minutes

  while (millis() - startTime < maxDuration) {
    for (int i = 0; i < length; i++) {
      unsigned long elapsed = millis() - startTime;
      if (elapsed >= maxDuration) break;

      int noteDuration = 1000 / durations[i];
      int pause = noteDuration * 1.30;

      if (melody[i] != REST) tone(BUZZER_PIN, melody[i], noteDuration);
      delay(pause);
      noTone(BUZZER_PIN);
    }
  }

  Serial.println("✅ Song finished.");
}

// Read song pattern and timestamp from Firebase
void readFirebaseCommand() {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();  // ⚠️ Accept all certificates (for testing)

  HTTPClient http;
  String url = "https://" + String(FIREBASE_HOST) + FIREBASE_PATH;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("📥 Firebase Response: " + payload);

    // Parse JSON response
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    String songPattern = doc["pattern"];  // Get the pattern from Firebase

    // Check the song pattern received from Firebase
    if (songPattern == "potter1") {
      playSong(melody1, durations1, sizeof(durations1) / sizeof(long)); // Play "Harry Potter"
    } else if (songPattern == "merryxmas") {
      playSong(melody2, durations2, sizeof(durations2) / sizeof(long)); // Play "Merry Christmas"
    } else if (songPattern == "pinkpanther") {
      playSong(melody3, durations3, sizeof(durations3) / sizeof(long)); // Play "Pink Panther"
    } else {
      Serial.println("❌ No song pattern received from Firebase");
    }
  } else {
    Serial.println("❌ Failed to get data from Firebase.");
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  forceSensor.begin();
}

void loop() {
  // Print WiFi status
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi not connected!");
  } else {
    Serial.println("✅ WiFi is connected.");
  }

  // Read and display raw sensor value
  int sensorValue = analogRead(FSR_PIN);
  Serial.print("🔍 Force sensor raw value: ");
  Serial.println(sensorValue);

  // Check if pressed long enough
  if (forceSensor.isSqueezedForDuration(10000)) {
    Serial.println("⏱️ Force sensor pressed for > 10 seconds!");

    // Check timestamp logic
    long long currentTime = getRealTimestamp();
    Serial.print("🕒 Current timestamp (ms): ");
    Serial.println(currentTime);

    // Read Firebase pattern and play song
    readFirebaseCommand();
  }

  delay(1000);  // Check every second here
}

