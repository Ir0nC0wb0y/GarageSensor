//#include <Filter.h>
#include <FastLED.h>

// Filter Data
//ExponentialFilter<float> SensorFilter(75, 0);
//ExponentialFilter<float> SensorTime(25, 0);

// Display Data
#define NUM_LEDS 24
#define LED_DATA D5
extern CRGB leds[NUM_LEDS];

// Distance Values
// (hardcoding is temporary)
#define DIST_MAX      140
#define DIST_THRESH_1  60
#define DIST_THRESH_2  50
#define DIST_THRESH_3  40
#define DIST_MIN       20

#define STATE_COLOR_1 DarkBlue
#define STATE_COLOR_2 Cyan
#define STATE_COLOR_3 Green
#define STATE_COLOR_4 Yellow
#define STATE_COLOR_5 Red
#define STATE_COLOR_ERR Purple