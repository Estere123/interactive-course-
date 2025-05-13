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
const int motorPin = 2;  // Motor pin
const int fsrPin = 5;    // FSR sensor pin
const int pickupThreshold = 20;  // FSR sensor threshold

// Global control flag
volatile bool stopRequested = false;

// Fetch heartbeat data from Firebase and vibrate motor
void fetchFirebaseHeartbeat() {
  if (stopRequested) {
    Serial.println("⛔ Vibration skipped due to stop command.");
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

    Serial.println("Fetched Heartbeat Data from Firebase:");
    Serial.print("Amplitude: "); Serial.println(amplitude);
    Serial.print("BPM: "); Serial.println(beatsPerMinute);
    Serial.print("Frequency: "); Serial.println(vibrationFrequencyHz);
    Serial.print("Timestamp: "); Serial.println(timestamp);

    if (beatsPerMinute > 0 && amplitude > 0.0) {
      Serial.println("➡️ Starting vibration with Firebase data...");

      int periodMs = 500;
      if (vibrationFrequencyHz > 0.0) {
        periodMs = (int)(1000.0 / vibrationFrequencyHz);
      }

      Serial.print("Calculated vibration period: ");
      Serial.print(periodMs);
      Serial.println(" ms");

      Firebase.RTDB.setBool(&fbdo, "/status/bear/vibration", true);

      unsigned long startTime = millis();
      unsigned long lastToggleTime = millis();
      bool motorOn = false;

      while (millis() - startTime < 300000) {  // 5 minutes
        checkSerialCommand();  // Check for stop command

        if (stopRequested) {
          digitalWrite(motorPin, LOW);
          Firebase.RTDB.setBool(&fbdo, "/status/bear/vibration", false);
          Serial.println("⛔ Vibration interrupted by user.");
          return;
        }

        if (millis() - lastToggleTime >= (periodMs / 2)) {
          motorOn = !motorOn;
          digitalWrite(motorPin, motorOn ? HIGH : LOW);
          lastToggleTime = millis();
        }

        delay(1);  // Yield CPU slightly
      }

      digitalWrite(motorPin, LOW);
      Firebase.RTDB.setBool(&fbdo, "/status/bear/vibration", false);
      Serial.println("✅ Vibration finished.");
    }
  } else {
    Serial.print("❌ Firebase read failed: ");
    Serial.println(fbdo.errorReason());
  }
}

// Handle serial commands
void checkSerialCommand() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.equalsIgnoreCase("stop")) {
      stopRequested = true;
      digitalWrite(motorPin, LOW);  // Stop motor immediately
      Firebase.RTDB.setBool(&fbdo, "/status/bear/vibration", false);
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
    Serial.println("📈 Pickup detected. Fetching heartbeat data from Firebase...");
    fetchFirebaseHeartbeat();
  }

  delay(1000);
}


