#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "pitches.h"
#include <time.h>

// WiFi credentials
#define WIFI_SSID "TLU"
#define WIFI_PASSWORD ""  // Add your WiFi password here

// Firebase host and path
#define FIREBASE_HOST "familybear-ab556-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_PATH "/commands/sound.json"

// Buzzer pin
#define BUZZER_PIN 19

// Track last played timestamp
long long lastPlayedTimestamp = 0;

// Melody and durations for the first song (Potter theme)
long melody1[] = {
  REST, NOTE_D4, NOTE_G4, NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_D5,
  NOTE_C5, NOTE_A4, NOTE_G4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_GS4,
  NOTE_D4, NOTE_D4, NOTE_G4, NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_D5,
  NOTE_F5, NOTE_E5, NOTE_DS5, NOTE_B4, NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_B4, NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_AS4, NOTE_D5,
  NOTE_AS4, NOTE_DS5, NOTE_D5, NOTE_CS5, NOTE_A4, NOTE_AS4, NOTE_D5,
  NOTE_CS5, NOTE_CS4, NOTE_D4, NOTE_D5, REST, NOTE_AS4, NOTE_D5,
  NOTE_AS4, NOTE_D5, NOTE_AS4, NOTE_F5, NOTE_E5, NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5, NOTE_CS4, NOTE_AS4, NOTE_G4
};

long durations1[] = {
  2, 4, 4, 8, 4, 2, 4, 2, 2, 4, 8, 4, 2, 4, 1, 4, 4, 8, 4, 2,
  4, 2, 4, 2, 4, 4, 8, 4, 2, 4, 1, 4, 2, 4, 2, 4, 2, 4, 2, 4,
  4, 8, 4, 2, 4, 1, 4, 2, 4, 2, 4, 2, 4, 2, 4, 4, 8, 4, 2, 4,
  1
};

// Melody and durations for the second song (Merry Christmas)
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

// Melody and durations for the third song (Pink Panther theme)
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
  String payload = "{\"pattern\":\"" + pattern + "\",\"timestamp\":" + String(timestamp) + "}";

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

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  connectToWiFi();

  // Estonia (UTC+3) NTP time
  configTime(3 * 3600, 0, "pool.ntp.org");

  delay(2000); // Let time sync

  // Read the command from Firebase (here we simulate it)
  // The song trigger will be received from the mobile app, so we do not hardcode song patterns anymore.

  // Get song pattern from Firebase (normally, this will be based on the app input)
  // In real use, you will receive the songPattern via Firebase, and it will decide which song to play.
  String songPattern = "potter1";  // This will be dynamically updated based on Firebase commands
  sendFirebaseCommand(songPattern); // Send the song trigger to Firebase

  // Play the appropriate song based on the pattern received
  if (songPattern == "potter1") {
    playSong(melody1, durations1, sizeof(durations1) / sizeof(long));
  } else if (songPattern == "merryxmas") {
    playSong(melody2, durations2, sizeof(durations2) / sizeof(long)); // Play "Merry Christmas"
  } else if (songPattern == "pinkpanther") {
    playSong(melody3, durations3, sizeof(durations3) / sizeof(long)); // Play "Pink Panther"
  }
}

void loop() {
  // Nothing needed, as the song is triggered once
}
