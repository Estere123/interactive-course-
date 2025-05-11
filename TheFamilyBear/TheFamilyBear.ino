#include <WiFi.h>
#include <Firebase_ESP_Client.h>

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

// Motor pin
const int motorPin = 2;

// Control flags
bool stopRequested = false;
bool vibrationEnabled = true;

// Handle serial commands
void checkSerialCommand() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.equalsIgnoreCase("stop")) {
      stopRequested = true;
      vibrationEnabled = false;
      Serial.println("🛑 Vibration disabled.");
    } else if (command.equalsIgnoreCase("start")) {
      stopRequested = false;
      vibrationEnabled = true;
      Serial.println("✅ Vibration enabled.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);

  Serial.println("Type 'stop' or 'start' in Serial Monitor.");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  checkSerialCommand();

  if (!vibrationEnabled) {
    delay(1000);
    return;
  }

  if (Firebase.RTDB.getJSON(&fbdo, "/commands/heartbeat")) {
    FirebaseJson &json = fbdo.to<FirebaseJson>();
    FirebaseJsonData result;

    float amplitude = 0.0;
    int beatsPerMinute = 0;
    double vibrationFrequencyHz = 0.0;
    long long timestamp = 0;

    if (json.get(result, "amplitude")) amplitude = result.to<float>();
    if (json.get(result, "beatsPerMinute")) beatsPerMinute = result.to<int>();
    if (json.get(result, "vibrationFrequencyHz")) vibrationFrequencyHz = result.to<double>();
    if (json.get(result, "timestamp")) timestamp = result.to<long long>();

    Serial.println("Heartbeat Data from Firebase:");
    Serial.print("Amplitude: "); Serial.println(amplitude);
    Serial.print("BPM: "); Serial.println(beatsPerMinute);
    Serial.print("Frequency: "); Serial.println(vibrationFrequencyHz);
    Serial.print("Timestamp: "); Serial.println(timestamp);

    // Only start vibration if the data is valid
    if (beatsPerMinute > 0 && amplitude > 0.0) {
      Serial.println("➡️ Starting vibration...");

      // Default vibration period (fallback to 500ms if no frequency is provided)
      int periodMs = 500;
      if (vibrationFrequencyHz > 0.0) {
        // Calculate period from the frequency (Hz)
        periodMs = (int)(1000.0 / vibrationFrequencyHz);
      }

      Serial.print("Calculated vibration period: ");
      Serial.print(periodMs);
      Serial.println(" ms");

      if (Firebase.RTDB.setBool(&fbdo, "/status/bear/vibration", true)) {
        Serial.println("📤 Sent: vibration = true");
      }

      unsigned long startTime = millis();
      stopRequested = false;

      // Vibrating for 5 minutes (300000ms)
      while (millis() - startTime < 300000) {
        checkSerialCommand();

        if (stopRequested) {
          digitalWrite(motorPin, LOW);
          Firebase.RTDB.setBool(&fbdo, "/status/bear/vibration", false);
          Serial.println("⛔ Vibration interrupted.");
          return;
        }

        // Turn the motor on
        digitalWrite(motorPin, HIGH);
        delay(periodMs / 2);  // Half the period for "on" time

        // Turn the motor off
        digitalWrite(motorPin, LOW);
        delay(periodMs / 2);  // Half the period for "off" time
      }

      // After 5 minutes, stop the vibration
      Firebase.RTDB.setBool(&fbdo, "/status/bear/vibration", false);
      Serial.println("✅ Vibration finished. Sent: vibration = false");
    } else {
      Serial.println("⚠️ Invalid heartbeat data, skipping vibration.");
    }

  } else {
    Serial.print("❌ Firebase read failed: ");
    Serial.println(fbdo.errorReason());
  }

  delay(1000);  // Check again in 1 second
}

  

