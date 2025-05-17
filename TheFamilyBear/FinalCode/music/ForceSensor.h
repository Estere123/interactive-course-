#ifndef FORCE_SENSOR_H
#define FORCE_SENSOR_H

#include <Arduino.h>

class ForceSensor {
  public:
    ForceSensor(int pin = 5);  // Default pin is 5
    void begin();
    bool isSqueezedForDuration(unsigned long duration);
  
  private:
    int _pin;
    unsigned long _squeezeStartTime;
    bool _isSqueezed;
};

#endif

