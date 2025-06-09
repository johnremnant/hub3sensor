/*  ============================================================================================================================================

     ESPnow ULP Trigger Sensors (All Sensors)
     Dual light and battery monitor using a 2 in 1 out Mux 
     Use a PIR or other sw input to trigger a reading 	 
     This SENDER includes Status + Light + Battery + DHT22/SDS18B20 and I2C sensors
     V2-V3 Swap Pin assignments Pins IO4 and IO13, IO5 and IO12 
	 For use with V3 and V3.1 PCBs ( the MUX pinout on V3 boards is incorrect. Fit R11 and just read the battery on these boards )
   And ESP02 PCB V1.0 - Same Pinout just physically differnt 
                       ESP12
                      --------------                                               Top      Bottom
                     | RST       TX |                                             ------    ------
          Mux Z ---->| ADC       RX |<----Repurpose as an input ConfigPin         |Vcc |    | D5 |   D0
                     | CH        05 |---->Spare IO pin or SCL                     |Gnd |    | D4 |   D2 have pads inset from board edge 
                     | 16        04 |---->DHT22 or SDS18B20 or SDA                | RX |    |D13 |
MUX Sel I0 or I1<----| 14        00 |                                             | Tx |    | A0 |
Sw/PIR/ Hall effect->| 12        02 |                                             |D12 |    |RST |
        Done Pin<----| 13        15 |                                             |D14 |    ------
                     | Vcc      GND |                                             ------    
                      --------------
Version logs
 v2.0.0 add wifi Scan, Config web page and OTA cabability may-25
 v2.0.1 add date and version to the config file
 v2.2.0 Separate the HTML out into spearate files
 v2.2.2 Seperate out all the sensors into their own files 
 v2.2.3 trying to fix LDR enabled issue 
 v2.2.4 Migrate to PlatformIO project 
 
 ##### increment version and date when changes are made #####


   ================================================================================================================================================== */

#include <Arduino.h>
const char* version = "2.2.4";  // Define as needed
const char* date = __DATE__; // Compile date
enum State {
  INIT,
  WAIT_FOR_ACK,
  CHECK_TIMEOUT,
  ENTER_CONFIG_MODE,
  SLEEP
};
#define DEBUG_FLAG
// put function declarations here:
void setup();
void loop();
void sendReading();
void OnDataSent(uint8_t* mac_addr, uint8_t sendStatus);
void gotoSleep();
#ifdef DEBUG_FLAG
void logState(State state);  // This will now compile
#endif
float readBatteryVoltage();




// Note Battery voltage sensor is always enabled 

/**** libary Includes ******/
#include <ESP8266WiFi.h>
#include <espnow.h>
// To enable config options via a webpage and AP the next 3 includes are needed
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "pins.h"
// to cater for OTA uploads
#include "sensor_types.h"
#include <Updater.h>
#include "config_web.h"
#include "sensor_manager.h"

#include "sensor_dht.h"
#include "sensor_bme280.h"
#include "sensor_ds18b20.h"
#include "sensor_bmp280.h"
#include "sensor_mlx90614.h"
#include "sensor_bmp180.h"
#include "sensor_ldr.h"


 SensorType currentSensor;

/****** Configuration variables *****/
#define CONFIG_PATH "/config.json"
#define CONFIG_MODE_TIMEOUT 3000  // Config button needs to be pressed for 3 secs to enter Config mode

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
ConfigData config;

/**** Group Variables ******/
//Avaialble sender groups - the groups defines what purpose the sensor is used for 
#define GROUP_SWITCH 1 // Window or door detect
#define GROUP_HT 2     // sends all available sensors for now , generally for temp controled applications
#define GROUP_MOTION 3 // PIR controll sensors for security alarms  
#define GROUP_LIGHT 4 // for light controlled applicatioons - must have a mux fitted 
#define GROUP_SMOKE 5 // to interface with old Smoke alarms - the smoke alarm will be the trigger 

/**** Other Variables ******/
#define longest_up_time 3000
uint8_t receiverAddress[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };  // the hub receiver has a user defined "12:34:56:78:90:abc" mac address
bool ack_received = false;
unsigned long start_time = millis();
int mesh_id1 = 123456;  // default mesh address - will be overwritten by the LittleFS stored value
byte category_id1;

State currentState = INIT;

/***** Structured messages *****/

typedef struct Group_type {
  int mesh_id;
  uint8_t sensor_id[6];  // the MAC address of this sender
  byte category;
  bool status;
  float temperature;
  float humidity;
  //float pressure; // If uncommented add to the Hub Structured messsage
  float battery;
  float light;
  //byte    wittyRGB; // for future use maybe // can utilise the tri colour LED and LDR - needs work in the HUB.
} Group_type;
Group_type Now_msg;

