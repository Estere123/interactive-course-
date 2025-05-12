const int motorPin = 2;

// Each cycle: 8 seconds ON
const int cycleDuration = 8000;     
const unsigned long maxRunTime = 180000; // 3 minutes

const int frequencyRanges[10][2] = {
  {20, 20}, {20, 40}, {40, 80}, {60, 80}, {80, 100},
  {100, 120}, {120, 140}, {140, 160}, {160, 180}, {180, 200}
};

void setup() {
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);

  Serial.begin(115200);  // Use faster baud rate
  delay(1000);  // Give time for serial to connect
  Serial.println("Wake-up mode starting...");
}

void loop() {
  unsigned long startMillis = millis();
  unsigned long totalElapsedTime = 0;
  unsigned long pauseDuration = (maxRunTime - (cycleDuration * 10)) / 10;

  for (int cycle = 0; cycle < 10; cycle++) {
    totalElapsedTime = millis() - startMillis;
    if (totalElapsedTime >= maxRunTime) break;

    int fMin = frequencyRanges[cycle][0];
    int fMax = frequencyRanges[cycle][1];

    Serial.print("Cycle ");
    Serial.print(cycle + 1);
    Serial.print(": ");
    Serial.print(fMin);
    Serial.print(" Hz to ");
    Serial.print(fMax);
    Serial.println(" Hz");

    unsigned long startTime = millis();

    if (cycle == 0) {
      // Cycle 1: Continuous 20 Hz vibration (50 ms period)
      unsigned long cycleEndTime = startTime + cycleDuration;
      int currentFreq = 20;
      int period = 1000 / currentFreq; // 50 ms
      int onTime = period / 2;         // 25 ms
      int offTime = period - onTime;

      while (millis() < cycleEndTime) {
        digitalWrite(motorPin, HIGH);
        delay(onTime);
        digitalWrite(motorPin, LOW);
        delay(offTime);
      }
    } else {
      // Other cycles: gradually increase frequency
      unsigned long cycleEndTime = startTime + cycleDuration;
      int lastFreq = -1;
      unsigned long lastPrintTime = 0;

      while (millis() < cycleEndTime) {
        unsigned long elapsedTime = millis() - startTime;
        int currentFreq = map(elapsedTime, 0, cycleDuration, fMin, fMax);

        // Print frequency update every 500 ms
        if (millis() - lastPrintTime >= 500) {
          Serial.print("  Frequency: ");
          Serial.print(currentFreq);
          Serial.println(" Hz");
          lastPrintTime = millis();
        }

        int period = 1000 / currentFreq;
        int onTime = period / 2;
        int offTime = period - onTime;

        digitalWrite(motorPin, HIGH);
        delay(onTime);
        digitalWrite(motorPin, LOW);
        delay(offTime);
      }
    }

    digitalWrite(motorPin, LOW);
    delay(pauseDuration);
  }

  Serial.println("Wake-up mode complete. Device will remain idle.");
  while (true); // Stop program after 3 minutes
}
