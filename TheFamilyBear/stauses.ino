#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <time.h>

// WiFi credentials
#define WIFI_SSID "TLU"
#define WIFI_PASSWORD ""  // Add your WiFi password

// Firebase credentials
#define API_KEY "AIzaSyCYFHx-Sq1v4dl9Ncqa4Hnq6IoaUL7IdDM"
#define DATABASE_URL "https://familybear-ab556-default-rtdb.europe-west1.firebasedatabase.app"
#define USER_EMAIL "estere12@tlu.ee"
#define USER_PASSWORD "Bear2025"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// FSR sensor pin
const int fsrPin = 5;
const int pickupThreshold = 20;

// Time settings
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 3600
#define DAYLIGHT_OFFSET_SEC 3600

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n❌ WiFi failed to connect. Check network or credentials.");
    while (true) delay(1000);  // Prevent further execution
  }

  Serial.println("\n✅ WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

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

void setup() {
  Serial.begin(115200);
  pinMode(fsrPin, INPUT);

  connectToWiFi();

  // Sync time
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  while (time(nullptr) < 100000) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n🕒 Time synced");

  // Firebase setup
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  int fsrValue = analogRead(fsrPin);
  bool pickedUp = fsrValue > pickupThreshold;

  Serial.printf("FSR Value: %d | Picked up: %s\n", fsrValue, pickedUp ? "true" : "false");

  if (Firebase.ready()) {
    if (!Firebase.RTDB.setBool(&fbdo, "/status/bear/fsr", pickedUp)) {
      Serial.print("❌ FSR update failed: ");
      Serial.println(fbdo.errorReason());
    } else {
      Serial.println("✅ FSR state sent.");
    }

    sendHeartbeat();  // ⏱️ Send lastSeen timestamp
  }

  delay(1000);
}
