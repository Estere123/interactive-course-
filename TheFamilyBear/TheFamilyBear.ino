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

// Pins
const int motorPin = 2;
const int fsrPin = 5;
const int pickupThreshold = 20;

volatile bool stopRequested = false;

void sendVibrationStatus(bool state) {
  Firebase.RTDB.setBool(&fbdo, "/status/bear/vibration", state);
  Serial.print("🔥 Sent vibration status to Firebase: ");
  Serial.println(state ? "true (ON)" : "false (OFF)");
}

void fetchFirebaseHeartbeat() {
  if (stopRequested) {
    Serial.println("⛔ Vibration skipped due to stop command.");
    sendVibrationStatus(false);
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

      sendVibrationStatus(true);  // Motor ON

      unsigned long startTime = millis();
      bool motorOn = false;

      while (millis() - startTime < 300000) {  // Max 5 minutes
        checkSerialCommand();
        if (stopRequested) {
          digitalWrite(motorPin, LOW);
          sendVibrationStatus(false);
          Serial.println("🛑 Vibration interrupted.");
          return;
        }

        motorOn = !motorOn;
        digitalWrite(motorPin, motorOn ? HIGH : LOW);
        delay(periodMs / 2);
      }

      digitalWrite(motorPin, LOW);
      sendVibrationStatus(false);  // Motor OFF
      Serial.println("✅ Vibration complete.");
    } else {
      sendVibrationStatus(false);  // Invalid data
    }
  } else {
    Serial.print("❌ Firebase read failed: ");
    Serial.println(fbdo.errorReason());
    sendVibrationStatus(false);  // Fetch error
  }
}

void checkSerialCommand() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.equalsIgnoreCase("stop")) {
      stopRequested = true;
      digitalWrite(motorPin, LOW);
      sendVibrationStatus(false);
      Serial.println("🛑 Vibration stopped by terminal.");
    } else if (command.equalsIgnoreCase("start")) {
      stopRequested = false;
      Serial.println("✅ Vibration start enabled.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(motorPin, OUTPUT);
  pinMode(fsrPin, INPUT);
  digitalWrite(motorPin, LOW);
  sendVibrationStatus(false);  // Initial state

  Serial.println("Type 'stop' or 'start' in Serial Monitor.");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  checkSerialCommand();

  int fsrValue = analogRead(fsrPin);
  Serial.printf("FSR value: %d\n", fsrValue);

  if (fsrValue > pickupThreshold && !stopRequested) {
    Serial.println("📈 Pickup detected. Fetching heartbeat...");
    fetchFirebaseHeartbeat();
  } else {
    digitalWrite(motorPin, LOW);
    sendVibrationStatus(false);  // No pickup = motor off
  }

  delay(1000);
}




