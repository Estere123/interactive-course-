#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <time.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
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

// Motor control
const int motorPin = 2;
const int cycleDuration = 8000;
const unsigned long maxRunTime = 180000;

const int frequencyRanges[10][2] = {
  {20, 20}, {20, 40}, {40, 80}, {60, 80}, {80, 100},
  {100, 120}, {120, 140}, {140, 160}, {160, 180}, {180, 200}
};

// Variables for Firebase data
bool wakeupEnabled = false;
String scheduledTime = "";
String lastCheckedMinute = "";

// Handle WiFi connection
void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n✅ WiFi connected.");
}

// Setup Firebase connection
void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Ensure Firebase is initialized correctly
  unsigned long startMillis = millis();
  while (!Firebase.ready()) {
    Serial.println("Waiting for Firebase to initialize...");
    delay(1000);
    if (millis() - startMillis > 30000) {
      Serial.println("❌ Timeout: Firebase initialization failed!");
      return;
    }
  }
  Serial.println("✅ Firebase initialized.");
}

// Fetch wakeup data from Firebase
void fetchWakeupData() {
  if (Firebase.RTDB.getJSON(&fbdo, "/commands/wakeupmode")) {
    FirebaseJson& json = fbdo.jsonObject();
    FirebaseJsonData result;

    // Get values from JSON
    if (json.get(result, "enabled")) {
      wakeupEnabled = result.to<bool>();
    }
    if (json.get(result, "time")) {
      scheduledTime = result.to<String>();
    }
    if (json.get(result, "timestamp")) {
      unsigned long timestamp = result.to<unsigned long>();
      Serial.print("🕓 Firebase timestamp = ");
      Serial.println(timestamp);
    }

    Serial.print("✅ Firebase: enabled = ");
    Serial.println(wakeupEnabled);
    Serial.print("🕒 Firebase time = ");
    Serial.println(scheduledTime);
  } else {
    Serial.println("❌ Failed to fetch data from Firebase.");
    Serial.println(fbdo.errorReason());
  }
}

// Get current time in Estonia timezone
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

// Send vibration status to Firebase
void updateVibrationStatus(bool isActive) {
  if (!Firebase.ready()) return;
  if (Firebase.RTDB.setBool(&fbdo, "/status/bear/vibration", isActive)) {
    Serial.printf("📤 Vibration status updated: %s\n", isActive ? "true" : "false");
  } else {
    Serial.print("❌ Failed to update vibration status: ");
    Serial.println(fbdo.errorReason());
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);

  connectToWiFi();

  configTime(10800, 0, "pool.ntp.org", "time.nist.gov");

  setupFirebase();
}

void loop() {
  if (!Firebase.ready()) {
    Serial.println("Firebase not ready...");
    delay(5000);
    return;
  }

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

  delay(10000);
}

void runMotorSequence() {
  updateVibrationStatus(true);  // Send "true" when motor starts
  unsigned long startMillis = millis();
  unsigned long totalElapsedTime = 0;
  unsigned long pauseDuration = (maxRunTime - (cycleDuration * 10)) / 10;

  for (int cycle = 0; cycle < 10; cycle++) {
    totalElapsedTime = millis() - startMillis;
    if (totalElapsedTime >= maxRunTime) break;

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
      }
    }

    digitalWrite(motorPin, LOW);
    delay(pauseDuration);
  }

  digitalWrite(motorPin, LOW);
  updateVibrationStatus(false);  // Send "false" when motor stops
  Serial.println("✅ Wake-up vibration complete.");
}


