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

// Initialize WiFi
#include "WiFi_Cred.h"
bool wifi_connected = false;

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
#define LOOP_TIME 1000 // 2 measurements printed per second
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

  // Setup LittleFS
  Serial.println();
  Serial.print("Setting up LittleFS: ");
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  } else {
    Serial.println("LittleFS Set Up");
  }

  // Setup Settings
  Serial.println("Setting up Settings");
  settings.init();

  // Setup Display
  Serial.println("Setting  up display");
  FastLED.addLeds<WS2812B, LED_DATA, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
  // Sets all LED's to one color (Black)
  for ( int i = 0; i <= NUM_LEDS-1; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  rainbow_show(5000,75,10);

  // Initialize WiFi
  Serial.println("Attempting to connect WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi Failed!");
    //return;
  } else {
    wifi_connected = true;
    Serial.println();
    Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
  }

  // Setup Webserver
  if (wifi_connected) {
    Serial.println("Setting up webserver");
    webserver_setup();
  } else {
    Serial.println("Skipping webserver, WiFi not connected");
  }

  // Setup Sensor
  Serial.println("Setting up sensor");
  Wire.begin(SENSOR_SDA,SENSOR_SCL);
  distSensor.SensorSetup();
  
  // Setup Filter, set initial value
  distSensor.Do_Measurement(1);
  loop_next = millis();

  // Set Range Coefficients
  Set_Range_coefs();
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

  if (wifi_connected) {
    webserver_loop();
  }

  if (millis() >= loop_next) {
    Serial.print("Distance(in): ");  Serial.println(SensorFilter.Current(), 1);
    loop_next = millis() + LOOP_TIME;
  }

}

