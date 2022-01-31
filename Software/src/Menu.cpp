#include "Menu.h"

Menu::Menu(void) {
}

void Menu::Init(void) {
  //Set up default values
  Reset(true);
  Serial.print("Thresh Max: "); Serial.println(_distances.threshMax);
  Serial.print("Thresh 1  : "); Serial.println(_distances.thresh1);
  Serial.print("Thresh 2  : "); Serial.println(_distances.thresh2);
  Serial.print("Thresh 3  : "); Serial.println(_distances.thresh3);
  Serial.print("Thresh Min: "); Serial.println(_distances.threshMin);

  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS, no further FS access will be attempted");
    return;
  } else {
    Serial.println("LittleFS mounted");
    if (LittleFS.exists("/distance_save.json")) {
      Serial.println("File exists");
      
      ////////////////////////////////////Read contents of Conf File///////////////////////////////////////////////////
      // /*
      // Open file for reading
      File file = LittleFS.open("/distance_save.json","r");
      if (!file) {
        Serial.println(F("Failed to read file"));
        return;
      }

      // Extract each characters by one by one
      while (file.available()) {
        Serial.print((char)file.read());
      }
      Serial.println();

      // Close the file
      file.close();
      // */
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      _FS_avoid = false;
      _Load_Conf();
      Serial.print("Thresh Max: "); Serial.println(_distances.threshMax);
      Serial.print("Thresh 1  : "); Serial.println(_distances.thresh1);
      Serial.print("Thresh 2  : "); Serial.println(_distances.thresh2);
      Serial.print("Thresh 3  : "); Serial.println(_distances.thresh3);
      Serial.print("Thresh Min: "); Serial.println(_distances.threshMin);
    } else {
      Serial.print("Creating config file from defaults");
      _Save_Conf();
    }
  }

}



void Menu::begin(bool first_start) {
  Advance(true);
  Serial.println();
}

void Menu::_Load_Conf() {
  // Loads config in file "/distance_save.json"
  Serial.println("Attempting to load Configuration");

  if (_FS_avoid) {
    Serial.println("FS previously failed to mount, avoiding FS calls");
  }

  //Open File for parsing
  File file = LittleFS.open("/distance_save.json", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    _Save_Conf();
    return;
  }
  
  // Do something with "file" (read/parse json)
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println(F("Failed to read file, using default configuration"));
    Reset(true);
  }

  if ( _distances.threshMax == 0 | _distances.thresh1 == 0 | _distances.thresh2 == 0 | _distances.thresh3 == 0 | _distances.threshMin == 0 ) {
    Serial.println("Bad Config, resetting to defaults");
    Reset(true);
  }
  _distances.threshMax = doc["threshMax"];
  _distances.thresh1   = doc["thresh1"];
  _distances.thresh2   = doc["thresh2"];
  _distances.thresh3   = doc["thresh3"];
  _distances.threshMin = doc["threshMin"];

  //Close "file"
  file.close();

}

void Menu::_Save_Conf() {
  // Overwrites config in file "/distance_save.json"
  Serial.println("Attempting to save Configuration");

  if (_FS_avoid) {
    Serial.println("FS failed to mount, avoiding FS calls");
    return;
  }

  // Delete config file
  //LittleFS.remove("/distance_save.json");

  //Open File for parsing
  File file = LittleFS.open("/distance_save.json", "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    _FS_avoid = true;
    return;
  }

  StaticJsonDocument<200> doc;
  doc["threshMax"] = _distances.threshMax;
  doc["thresh1"]   = _distances.thresh1;
  doc["thresh2"]   = _distances.thresh2;
  doc["thresh3"]   = _distances.thresh3;
  doc["threshMin"] = _distances.threshMin;
  
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  file.close();

}


void Menu::loop(int enc_value) {
    // Handle display
    if (_menu_item > 0) {
        // Handle the menu
        _Run_menu(enc_value);
    }
}

