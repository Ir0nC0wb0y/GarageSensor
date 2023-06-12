// External Libraries
#include <Arduino.h>
#include <Wire.h>

//#include <Filter.h>
//#include <FastLED.h>
//#include <ESPRotary.h>
//#include <Button2.h>

// Internal Modules
#include "common.h"
#include "display.h"
#include "DistanceSensor.h"
#include "Settings.h"
#include "WiFi_Cred.h"

// Setup I2C Bus (Wire)
#define SENSOR_SCL D1
#define SENSOR_SDA D2

// Initialize Settings
Settings settings;

// Initialize Sensor
DistSensor distSensor;

// Initialize Filter
ExponentialFilter<float> SensorFilter(75, 0);

// Loop Parameters
#define LOOP_TIME 500 // 2 measurements printed per second
unsigned long loop_next = 0;

// Display Setup
CRGB leds[NUM_LEDS];

// Initialize Webserver
  #include "webserver.h"
  #include "webpage_main.cpp"

void setup() {
  // Setup Serial
  Serial.begin(115200);
  Serial.println("Starting sketch");
  Serial.println();

  // Initialize LittleFS
  Serial.println();
  Serial.print("Setting up LittleFS: ");
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  } else {
    Serial.println("LittleFS Set Up");
  }

  // Initialize Settings
  settings.init();

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize Webserver
  webserver_setup();

  // Setup Display
  FastLED.addLeds<WS2812B, LED_DATA, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
  // Sets all LED's to one color (Black)
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  rainbow_show(5000,75,10);

  // Setup Sensor
  Wire.begin(SENSOR_SDA,SENSOR_SCL);
  distSensor.SensorSetup();
  
  // Setup Filter, set initial value
  distSensor.Do_Measurement(1);
  loop_next = millis();

  // Set Range Coefficients
  Set_Range_coefs(); // adding this function now will make it easier to recalc on the fly, when there is some user engagement
  }

void loop() {
  /*
  r.loop();
  b.loop();
  menu.loop(r.getPosition());
  if (menu.newValues()) {
    Set_Range_coefs();
  }
  */
  distSensor.Do_Measurement();

  webserver_loop();

  if (millis() >= loop_next) {
    Serial.print("Distance(in): ");  Serial.println(SensorFilter.Current(), 1);
    loop_next = millis() + LOOP_TIME;
  }

}

