#include <Arduino.h>
#include "DistanceSensor.h"
#include "display.h"

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
  distanceSensor.setTimingBudgetInMs(SENSOR_TIMING_BUDGET);
  distanceSensor.setIntermeasurementPeriod(SENSOR_INTER_PERIOD);

  // Determine Sensor Settings
  Serial.print("Sensor Sigma Threshold: ");
    Serial.println(distanceSensor.getSigmaThreshold());
  Serial.print("Sensor Signal Threshold: ");
    Serial.println(distanceSensor.getSignalThreshold());

  distanceSensor.startRanging();
}

void DistSensor::Do_Measurement(bool _initial) {
  if (_initial) {
    while (!distanceSensor.checkForDataReady()) {
        delay(1);
        yield();
    }
  }
  if (distanceSensor.checkForDataReady()) {
    new_measurement = true;
    int sensor_raw = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
    _Distance = sensor_raw * SENSOR_CONVERSION;
    sensor_reading_this = millis();
    if (_initial) {
      SensorFilter.SetCurrent(_Distance);
      SensorChange.SetCurrent(0);
    }

    // Filter Weight Manipulation
    int range_status = distanceSensor.getRangeStatus();
    //Serial.print("Sensor range status: ");
      //Serial.println(range_status);
    if (range_status == 0) {
      SensorFilter.SetWeight(FILTER_WEIGHT_GOOD);
      SensorChange.SetWeight(FILTER_WEIGHT_GOOD);
    } else if (range_status == 1 || range_status == 2) {
      //Serial.print("Sensor range status ");
        //Serial.println(range_status);
      SensorFilter.SetWeight(FILTER_WEIGHT_POOR);
      SensorChange.SetWeight(FILTER_WEIGHT_POOR);
    } else {
      //Serial.print("SENSOR RANGE STATUS ");
        //Serial.println(range_status);
      distanceSensor.clearInterrupt();
      return;
    }

    
    SensorFilter.Filter(_Distance);
    if (sensor_reading_last != 0) {
      float dx = 0.0;
      float dt = 0.0;
      dx = _Distance - _Distance_last;
      dt = ((float)sensor_reading_this - (float)sensor_reading_last) / 1000.0;
      SensorChange.Filter(dx / dt); // This should have units of [in/sec]
    }
    sensor_reading_last = sensor_reading_this;
    //_sensor_last = SensorFilter.Current();
    _Distance_last = _Distance;

    distanceSensor.clearInterrupt();

    
    _SignalPerSpad = distanceSensor.getSignalPerSpad();
    _AmbientPerSpad = distanceSensor.getAmbientPerSpad();
    _SignalRate = distanceSensor.getSignalRate();
    _RangeStatus = distanceSensor.getRangeStatus();
  }

}

