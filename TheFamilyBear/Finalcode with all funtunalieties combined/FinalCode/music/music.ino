#include <Firebase_ESP_Client.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include "pitches.h"
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <math.h> // for cos(), radians()


// WiFi credentials
#define WIFI_SSID "TLU"
#define WIFI_PASSWORD ""

// Firebase credentials
#define API_KEY "AIzaSyCYFHx-Sq1v4dl9Ncqa4Hnq6IoaUL7IdDM"
#define DATABASE_URL "https://familybear-ab556-default-rtdb.europe-west1.firebasedatabase.app"
#define USER_EMAIL "estere12@tlu.ee"
#define USER_PASSWORD "Bear2025"

// Firebase paths
#define FIREBASE_PATH "/commands/sound"
#define FIREBASE_URL "https://familybear-ab556-default-rtdb.europe-west1.firebasedatabase.app/status/gps.json"

// Pins
#define FSR_PIN 5
#define BUZZER_PIN 19
#define motorPin 2

#define GPS_RX 6  // GPS TX --> ESP32 RX1 (GPIO6)
#define GPS_TX 7  // GPS RX --> ESP32 TX1 (GPIO7)




// Simulation control
bool simulateGPS = false;
bool simulateInside = false;

// Threshold
const int pickupThreshold = 20;

// Motor settings
const int cycleDuration = 8000;
const unsigned long maxRunTime = 180000;

// Heartbeat tracking
unsigned long lastHeartbeatSent = 0;
unsigned long lastHeartbeatTime = 0;



//Mix and Max freequencies for wakeup mode cycles
const int frequencyRanges[10][2] = {
  {20, 20}, {20, 40}, {40, 80}, {60, 80}, {80, 100},
  {100, 120}, {120, 140}, {140, 160}, {160, 180}, {180, 200}
};

//Needed to acuratly reflect vibration status on the app 
bool wakeupEnabled = false;
String scheduledTime = "";
String lastCheckedMinute = "";

// defining estonian summer timezone
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 7200
#define DAYLIGHT_OFFSET_SEC 3600
#define REST 0

//  objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
TinyGPSPlus gps;
HardwareSerial SerialGPS(1);


// Safe zone center
const double SAFE_LAT = 59.438940;
const double SAFE_LNG = 24.772575;
const double SAFE_RADIUS_METERS = 50.0;



// Global flag to stop vibration from serial command
bool stopRequested = false;
bool vibrationState = false;

long long lastPlayedTimestamp = 0;

// ==== GPS OFFSET ====
double meters_per_degree_lon = 111320 * cos(radians(SAFE_LAT));
double offset_lon = 55.0 / meters_per_degree_lon;

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
void updateVibrationStatus(bool isActive);
void fetchWakeupData();
void readFirebaseCommand();
void runMotorSequence();
void playSong(long melody[], long durations[], int length, unsigned long playMillis);
String getCurrentTimeEstonia();
long long getRealTimestamp();
void fetchFirebaseHeartbeat();
void checkSerialCommand();
void handleGPS();
void sendToFirebaseGPS(String payload);

//Commands to simulate from terminal 
void checkSerialCommand() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.equalsIgnoreCase("stop")) {
      stopRequested = true;
      Serial.println("Stop command received.");
    } else if (command.equalsIgnoreCase("start")) {
      stopRequested = false;
      Serial.println("Start command received.");
    } else if (command.equalsIgnoreCase("inside")) {
      simulateGPS = true;
      simulateInside = true;
      Serial.println("✅ Simulation: INSIDE safe zone");
    } else if (command.equalsIgnoreCase("outside")) {
      simulateGPS = true;
      simulateInside = false;
      Serial.println("✅ Simulation: OUTSIDE safe zone");
    } else if (command.equalsIgnoreCase("real")) {
      simulateGPS = false;
      Serial.println("✅ Switched to REAL GPS data");
    } else {
      Serial.println("❓ Unknown command");
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(FSR_PIN, INPUT);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);
   Serial.println("Type 'stop' or 'start' in Serial Monitor.");
  connectToWiFi();

  // Sync time
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  while (time(nullptr) < 100000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n🕒 Time synced");

  // Settingup Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  updateVibrationStatus(false);  // Initial vibration state off
}

