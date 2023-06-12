#include <Arduino.h>
#include "webserver.h"

// Create variables
int sensor_val_last = sensor_val;
unsigned long webpage_update_sensor = 0;
int webpage_update_sensor_skip      = 0;
unsigned long webpage_update_ping   = 0;
#define WEBPAGE_UPDATE_SENSOR         500
#define WEBPAGE_SENSOR_SKIP_THRESH    5
#define WEBPAGE_UPDATE_PING           5000

// Webserver Initiation
  // Create AsyncWebServer object on port 80
  AsyncWebServer server(80);
  // Create an Event Source on /events
  AsyncEventSource events("/events");


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

// Replaces placeholder with stored values
String processor(const String& var){
  //StopLimit 
  //OkEnd
  //GoodEnd
  //GoodStart
  //FarStart
  if(var == "StopLimit"){
    return String(settings.StopLimit);
  } else if(var == "OkEnd"){
    return String(settings.OkEnd);
  } else if(var == "GoodEnd"){
    return String(settings.GoodEnd);
  } else if(var == "GoodStart"){
    return String(settings.GoodStart);
  } else if(var == "FarStart"){
    return String(settings.FarStart);
  } else if(var == "SensorValue"){
    return String(sensor_val);
  }
  return String();
}

void webserver_setup() {
  Serial.print("Setting up WebServer");
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  events.onConnect([](AsyncEventSourceClient *client){
      if(client->lastId()){
        Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
      }
      // send event with message "hello!", id current millis
      // and set reconnect delay to 1 second
      client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(PARAM_STOP_LIMIT)) {
      inputMessage = request->getParam(PARAM_STOP_LIMIT)->value();
      if (inputMessage == "default") {
        inputMessage = String(DEF_STOP_LIMIT);
        Serial.println("Setting StopLimit to Default");
      }
      if (inputMessage == "reset") {
        settings.ResetAll();
        Serial.println("Setting all values to Default");
        return;
      }
      settings.SetSetting(PARAM_STOP_LIMIT,inputMessage.toFloat());
    } else if (request->hasParam(PARAM_OK_END)) {
      inputMessage = request->getParam(PARAM_OK_END)->value();
      if (inputMessage == "default") {
        inputMessage = String(DEF_OK_END);
        Serial.println("Setting StopLimit to Default");
      }
      if (inputMessage == "reset") {
        settings.ResetAll();
        Serial.println("Setting all values to Default");
        return;
      }
      settings.SetSetting(PARAM_OK_END,inputMessage.toFloat());
    } else if (request->hasParam(PARAM_GOOD_END)) {
      inputMessage = request->getParam(PARAM_GOOD_END)->value();
      if (inputMessage == "default") {
        inputMessage = String(DEF_GOOD_END);
        Serial.println("Setting StopLimit to Default");
      }
      if (inputMessage == "reset") {
        settings.ResetAll();
        Serial.println("Setting all values to Default");
        return;
      }
      settings.SetSetting(PARAM_GOOD_END,inputMessage.toFloat());
    } else if (request->hasParam(PARAM_GOOD_START)) {
      inputMessage = request->getParam(PARAM_GOOD_START)->value();
      if (inputMessage == "default") {
        inputMessage = String(DEF_GOOD_START);
        Serial.println("Setting StopLimit to Default");
      }
      if (inputMessage == "reset") {
        settings.ResetAll();
        Serial.println("Setting all values to Default");
        return;
      }
      settings.SetSetting(PARAM_GOOD_START,inputMessage.toFloat());
    } else if (request->hasParam(PARAM_FAR_START)) {
      inputMessage = request->getParam(PARAM_FAR_START)->value();
      if (inputMessage == "default") {
        inputMessage = String(DEF_FAR_START);
        Serial.println("Setting StopLimit to Default");
      }
      if (inputMessage == "reset") {
        settings.ResetAll();
        Serial.println("Setting all values to Default");
        return;
      }
      settings.SetSetting(PARAM_FAR_START,inputMessage.toFloat());
    } else {
      //Serial.println(inputMessage);
      Serial.print("unexpected input message parameter");
    }
    //Serial.println(inputMessage);
    //request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();
  Serial.println(".... Done!");
}

void webserver_loop() {
    // Update Webpage
    // Ping
    if (millis() >= webpage_update_ping) {
      webpage_update_ping = millis() + WEBPAGE_UPDATE_PING;
      events.send("ping",NULL,millis());
      Serial.println();
      Serial.print("Current Sensor Value: "); Serial.println(sensor_val);
    }

    // Sensor
    if (millis() >= webpage_update_sensor) {
      if (abs(sensor_val - sensor_val_last) >= SENSOR_MIN_CHANGE) {
        events.send(String(sensor_val).c_str(),"SensorValue",millis());
        Serial.print(".");
        sensor_val_last = sensor_val;
        webpage_update_sensor_skip = 0;
        //Serial.println("Webpage Sensor updated");
      } else {
        // skip sending update if value not changed
        if (webpage_update_sensor_skip >= WEBPAGE_SENSOR_SKIP_THRESH) {
          events.send(String(sensor_val).c_str(),"SensorValue",millis());
          Serial.print(".");
          sensor_val_last = sensor_val;
          webpage_update_sensor_skip = 0;
          //Serial.println("Webpage Sensor update forced");
        } else {
          webpage_update_sensor_skip++;
          Serial.print("-");
          //Serial.print("Webpage Sensor update skipped, "); Serial.println(webpage_update_sensor_skip);
        }
      }
      
      webpage_update_sensor = millis() + WEBPAGE_UPDATE_SENSOR;
    }
}