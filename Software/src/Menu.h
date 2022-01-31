#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <ARduinoJSON.h>
#include <LittleFS.h>

#define NUM_LEDS 24

extern CRGB leds[NUM_LEDS];

// Menu will proceed thusly:
//  1 - Set Acceptable Length Target
//  2 - Set Danger Length
//  3 - Set Acceptable Length Maximum
//  4 - Set Acceptable Length Minimum

struct distances {
    float threshMax; // max dependable range         (black)
    float thresh1;   // acceptable length maximum    (cyan)
    float thresh2;   // acceptable length target     (green)
    float thresh3;   // acceptable length minimum    (yellow)
    float threshMin; // danger                       (red)
};

class Menu {
    protected:
        #define _DEFAULT_THRESH_MAX 120.0
        #define _DEFAULT_THRESH_1    37.0
        #define _DEFAULT_THRESH_2    22.0
        #define _DEFAULT_THRESH_3    17.0
        #define _DEFAULT_THRESH_MIN  12.0
        distances _distances;
        bool _FS_avoid = true;
        int   _menu_item   =     0;
        float _thresh_temp =   0.0;
        #define MIN_WIDTH      4.0
        float _temp_min    =   0.0;
        float _temp_max    =   0.0;
        bool _new_values   = false;

        unsigned long _blink_next = 0;
        #define _BLINK_TIME 500
        int led_almax     = 0;
        int led_altarget  = 0;
        int led_almin     = 0;
        int led_danger    = 0;
        #define _ENCODER_MULTIPLIER 1
        
        void _Run_menu(int encoder_value);
        void _Load_Conf();
        void _Save_Conf();

    public:
        Menu();
        void Init();
        void begin(bool first_start = false);
        void loop(int enc_value);
        void Advance(bool start_check = false);
        void Exit();
        void Reset(bool force = false);
        
        int getMenuItem();
        float getThresh_Min();
        float getThresh_1();
        float getThresh_2();
        float getThresh_3();
        float getThresh_Max();

        bool newValues();
        bool activeMenu();

     

};

