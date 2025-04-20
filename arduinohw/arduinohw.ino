#include <ArduinoBLE.h>

#define BUZZER_PIN 7
#define LED1_PIN 8
#define LED2_PIN 13 

void setup() {
  // Set pin modes first
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  
  // Play a simple scale
  tone(BUZZER_PIN, 262); // C4
  delay(300);
  tone(BUZZER_PIN, 294); // D4
  delay(300);
  tone(BUZZER_PIN, 330); // E4
  delay(300);
  tone(BUZZER_PIN, 349); // F4
  delay(300);
  tone(BUZZER_PIN, 392); // G4
  delay(300);
  noTone(BUZZER_PIN);    // Stop sound
}

void loop() {
  digitalWrite(LED1_PIN, HIGH); 
   digitalWrite(LED2_PIN, HIGH);// Turn LED1 ON
  delay(1000);                  // Wait 1 second
  digitalWrite(LED1_PIN, LOW);
   digitalWrite(LED2_PIN, LOW);  // Turn LED1 OFF
  delay(1000);                  // Wait 1 second
}