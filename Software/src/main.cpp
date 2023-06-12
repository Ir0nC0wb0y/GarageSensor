// External Libraries
#include <Arduino.h>
#include <Wire.h>

#include <Filter.h>
//#include <FastLED.h>
//#include <ESPRotary.h>
//#include <Button2.h>

// Internal Modules
#include "common.h"
#include "display.h"
#include "DistanceSensor.h"
//#include "Menu.h"

// Setup I2C Bus (Wire)
#define SENSOR_SCL D1
#define SENSOR_SDA D2

// Initialize Sensor
DistSensor distSensor;

// Initialize Filter
ExponentialFilter<float> SensorFilter(75, 0);

// Loop Parameters
#define LOOP_TIME 500 // 2 measurements printed per second
unsigned long loop_next = 0;

// Display Setup
CRGB leds[NUM_LEDS];



void setup() {
  // Setup Serial
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
  rainbow_show(5000,75,10);

  // Setup Sensor
  Wire.begin(SENSOR_SDA,SENSOR_SCL);
  distSensor.SensorSetup();
  

  // Setup Filter, set initial value
  distSensor.Do_Measurement(1);
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
  distSensor.Do_Measurement();
  //if (new_measurement & !menu.activeMenu()) {
  

  if (millis() >= loop_next) {
    Serial.print("Distance(in): ");  Serial.println(SensorFilter.Current(), 1); //Serial.print("\tDistance, raw: ");  Serial.println(sensor_raw_float, 3);
    loop_next = millis() + LOOP_TIME;
  }

}

