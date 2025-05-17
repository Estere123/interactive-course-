#include <Firebase_ESP_Client.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include "pitches.h"
#include <ArduinoJson.h>

// WiFi credentials
#define WIFI_SSID "TLU"
#define WIFI_PASSWORD ""

// Firebase credentials for RTDB
#define API_KEY "AIzaSyCYFHx-Sq1v4dl9Ncqa4Hnq6IoaUL7IdDM"
#define DATABASE_URL "https://familybear-ab556-default-rtdb.europe-west1.firebasedatabase.app"
#define USER_EMAIL "estere12@tlu.ee"
#define USER_PASSWORD "Bear2025"

// Firebase path for sound commands (using Firebase ESP Client)
#define FIREBASE_PATH "/commands/sound"

// Pins
#define FSR_PIN 5
#define BUZZER_PIN 19

// Threshold for picking up bear
const int pickupThreshold = 20;

// Time settings
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 3600
#define DAYLIGHT_OFFSET_SEC 3600

#define REST 0  // Define REST note

// Firebase objects for RTDB
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
long long lastPlayedTimestamp = 0;



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


// Function declarations
void connectToWiFi();
void sendHeartbeat();
long long getRealTimestamp();
void sendFirebaseCommand(String pattern);
void playSong(long melody[], long durations[], int length, unsigned long playMillis);
void readFirebaseCommand();

void setup() {
  Serial.begin(115200);
  pinMode(FSR_PIN, INPUT);
  connectToWiFi();

  // Sync time
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  while (time(nullptr) < 100000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n🕒 Time synced");

  // Setup Firebase Realtime DB
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Read FSR sensor for bear status
  int fsrValue = analogRead(FSR_PIN);
  bool pickedUp = fsrValue > pickupThreshold;
  Serial.printf("FSR Value: %d | Picked up: %s\n", fsrValue, pickedUp ? "true" : "false");

  // Update Firebase with bear status and timestamp heartbeat
  if (Firebase.ready()) {
    if (!Firebase.RTDB.setBool(&fbdo, "/status/bear/fsr", pickedUp)) {
      Serial.print("❌ FSR update failed: ");
      Serial.println(fbdo.errorReason());
    } else {
      Serial.println("✅ FSR state sent.");
    }
    sendHeartbeat();
  }

  // Check if FSR is pressed for 10 seconds to trigger playing music
  static unsigned long pressedStart = 0;
  if (pickedUp) {
    if (pressedStart == 0) pressedStart = millis();
    else if (millis() - pressedStart >= 10000) {
      Serial.println("⏱️ Force sensor pressed for > 10 seconds!");

      // Read Firebase pattern and play song for 2 minutes (120000 ms)
      readFirebaseCommand();

      // Reset pressedStart to avoid repeated triggers
      pressedStart = 0;
    }
  } else {
    pressedStart = 0;
  }

  delay(1000);
}

// Send lastSeen timestamp to Firebase
void sendHeartbeat() {
  if (Firebase.ready()) {
    time_t now = time(nullptr);
    if (!Firebase.RTDB.setInt(&fbdo, "/status/bear/lastSeen", now)) {
      Serial.print("❌ Failed to send heartbeat: ");
      Serial.println(fbdo.errorReason());
    } else {
      Serial.println("🫀 Heartbeat (lastSeen) sent.");
    }
  }
}

// Get current timestamp in milliseconds
long long getRealTimestamp() {
  time_t now = time(nullptr);
  return now > 0 ? (long long)now * 1000 : 0;
}

// Play song based on pattern arrays — now loops for playMillis ms
void playSong(long melody[], long durations[], int length, unsigned long playMillis) {
  Serial.println("🎵 Playing song...");
  unsigned long startTime = millis();
  int i = 0;

  while (millis() - startTime < playMillis) {
    int noteDuration = 1000 / durations[i];
    int pauseBetweenNotes = noteDuration * 1.30;

    if (melody[i] != REST) tone(BUZZER_PIN, melody[i], noteDuration);
    delay(pauseBetweenNotes);
    noTone(BUZZER_PIN);

    i++;
    if (i >= length) i = 0;  // Loop the melody
  }
  Serial.println("✅ Song finished.");
}

// Read Firebase command and play corresponding song using Firebase ESP Client library
void readFirebaseCommand() {
  if (!Firebase.ready()) {
    Serial.println("⚠️ Firebase not ready!");
    return;
  }

  if (!Firebase.RTDB.getJSON(&fbdo, FIREBASE_PATH)) {
    Serial.print("❌ Failed to read command: ");
    Serial.println(fbdo.errorReason());
    return;
  }

  String payload = fbdo.jsonString();
  Serial.println("📥 Firebase Response: " + payload);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print("❌ JSON parse failed: ");
    Serial.println(error.c_str());
    return;
  }

  String songPattern = doc["pattern"].as<String>();

  int length;
  if (songPattern == "potter1") {
    length = sizeof(durations1) / sizeof(durations1[0]);
    playSong(melody1, durations1, length, 120000);  // 2 minutes
  } else if (songPattern == "merryxmas") {
    length = sizeof(durations2) / sizeof(durations2[0]);
    playSong(melody2, durations2, length, 120000);
  } else if (songPattern == "pinkpanther") {
    length = sizeof(durations3) / sizeof(durations3[0]);
    playSong(melody3, durations3, length, 120000);
  } else {
    Serial.println("❌ No recognized song pattern from Firebase");
  }
}

// Connect to WiFi
void connectToWiFi() {
  Serial.printf("Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}
