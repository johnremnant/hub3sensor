#include "config_web.h"
#include "pins.h"
#include "sensor_types.h"
#include "debug_log.h"

extern bool debugEnabled;
extern ESP8266WebServer server;
extern ESP8266HTTPUpdateServer httpUpdater;
extern const char* version;
extern const char* date;
extern ConfigData config;

String loadHtml(const char* path, const String& placeholder, const String& value) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    return "<html><body><h3>File not found: " + String(path) + "</h3></body></html>";
  }

  String content = file.readString();
  file.close();

  content.replace(placeholder, value);
  return content;
}

String getSSIDOptions() {
  String ssidOptions = "";
  int scanResult = WiFi.scanComplete();

  if (scanResult == WIFI_SCAN_FAILED || scanResult == -1) {
    WiFi.scanNetworks(true);  // Start async scan
    unsigned long startTime = millis();
    const unsigned long timeout = 120000; // 120 seconds

    while (millis() - startTime < timeout) {
      scanResult = WiFi.scanComplete();
      if (scanResult >= 0) break;
      delay(100);
    }

    if (scanResult == -1 || scanResult == WIFI_SCAN_FAILED) {
 
      debugPrintln("SSID scan timed out.");
 
      ssidOptions += "<option>Scan timeout</option>";
      return ssidOptions;
    }
  }

  if (scanResult == 0) {
    ssidOptions += "<option>No networks found</option>";
  } else {
    for (int i = 0; i < scanResult; ++i) {
      ssidOptions += "<option value='";
      ssidOptions += WiFi.SSID(i);
      ssidOptions += "'>";
      ssidOptions += WiFi.SSID(i);
      ssidOptions += "</option>";
    }
  }

  WiFi.scanDelete();  // Clear memory
  return ssidOptions;
}


String getGroupOptions(byte selectedGroup) {
  String options = "";

  struct GroupOption {
    byte id;
    const char* name;
  } groups[] = {
    {1, "Window/Door (Switch)"},
    {2, "Temperature/Humidity"},
    {3, "Motion (PIR)"},
    {4, "Light/Mux"},
    {5, "Smoke Alarm"}
  };

  for (auto& group : groups) {
    options += "<option value='";
    options += group.id;
    options += "'";
    if (group.id == selectedGroup) {
      options += " selected";
    }
    options += ">";
    options += group.name;
    options += "</option>";
  }

  return options;
}

String getSensorOptions(byte selectedSensor) {
  String options = "";

  for (int i = SENSOR_NONE; i <= SENSOR_MLX90614; ++i) {
    options += "<option value='";
    options += i;
    options += "'";
    if (i == selectedSensor) {
      options += " selected";
    }
    options += ">";
    options += sensorTypeName(static_cast<SensorType>(i));
    options += "</option>";
  }

  return options;
}



bool loadWifiCredentials(ConfigData& config) {
  File file = LittleFS.open(CONFIG_PATH, "r");
  if (!file) {
 
    debugPrintln("No WiFi config found.");
 
    return false;
  }

  DynamicJsonDocument doc(256);
  DeserializationError err = deserializeJson(doc, file);
  file.close();

  if (err) {
 
    debugPrintln("Failed to parse WiFi config.");
 
    return false;
  }

  config.ssid = doc["ssid"].as<String>();
  config.password = doc["password"].as<String>();
  config.mesh_id = doc["mesh"].as<int>();
  config.category_id = doc["cat"].as<int>();
  config.sensor_type = doc["sensor"] | 0;
  config.ldr_enabled = doc["ldr"] | false;

  return true;
}

void injectLDROptions(String& html, bool ldrEnabled) {
  html.replace("{{LDR_ON}}", ldrEnabled ? "selected" : "");
  html.replace("{{LDR_OFF}}", !ldrEnabled ? "selected" : "");
}

void saveConfig(const String& ssid, const String& password,
                unsigned long mesh_id1, byte category_id1,
                const char* version, const char* date,
                byte sensor_type, bool ldr_enabled) {
  StaticJsonDocument<512> doc;

  doc["ssid"] = ssid;
  doc["password"] = password;
  doc["mesh"] = mesh_id1;
  doc["cat"] = category_id1;
  doc["version"] = version;
  doc["date"] = date;
  doc["sensor"] = sensor_type;
  doc["ldr"] = ldr_enabled;  

  File configFile = LittleFS.open(CONFIG_PATH, "w");
  if (configFile) {
    serializeJson(doc, configFile);
    configFile.close();

 
    debugPrint("Saved SSID: "); debugPrintln(ssid);
    debugPrint("Saved password: "); debugPrintln(password);
    debugPrintln("Config saved.");
    debugPrint("Saved MESH ID: "); debugPrintln(mesh_id1);
    debugPrint("Saved Category ID: "); debugPrintln(category_id1);
    debugPrint("Saved version: "); debugPrintln(version);
    debugPrint("Saved Uploaded date: "); debugPrintln(date);
    debugPrint("Saved LDR selected: "); debugPrintln(ldr_enabled);
    debugPrintln("saved config succesful.");
 
  } else {
 
    debugPrintln("Failed to save config.");
 
  }
}

