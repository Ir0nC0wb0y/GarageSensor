#include "display.h"
#include "common.h"

// Setup Variables
  // import from other modules
  extern ExponentialFilter<float> SensorFilter;
  extern ExponentialFilter<float> SensorChange;
  extern Settings settings;

  // Basic Deisplay parameters
  int distance_state               =     0;
  bool ds5_last                    = false;
  unsigned int ds5_flash_next      =     0;
  #define DS5_FLASH_TIME               250

  // Display refresh rate
  unsigned long display_refresh_last =    0;
  #define DISPLAY_REFRESH              100  // display refresh rate in ms
  
  // Variables for turning off the display if no change in measurement
  unsigned long display_m_time      =     0;
  //float display_m_last             =   0.0;
  #define DISPLAY_M_THRESH             0.25 // this requires a minimum speed of thresh/refresh [in/ms]
  //#define DISPLAY_M_TIME            300000

// Display 

//extern int distance_state;

//boolean coefficient_state = false;
float rng_d[4] = {0.0, 0.0, 0.0, 0.0};
float rng_c[4] = {0.0, 0.0, 0.0, 0.0};
float rng_b[4] = {0.0, 0.0, 0.0, 0.0};
float rng_a[4] = {0.0, 0.0, 0.0, 0.0};

void Display_Setup() {
  Serial.println("Setting  up display");
  FastLED.addLeds<WS2812B, LED_DATA, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);
  // Sets all LED's to one color (Black)
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  //rainbow_show(500,75,10);
}

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
  // Range 1
  Range_coefs(1,settings.GoodStart, settings.FarStart);
  // Range 2
  Range_coefs(2,settings.GoodEnd, settings.GoodStart);
  // Range 3
  Range_coefs(3,settings.OkEnd,settings.GoodEnd);
  // Range 4
  Range_coefs(4,settings.StopLimit, settings.OkEnd);

  Serial.println("Updated range coefficients");
}

void Set_Distance_State(float dist_compare) {
  //distance states:
  // 0:                 distance >= DIST_MAX
  // 1:      DIST_MAX > distance >= DIST_THRESH_1
  // 2: DIST_THRESH_1 > distance >= DIST_THRESH_2
  // 3: DIST_THRESH_2 > distance >= DIST_THRESH_3
  // 4: DIST_THRESH_3 > distance >= DIST_MIN
  // 5:      DIST_Min > distance

  int distance_state_last = distance_state;

  if (dist_compare >= settings.FarStart){
    distance_state = 0;
  } else if (settings.FarStart > dist_compare and dist_compare >= settings.GoodStart) {
    distance_state = 1;
  } else if (settings.GoodStart > dist_compare and dist_compare >= settings.GoodEnd) {
    distance_state = 2;
  } else if (settings.GoodEnd > dist_compare and dist_compare >= settings.OkEnd) {
    distance_state = 3;
  } else if (settings.OkEnd > dist_compare and dist_compare >= settings.StopLimit) {
    distance_state = 4;
  } else if (settings.StopLimit > dist_compare) {
    distance_state = 5;
    if (distance_state != distance_state_last) {
      ds5_last = true;
      ds5_flash_next = millis() + DS5_FLASH_TIME;
    }
  } else {
    distance_state = -1;
  }

}


void Display_State0(float sensor_value) {
  //Serial.print("Distance state 0");
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
      leds[i] = CRGB::STATE_COLOR_1;
      //Serial.print(".");
  }
  FastLED.show();
  //Serial.println("complete!");
}

void Display_State1(float sensor_value) {
  //Serial.print("Distance state 1");
  float x_adj = sensor_value - settings.GoodStart;
  int led_good = (rng_a[0] * pow(x_adj,3) + rng_b[0] * pow(x_adj,2) + rng_c[0] * x_adj + rng_d[0]);
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
    if (i < led_good) {
      leds[i] = CRGB::STATE_COLOR_1;
    } else {
      leds[i] = CRGB::STATE_COLOR_2;
    }
    //Serial.print(".");
  }
  FastLED.show();
  //Serial.println("complete!");
}

void Display_State2(float sensor_value) {
  //Serial.print("Distance state 2");
  float x_adj = sensor_value - settings.GoodEnd;
  int led_good = (rng_a[1] * pow(x_adj,3) + rng_b[1] * pow(x_adj,2) + rng_c[1] * x_adj + rng_d[1]);
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
    if (i < led_good) {
      leds[i] = CRGB::STATE_COLOR_2;
    } else {
      leds[i] = CRGB::STATE_COLOR_3;
    }
    //Serial.print(".");
  }
  FastLED.show();
  //Serial.println("complete!");
}