void loop() {
  if (!Firebase.ready()) {
    Serial.println("Firebase not ready...");
    delay(5000);
    return;
  }
     checkSerialCommand();
  // FSR logic
  int fsrValue = analogRead(FSR_PIN);
  bool pickedUp = fsrValue > pickupThreshold;
  Serial.printf("FSR Value: %d | Picked up: %s\n", fsrValue, pickedUp ? "true" : "false");

  // Firebase update
  if (Firebase.RTDB.setBool(&fbdo, "/status/bear/fsr", pickedUp)) {
    Serial.println("✅ FSR state sent.");
    sendHeartbeat();
  } else {
    Serial.print("❌ FSR update failed: ");
    Serial.println(fbdo.errorReason());
  }

  // Long press detection
  static unsigned long pressedStart = 0;
  if (pickedUp) {
    if (pressedStart == 0) pressedStart = millis();
    else if (millis() - pressedStart >= 10000) {
      Serial.println("⏱️ Force sensor pressed for > 10 seconds!");
      readFirebaseCommand();
      pressedStart = 0;
    }
      fetchFirebaseHeartbeat();
  } else {
    pressedStart = 0;
  }

  // Wakeup check
  fetchWakeupData();
  String currentTime = getCurrentTimeEstonia();
  Serial.print("⌚ Current time: ");
  Serial.println(currentTime);

  if (wakeupEnabled && currentTime == scheduledTime && currentTime != lastCheckedMinute) {
    Serial.println("✅ Conditions met: running motor sequence.");
    runMotorSequence();
    lastCheckedMinute = currentTime;
  } else {
    Serial.println("⏸️ Conditions not met or already triggered.");
  }
   handleGPS(); 
  delay(1000);
}

//Logic about GPS

void handleGPS() {
  static unsigned long lastUpdate = 0;
  static unsigned long lastAlert = 0;
  static unsigned long lastFallbackSent = 0;

  double lat, lng;
  bool insideGeofence = false;

if (simulateGPS) {
    lat = SAFE_LAT;
    lng = simulateInside ? SAFE_LNG : (SAFE_LNG + offset_lon);
    insideGeofence = TinyGPSPlus::distanceBetween(lat, lng, SAFE_LAT, SAFE_LNG) <= SAFE_RADIUS_METERS;
  } else {
    while (SerialGPS.available() > 0) gps.encode(SerialGPS.read());

    if (gps.location.isValid() && gps.location.isUpdated()) {
      lat = gps.location.lat();
      lng = gps.location.lng();
      insideGeofence = TinyGPSPlus::distanceBetween(lat, lng, SAFE_LAT, SAFE_LNG) <= SAFE_RADIUS_METERS;
    } else if (millis() - lastUpdate > 5000 && millis() - lastFallbackSent > 30000) {
      lastFallbackSent = millis();
      String fallbackPayload = "{\"geofence\": false}";
      sendToFirebaseGPS(fallbackPayload);
      return;
    }
  }

  if (millis() - lastUpdate > 2000 && (simulateGPS || gps.location.isValid())) {
    lastUpdate = millis();
    String payload = "{";
    payload += "\"geofence\": " + String(insideGeofence ? "true" : "false");
    payload += ",\"latitude\": " + String(lat, 6);
    payload += ",\"longitude\": " + String(lng, 6);
    payload += "}";
    sendToFirebaseGPS(payload);
  }

  // Alert for no satellites
  if (!simulateGPS && millis() - lastAlert > 5000) {
    lastAlert = millis();
    if (gps.satellites.value() == 0) {
      Serial.println("⚠️ NO SATELLITES DETECTED");
    }
  }

  delay(1000);
}

// ----------- SUPPORT FUNCTIONS -----------

//Connecting ro wifi
void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");
}

//Sending healthcheck to firebase so in the appit it is possible to see if the bear is connected
void sendHeartbeat() {
  time_t now = time(nullptr);
  if (!Firebase.RTDB.setInt(&fbdo, "/status/bear/lastSeen", now)) {
    Serial.print("❌ Failed to send heartbeat: ");
    Serial.println(fbdo.errorReason());
  } else {
    Serial.println("🫀 Healthcheck sent.");
  }
}

