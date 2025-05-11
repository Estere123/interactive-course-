#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <time.h>

// WiFi credentials
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

// FSR sensor pin
const int fsrPin = 5; // Use a valid analog input pin

// Pickup threshold
const int pickupThreshold = 20;

// Timezone info
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
    while (true) delay(1000);
  }

  Serial.println("\n✅ WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  pinMode(fsrPin, INPUT);

  // Connect to Wi-Fi
  connectToWiFi();

  // Initialize NTP
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  // Wait for valid time
  Serial.print("Waiting for NTP time...");
  while (time(nullptr) < 1) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ Time synchronized!");

  // Firebase setup
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

long long getRealTimestamp() {
  time_t now = time(nullptr);
  return (long long)now * 1000;  // Convert to milliseconds
}

void loop() {
  // Read raw value from FSR
  int fsrValue = analogRead(fsrPin);

  // Debugging: Print raw value
  Serial.print("FSR Value: ");
  Serial.println(fsrValue);

  // Detect pickup
  bool pickedUp = fsrValue > pickupThreshold;

  // Get timestamp
  long long timestamp = getRealTimestamp();

  // Prepare JSON for Firebase
  FirebaseJson fsrJson;
  fsrJson.set("pickedUp", pickedUp);
  fsrJson.set("value", fsrValue);
  fsrJson.set("timestamp", timestamp);

  // Send to Firebase
  if (Firebase.RTDB.setJSON(&fbdo, "/status/fsr", &fsrJson)) {
    Serial.println("✅ FSR data uploaded to Firebase.");
  } else {
    Serial.print("❌ Firebase FSR upload failed: ");
    Serial.println(fbdo.errorReason());
  }

  delay(1000);  // Delay before next loop
}

