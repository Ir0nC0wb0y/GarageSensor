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
    if (!_initial) {
      SensorFilter.SetCurrent(sensor_raw * SENSOR_CONVERSION);
      SensorChange.SetCurrent(0);
    }

    int range_status = distanceSensor.getRangeStatus();
    //Serial.print("Sensor range status: ");
      //Serial.println(range_status);
    if (range_status == 0) {
      SensorFilter.SetWeight(FILTER_WEIGHT_GOOD);
      SensorChange.SetWeight(FILTER_WEIGHT_GOOD/2);
    } else if (range_status == 1 || range_status == 2) {
      Serial.print("Sensor range status ");
        Serial.println(range_status);
      SensorFilter.SetWeight(FILTER_WEIGHT_POOR);
      SensorChange.SetWeight(FILTER_WEIGHT_POOR/2);
    } else {
      Serial.print("SENSOR RANGE STATUS ");
        Serial.println(range_status);
      distanceSensor.clearInterrupt();
      return;
    }

    SensorFilter.Filter(sensor_raw * SENSOR_CONVERSION);
    SensorChange.Filter(sensor_last - SensorFilter.Current());
    sensor_last = SensorFilter.Current();

    distanceSensor.clearInterrupt();
  }

  /*
  if (new_measurement) {
    //Do_Display(SensorFilter.Current());
    Do_Display();
    //Maintain_Display();
    new_measurement = false;
  }
  */
  
  
  
}

