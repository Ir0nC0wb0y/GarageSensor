#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "Settings.h"

// Pull in external variables
  extern AsyncWebServer server;
  extern AsyncEventSource events;
  extern const char index_html[]; // This is the mainpage, from webpage_main.cpp
  extern Settings settings;
  // Sensor
  extern int sensor_val;
  extern int sensor_val_last;
  extern int sensor_change;
  #define SENSOR_MIN_CHANGE   100

void webserver_setup();

void webserver_loop();