//Sending GPS data to firebase 
void sendToFirebaseGPS(String payloadStr) {
  FirebaseJson json;
  json.setJsonData(payloadStr);  // Converting the String to FirebaseJson format
  if (Firebase.RTDB.setJSON(&fbdo, "/status/gps", &json)) {
    Serial.println("📡 GPS data sent to Firebase.");
  } else {
    Serial.print("❌ Failed to send GPS data: ");
    Serial.println(fbdo.errorReason());
  }
}

// Reciving parents hearbeat 
void fetchFirebaseHeartbeat() {
  if (stopRequested) {
    Serial.println("⛔ Vibration skipped due to stop command.");
    updateVibrationStatus(false);
    digitalWrite(motorPin, LOW);
    return;
  }

  if (Firebase.RTDB.getJSON(&fbdo, "/commands/heartbeat")) {
    FirebaseJson &json = fbdo.to<FirebaseJson>();
    FirebaseJsonData result;

    float amplitude = 0.0;
    int bpm = 0;
    double freqHz = 0.0;

    if (json.get(result, "amplitude")) amplitude = result.to<float>();
    if (json.get(result, "beatsPerMinute")) bpm = result.to<int>();
    if (json.get(result, "vibrationFrequencyHz")) freqHz = result.to<double>();

    if (bpm > 0 && amplitude > 0.0) {
      Serial.println("➡️ Starting vibration...");

      int periodMs = (freqHz > 0.0) ? (int)(1000.0 / freqHz) : 500;

      updateVibrationStatus(true);  // Motor ON

      unsigned long startTime = millis();
      bool motorOn = false;

      while (millis() - startTime < 300000) {  // Run for 5 minutes (300,000 ms)
        checkSerialCommand();
        if (stopRequested) {
          digitalWrite(motorPin, LOW);
          updateVibrationStatus(false);
          Serial.println("🛑 Vibration interrupted.");
          return;
        }

        motorOn = !motorOn;
        digitalWrite(motorPin, motorOn ? HIGH : LOW);
        delay(periodMs / 2);

        // Send heartbeat every 1 second while vibration runs
        unsigned long now = millis();
        if (now - lastHeartbeatTime > 1000) {  // every 1 sec
          lastHeartbeatTime = now;
          sendHeartbeat();
        }
      }

      digitalWrite(motorPin, LOW);
      updateVibrationStatus(false);  // Motor OFF
      Serial.println("✅ Vibration complete.");
    } else {
      updateVibrationStatus(false);  // Invalid data
    }
  } else {
    Serial.print("❌ Firebase read failed: ");
    Serial.println(fbdo.errorReason());
    updateVibrationStatus(false);  // Fetch error
  }
}

//Updating vibration status so on the up it ispossible to see if vibration is on or off
void updateVibrationStatus(bool isActive) {
  if (vibrationState == isActive) return; // no change, skip
  if (!Firebase.ready()) return;
  if (Firebase.RTDB.setBool(&fbdo, "/status/bear/vibration", isActive)) {
    Serial.printf("📤 Vibration status: %s\n", isActive ? "true" : "false");
    vibrationState = isActive;
  } else {
    Serial.print("❌ Failed to update vibration status: ");
    Serial.println(fbdo.errorReason());
  }
}


//Fetching wakeup data from firebase
void fetchWakeupData() {
  if (Firebase.RTDB.getJSON(&fbdo, "/commands/wakeupmode")) {
    FirebaseJson& json = fbdo.jsonObject();
    FirebaseJsonData result;
    if (json.get(result, "enabled")) wakeupEnabled = result.to<bool>();
    if (json.get(result, "time")) scheduledTime = result.to<String>();
  } else {
    Serial.println("❌ Failed to fetch wakeup data.");
    Serial.println(fbdo.errorReason());
  }
}

//Getting local time
String getCurrentTimeEstonia() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("❌ Failed to get local time");
    return "";
  }
  char buffer[6];
  strftime(buffer, sizeof(buffer), "%H:%M", &timeinfo);
  return String(buffer);
}

