#include <ArduinoBLE.h>

// BLE Service and Characteristics
BLEService ledService("4f0c973c-4a93-4949-b1bc-58940bb33b45");

BLEByteCharacteristic switchCharacteristic("4f0c973c-4a93-4949-b1bc-58940bb33b46", BLERead | BLEWrite);
BLEByteCharacteristic distanceCharacteristic("4f0c973c-4a93-4949-b1bc-58940bb33b47", BLERead | BLEWrite);
BLEByteCharacteristic lightCharacteristic("4f0c973c-4a93-4949-b1bc-58940bb33b48", BLERead | BLEWrite);

// Pin assignments
const int buttonLedPin = 13;
const int lightLedPin = 8;
const int buzzerPin = 7;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  pinMode(buttonLedPin, OUTPUT);
  pinMode(lightLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  if (!BLE.begin()) {
    Serial.println("Starting BLE module failed!");
    while (1);
  }
  
  BLE.setLocalName("LED");
  BLE.setAdvertisedService(ledService);
  
  ledService.addCharacteristic(switchCharacteristic);
  ledService.addCharacteristic(distanceCharacteristic);
  ledService.addCharacteristic(lightCharacteristic);
  
  BLE.addService(ledService);
  
  switchCharacteristic.writeValue(0);
  distanceCharacteristic.writeValue(0);
  lightCharacteristic.writeValue(0);
  
  BLE.advertise();
  Serial.println("BLE Peripheral Ready");
}

void loop() {
 
  BLEDevice central = BLE.central();
  
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    
    while (central.connected()) {
      // Button LED
      if (switchCharacteristic.written()) {
        byte switchVal = switchCharacteristic.value();
        Serial.print("Switch: ");
        Serial.println(switchVal);
        digitalWrite(buttonLedPin, switchVal ? HIGH : LOW);
      }
      
      // Distance Buzzer
      if (distanceCharacteristic.written()) {
        String distanceStr = String(distanceCharacteristic.value());
int distance = distanceStr.toInt(); // convert to usable number

Serial.print("Distance: ");
Serial.println(distance);

if (distance > 10) {
  tone(buzzerPin, 1000); 
} else {
          noTone(buzzerPin);
        }

}
        
  
      
      // Light LED
      if (lightCharacteristic.written()) {
        byte light = lightCharacteristic.value();
        Serial.print("Light: ");
        Serial.println(light);
        digitalWrite(lightLedPin, light == 1 ? HIGH : LOW);
      }
    }
    
    // Make sure all outputs are off when disconnected
    digitalWrite(buzzerPin, LOW);
    Serial.print("Disconnected from: ");
    Serial.println(central.address());
  }
}