void enterConfigMode() {
 
  debugPrintln("Entering CONFIG MODE...");
 

  if (!LittleFS.begin()) {
 
    debugPrintln("LittleFS mount failed!");
 
    return;
  }

  loadWifiCredentials(config);

  if (config.ssid.length() == 0) {
 
    debugPrintln("No SSID stored - entering AP mode...");
 
    WiFi.disconnect();
    delay(100);
    WiFi.mode(WIFI_STA);
    WiFi.begin();
    delay(100);
    WiFi.scanNetworks();
    delay(1000);

    WiFi.mode(WIFI_AP);
    WiFi.softAP("HUB3_sender_Config");

    server.on("/", HTTP_GET, []() {
      String html = loadHtml("/wifi.html", "{{SSID_OPTIONS}}", getSSIDOptions());
      server.send(200, "text/html", html);
    });

    server.on("/save", HTTP_POST, []() {
      String ssid = server.arg("ssid");
      String password = server.arg("pass");
      byte sensor_type = server.arg("sensor").toInt();
      bool ldr_enabled = (sensor_type == SENSOR_WITTY) ? true : server.arg("ldr") == "1";

   
      debugPrintln("Save button clicked. Preparing to save config1...");
   
      saveConfig(ssid, password, config.mesh_id, config.category_id,  version, date, sensor_type, ldr_enabled);
      server.send(200, "text/html", "<h3>Wi-Fi Settings saved. Restarting...</h3>");
      delay(2000);
     if (config.sensor_type != SENSOR_WITTY) digitalWrite(donePin, LOW); // cut power to the ESP

    });

    httpUpdater.setup(&server);
    server.begin();

    unsigned long startConfig = millis();
    while (millis() - startConfig < 180000) {
      server.handleClient();
      yield();
    }

   if (config.sensor_type != SENSOR_WITTY) digitalWrite(donePin, LOW); // cut power to the ESP

  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid.c_str(), config.password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    debugPrint(".");
  }

 
  debugPrintln("\nWi-Fi connected!");
  debugPrint("IP Address: ");
  debugPrintln(WiFi.localIP());
 

  server.on("/", HTTP_GET, []() {
    server.sendHeader("Location", "/config", true);
    server.send(302, "text/plain", "");
  });

  server.on("/config", HTTP_GET, []() {
  String html = loadHtml("/config.html", "{{GROUP_OPTIONS}}", getGroupOptions(config.category_id));
 html.replace("{{SENSOR_OPTIONS}}", getSensorOptions(config.sensor_type));

// Inject JS that enables LDR + disables checkbox if sensor is Witty
String jsScript = R"rawliteral(
<script>
  document.addEventListener("DOMContentLoaded", function() {
    const sensorSelect = document.getElementById("sensor");
    const ldrCheckbox = document.getElementById("ldr");

    function updateLDRState() {
      const SENSOR_WITTY = 6;
      const selected = parseInt(sensorSelect.value);
      if (selected === SENSOR_WITTY) {
        ldrCheckbox.checked = true;
        ldrCheckbox.disabled = true;
      } else {
        ldrCheckbox.disabled = false;
      }
    }

    sensorSelect.addEventListener("change", updateLDRState);
    updateLDRState();
  });
</script>
)rawliteral";

html += jsScript;

  injectLDROptions(html, config.ldr_enabled);
  server.send(200, "text/html", html);
});


  server.on("/update.html", HTTP_GET, []() {
    server.send(200, "text/html", loadHtml("/update.html", "", ""));
  });

  server.on("/save2", HTTP_POST, []() {
    unsigned long mesh = server.arg("mesh").toInt();
    byte cat = server.arg("cat").toInt();
    byte sensor_type = server.arg("sensor").toInt();
    bool ldr_enabled = (sensor_type == SENSOR_WITTY) ? true : server.arg("ldr") == "1";

 	
    debugPrintln("Save button clicked. Preparing to save config2...");
 
    saveConfig(config.ssid, config.password, mesh, cat, version, date, sensor_type, ldr_enabled);
    server.send(200, "text/html", "<h3>Settings saved. Restarting...</h3>");
    delay(2000);
    if (config.sensor_type != SENSOR_WITTY)digitalWrite(donePin, LOW); // cut power to the ESP
  });

  httpUpdater.setup(&server);
  server.begin();
 
  debugPrintln("Web server started.");
 

  unsigned long configStart = millis();
  unsigned long lastBlink = 0;
  bool ledState = false;

  pinMode(ledPin, OUTPUT);
  while (millis() - configStart < 300000) {
    server.handleClient();
    if (millis() - lastBlink >= 500) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      lastBlink = millis();
    }
    delay(1);
  }

  digitalWrite(ledPin, LOW);
  if (config.sensor_type != SENSOR_WITTY)digitalWrite(donePin, LOW); // cut power to the ESP

}
