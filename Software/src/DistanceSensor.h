#pragma once
#include <Arduino.h>
#include <SparkFun_VL53L1X.h>
#include <Filter.h>



//extern SFEVL53L1X distanceSensor;
extern ExponentialFilter<float> SensorFilter;
extern ExponentialFilter<float> SensorChange;

#define SENSOR_CONVERSION 0.0393700787401575 // mm to inch
#define SENSOR_TIMING_BUDGET 50
#define SENSOR_INTER_PERIOD  50

#define FILTER_WEIGHT_GOOD   50
#define FILTER_WEIGHT_POOR    5

class DistSensor {
  public:
    DistSensor();
    void Do_Measurement(bool _initial = false);
    void SensorSetup();
    bool new_measurement = false;
  private:
    SFEVL53L1X distanceSensor;
    float sensor_last = 0.0;
};