//Logic how vibration motors need to work in wakeup mode
void runMotorSequence() {
  updateVibrationStatus(true);
  unsigned long startMillis = millis();
  unsigned long pauseDuration = (maxRunTime - (cycleDuration * 10)) / 10;

  for (int cycle = 0; cycle < 10; cycle++) {
    if (millis() - startMillis >= maxRunTime) break;

    int fMin = frequencyRanges[cycle][0];
    int fMax = frequencyRanges[cycle][1];
    Serial.printf("🔁 Cycle %d: %d Hz to %d Hz\n", cycle + 1, fMin, fMax);
    unsigned long cycleEndTime = millis() + cycleDuration;

    if (cycle == 0) {
      int period = 1000 / 20;
      int onTime = period / 2;
      int offTime = period - onTime;

      while (millis() < cycleEndTime) {
        digitalWrite(motorPin, HIGH);
        delay(onTime);
        digitalWrite(motorPin, LOW);
        delay(offTime);
        if (millis() - lastHeartbeatSent >= 1000) {
          sendHeartbeat();
          lastHeartbeatSent = millis();
        }
      }
    } else {
      unsigned long startTime = millis();
      unsigned long lastPrintTime = 0;

      while (millis() < cycleEndTime) {
        unsigned long elapsed = millis() - startTime;
        int currentFreq = map(elapsed, 0, cycleDuration, fMin, fMax);
        int period = 1000 / currentFreq;
        int onTime = period / 2;
        int offTime = period - onTime;

        if (millis() - lastPrintTime >= 500) {
          Serial.printf("  🔊 Frequency: %d Hz\n", currentFreq);
          lastPrintTime = millis();
        }

        digitalWrite(motorPin, HIGH);
        delay(onTime);
        digitalWrite(motorPin, LOW);
        delay(offTime);
        if (millis() - lastHeartbeatSent >= 1000) {
          sendHeartbeat();
          lastHeartbeatSent = millis();
        }
      }
    }

    digitalWrite(motorPin, LOW);

    // Replacing  delay with heartbeat sending loop during pause
    unsigned long pauseStart = millis();
    while (millis() - pauseStart < pauseDuration) {
      if (millis() - lastHeartbeatSent >= 1000) {
        sendHeartbeat();
        lastHeartbeatSent = millis();
      }
      delay(10);  // small delay to avoid CPU hogging
    }
  }

  digitalWrite(motorPin, LOW);
  updateVibrationStatus(false);
  Serial.println("✅ Wake-up vibration complete.");
}


long long getRealTimestamp() {
  time_t now = time(nullptr);
  return now > 0 ? (long long)now * 1000 : 0;
}

//Playing song for 2 minutes 
void playSong(long melody[], long durations[], int length, unsigned long playMillis) {
  Serial.println("🎵 Playing song...");
  unsigned long startTime = millis();
  int i = 0;
 
  while (millis() - startTime < playMillis) {
    int noteDuration = 1000 / durations[i];

    if (melody[i] != REST) {
      tone(BUZZER_PIN, melody[i], noteDuration);
    }

    // Delay in smaller chunks to allow heartbeat updates to shiw that bear is connected to wifi:
    unsigned long noteStart = millis();
    while (millis() - noteStart < noteDuration * 1.3) {
      // Send heartbeat every 1 second while playing
      if (millis() - lastHeartbeatSent >= 1000) {
        sendHeartbeat();
        lastHeartbeatSent = millis();
      }
      delay(10); // small delay to not block too long
    }

    noTone(BUZZER_PIN);
    i = (i + 1) % length;
  }
  Serial.println("✅ Song finished.");
}

void readFirebaseCommand() {
  if (!Firebase.RTDB.getJSON(&fbdo, FIREBASE_PATH)) {
    Serial.print("❌ Failed to read command: ");
    Serial.println(fbdo.errorReason());
    return;
  }

  String payload = fbdo.jsonString();
  Serial.println("📥 Firebase Response: " + payload);

  DynamicJsonDocument doc(1024);
  if (deserializeJson(doc, payload)) {
    Serial.println("❌ JSON parse failed.");
    return;
  }

  String songPattern = doc["pattern"].as<String>();
  int length;

  if (songPattern == "potter1") {
    length = sizeof(durations1) / sizeof(durations1[0]);
    playSong(melody1, durations1, length, 120000);
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
