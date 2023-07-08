#pragma once

#include <Arduino.h>
#include <LittleFS.h>


// Default values
#define DEF_STOP_LIMIT   38.0
#define DEF_OK_END       50.0
#define DEF_GOOD_END     57.0
#define DEF_GOOD_START   60.0
#define DEF_FAR_START    80.0
#define DEF_TIMEOUT       5.0

#define DIR_STOP_LIMIT  "/StopLimit.txt"
#define DIR_OK_END      "/OkEnd.txt"
#define DIR_GOOD_END    "/GoodEnd.txt"
#define DIR_GOOD_START  "/GoodStart.txt"
#define DIR_FAR_START   "/FarStart.txt"
#define DIR_TIMEOUT     "/Timeout.txt"

#define PARAM_STOP_LIMIT  "StopLimit"
#define PARAM_OK_END      "OkEnd"
#define PARAM_GOOD_END    "GoodEnd"
#define PARAM_GOOD_START  "GoodStart"
#define PARAM_FAR_START   "FarStart"
#define PARAM_TIMEOUT     "Timeout"



//enum SettingNames {StopLimit, OkEnd, GoodEnd, GoodStart, FarStart};

class SettingsTest {
  public:
    bool settings_test = false;
};


class Settings {
  public:
    float StopLimit = 0.0;
    float OkEnd     = 0.0;
    float GoodEnd   = 0.0;
    float GoodStart = 0.0;
    float FarStart  = 0.0;
    float Timeout   = 0.0;
    unsigned long Timeout_ms = 0;
    Settings();
    void init();
    //float GetSetting(const char * setting);
    bool SetSetting(const char * setting, float setting_value);
    void ResetAll();
    
    
  private:
    float _InitSetting(const char * settingDir, float settingDefault, bool forceDefault = false);
    int _InitSetting_int(const char * settingDir, float settingDefault, bool forceDefault = false);
    //bool  _CompareSetting(const char * setting, float newValue);

};