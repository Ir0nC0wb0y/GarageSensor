#include <FastLED.h>
#include <NewPing.h>

// Debugging
#define DEBUG 1

// Used by FastLED
#define NUM_LEDS 24
#define LED_HALF NUM_LEDS/2
#define DATA_PIN 3

// Used by NewPing
#define TRIGGER_PIN   6
#define ECHO_PIN      5
//#define MAX_DISTANCE 400
#define MEDIAN_IT    10

// Used by States
#define DIST_MAX 400
#define DIST_STAGING_INT 100

// Used for Too Close Animation
#define TC_ANI 2

// Animation Colors
#define COL_ENTER Green
#define COL_STAGE_PRI Green
#define COL_STAGE_SEC Yellow
#define COL_GOOD Red
#define COL_STOP Purple

CRGB leds[NUM_LEDS];
NewPing sonar(TRIGGER_PIN, ECHO_PIN, DIST_MAX); // NewPing setup of pins and maximum distance.

// LED Variables
int LED_number = 0;
int LED_color = 240;
int LED_state = 1;
long LED_last_time = 0;

// Distance Variables
  // These variables are currently "INT" but should become floats eventually
int DIST_set;
int DIST_read;
int DIST_set_range = 10;
int DIST_staging;
int DIST_stop = 10;

// State Variable
int STATE = 0;
int fset_cnt = 0;
#define FSET_NUM 10

/*
// These palettes were used in development, but have been disused in favor of more simple animations.

DEFINE_GRADIENT_PALETTE( too_far ) { // row: palette index, R, G, B
      0,   0, 255,   0,    // Green
     60, 255, 255,   0,    // Yellow
    240, 255,   0,   0,    // Red
    255,   0,   0,   0};   // Black

DEFINE_GRADIENT_PALETTE( MYrainbow ) { // row: palette index, R, G, B
      0, 255,   0,   0,    // Green
     48, 255, 165,   0,    // Orange
     96, 255, 255,   0,    // Yellow
    144,   0, 255,   0,    // Green
    192,   0,   0, 255,    // Blue
    240, 128,   0, 128,    // Purple
    255,   0,   0,   0};   // Black
*/


void setup() {
  if ( DEBUG ) { Serial.begin(9600); Serial.println("Test Start NOW"); }
  FastLED.addLeds<WS2812B,DATA_PIN,GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
}

void loop(){
	// To reduce the load of analogRead, the set point is only read every FSET_NUM loop iterations.
  if (fset_cnt > FSET_NUM) {
    fSET_dist_set();
    fset_cnt = 0;
  } else {
    fset_cnt++;
  }

  fGet_Distance(2);

  fState();
  
  FastLED.show();
  
  delay(20); // a delay is required for the NewPing library
}

void fGet_Distance(int method) {
  if (method == 1) {
    // Used for debugging, this mode just reads from analog pin A1
    DIST_read = map(analogRead(A1),0,1023,0,DIST_MAX);
  } else if (method == 2) {
    // This method uses the ultrasonic sensor defined above for the NewPing library
    int time_read;
    time_read = sonar.ping_median(MEDIAN_IT); // reads distance MEDIAN_IT times (to reduce extraneous distance reads)
    DIST_read = sonar.convert_cm(time_read); // converts the time returned into a usable distance
  }
  if (DEBUG) {Serial.print("DIST_read: "); Serial.println(DIST_read);}
}

void fState() {
  // Logic to determine state
  if ( DIST_read > DIST_staging ) {
    // check for case 0 (Too far away)
    STATE = 0; // should this state spawn an action?
  
  } else if (DIST_read <= DIST_staging && DIST_read > DIST_set + DIST_set_range ) {
    // check for case 1 (Staging)
    STATE = 1;
    
  } else if (DIST_read <= DIST_set + DIST_set_range && DIST_read >= DIST_set - DIST_set_range){
    // check for case 2 (Good)
    STATE = 2;
    
  } else if (DIST_read < DIST_set - DIST_set_range && DIST_read > DIST_stop) {
    // check for case 3 (Too Close)
    STATE = 3;
    
  } else if (DIST_read <= DIST_stop) {
    // check for case 4 (Stop, Stop, STOP)
    STATE = 4;
  
  } else {
    // Exception handling
    STATE = 5;
    //fANI_Exception; // placeholder for future development
  }

  // Move on to next function
  if (DEBUG) { Serial.print("STATE: "); Serial.println(STATE);}
  if (STATE >=0 && STATE <= 4) {
    fLED_set_ring(STATE);
  }
}

