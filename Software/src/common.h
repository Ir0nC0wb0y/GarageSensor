#pragma once

#include <FastLED.h>
#include <colorutils.h>
#include <Filter.h>
#include "Settings.h"


// Display Data
#define NUM_LEDS 24
#define LED_DATA D5
extern CRGB leds[NUM_LEDS];
extern ExponentialFilter<float> SensorFilter;


// Setting value Name Translation
//STOP_LIMIT -> DIST_MIN      -> STATE5
//OK_END     -> DIST_THRESH_3 -> STATE4
//GOOD_END   -> DIST_THRESH_2 -> STATE3
//GOOD_START -> DIST_THRESH_1 -> STATE2
//FAR_START  -> DIST_MAX      -> STATE1

extern Settings settings; // This is the new website configuration method


#define STATE_COLOR_1     DarkBlue
#define STATE_COLOR_2     Cyan
#define STATE_COLOR_3     Green
#define STATE_COLOR_4     Yellow
#define STATE_COLOR_5_ON  Red
#define STATE_COLOR_5_OFF DarkRed //DarkRed, maybe DarkOrchid, Crimson, MidnightBlue, Navy
#define STATE_COLOR_ERR   Purple