void Display_State3(float sensor_value) {
  //Serial.print("Distance state 3");
  float x_adj = sensor_value - settings.OkEnd;
  int led_good = (rng_a[2] * pow(x_adj,3) + rng_b[2] * pow(x_adj,2) + rng_c[2] * x_adj + rng_d[2]);
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
    if (i < led_good) {
      leds[i] = CRGB::STATE_COLOR_3;
    } else {
      leds[i] = CRGB::STATE_COLOR_4;
    }
    //Serial.print(".");
  }
  FastLED.show();
  //Serial.println("complete!");
}

void Display_State4(float sensor_value) {
  //Serial.print("Distance state 4");
  float x_adj = sensor_value - settings.StopLimit;
  int led_good = (rng_a[3] * pow(x_adj,3) + rng_b[3] * pow(x_adj,2) + rng_c[3] * x_adj + rng_d[3]);
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
    if (i < led_good) {
      leds[i] = CRGB::STATE_COLOR_4;
    } else {
      leds[i] = CRGB::STATE_COLOR_5_ON;
    }
    //Serial.print(".");
  }
  FastLED.show();
  //Serial.println("complete!");
}

void Display_State5(float sensor_value) {
  //Serial.print("Distance state 5");
  if (ds5_last) {
    for ( int i = 0; i <= NUM_LEDS-1; i++) {
      leds[i] = CRGB::STATE_COLOR_5_ON;
      //Serial.print(".");
    }
  } else {
    for ( int i = 0; i <= NUM_LEDS-1; i++) {
      leds[i] = CRGB::STATE_COLOR_5_OFF;
      //Serial.print(".");
    }
  }
  if (millis() >= ds5_flash_next) {
    ds5_last = !ds5_last;
    ds5_flash_next = millis() + DS5_FLASH_TIME;
  }
  FastLED.show();
  //Serial.println("complete!");
}

void Do_Display() {
  // Do logic for refresh
  bool display_refresh = false;
  if (millis() - display_refresh_last >= DISPLAY_REFRESH) {
    display_refresh = true;
    //Serial.print("Time since last display: ");
      //Serial.println(millis() - display_refresh_last);
    display_refresh_last = millis();
  } else {
    return;
  }

  float sensor_value = SensorFilter.Current();
  // Do logic for measurement change
  bool display_m = false;
  //Serial.print("Filtered sensor change: ");
    //Serial.println(SensorChange.Current());
  if (abs(SensorChange.Current()) > DISPLAY_M_THRESH) {
    display_m = true;
    if (display_m_time != 0) {
      display_m_time = 0;
      Serial.println("Cleared display timeout");
    }
  } else {
    if (display_m_time != 0) {
      if (millis() - display_m_time <= settings.Timeout_ms) {
        display_m = true;
        //Serial.print("Display timout in ");
          //Serial.println(millis() - display_m_time);
      } else {
        //Serial.println("Display timed out due to measurement change");
        for ( int i = 0; i <= NUM_LEDS-1; i++) {
          leds[i] = CRGB::Black;
        }
        FastLED.show();
      }
    } else {
      display_m = true;
      display_m_time = millis();
      Serial.println("Setting display timeout");
    } 
  }
  
  if (display_refresh && display_m) {
    Set_Distance_State(sensor_value);
    if (distance_state == 0) {
      Display_State0(sensor_value);
    } else if (distance_state == 1) {
      Display_State1(sensor_value);
    } else if (distance_state == 2) {
      Display_State2(sensor_value);
    } else if (distance_state == 3) {
      Display_State3(sensor_value);
    } else if (distance_state == 4) {
      Display_State4(sensor_value);
    } else if (distance_state == 5) {
      Display_State5(sensor_value);
    } else {
      Serial.println("An error has occurred!");
      for ( int i = 0; i <= NUM_LEDS-1; i++) {
        leds[i] = CRGB::STATE_COLOR_ERR;
      }
      FastLED.show();
    }
  }
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

void rainbow_frame(int thisSpeed) {
  uint8_t thisHue = beat8(thisSpeed,255);                     // A simple rainbow march.
  fill_rainbow(leds, NUM_LEDS, thisHue, 10);            // Use FastLED's fill_rainbow routine.
  FastLED.show();
}
