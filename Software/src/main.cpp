#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_VL53L1X.h"
#include "filter.h"

// Setup sensor
SFEVL53L1X distanceSensor;
#define SENSOR_SCL D1
#define SENSOR_SDA D2

#define SENSOR_CONVERSION 0.0393700787401575

// Setup Filter
#include "Filter.h"
ExponentialFilter<float> SensorFilter(50, 0);

// Loop Parameters
#define LOOP_TIME 500 // 2 measurements printed per second
unsigned long loop_next = 0;

// Display Setup
#include "FastLED.h"
#define NUM_LEDS 24
#define LED_DATA D5
CRGB leds[NUM_LEDS];

// Temporary Distance Values
#define DIST_MAX  60
#define DIST_MIN   4


void Do_Display() {
  int led_good = NUM_LEDS*((SensorFilter.Current()-DIST_MIN)/(DIST_MAX-DIST_MIN));
  //Serial.print("Setting "); Serial.print
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
    if (i < led_good) {
      leds[i] = CRGB::Green;
    } else {
      leds[i] = CRGB::Yellow;
    }
  }
  FastLED.show();
}

//void Do_Measurement(int _initial=0);
void Do_Measurement(int _initial=0) {
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
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
  distanceSensor.stopRanging();

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

  // Setup Filter, set initial value
  Do_Measurement(1);
  loop_next = millis();

  
  }

void loop() {
  Do_Measurement();
  Do_Display();

  if (millis() >= loop_next) {
    Serial.print("Distance(in): ");  Serial.println(SensorFilter.Current(), 1); //Serial.print("\tDistance, raw: ");  Serial.println(sensor_raw_float, 3);
    loop_next = millis() + LOOP_TIME;
  }

}

