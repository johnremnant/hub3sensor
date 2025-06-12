#ifndef CONFIG_WEB_H
#define CONFIG_WEB_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPUpdateServer.h>
#include "sensor_types.h"

#define CONFIG_PATH "/config.json"

struct ConfigData {
  String ssid;
  String password;
  unsigned long mesh_id;
  byte category_id;
  byte sensor_type;   // primary sensor (DHT, BME280, etc.)
  bool ldr_enabled;   // separate 
  
};

String getSensorOptions(byte selected) ;

extern ConfigData config;  // Declare globally so other files can access it

void enterConfigMode();
String loadHtml(const char* path, const String& placeholder = "", const String& value = "");
void saveConfig(const String& ssid, const String& password,
                unsigned long mesh_id1, byte category_id1,
                const char* version, const char* date,
                byte sensor_type, bool ldr_enabled);

bool loadWifiCredentials(ConfigData& config);
String getSSIDOptions();

#endif
