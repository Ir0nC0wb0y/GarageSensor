#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <FastLED.h>

#define NUM_LEDS 24

extern CRGB leds[NUM_LEDS];

// Menu will proceed thusly:
//  1 - Set Acceptable Length Target
//  2 - Set Danger Length
//  3 - Set Acceptable Length Maximum
//  4 - Set Acceptable Length Minimum


class Menu {
    protected:
        #define _DEFAULT_THRESH_MAX 120.0
        #define _DEFAULT_THRESH_1    84.0
        #define _DEFAULT_THRESH_2    60.0
        #define _DEFAULT_THRESH_3    50.0
        #define _DEFAULT_THRESH_MIN  40.0
        int   _menu_item   =     0;
        float _thresh_temp =   0.0;
        float _thresh_max  = _DEFAULT_THRESH_MAX; // max dependable range         (black)
        float _thresh_1    = _DEFAULT_THRESH_1;   // acceptable length maximum    (cyan)
        float _thresh_2    = _DEFAULT_THRESH_2;   // acceptable length target     (green)
        float _thresh_3    = _DEFAULT_THRESH_3;   // acceptable length minimum    (yellow)
        float _thresh_min  = _DEFAULT_THRESH_MIN; // danger                       (red)
        #define MIN_WIDTH      4.0
        float _temp_min    =   0.0;
        float _temp_max    =   0.0;

        unsigned long _blink_next = 0;
        #define _BLINK_TIME 500
        int led_almax     = 0;
        int led_altarget  = 0;
        int led_almin     = 0;
        int led_danger    = 0;
        
        void _Run_menu(int encoder_value);
    
    public:
        Menu();
        void begin(bool first_start = false);
        void loop(int enc_value);
        void Advance(bool start_check = false);
        void Exit();
        void Reset();
        
        int getMenuItem();

};