/* ############################ Setup ############################################ */
void setup() {

#ifdef DEBUG_FLAG
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starting in debug mode.");
#endif

  if (!LittleFS.begin()) {
#ifdef DEBUG_FLAG
    Serial.println("LittleFS mount failed.");
#endif
    ESP.restart();
  }

   
  // read config optins from LitleFS

  File file = LittleFS.open(CONFIG_PATH, "r");
  if (file) {
    String content = file.readString();
    file.close();

    DynamicJsonDocument doc(256);
    DeserializationError err = deserializeJson(doc, content);
    if (!err) {
      config.mesh_id = doc["mesh"] | 123456;  //Use doc["mesh_id"] if it exists, otherwise default to 6734922
      config.category_id = doc["cat"] | 2;
      

      mesh_id1 = config.mesh_id;  //overwrite the MESH ID with what is stored in littleFS
      category_id1 = config.category_id;
 
#ifdef DEBUG_FLAG
      Serial.println("Loaded config from LittleFS:");
      Serial.print("Mesh ID: ");
      Serial.println(config.mesh_id);
      Serial.print("Category ID: ");
      Serial.println(config.category_id);
      Serial.print("Version: ");
      Serial.println(version);
      Serial.print("Date: ");
      Serial.println(date);
            
      Serial.print("Raw config content: ");
Serial.println(content);
#endif
    } else {
#ifdef DEBUG_FLAG
      Serial.println("Failed to parse config JSON.");
#endif
    }
  } else {
#ifdef DEBUG_FLAG
    Serial.println("No config file found, entering config mode.");
#endif
  }

if (!loadWifiCredentials(config)) {
    enterConfigMode();  // Force config mode even if pin is not LOW
    return;
  }
  

  pinMode(configPin, INPUT_PULLUP); // To enter Config mode Pull this pin low, via a 10k resistor to gnd
  pinMode(switchPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(muxSel, OUTPUT);
  digitalWrite(donePin, HIGH);  //Important for it to work. This line needs to be before the pinMode assignment ??? JSR 9/11/2024
  pinMode(donePin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(muxSel, LOW);

  
 
  ack_received = false;
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    delay(100);
    ESP.restart();
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  sendReading();
}
/* ##############################end of setup ################################### */
/* ################################### Loop ##################################### */

void loop() {
  if (digitalRead(configPin) == LOW) {
    
  loadWifiCredentials(config);
  initSensor(static_cast<SensorType>(config.sensor_type));
   enterConfigMode();
}
#ifdef DEBUG_FLAG
  logState(currentState);
#endif

  switch (currentState) {
    case INIT:
      sendReading();
      currentState = WAIT_FOR_ACK;
      break;

    case WAIT_FOR_ACK:
      if (ack_received) {
        currentState = SLEEP;
      } else if (millis() - start_time >= longest_up_time) {
        currentState = CHECK_TIMEOUT;
      }
      break;

    case CHECK_TIMEOUT:
      if (digitalRead(configPin) == LOW) {
        currentState = SLEEP;
      } else if (millis() - start_time > CONFIG_MODE_TIMEOUT) {
        currentState = ENTER_CONFIG_MODE;
      }
      break;

    case ENTER_CONFIG_MODE:
      enterConfigMode();  // this blocks and reboots after 5 minutes
      break;

    case SLEEP:
      gotoSleep();  // this turns off power or enters deep sleep
      break;
  }
}

/* ############################End of Loop ##################################### */
// put function definitions here:
void sendReading() {
  /**** Create the ESPnow message values ******/
  Now_msg.mesh_id = mesh_id1;
  Now_msg.category = category_id1;  // Change the Group name to what is appropriate for this sender
  WiFi.macAddress(Now_msg.sensor_id);
  Now_msg.status = digitalRead(switchPin);  //reads the value of the PIR or other input that woke the board up
  //Add sensor readings to ESP now message 
  float temperature = 0.0;
  float humidity = 0.0;
  readSensor(static_cast<SensorType>(config.sensor_type), temperature, humidity);

  Now_msg.temperature = temperature;
  Now_msg.humidity = humidity; //on BMP280 and oterh temp sensors this is not avaialble
//we are noot currently using the pressure reading from the BMx devices 



  digitalWrite(muxSel, LOW);
  delay(5);                                     // Allow settling time
  Now_msg.battery = readBatteryVoltage();

  //float light = 0.0;
  Now_msg.light = readLDRSensor(config.ldr_enabled);


  /**** Create the ESPnow message values end ******/

  // now send the ESPnow meassage
  esp_now_send(receiverAddress, (uint8_t*)&Now_msg, sizeof(Now_msg));
}

void OnDataSent(uint8_t* mac_addr, uint8_t sendStatus) {
  ack_received = (sendStatus == 0);  // 0 = success
#ifdef DEBUG_FLAG
  Serial.print("ESP-NOW send status: ");
  Serial.println(sendStatus == 0 ? "Success" : "Fail");
#endif
}

void gotoSleep() {
  if (digitalRead(configPin) == HIGH) {
    digitalWrite(donePin, LOW);  // This pin disables the 3.3V supply to the ESP12 or ESP02.
    delay(10);
#ifdef DEBUG_FLAG
    Serial.println("Message sent - going to sleep...");
#endif
    ESP.deepSleep(0);  //doesn't do anything as the power to the ESP has been removed
  } else {

    if (millis() - start_time > CONFIG_MODE_TIMEOUT) {
      #ifdef DEBUG_FLAG
      Serial.print("ConfigPin State ");
      Serial.println(digitalRead(configPin));
      #endif
      enterConfigMode();
      delay(10);
    }
  }
}

#ifdef DEBUG_FLAG
void logState(State state) {
  static State lastLoggedState = (State)-1;  // invalid initial state to force first log

  if (state != lastLoggedState) {
    lastLoggedState = state;

    switch (state) {
      case INIT: Serial.println("State: INIT"); break;
      case WAIT_FOR_ACK: Serial.println("State: WAIT_FOR_ACK"); break;
      case CHECK_TIMEOUT: Serial.println("State: CHECK_TIMEOUT"); break;
      case ENTER_CONFIG_MODE: Serial.println("State: ENTER_CONFIG_MODE"); break;
      case SLEEP: Serial.println("State: SLEEP"); break;
    }
  }
}
#endif

float readBatteryVoltage() {
  digitalWrite(muxSel, LOW);
  delay(5);
  int raw = analogRead(MUX_Z);
  float v = raw * (1 / 1023.0);
  return v * 4.3;
}