void Menu::Advance(bool start_check) {
    // Advance the menu to next item
    
    if (_menu_item > 0) {
        _menu_item++;
        if (_menu_item > 5) {
            _menu_item = 0;
            Exit();
        }
    }

    if (start_check) {
        _menu_item++;
    }

    if (_menu_item > 0) {
        // Set the temporary value to the appropriate thresh value
        switch (_menu_item) {
            case 1:
                _thresh_temp = _distances.thresh2;
                _temp_min = MIN_WIDTH * 3.0;
                _temp_max = 120.0 - MIN_WIDTH * 2.0;
                Serial.print("Entering menu item,     target: "); Serial.println(_thresh_temp);
                break;
            
            case 2:
                if (abs(_thresh_temp - _distances.thresh2) > .01) {
                    Serial.print("Difference, "); Serial.print(abs(_thresh_temp - _distances.thresh2)); Serial.print("Setting new target: "); Serial.println(_thresh_temp);
                    _distances.thresh2 = _thresh_temp;
                    _new_values = true;
                }
                _thresh_temp =_distances.threshMin;
                _temp_min = 8.0;
                _temp_max = _distances.thresh2 - MIN_WIDTH * 2.0;
                if (_thresh_temp - _temp_min < 0) {
                    _thresh_temp = _temp_min;
                } else if (_thresh_temp - _temp_max > 0) {
                    _thresh_temp = _temp_max;
                }
                Serial.print("Advance to menu "); Serial.print(_menu_item); Serial.print(" item, danger: "); Serial.println(_thresh_temp);
                break;
            
            case 3:
                if (abs(_distances.threshMin - _thresh_temp) > .01) {
                    Serial.print("Difference, "); Serial.print(abs(_distances.threshMin - _thresh_temp)); Serial.print("Setting new danger: "); Serial.println(_thresh_temp);
                    _distances.threshMin = _thresh_temp;
                    _new_values = true;
                }
                _thresh_temp = _distances.thresh1;
                _temp_min = _distances.thresh2 + MIN_WIDTH;
                _temp_max = 120 - MIN_WIDTH;
                if (_thresh_temp - _temp_min < 0) {
                    _thresh_temp = _temp_min;
                } else if (_thresh_temp - _temp_max > 0) {
                    _thresh_temp = _temp_max;
                }
                Serial.print("Advance to menu "); Serial.print(_menu_item); Serial.print(" item, target max: "); Serial.println(_thresh_temp);
                break;
            
            case 4:
                if (abs(_distances.thresh1 - _thresh_temp) > .01) {
                    Serial.print("Difference, "); Serial.print(abs(_distances.thresh1 - _thresh_temp)); Serial.print("Setting new target max: "); Serial.println(_thresh_temp);
                    _distances.thresh1 = _thresh_temp;
                    _new_values = true;
                }
                _thresh_temp = _distances.thresh3;
                _temp_min = _distances.threshMin + MIN_WIDTH;
                _temp_max = _distances.thresh2 - MIN_WIDTH;
                if (_thresh_temp - _temp_min < 0) {
                    _thresh_temp = _temp_min;
                } else if (_thresh_temp - _temp_max > 0) {
                    _thresh_temp = _temp_max;
                }
                Serial.print("Advance to menu "); Serial.print(_menu_item); Serial.print(" item, target min: "); Serial.println(_thresh_temp);
                break;
            case 5:
                if (abs(_distances.thresh3 - _thresh_temp) > .01) {
                    Serial.print("Difference, "); Serial.print(abs(_distances.thresh3 - _thresh_temp)); Serial.print("Setting new target min: "); Serial.println(_thresh_temp);
                    _distances.thresh3 = _thresh_temp;
                    _new_values = true;
                }
            default:
                _menu_item = 0;
                Exit();
                break;
        }
    }
}

