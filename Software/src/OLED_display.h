#pragma once
#include <Arduino.h>

//#include "DistanceSensor.h"

#define SCREEN_WIDTH          128 // OLED display width, in pixels
#define SCREEN_HEIGHT          64 // OLED display height, in pixels

#define SCROLL_TIME          2000 // Time to scroll text

#define OLED_SHUTOFF_THRESH  0.75
#define OLED_SHUTOFF_TIME   10000

#define DISPLAY_RATE_VALUE    500 // Rate numberical value changes [ms]
#define DISPLAY_RATE_GRAPH    200 // Rate numberical graph changes [ms]

#define SENSOR_MIN_VALUE      2.0 // Maximum value Distance Graph Shows
#define SENSOR_MAX_VALUE    150.0 // Maximum value Distance Graph Shows

#define VALUE_TEXT_SIZE         4

class OLED_Display {
  public:
    OLED_Display();
    void Setup();
    void ShowIP(String IpAddress);
    void ShowText(String text, int size = 2);
    void ShowDistance(float Display);
    void Display(float Display_dist, float Display_speed);

  private:
    float _circularBuffer[SCREEN_WIDTH]; //fast way to store values 
    float _lastValue_dist = 0.0;
    //float _lastValue_speed = 0.0;
    int _curWriteIndex = 0; // tracks where we are in the circular buffer
    int _graphHeight = SCREEN_HEIGHT / 2;
    unsigned int _time_last_value = 0;
    unsigned int _time_last_graph = 0;
    void _Display_Value(float Display);
    void _Display_Graph();
    void _drawLine(int xPos, float Buffer_val);
    void _OLED_Power(bool onoff);
    unsigned long _time_shutoff = 0;
    bool _oled_state = true;
};