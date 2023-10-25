// External Libraries
#include <Arduino.h>
#include <Wire.h>

// Internal Modules
  #include "common.h"
  #include "display.h"
  #include "DistanceSensor.h"
  #include "OLED_display.h"
  #include "Settings.h"
  #include "WiFi_Setup.h"

// Setup I2C Bus (Wire)
  #define SENSOR_SCL D1
  #define SENSOR_SDA D2

// Initialize Settings
  Settings settings;

// Initialize Sensor
  DistSensor distSensor;

// Initialize OLED Display
  OLED_Display oled;

// Initialize Filter
  ExponentialFilter<float> SensorFilter(FILTER_WEIGHT_GOOD, 0);
  ExponentialFilter<float> SensorChange(FILTER_WEIGHT_GOOD, 0);

// Loop Parameters
  #define OUTPUT_TIME             1000 // measurements print period
  unsigned long loop_output =        0;
  #define WIFI_CHECK_TIME       300000
  unsigned long loop_wifi   =        0;

// Display Setup
  CRGB leds[NUM_LEDS];

// Initialize Webserver
  #include "webserver.h"
  #include "webpage_main.cpp"

void setup() {
  // Setup Serial
  Serial.begin(115200);
  Serial.println();

  // Setup LittleFS
  Serial.println();
  Serial.print("Setting up LittleFS: ");
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  } else {
    Serial.println("LittleFS Set Up");
  }
  Serial.println();

  // Setup Settings
  Serial.println("Setting up Settings");
  settings.init();
  Serial.println();

  // Setup OLED
  Serial.println("Setting up OLED Display");
  oled.Setup();
  //oled.ShowIP("192.168.10.100");

  // Setup Display
  Display_Setup();
  Serial.println();

  // Initialize WiFi
  WiFi_Setup();
  Serial.println();

  // Setup Webserver
  if (wifi_connected) {
    webserver_setup();
  } else {
    Serial.println("Skipping webserver, WiFi not connected");
  }
  Serial.println();

  // Setup Sensor
  Serial.println("Setting up sensor");
  Wire.begin(SENSOR_SDA,SENSOR_SCL);
  distSensor.SensorSetup();

  // Set Range Coefficients
  Set_Range_coefs();
  Serial.println();
  
  // Setup Filter, set initial value
  distSensor.Do_Measurement(true);
  loop_output = millis();
  loop_wifi = millis() + WIFI_CHECK_TIME;
  }

void loop() {
  distSensor.Do_Measurement();
  Do_Display();
  oled.Display(SensorFilter.Current(),SensorChange.Current());

  if (millis() >= loop_wifi) {
    WiFi_Check();
    loop_wifi = millis() + WIFI_CHECK_TIME;
  }

  if (wifi_connected) {
    webserver_loop();
  }

  if (millis() >= loop_output) {
    Serial.print("Distance(in): ");  Serial.println(SensorFilter.Current(), 1);
    loop_output = millis() + OUTPUT_TIME;
  }

}