void Menu::Exit() {
    // Safely Exit the menu
    for ( int i = 0; i <= NUM_LEDS-1; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    _menu_item = 0;
    if (_new_values) {
      _Save_Conf();

      Serial.print("Thresh Max: "); Serial.println(_distances.threshMax);
      Serial.print("Thresh 1  : "); Serial.println(_distances.thresh1);
      Serial.print("Thresh 2  : "); Serial.println(_distances.thresh2);
      Serial.print("Thresh 3  : "); Serial.println(_distances.thresh3);
      Serial.print("Thresh Min: "); Serial.println(_distances.threshMin);

    }
    //Serial.println("Exiting menu");
}

void Menu::Reset(bool force) {
  if (_FS_avoid | force) {
    Serial.println("Reset all to hard coded defaults");
    _distances.threshMax  = _DEFAULT_THRESH_MAX; // max dependable range         (black)
    _distances.thresh1    = _DEFAULT_THRESH_1;   // acceptable length maximum    (cyan)
    _distances.thresh2    = _DEFAULT_THRESH_2;   // acceptable length target     (green)
    _distances.thresh3    = _DEFAULT_THRESH_3;   // acceptable length minimum    (yellow)
    _distances.threshMin  = _DEFAULT_THRESH_MIN; // danger                       (red)
    if (force) {
      _Save_Conf();
    }
  } else {
    _Load_Conf();
    Serial.println("Reloaded ");
  }
    Exit();
}


void Menu::_Run_menu(int encoder_value) {
    // This should blink the location of the LED(s)
    //Serial.println("Running the menu");

    // set temp value based on encoder position
    switch (_menu_item) {
        case 1:
            // AL_Target
            // set temp value based on encoder position
                // compare the calculated position to the limits
            _thresh_temp = _distances.thresh2 + (float)encoder_value / _ENCODER_MULTIPLIER;

            if (_thresh_temp - _temp_min < 0) {
                _thresh_temp = _temp_min;
            } else if (_thresh_temp - _temp_max > 0) {
                _thresh_temp = _temp_max;
            }
            
            // set LED locations (linear, 0-120 in)
            //led_almax     = map(_distances.thresh1,0,_distances.threshMax,0,NUM_LEDS);
            led_altarget  = map(_thresh_temp,0,_distances.threshMax,0,NUM_LEDS);
            //led_almin     = map(_distances.thresh3,0,_distances.threshMax,0,NUM_LEDS);
            //led_danger    = map(_distances.threshMin,0,_distances.threshMax,0,NUM_LEDS);
            led_almax  = -1;
            led_almin  = -1;
            led_danger = -1;
            break;
        
        case 2:
            // Danger
            // set temp value based on encoder position
            _thresh_temp = _distances.threshMin + (float)encoder_value / _ENCODER_MULTIPLIER;
            if (_thresh_temp - _temp_min < 0) {
                _thresh_temp = _temp_min;
            } else if (_thresh_temp - _temp_max > 0) {
                _thresh_temp = _temp_max;
            }
            // set LED locations (linear, 0-120 in)
            //led_almax     = map(_distances.thresh1,0,_distances.threshMax,0,NUM_LEDS);
            led_altarget  = map(_distances.thresh2,0,_distances.threshMax,0,NUM_LEDS);
            //led_almin     = map(_distances.thresh3,0,_distances.threshMax,0,NUM_LEDS);
            led_danger    = map(_thresh_temp,0,_distances.threshMax,0,NUM_LEDS);
            led_almax = -1;
            led_almin = -1;
            break;
        
        case 3:
            // AL_Max
            // set temp value based on encoder position
            _thresh_temp = _distances.thresh1 + (float)encoder_value / _ENCODER_MULTIPLIER;
            if (_thresh_temp - _temp_min < 0) {
                _thresh_temp = _temp_min;
            } else if (_thresh_temp - _temp_max > 0) {
                _thresh_temp = _temp_max;
            }
            // set LED locations (linear, 0-120 in)
            led_almax     = map(_thresh_temp,0,_distances.threshMax,0,NUM_LEDS);
            led_altarget  = map(_distances.thresh2,0,_distances.threshMax,0,NUM_LEDS);
            //led_almin     = map(_distances.thresh3,0,_distances.threshMax,0,NUM_LEDS);
            led_danger    = map(_distances.threshMin,0,_distances.threshMax,0,NUM_LEDS);
            led_almin = -1;
            break;
        
        case 4:
            // AL_Min
            // set temp value based on encoder position
            _thresh_temp = _distances.thresh3 + (float)encoder_value / _ENCODER_MULTIPLIER;
            if (_thresh_temp - _temp_min < 0) {
                _thresh_temp = _temp_min;
            } else if (_thresh_temp - _temp_max > 0) {
                _thresh_temp = _temp_max;
            }
            // set LED locations (linear, 0-120 in)
            led_almax     = map(_distances.thresh1,0,_distances.threshMax,0,NUM_LEDS);
            led_altarget  = map(_distances.thresh2,0,_distances.threshMax,0,NUM_LEDS);
            led_almin     = map(_thresh_temp,0,_distances.threshMax,0,NUM_LEDS);
            led_danger    = map(_distances.threshMin,0,_distances.threshMax,0,NUM_LEDS);
            break;
        
        
        default:
            break;
    }
    // run display
    for ( int i = 0; i <= NUM_LEDS-1; i++) {
        if ( i == led_almax ) {
            leds[i] = CRGB::Cyan;
        } else if (i == led_altarget) {
            leds[i] = CRGB::Green;
        } else if (i == led_almin) {
            leds[i] = CRGB::Yellow;
        } else if (i == led_danger) {
            leds[i] = CRGB::Red;
        } else if (i == 23) {
            leds[i] = CRGB::Blue;
        } else {
            leds[i] = CRGB::Black;
        }
    }
    FastLED.show();
}

int Menu::getMenuItem() {
    return _menu_item;
}

float Menu::getThresh_Min() {
    return _distances.threshMin;
}

float Menu::getThresh_1() {
    return _distances.thresh1;
}

float Menu::getThresh_2() {
    return _distances.thresh2;
}

float Menu::getThresh_3() {
    return _distances.thresh3;
}

float Menu::getThresh_Max() {
    return _distances.threshMax;
}

bool Menu::newValues() {
    if ( ( _menu_item == 0 ) && ( _new_values) ) {
        _new_values = false;
        return true;
    } else {
        return false;
    }
}

bool Menu::activeMenu() {
    if (_menu_item > 0) {
        return true;
    } else {
        return false;
    }
}