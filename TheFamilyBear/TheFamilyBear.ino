#include <WiFi.h>
#include <Firebase_ESP_Client.h>

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

void setup() {
  Serial.begin(115200);
  pinMode(motorPin, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Firebase setup
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.RTDB.getJSON(&fbdo, "/commands/heartbeat")) {
    FirebaseJson &json = fbdo.to<FirebaseJson>();
    FirebaseJsonData result;

    float amplitude = 0.0;
    int beatsPerMinute = 0;
    int durationMs = 0;
    double vibrationFrequencyHz = 0.0;
    long long timestamp = 0;
    bool wakeupMode = false;

    if (json.get(result, "amplitude")) amplitude = result.to<float>();
    if (json.get(result, "beatsPerMinute")) beatsPerMinute = result.to<int>();
    if (json.get(result, "durationMs")) durationMs = result.to<int>();
    if (json.get(result, "vibrationFrequencyHz")) vibrationFrequencyHz = result.to<double>();
    if (json.get(result, "timestamp")) timestamp = result.to<long long>();
    if (json.get(result, "wakeupMode")) wakeupMode = result.to<bool>();

    Serial.println("Heartbeat Data Received:");
    Serial.print("Amplitude: "); Serial.println(amplitude);
    Serial.print("Beats Per Minute: "); Serial.println(beatsPerMinute);
    Serial.print("Duration (ms): "); Serial.println(durationMs);
    Serial.print("Vibration Frequency (Hz): "); Serial.println(vibrationFrequencyHz);
    Serial.print("Timestamp: "); Serial.println(timestamp);
    Serial.print("Wakeup Mode: "); Serial.println(wakeupMode ? "true" : "false");

    if (beatsPerMinute > 0 && amplitude > 0.0 && durationMs > 0) {
      int delayBetweenBeats = 60000 / beatsPerMinute;
      int pulseDuration = (int)(amplitude * 200);
      pulseDuration = constrain(pulseDuration, 10, delayBetweenBeats - 10);

      Serial.println("Starting Vibration...");

      unsigned long startTime = millis();
      while (millis() - startTime < durationMs) {
        digitalWrite(motorPin, HIGH);
        delay(pulseDuration);
        digitalWrite(motorPin, LOW);
        delay(delayBetweenBeats - pulseDuration);
      }

      Serial.println("Vibration Completed.");
    } else {
      Serial.println("Invalid heartbeat data.");
    }

  } else {
    Serial.print("Firebase read failed: ");
    Serial.println(fbdo.errorReason());
  }

  delay(10000);  // Wait before checking again
}
  

