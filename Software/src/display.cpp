//#include <FastLED.h>

#include "common.h"

extern int distance_state;

void Set_Distance_State(float dist_compare) {
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

void Do_Display(float sensor_value) {
  int led_good = 0;
  //Serial.print("State("); Serial.print(distance_state); Serial.print(")");
  switch (distance_state) {
    case 0:
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
          leds[i] = CRGB::STATE_COLOR_1;
      }
      break;
    case 1:
      led_good = NUM_LEDS * ((sensor_value - DIST_THRESH_1) / (DIST_MAX - DIST_THRESH_1));
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
      led_good = NUM_LEDS * ((sensor_value - DIST_THRESH_2) / (DIST_THRESH_1 - DIST_THRESH_2));
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
      led_good = NUM_LEDS * ((sensor_value - DIST_THRESH_3) / (DIST_THRESH_2 - DIST_THRESH_3));
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
      led_good = NUM_LEDS * ((sensor_value - DIST_MIN) / (DIST_THRESH_3 - DIST_MIN));
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