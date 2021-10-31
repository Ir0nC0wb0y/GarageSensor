// External Libraries
#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_VL53L1X.h"
#include "Filter.h"
#include "FastLED.h"

// Internal Modules
#include "common.h"
#include "display.h"

// Setup sensor
SFEVL53L1X distanceSensor;
#define SENSOR_SCL D1
#define SENSOR_SDA D2
#define SENSOR_CONVERSION 0.0393700787401575 // mm to inch
//extern int distance_state = 0;

// Setup Filter
ExponentialFilter<float> SensorFilter(75, 0);
ExponentialFilter<float> SensorTime(25, 0);
unsigned long sensor_loop = 0;

// Loop Parameters
#define LOOP_TIME 500 // 2 measurements printed per second
unsigned long loop_next = 0;

// Display Setup
CRGB leds[NUM_LEDS];


//void Do_Measurement(int _initial=0);
void Do_Measurement(int _initial=0) {
  int sensor_start = millis();
  //distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
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
  //distanceSensor.stopRanging();
  SensorTime.Filter(millis()-sensor_start);


    //distance states:
      // 0:                 distance >= DIST_MAX
      // 1:      DIST_MAX > distance >= DIST_THRESH_1
      // 2: DIST_THRESH_1 > distance >= DIST_THRESH_2
      // 3: DIST_THRESH_2 > distance >= DIST_THRESH_3
      // 4: DIST_THRESH_3 > distance >= DIST_MIN
      // 5:      DIST_Min > distance
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println("Starting sketch");
  Serial.println();

  // Setup Display
  FastLED.addLeds<WS2812B, LED_DATA, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
    leds[i] = CRGB::Blue;
  }
  FastLED.show();

  // Setup Sensor
  Wire.begin(SENSOR_SDA,SENSOR_SCL);
  Serial.println("Starting distance sensor:");
  if (distanceSensor.begin() != 0) { //Begin returns 0 on a good init 
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }
  Serial.println("Sensor online!");
  distanceSensor.setDistanceModeLong();
  distanceSensor.setTimingBudgetInMs(50);
  distanceSensor.setIntermeasurementPeriod(50);
  distanceSensor.startRanging();

  // Setup Filter, set initial value
  Do_Measurement(1);
  loop_next = millis();

  
  }

void loop() {
  Do_Measurement();
  Do_Display(SensorFilter.Current());

  if (millis() >= loop_next) {
    Serial.print("Distance(in): ");  Serial.println(SensorFilter.Current(), 1); //Serial.print("\tDistance, raw: ");  Serial.println(sensor_raw_float, 3);
    loop_next = millis() + LOOP_TIME;
  }

  if (millis() >= sensor_loop) {
    Serial.print("Sensor Time: "); Serial.print(SensorTime.Current()); Serial.println(" ms");
    sensor_loop = millis() + 5000;
  }

}

