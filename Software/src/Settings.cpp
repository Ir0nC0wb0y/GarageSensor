#include <Arduino.h>
#include "Settings.h"
#include "display.h"

extern FS LittleFS;


String readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println(" - empty file or failed to open file");
    return String();
  } else {
    Serial.println(" - Success!");
  }
  //Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  file.close();
  //Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println(" - failed to open file for writing");
    return;
  }

  if(file.print(message)){
    Serial.println(" - Success!");
  } else {
    Serial.println(" - write failed");
  }
  file.close();
}

Settings::Settings() {
  return;
}

void Settings::init() {
  // This should attempt to load the 5 settings in the following process (each):
    // - check if the file exists
      // - if file not exist, create it from default value
    // - set setting in class

  // STOP_LIMIT
  StopLimit = _InitSetting(DIR_STOP_LIMIT,DEF_STOP_LIMIT);

  //OK_END
  OkEnd     = _InitSetting(DIR_OK_END,DEF_OK_END);

  //GOOD_END
  GoodEnd   = _InitSetting(DIR_GOOD_END,DEF_GOOD_END);

  //GOOD_START
  GoodStart = _InitSetting(DIR_GOOD_START,DEF_GOOD_START);

  //FAR_START
  FarStart  = _InitSetting(DIR_FAR_START,DEF_FAR_START);

  //Timeout
  Timeout   = _InitSetting(DIR_TIMEOUT,DEF_TIMEOUT);
  Timeout_ms = (unsigned long) Timeout * 1000;

}

float Settings::_InitSetting(const char * settingDir, float settingDefault, bool forceDefault) {
  float setting_out = 0.0;
  if (LittleFS.exists(settingDir) && !forceDefault) {
    setting_out = readFile(LittleFS, settingDir).toFloat();
  } else {
    String setting = String(settingDefault);
    writeFile(LittleFS, settingDir, setting.c_str());
    setting_out = settingDefault;
  }
  return setting_out;
}

int Settings::_InitSetting_int(const char * settingDir, float settingDefault, bool forceDefault) {
  float setting_out = 0.0;
  if (LittleFS.exists(settingDir) && !forceDefault) {
    setting_out = readFile(LittleFS, settingDir).toInt();
  } else {
    String setting = String(settingDefault);
    writeFile(LittleFS, settingDir, setting.c_str());
    setting_out = settingDefault;
  }
  return setting_out;
}

bool Settings::SetSetting(const char * setting, float setting_value) {
  // Check if setting_value has changed
    // if change
      // validate the value (compare to surrounding boundary(ies))
      // save to file
      // set value in class

  bool setting_update = false;

  if (strcmp(setting,PARAM_STOP_LIMIT) == 0) {
    if (setting_value != StopLimit && setting_value != 0) {
      StopLimit = setting_value;
      writeFile(LittleFS, DIR_STOP_LIMIT, String(StopLimit).c_str());
      setting_update = true;
    }
  } else if (strcmp(setting,PARAM_OK_END) == 0) {
    if (setting_value != OkEnd && setting_value != 0) {
      OkEnd = setting_value;
      writeFile(LittleFS, DIR_OK_END, String(OkEnd).c_str());
      setting_update = true;
    }
  } else if (strcmp(setting,PARAM_GOOD_END) == 0) {
    if (setting_value != GoodEnd && setting_value != 0) {
      GoodEnd = setting_value;
      writeFile(LittleFS, DIR_GOOD_END, String(GoodEnd).c_str());
      setting_update = true;
    }
  } else if (strcmp(setting,PARAM_GOOD_START) == 0) {
    if (setting_value != GoodStart && setting_value != 0) {
      GoodStart = setting_value;
      writeFile(LittleFS, DIR_GOOD_START, String(GoodStart).c_str());
      setting_update = true;
    }
  } else if (strcmp(setting,PARAM_FAR_START) == 0) {
    if (setting_value != FarStart && setting_value != 0) {
      FarStart = setting_value;
      writeFile(LittleFS, DIR_FAR_START, String(FarStart).c_str());
      setting_update = true;
    }
  } else if (strcmp(setting,PARAM_TIMEOUT) == 0) {
    if (setting_value != Timeout && setting_value != 0) {
      Timeout = setting_value;
      Timeout_ms = (unsigned long) (Timeout * 1000);
      Serial.print("New timeout: "); Serial.println(Timeout_ms);
      writeFile(LittleFS, DIR_TIMEOUT, String(Timeout).c_str());
      //setting_update = true; // this is not needed as this is used to modify the display coefficients
    }
  } else {
    Serial.print("Unexpected pameter: ");
      Serial.print(setting);
      Serial.print("      ");
  }

  if (setting_update) {
    Set_Range_coefs();
  }
  return setting_update;
}

void Settings::ResetAll() {
  // Resets all values to factory defaults

  // STOP_LIMIT
  StopLimit = _InitSetting(DIR_STOP_LIMIT,DEF_STOP_LIMIT,true);

  //OK_END
  OkEnd     = _InitSetting(DIR_OK_END,DEF_OK_END,true);

  //GOOD_END
  GoodEnd   = _InitSetting(DIR_GOOD_END,DEF_GOOD_END,true);

  //GOOD_START
  GoodStart = _InitSetting(DIR_GOOD_START,DEF_GOOD_START,true);

  //FAR_START
  FarStart  = _InitSetting(DIR_FAR_START,DEF_FAR_START,true);

  //Timeout
  Timeout   = _InitSetting(DIR_TIMEOUT,DEF_TIMEOUT,true);
  Timeout_ms = (unsigned long) Timeout * 60 * 1000;
  
  // Update Display parameters
  Set_Range_coefs();
}