#include <Arduino.h>
#include "DistanceSensor.h"

DistSensor::DistSensor() {
  return;
}

void DistSensor::SensorSetup() {
  Serial.println("Starting distance sensor:");
  if (distanceSensor.begin() != 0) { //Begin returns 0 on a good init 
    Serial.println("Sensor failed to begin. Retrying in 30 seconds");
    bool sensor_start = false;
    while (!sensor_start) {
      rainbow_show(0,50,10);
      Serial.println("Re-attempting sensor");
      distanceSensor.begin();
      sensor_start = distanceSensor.checkBootState();
      Serial.println("Sensor failed to begin. Retrying in 30 seconds");
    }
  }
  Serial.println("Sensor online!");
  distanceSensor.setDistanceModeLong();
  distanceSensor.setTimingBudgetInMs(50);
  distanceSensor.setIntermeasurementPeriod(50);
  distanceSensor.startRanging();
}

void DistSensor::Do_Measurement(int _initial) {
  if (distanceSensor.checkForDataReady()) {
    new_measurement = true;
    while (!distanceSensor.checkForDataReady()) {
      delay(1);
    }
    int sensor_raw = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
    if (_initial == 0) {
      SensorFilter.Filter(sensor_raw * SENSOR_CONVERSION);
    } else {
      SensorFilter.SetCurrent(sensor_raw * SENSOR_CONVERSION);
      }
    distanceSensor.clearInterrupt();
  }

  if (new_measurement) {
    Do_Display(SensorFilter.Current());
    new_measurement = false;
  }
}

