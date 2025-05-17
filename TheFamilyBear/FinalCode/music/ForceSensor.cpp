#include "ForceSensor.h"

ForceSensor::ForceSensor(int pin) {
  _pin = pin;
  _squeezeStartTime = 0;
  _isSqueezed = false;
}

void ForceSensor::begin() {
  pinMode(_pin, INPUT);
}

bool ForceSensor::isSqueezedForDuration(unsigned long duration) {
  int sensorValue = analogRead(_pin);

  // Considered "squeezed" if the force is above a threshold value of 30
  if (sensorValue > 20) {  // Adjusted threshold value
    if (!_isSqueezed) {
      _squeezeStartTime = millis();
      _isSqueezed = true;
    }
  } else {
    _isSqueezed = false;
  }

  // Check if the force sensor has been squeezed for the given duration (10 seconds)
  if (_isSqueezed && millis() - _squeezeStartTime >= 10000) {  // Duration is now hardcoded to 10 seconds (10,000 ms)
    return true;  // The sensor has been pressed for more than 10 seconds
  }

  return false;
}


