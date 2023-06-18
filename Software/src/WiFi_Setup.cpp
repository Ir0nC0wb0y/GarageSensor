#include <Arduino.h>

#include "WiFi_Setup.h"
#include "WiFi_Cred.h"

bool wifi_connected = false;
#define WIFI_CONNECTION_MAX_TIME 10000

void WiFi_Setup() {
  WiFi.mode(WIFI_STA);
  WiFi_Connect();
}

void WiFi_Connect() {
  Serial.println("Attempting to connect to WiFi");
  WiFi.begin(ssid, password);

  unsigned long wifi_connection_timeout = millis() + WIFI_CONNECTION_MAX_TIME;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    rainbow_frame(75);
    yield();
    if (millis() >= wifi_connection_timeout) {
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    Serial.print("Successful Connection, IP Address: ");
      Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi Failed!");
    wifi_connected = false;
  }
}

void WiFi_Check() {
  Serial.print("Checking WiFi Status:");
  wifi_connected = false;
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("   Good!");
    wifi_connected = true;
  } else {
    Serial.println("   Not Connected");
    WiFi_Connect();
    if (WiFi.status() == WL_CONNECTED) {
      // Set up AP?
      wifi_connected = false;
    } else {
      wifi_connected = true;
    }
  }
}