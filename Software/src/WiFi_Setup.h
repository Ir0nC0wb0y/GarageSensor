#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>


extern bool wifi_connected;

void WiFi_Setup();
void WiFi_Connect();
void WiFi_Check();