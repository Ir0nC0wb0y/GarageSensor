#pragma once
#include <Arduino.h>
#include <SparkFun_VL53L1X.h>
#include <Filter.h>



//extern SFEVL53L1X distanceSensor;
extern ExponentialFilter<float> SensorFilter;
extern ExponentialFilter<float> SensorChange;

#define SENSOR_CONVERSION 0.0393700787401575 // mm to inch
#define SENSOR_TIMING_BUDGET 100
#define SENSOR_INTER_PERIOD  100

#define FILTER_WEIGHT_GOOD   20
#define FILTER_WEIGHT_POOR    2

class DistSensor {
  public:
    DistSensor();
    void Do_Measurement(bool _initial = false);
    void SensorSetup();
    bool new_measurement = false;
  private:
    SFEVL53L1X distanceSensor;
    float _Distance = 0;
    float _Distance_last = 0.0;
    uint16_t _SignalPerSpad = 0;
    uint16_t _AmbientPerSpad = 0;
    uint16_t _SignalRate = 0;
    uint16_t _RangeStatus = 0;
    unsigned long sensor_reading_this = 0;
    unsigned long sensor_reading_last = 0;
};
