//#include <FastLED.h>

#include "common.h"

extern int distance_state;

//boolean coefficient_state = false;
float rng_d[4] = {0.0, 0.0, 0.0, 0.0};
float rng_c[4] = {0.0, 0.0, 0.0, 0.0};
float rng_b[4] = {0.0, 0.0, 0.0, 0.0};
float rng_a[4] = {0.0, 0.0, 0.0, 0.0};


void Range_coefs(int range, float x0, float xf) {
    int range_index = range - 1;
    float dx = xf - x0;
    float dx_2 = dx * dx;
    #define y0       0
    #define y0_prime 0
    #define yf       1
    #define yf_prime 0
    
    // coeff d
    rng_d[range_index] = y0;
    
    // coeff c
    rng_c[range_index] = y0_prime;

    // coeff a
    rng_a[range_index] = (1 / (dx_2)) * (yf_prime - rng_c[range_index] - (2 / dx) * (yf - rng_c[range_index] * dx - rng_d[range_index]));

    // coeff b
    rng_b[range_index] = (yf - rng_c[range_index] * dx - rng_d[range_index] - rng_a[range_index] * (dx_2 * dx)) / (dx * dx);

    Serial.print("Range "); Serial.print(range); Serial.print(" a:"); Serial.print(rng_a[range_index],4); Serial.print(" b:"); Serial.print(rng_b[range_index],4); Serial.print(" c:"); Serial.print(rng_c[range_index],4); Serial.print(" d:"); Serial.println(rng_d[range_index],4);
}

void Set_Range_coefs() {
  // Range 1
  Range_coefs(1,DIST_THRESH_1, DIST_MAX);
  // Range 2
  Range_coefs(2,DIST_THRESH_2, DIST_THRESH_1);
  // Range 3
  Range_coefs(2,DIST_THRESH_3, DIST_THRESH_1);
  // Range 4
  Range_coefs(2,DIST_MIN, DIST_THRESH_3);
}

void Set_Distance_State(float dist_compare) {
  //distance states:
  // 0:                 distance >= DIST_MAX
  // 1:      DIST_MAX > distance >= DIST_THRESH_1
  // 2: DIST_THRESH_1 > distance >= DIST_THRESH_2
  // 3: DIST_THRESH_2 > distance >= DIST_THRESH_3
  // 4: DIST_THRESH_3 > distance >= DIST_MIN
  // 5:      DIST_Min > distance
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
  Set_Distance_State(sensor_value);
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
      //float x_adj = sensor_value - DIST_THRESH_1;
      //led_good = NUM_LEDS *
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