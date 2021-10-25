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
ExponentialFilter<float> SensorFilter(75, 0);
ExponentialFilter<float> SensorTime(25, 0);
unsigned long sensor_loop = 0;

// Loop Parameters
#define LOOP_TIME 500 // 2 measurements printed per second
unsigned long loop_next = 0;

// Display Setup
#include "FastLED.h"
#define NUM_LEDS 24
#define LED_DATA D5
CRGB leds[NUM_LEDS];

// Temporary Distance Values
#define DIST_MAX      140
#define DIST_THRESH_1  80
#define DIST_THRESH_2  40
#define DIST_THRESH_3  18
#define DIST_MIN        4
int distance_state = 0;
    //distance states:
    // 0:                 distance >= DIST_MAX
    // 1:      DIST_MAX > distance >= DIST_THRESH_1
    // 2: DIST_THRESH_1 > distance >= DIST_THRESH_2
    // 3: DIST_THRESH_2 > distance >= DIST_THRESH_3
    // 4: DIST_THRESH_3 > distance >= DIST_MIN
    // 5:      DIST_Min > distance
#define STATE_COLOR_1 DarkBlue
#define STATE_COLOR_2 Cyan
#define STATE_COLOR_3 Green
#define STATE_COLOR_4 Yellow
#define STATE_COLOR_5 Red
#define STATE_COLOR_ERR Purple



void Do_Display() {
  //int led_good = NUM_LEDS*((SensorFilter.Current()-DIST_MIN)/(DIST_MAX-DIST_MIN));
  int led_good = 0;
  //Serial.print("State("); Serial.print(distance_state); Serial.print(")");
  switch (distance_state) {
    case 0:
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
          leds[i] = CRGB::STATE_COLOR_1;
      }
      break;
    case 1:
      led_good = NUM_LEDS * ((SensorFilter.Current() - DIST_THRESH_1) / (DIST_MAX - DIST_THRESH_1));
      //Serial.print(", led_good: "); Serial.print(led_good);
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        if (i < led_good) {
          leds[i] = CRGB::STATE_COLOR_1;
        } else {
          leds[i] = CRGB::STATE_COLOR_2;
        }
      }
      break;
    case 2:
      led_good = NUM_LEDS * ((SensorFilter.Current() - DIST_THRESH_2) / (DIST_THRESH_1 - DIST_THRESH_2));
      //Serial.print(", led_good: "); Serial.print(led_good);
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        if (i < led_good) {
          leds[i] = CRGB::STATE_COLOR_2;
        } else {
          leds[i] = CRGB::STATE_COLOR_3;
        }
      }
      break;
    case 3:
      led_good = NUM_LEDS * ((SensorFilter.Current() - DIST_THRESH_3) / (DIST_THRESH_2 - DIST_THRESH_3));
      //Serial.print(", led_good: "); Serial.print(led_good);
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        if (i < led_good) {
          leds[i] = CRGB::STATE_COLOR_3;
        } else {
          leds[i] = CRGB::STATE_COLOR_4;
        }
      }
      break;
    case 4:
      led_good = NUM_LEDS * ((SensorFilter.Current() - DIST_MIN) / (DIST_THRESH_3 - DIST_MIN));
      //Serial.print(", led_good: "); Serial.print(led_good);
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        if (i < led_good) {
          leds[i] = CRGB::STATE_COLOR_4;
        } else {
          leds[i] = CRGB::STATE_COLOR_5;
        }
      }
      break;
    case 5:
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        leds[i] = CRGB::STATE_COLOR_5;
      }
      break;
    default:
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        leds[i] = CRGB::STATE_COLOR_ERR;
      }
      break;
  }
  //Serial.println();
  FastLED.show();
}

//void Do_Measurement(int _initial=0);
void Do_Measurement(int _initial=0) {
  int sensor_start = millis();
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
  SensorTime.Filter(millis()-sensor_start);


    //distance states:
      // 0:                 distance >= DIST_MAX
      // 1:      DIST_MAX > distance >= DIST_THRESH_1
      // 2: DIST_THRESH_1 > distance >= DIST_THRESH_2
      // 3: DIST_THRESH_2 > distance >= DIST_THRESH_3
      // 4: DIST_THRESH_3 > distance >= DIST_MIN
      // 5:      DIST_Min > distance
  float dist_compare = SensorFilter.Current();
  if (dist_compare >= DIST_MAX){
    distance_state = 0;
  } else if (DIST_MAX > dist_compare and dist_compare >= DIST_THRESH_1) {
    distance_state = 1;
  } else if (DIST_THRESH_1 > dist_compare and dist_compare >= DIST_THRESH_2) {
    distance_state = 2;
  } else if (DIST_THRESH_2 > dist_compare and dist_compare >= DIST_THRESH_3) {
    distance_state = 3;
  } else if (DIST_THRESH_3 > dist_compare and dist_compare >= DIST_MIN) {
    distance_state = 4;
  } else if (DIST_MIN > dist_compare) {
    distance_state = 5;
  } else {
    distance_state = -1;
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

  if (millis() >= sensor_loop) {
    Serial.print("Sensor Time: "); Serial.print(SensorTime.Current()); Serial.println(" ms");
    sensor_loop = millis() + 5000;
  }

}