void fSET_dist_set() {
  // Routine sets the "good" center distance, making sure it is within bounds
  int dist_set_read;
  dist_set_read = map(analogRead(A0),0,1023,DIST_stop,DIST_MAX);

  // Logic assumes dist_set_read cannot be less than DIST_stop
  if (dist_set_read < DIST_MAX - DIST_STAGING_INT - DIST_set_range && dist_set_read > DIST_stop + 20) {
    DIST_set = dist_set_read;
    DIST_staging = DIST_set + DIST_set + DIST_STAGING_INT;
  } else if (dist_set_read > DIST_MAX - DIST_STAGING_INT && dist_set_read < DIST_MAX) {
    DIST_set = dist_set_read;
    DIST_staging = DIST_MAX;
  } else if (dist_set_read < DIST_stop + 20 ) {
    DIST_set = DIST_stop + 20;
    DIST_staging = DIST_set + DIST_STAGING_INT + DIST_set_range;
  } else {
    // catches
    DIST_set = DIST_MAX - 25;
    DIST_staging = DIST_MAX;
  }
  if (DEBUG) {Serial.print("DIST_set: "); Serial.print(DIST_set); Serial.print(" DIST_staging: "); Serial.println(DIST_staging); }
}


void fLED_set_ring(int LED_animation) {
  switch (LED_animation) {
    case 0: // "Too Far Away"      (entering measurable distance)
    leds[0] = CRGB::COL_ENTER;
    fANI_Set_led0();
      break;
      
    case 1: // "Staging"           (meaningful distance, but outside of parking zone
      // Animation for initial entering (and exiting), cascade down both sides from top center
      int LED_ring_qty;
      LED_ring_qty = map(DIST_read,DIST_set+DIST_set_range,DIST_staging,0,LED_HALF);
      if (DEBUG) { Serial.print("LED_ring_qty: "); Serial.println(LED_ring_qty);}
      
      fANI_Staging(LED_ring_qty);
      break;
      
    case 2: // "Good"              (parking zone)
      leds[0] = CRGB::COL_GOOD;
      fANI_Set_led0();
      break;
      
    case 3: // "Too Close"         (area inside of parking zone, but not impossible)
      // This routine could take 2 animations: flash and staging

      // Temporary Animation
      if (TC_ANI == 0) {
        leds[0] = CRGB::Teal;
        fANI_Set_led0();
      }

      // flash Animation
      if (TC_ANI == 1) {
        
      }

      // Staging Animation
      if (TC_ANI == 2) {
        // map value from 0 to 12
        int LED_ring_qty;
        LED_ring_qty = map(DIST_read,DIST_stop,DIST_set-DIST_set_range,0,NUM_LEDS);
        fANI_TC_stage(LED_ring_qty);
      }
      break;
      
    case 4: // "Stop, Stop, STOP"  (area that starts to be too close for comfort)
      leds[0] = CRGB::COL_STOP;
      fANI_Set_led0();
      break;
  }
}

void fANI_Set_led0() {
  for ( int i = 1; i <= NUM_LEDS-1; i++) {
      leds[i] = leds[0];
    }
}


void fANI_Staging(int ring_qty) {
  // for loop for each side - Right/Forward
    for (int i=0; i<LED_HALF; i++) {
      // Iterate forward through right side LED's
      if ( i <= ring_qty ) {
        leds[i] = CRGB::COL_STAGE_PRI; //                                           Green is static!!!
      } else {
        leds[i] = CRGB::COL_STAGE_SEC; //                                          Yellow is static!!!
      }
    }
    for (int i=23; i>= LED_HALF; i--) {
      // Iterate BACKWARD through left side LED's
      if ( i >= NUM_LEDS-ring_qty-1) {
        leds[i] = CRGB::Green;
      } else {
        leds[i] = CRGB::COL_STAGE_SEC;
      }
    }
}

void fANI_TC_stage(int ring_qty) {
  // for loop for each side - Right/Forward
    for (int i=0; i<NUM_LEDS; i++) {
      // Iterate forward through right side LED's
      if ( NUM_LEDS - i >= ring_qty ) {
        leds[i] = CRGB::COL_STOP; //                                           Green is static!!!
      } else {
        leds[i] = CRGB::COL_GOOD; //                                          Yellow is static!!!
      }
    }
}

/*
void fANI_Flash() {
  
}
*/

/*
void fANI_Exception() {
  
}*/
