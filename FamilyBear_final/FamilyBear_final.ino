#include <Arduino_LSM6DS3.h>
#include <Firebase_Arduino_WiFiNINA.h>

#define FIREBASE_HOST "familybear-ab556.firebaseio.com"
#define FIREBASE_AUTH "HyyjQSiGSRL6mH2CetwCWMvBOkmlQTMvYsYXWZOX"
#define WIFI_SSID "Your WiFi SSID"
#define WIFI_PASSWORD "Your WiFi Password"

FirebaseData firebaseData;

String path = "/IMU_LSM6DS3";
String jsonStr;

void setup()
{
  Serial.begin(9600);
  delay(1000);
  Serial.println();

  Serial.print("Initialize IMU sensor...");
  if (!IMU.begin()) {
    Serial.println(" failed!");
    while (1);
  }
  Serial.println(" done");
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  Serial.print("Connecting to WiFi...");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }
  Serial.print(" IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);
}

void loop()
{
float frequency = 0;
float amplitude = 0;
int duration = 0;

if (Firebase.getFloat(firebaseData, "/vibrationSettings/frequency")) {
  frequency = firebaseData.floatData();
}

if (Firebase.getFloat(firebaseData, "/vibrationSettings/amplitude")) {
  amplitude = firebaseData.floatData();
}

if (Firebase.getInt(firebaseData, "/vibrationSettings/duration")) {
  duration = firebaseData.intData();
}

Serial.println("=== Vibration Settings Received ===");
Serial.print("Frequency: "); Serial.println(frequency);
Serial.print("Amplitude: "); Serial.println(amplitude);
Serial.print("Duration: "); Serial.println(duration);

  
}



//const int motorPin = 9;  // Vibration motor connected to pin 9

//void setup() {
 // pinMode(motorPin, OUTPUT);  // Set pin as output
//}

//void loop() {
  // Ramp up vibration intensity (if using PWM)
  //for (int i = 0; i <= 255; i++) {
   // analogWrite(motorPin, i);  // PWM control (if motor supports it)
   // delay(10);
  //}
  
  // Ramp down vibration intensity
  //for (int i = 255; i >= 0; i--) {
    //analogWrite(motorPin, i);
    //delay(10);
 // }
  
 // delay(1000);  // Pause between pulses
//}