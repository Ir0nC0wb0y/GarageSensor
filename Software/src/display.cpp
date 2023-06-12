#include "common.h"

// Setup Variables
  int distance_state = 0;
  bool ds5_last               = false;
  unsigned int ds5_flash_next =     0;
  #define DS5_FLASH_TIME          250

//extern int distance_state;

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
    #define y0_prime 1
    #define yf       NUM_LEDS
    #define yf_prime 1
    
    // coeff d
    rng_d[range_index] = y0;
    
    // coeff c
    rng_c[range_index] = y0_prime;

    // coeff a
    rng_a[range_index] = (1 / (dx_2)) * (yf_prime - rng_c[range_index] - (2 / dx) * (yf - rng_c[range_index] * dx - rng_d[range_index]));

    // coeff b
    rng_b[range_index] = (yf - rng_c[range_index] * dx - rng_d[range_index] - rng_a[range_index] * (dx_2 * dx)) / (dx * dx);

    Serial.print("Range "); Serial.print(range); Serial.print(" a:"); Serial.print(rng_a[range_index],8); Serial.print(" b:"); Serial.print(rng_b[range_index],8); Serial.print(" c:"); Serial.print(rng_c[range_index],8); Serial.print(" d:"); Serial.println(rng_d[range_index],8);
}

void Set_Range_coefs() {
  /*
  // Range 1
  Range_coefs(1,menu.getThresh_1(), menu.getThresh_Max());
  // Range 2
  Range_coefs(2,menu.getThresh_2(), menu.getThresh_1());
  // Range 3
  Range_coefs(3,menu.getThresh_3(),menu.getThresh_2());
  // Range 4
  Range_coefs(4,menu.getThresh_Min(), menu.getThresh_3());
  */
  
  // Range 1
  Range_coefs(1,DIST_THRESH_1, DIST_MAX);
  // Range 2
  Range_coefs(2,DIST_THRESH_2, DIST_THRESH_1);
  // Range 3
  Range_coefs(3,DIST_THRESH_3,DIST_THRESH_2);
  // Range 4
  Range_coefs(4,DIST_MIN, DIST_THRESH_3);
}

void Set_Distance_State(float dist_compare) {
  //distance states:
  // 0:                 distance >= DIST_MAX
  // 1:      DIST_MAX > distance >= DIST_THRESH_1
  // 2: DIST_THRESH_1 > distance >= DIST_THRESH_2
  // 3: DIST_THRESH_2 > distance >= DIST_THRESH_3
  // 4: DIST_THRESH_3 > distance >= DIST_MIN
  // 5:      DIST_Min > distance
  /*
  if (dist_compare >= menu.getThresh_Max()){
    distance_state = 0;
  } else if (menu.getThresh_Max() > dist_compare and dist_compare >= menu.getThresh_1()) {
    distance_state = 1;
  } else if (menu.getThresh_1() > dist_compare and dist_compare >= menu.getThresh_2()) {
    distance_state = 2;
  } else if (menu.getThresh_2() > dist_compare and dist_compare >= menu.getThresh_3()) {
    distance_state = 3;
  } else if (menu.getThresh_3() > dist_compare and dist_compare >= menu.getThresh_Min()) {
    distance_state = 4;
  } else if (menu.getThresh_Min() > dist_compare) {
    distance_state = 5;
  } else {
    distance_state = -1;
  }
  */

  int distance_state_last = distance_state;

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
    if (distance_state != distance_state_last) {
      ds5_last = true;
      ds5_flash_next = millis() + DS5_FLASH_TIME;
    }
  } else {
    distance_state = -1;
  }

}

void Do_Display(float sensor_value) {
  Set_Distance_State(sensor_value);
  int led_good = 0;
  float x_adj = 0.0;
  switch (distance_state) {
    case 0:
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
          leds[i] = CRGB::STATE_COLOR_1;
      }
      break;
    case 1:
      x_adj = sensor_value - DIST_THRESH_1;
      led_good = (rng_a[0] * pow(x_adj,3) + rng_b[0] * pow(x_adj,2) + rng_c[0] * x_adj + rng_d[0]);
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        if (i < led_good) {
          leds[i] = CRGB::STATE_COLOR_1;
        } else {
          leds[i] = CRGB::STATE_COLOR_2;
        }
      }
      break;
    case 2:
      x_adj = sensor_value - DIST_THRESH_2;
      led_good = (rng_a[1] * pow(x_adj,3) + rng_b[1] * pow(x_adj,2) + rng_c[1] * x_adj + rng_d[1]);
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        if (i < led_good) {
          leds[i] = CRGB::STATE_COLOR_2;
        } else {
          leds[i] = CRGB::STATE_COLOR_3;
        }
      }
      break;
    case 3:
      x_adj = sensor_value - DIST_THRESH_3;
      led_good = (rng_a[2] * pow(x_adj,3) + rng_b[2] * pow(x_adj,2) + rng_c[2] * x_adj + rng_d[2]);
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        if (i < led_good) {
          leds[i] = CRGB::STATE_COLOR_3;
        } else {
          leds[i] = CRGB::STATE_COLOR_4;
        }
      }
      break;
    case 4:
      x_adj = sensor_value - DIST_MIN;
      led_good = (rng_a[3] * pow(x_adj,3) + rng_b[3] * pow(x_adj,2) + rng_c[3] * x_adj + rng_d[3]);
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        if (i < led_good) {
          leds[i] = CRGB::STATE_COLOR_4;
        } else {
          leds[i] = CRGB::STATE_COLOR_5_ON;
        }
      }
      break;
    case 5:
      if (ds5_last) {
        for ( int i = 0; i <= NUM_LEDS-1; i++) {
          leds[i] = CRGB::STATE_COLOR_5_ON;
        }
      } else {
        for ( int i = 0; i <= NUM_LEDS-1; i++) {
          leds[i] = CRGB::STATE_COLOR_5_OFF;
        }
      }
      if (millis() >= ds5_flash_next) {
        ds5_last = !ds5_last;
        ds5_flash_next = millis() + DS5_FLASH_TIME;
      }
      break;
    default:
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        leds[i] = CRGB::STATE_COLOR_ERR;
      }
      break;
  }
  FastLED.show();
}

void rainbow_show(unsigned int rainbow_duration, int thisSpeed, int deltaHue) {
  if (rainbow_duration < 10) {
    rainbow_duration = 30000;
  }
  
  unsigned int rainbow_time = millis();
  Serial.print("Starting Rainbow for ");
    Serial.print(rainbow_duration/1000);
    Serial.println(" seconds");
  while (millis() - rainbow_time < rainbow_duration) {
    // uint8_t thisHue = beatsin8(thisSpeed,0,255);                // A simple rainbow wave.
    uint8_t thisHue = beat8(thisSpeed,255);                     // A simple rainbow march.
      
    fill_rainbow(leds, NUM_LEDS, thisHue, deltaHue);            // Use FastLED's fill_rainbow routine.
    FastLED.show();
    yield();
  }
}
