#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "OLED_display.h"
#include "common.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

OLED_Display::OLED_Display() {
  for (int i = 0; i < SCREEN_WIDTH; i++) {
    _circularBuffer[i] = 0.0;
  }
  return;
}


// Map function for floating variables
float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void OLED_Display::Setup() {
  Serial.println("Setting up OLED Display");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void OLED_Display::ShowIP(String IpAddress) {
  // Parse IP Address
  // Combine IP Address
  String IP12 = "192.168";
  String IP34 = "10.100";
  // Display IP Address
  display.clearDisplay();
  display.setTextSize(2);
  display.println(IP12);
  display.println(IP34);

  display.display();

  unsigned long start_scroll = millis();
  while (millis() - start_scroll < SCROLL_TIME) {
    yield();
  }
}

void OLED_Display::ShowText(String text, int size) {

  display.clearDisplay();
  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.println(text);
  display.display();
  display.startscrollleft(0,7);
  unsigned long start_scroll = millis();
  while (millis() - start_scroll < SCROLL_TIME) {
    yield();
  }
  display.stopscroll();

}

void OLED_Display::ShowDistance(float Display) {
  // Format incoming Display
  int sensor_precision = 2;
  if (Display >= 100.0) {
    sensor_precision = 1;
  }
  if (Display < 10.0) {
    sensor_precision = 3;
  }
  String Display_String = String(Display,sensor_precision);

  // display the distance, centered in the screen
  
  display.clearDisplay();

  // Determine the size of the text
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(Display_String, 0, 0, &x1, &y1, &width, &height);
  display.setTextSize(4);

  
  display.setTextColor(WHITE);
  // Put the text in the center of the screen
  display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) -10);
  // Display static text
  display.println(Display_String);
  display.display();
  
}

void OLED_Display::Display(float Display_dist, float Display_speed) {
  // Logic for measurement change
  //bool oled_shutoff = false;
  if (abs(Display_speed) < OLED_SHUTOFF_THRESH) {
    if (_time_shutoff == 0) {
      //Serial.println("Set OLED Timeout");
      _time_shutoff = millis();
    } else {
      if (millis() - _time_shutoff >= settings.Timeout_ms) {
        if (_oled_state == true) {
          _OLED_Power(false);
          
          // clear graph history
          //for (int i = 0; i < SCREEN_WIDTH; i++) {
          //  _circularBuffer[i] = 0.0;
          //}
        }
        return;
      }
    }
  } else if (_time_shutoff > 0) {
    //Serial.println("Clearing OLED Timeout");
    _time_shutoff = 0;
    display.ssd1306_command(SSD1306_DISPLAYON);
    //oled_shutoff = false
  }

  // Run the display
  //if (!oled_shutoff) {
    // Do Timing for Value

    if (_oled_state == false) {
      _OLED_Power(true);
    }

    bool timing_value = false;
    if (millis() - _time_last_value >= DISPLAY_RATE_VALUE) {
      // Prepare to update Value
      _time_last_value = millis();
      timing_value = true;
    }

    // Do Timing for Graph
    bool timing_graph = false;
    if (millis() - _time_last_graph >= DISPLAY_RATE_GRAPH) {
      // Prepare to update Graph
      _time_last_graph = millis();
      timing_graph = true;
    }

    if (timing_value || timing_graph) {
      display.clearDisplay();
      // Add Value
      if (timing_value) {
        _Display_Value(Display_dist);
        _lastValue_dist = Display_dist;
      } else {
        _Display_Value(_lastValue_dist);
      }

      // Add Graph
      if (timing_graph) {
        // Store the value in circular buffer
        _circularBuffer[_curWriteIndex++] = Display_dist;
        if (_curWriteIndex >= SCREEN_WIDTH) {
          _curWriteIndex = 0;
        }
      }
      _Display_Graph();

      display.display();
    }
  //}

}

void OLED_Display::_Display_Value(float Display) {
// Format incoming Display
  int sensor_precision = 2;
  if (Display >= 100.0) {
    sensor_precision = 1;
  }
  if (Display < 10.0) {
    sensor_precision = 3;
  }
  String Display_String = String(Display,sensor_precision);

  // Determine the size of the text
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(Display_String, 0, 0, &x1, &y1, &width, &height);
  display.setTextSize(VALUE_TEXT_SIZE);
  display.setTextColor(WHITE);

  // This is for the center of the screen
  display.setCursor((SCREEN_WIDTH - width) / 2, 0);
  display.println(Display_String);
}

void OLED_Display::_drawLine(int xPos, float Buffer_val){
  //int Buffer_val_int = mapf(Buffer_val, SENSOR_MIN_VALUE, SENSOR_MAX_VALUE, 0, _graphHeight);
  int Buffer_val_int = mapf(Buffer_val, settings.StopLimit, settings.FarStart, 0, _graphHeight);
  if (Buffer_val_int > _graphHeight) {
    // when map gets too big
    Buffer_val_int = _graphHeight;
  } else if (Buffer_val_int < 1) {
    //when map gets too small
    Buffer_val_int = 1;
  }
  int yPos = SCREEN_HEIGHT - Buffer_val_int;
  //display.drawFastVLine(xPos, yPos, Buffer_val_int, SSD1306_WHITE);
  display.drawPixel(xPos, yPos, SSD1306_WHITE);
}

void OLED_Display::_Display_Graph() {
  int xPos = 0;
  int idx = 0;
  for (int i = 0; i < SCREEN_WIDTH; i++){
    idx = i + _curWriteIndex;
    if (idx > SCREEN_WIDTH-1) {
      idx = idx - SCREEN_WIDTH;
    }
    float Buffer_val = _circularBuffer[idx];

    _drawLine(xPos, Buffer_val);
    xPos++;
  }
  //Serial.println();

}

//Function to turn on or off OLED display
void OLED_Display::_OLED_Power(bool onoff) {
  if (onoff) {
    // turn on display
    Serial.println("Turning on OLED");
    // clear display
    display.clearDisplay();
    display.ssd1306_command(SSD1306_DISPLAYON);
    _oled_state = true;
  } else {
    //turn off display
    Serial.println("Turning off OLED");
    // clear display
    display.clearDisplay();
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    _oled_state = false;
  }
}