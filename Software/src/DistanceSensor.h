#include <Arduino.h>
#include <SparkFun_VL53L1X.h>
#include <Filter.h>

#include "display.h"

//extern SFEVL53L1X distanceSensor;
extern ExponentialFilter<float> SensorFilter;

#define SENSOR_CONVERSION 0.0393700787401575 // mm to inch

class DistSensor {
  public:
    DistSensor();
    void Do_Measurement(int _initial=0);
    void SensorSetup();
    bool new_measurement = false;
  private:
    SFEVL53L1X distanceSensor;
};
