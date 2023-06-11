// External Libraries
#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_VL53L1X.h>
#include <Filter.h>
#include <FastLED.h>
//#include <ESPRotary.h>
//#include <Button2.h>

// Internal Modules
#include "common.h"
#include "display.h"
//#include "Menu.h"

// Setup sensor
SFEVL53L1X distanceSensor;
#define SENSOR_SCL D1
#define SENSOR_SDA D2
#define SENSOR_CONVERSION 0.0393700787401575 // mm to inch
bool new_measurement = false;

// Setup Filter
ExponentialFilter<float> SensorFilter(75, 0);
unsigned long sensor_loop = 0;

// Loop Parameters
#define LOOP_TIME 500 // 2 measurements printed per second
unsigned long loop_next = 0;

// Display Setup
CRGB leds[NUM_LEDS];
/*
// Setup Menu
  // Rotary Encoder
  #define ROTARY_A D6
  #define ROTARY_B D7
  #define ROTARY_STEPS_CLICK 4
  ESPRotary r;

  // Button
  #define ROTARY_BUTTON D3
  Button2 b;

  // Menu
  Menu menu;

  // Button Routines
  void Button_Click(Button2& btn) {
    menu.Advance();
    r.resetPosition(0);
  }

  void Button_Double(Button2& btn) {
    r.resetPosition();
    menu.begin();
  }

  void Button_Long(Button2& btn) {
    menu.Reset();
  }
*/

void Do_Measurement(int _initial=0) {
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
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println("Starting sketch");
  Serial.println();

  // Setup Display
  FastLED.addLeds<WS2812B, LED_DATA, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
  // Sets all LED's to one color (Black)
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  rainbow_show(500,2);

  // Setup Sensor
  Wire.begin(SENSOR_SDA,SENSOR_SCL);
  Serial.println("Starting distance sensor:");
  if (distanceSensor.begin() != 0) { //Begin returns 0 on a good init 
    Serial.println("Sensor failed to begin. Retrying in 30 seconds");
    bool sensor_start = false;
    while (!sensor_start) {
      rainbow_show(0);
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

  // Setup Filter, set initial value
  Do_Measurement(1);
  loop_next = millis();

  /*
  // Setup Input (Encoder and Button)
  r.begin(ROTARY_A,ROTARY_B,ROTARY_STEPS_CLICK);
  b.begin(ROTARY_BUTTON);
  b.setClickHandler(Button_Click);
  b.setDoubleClickHandler(Button_Double);
  b.setLongClickTime(2000);
  b.setLongClickHandler(Button_Long);
  */

  // Set Range Coefficients
  Set_Range_coefs(); // adding this function now will make it easier to recalc on the fly, when there is some user engagement
  }

void loop() {
  /*
  r.loop();
  b.loop();
  menu.loop(r.getPosition());
  if (menu.newValues()) {
    Set_Range_coefs();
  }
  */
  Do_Measurement();
  //if (new_measurement & !menu.activeMenu()) {
  if (new_measurement) {
    Do_Display(SensorFilter.Current());
    new_measurement = false;
  }

  if (millis() >= loop_next) {
    Serial.print("Distance(in): ");  Serial.println(SensorFilter.Current(), 1); //Serial.print("\tDistance, raw: ");  Serial.println(sensor_raw_float, 3);
    loop_next = millis() + LOOP_TIME;
  }

}

