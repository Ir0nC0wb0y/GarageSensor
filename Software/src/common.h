#include <FastLED.h>
#include <colorutils.h>
//#include "Menu.h"

// Display Data
#define NUM_LEDS 24
#define LED_DATA D5
extern CRGB leds[NUM_LEDS];

// Menu
//extern Menu menu;

// Distance Values
#define DIST_MAX      120 // maximum start of sensor readings
#define DIST_THRESH_1  45 // beginning of acceptable range
#define DIST_THRESH_2  40 // preferred parking distance
#define DIST_THRESH_3  20 // end of acceptable range
#define DIST_MIN       10 // start of danger

#define STATE_COLOR_1     DarkBlue
#define STATE_COLOR_2     Cyan
#define STATE_COLOR_3     Green
#define STATE_COLOR_4     Yellow
#define STATE_COLOR_5_ON  Red
#define STATE_COLOR_5_OFF DarkRed //DarkRed, maybe DarkOrchid, Crimson, MidnightBlue, Navy
#define STATE_COLOR_ERR   Purple