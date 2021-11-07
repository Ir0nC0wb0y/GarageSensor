#include "Menu.h"

Menu::Menu(void) {
}

void Menu::begin(bool first_start) {
    if (first_start == true) {
        // This would be the point to check for stored distance values
        Serial.print("Not reading stored memory, come back another time");
    }
    Advance(true);
    Serial.println();
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
                _thresh_temp= _thresh_2;
                _temp_min = MIN_WIDTH * 3.0;
                _temp_max = 120.0 - MIN_WIDTH * 2.0;
                Serial.print("Entering menu item,     target: "); Serial.println(_thresh_temp);
                break;
            
            case 2:
                if (abs(_thresh_temp - _thresh_2) > .01) {
                    Serial.print("Difference, "); Serial.print(abs(_thresh_temp - _thresh_2)); Serial.print("Setting new target: "); Serial.println(_thresh_temp);
                    _thresh_2 = _thresh_temp;
                    _new_values = true;
                }
                _thresh_temp =_thresh_min;
                _temp_min = 8.0;
                _temp_max = _thresh_2 - MIN_WIDTH * 2.0;
                if (_thresh_temp - _temp_min < 0) {
                    _thresh_temp = _temp_min;
                } else if (_thresh_temp - _temp_max > 0) {
                    _thresh_temp = _temp_max;
                }
                Serial.print("Advance to menu "); Serial.print(_menu_item); Serial.print(" item, danger: "); Serial.println(_thresh_temp);
                break;
            
            case 3:
                if (abs(_thresh_min - _thresh_temp) > .01) {
                    Serial.print("Difference, "); Serial.print(abs(_thresh_min - _thresh_temp)); Serial.print("Setting new danger: "); Serial.println(_thresh_temp);
                    _thresh_min = _thresh_temp;
                    _new_values = true;
                }
                _thresh_temp = _thresh_1;
                _temp_min = _thresh_2 + MIN_WIDTH;
                _temp_max = 120 - MIN_WIDTH;
                if (_thresh_temp - _temp_min < 0) {
                    _thresh_temp = _temp_min;
                } else if (_thresh_temp - _temp_max > 0) {
                    _thresh_temp = _temp_max;
                }
                Serial.print("Advance to menu "); Serial.print(_menu_item); Serial.print(" item, target max: "); Serial.println(_thresh_temp);
                break;
            
            case 4:
                if (abs(_thresh_1 - _thresh_temp) > .01) {
                    Serial.print("Difference, "); Serial.print(abs(_thresh_1 - _thresh_temp)); Serial.print("Setting new target max: "); Serial.println(_thresh_temp);
                    _thresh_1 = _thresh_temp;
                    _new_values = true;
                }
                _thresh_temp = _thresh_3;
                _temp_min = _thresh_min + MIN_WIDTH;
                _temp_max = _thresh_2 - MIN_WIDTH;
                if (_thresh_temp - _temp_min < 0) {
                    _thresh_temp = _temp_min;
                } else if (_thresh_temp - _temp_max > 0) {
                    _thresh_temp = _temp_max;
                }
                Serial.print("Advance to menu "); Serial.print(_menu_item); Serial.print(" item, target min: "); Serial.println(_thresh_temp);
                break;
            case 5:
                if (abs(_thresh_3 - _thresh_temp) > .01) {
                    Serial.print("Difference, "); Serial.print(abs(_thresh_3 - _thresh_temp)); Serial.print("Setting new target min: "); Serial.println(_thresh_temp);
                    _thresh_3 = _thresh_temp;
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
    Serial.println("Exiting menu");
}

void Menu::Reset() {
    _thresh_max  = _DEFAULT_THRESH_MAX; // max dependable range         (black)
    _thresh_1    = _DEFAULT_THRESH_1;   // acceptable length maximum    (cyan)
    _thresh_2    = _DEFAULT_THRESH_2;   // acceptable length target     (green)
    _thresh_3    = _DEFAULT_THRESH_3;   // acceptable length minimum    (yellow)
    _thresh_min  = _DEFAULT_THRESH_MIN; // danger                       (red)
    Serial.println("Reset all to defaults");
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
            _thresh_temp = _thresh_2 + (float)encoder_value / _ENCODER_MULTIPLIER;

            if (_thresh_temp - _temp_min < 0) {
                _thresh_temp = _temp_min;
            } else if (_thresh_temp - _temp_max > 0) {
                _thresh_temp = _temp_max;
            }
            
            // set LED locations (linear, 0-120 in)
            //led_almax     = map(_thresh_1,0,_thresh_max,0,NUM_LEDS);
            led_altarget  = map(_thresh_temp,0,_thresh_max,0,NUM_LEDS);
            //led_almin     = map(_thresh_3,0,_thresh_max,0,NUM_LEDS);
            //led_danger    = map(_thresh_min,0,_thresh_max,0,NUM_LEDS);
            led_almax  = -1;
            led_almin  = -1;
            led_danger = -1;
            break;
        
        case 2:
            // Danger
            // set temp value based on encoder position
            _thresh_temp = _thresh_min + (float)encoder_value / _ENCODER_MULTIPLIER;
            if (_thresh_temp - _temp_min < 0) {
                _thresh_temp = _temp_min;
            } else if (_thresh_temp - _temp_max > 0) {
                _thresh_temp = _temp_max;
            }
            // set LED locations (linear, 0-120 in)
            //led_almax     = map(_thresh_1,0,_thresh_max,0,NUM_LEDS);
            led_altarget  = map(_thresh_2,0,_thresh_max,0,NUM_LEDS);
            //led_almin     = map(_thresh_3,0,_thresh_max,0,NUM_LEDS);
            led_danger    = map(_thresh_temp,0,_thresh_max,0,NUM_LEDS);
            led_almax = -1;
            led_almin = -1;
            break;
        
        case 3:
            // AL_Max
            // set temp value based on encoder position
            _thresh_temp = _thresh_1 + (float)encoder_value / _ENCODER_MULTIPLIER;
            if (_thresh_temp - _temp_min < 0) {
                _thresh_temp = _temp_min;
            } else if (_thresh_temp - _temp_max > 0) {
                _thresh_temp = _temp_max;
            }
            // set LED locations (linear, 0-120 in)
            led_almax     = map(_thresh_temp,0,_thresh_max,0,NUM_LEDS);
            led_altarget  = map(_thresh_2,0,_thresh_max,0,NUM_LEDS);
            //led_almin     = map(_thresh_3,0,_thresh_max,0,NUM_LEDS);
            led_danger    = map(_thresh_min,0,_thresh_max,0,NUM_LEDS);
            led_almin = -1;
            break;
        
        case 4:
            // AL_Min
            // set temp value based on encoder position
            _thresh_temp = _thresh_3 + (float)encoder_value / _ENCODER_MULTIPLIER;
            if (_thresh_temp - _temp_min < 0) {
                _thresh_temp = _temp_min;
            } else if (_thresh_temp - _temp_max > 0) {
                _thresh_temp = _temp_max;
            }
            // set LED locations (linear, 0-120 in)
            led_almax     = map(_thresh_1,0,_thresh_max,0,NUM_LEDS);
            led_altarget  = map(_thresh_2,0,_thresh_max,0,NUM_LEDS);
            led_almin     = map(_thresh_temp,0,_thresh_max,0,NUM_LEDS);
            led_danger    = map(_thresh_min,0,_thresh_max,0,NUM_LEDS);
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
    return _thresh_min;
}

float Menu::getThresh_1() {
    return _thresh_1;
}

float Menu::getThresh_2() {
    return _thresh_2;
}

float Menu::getThresh_3() {
    return _thresh_3;
}

float Menu::getThresh_Max() {
    return _thresh_max;
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