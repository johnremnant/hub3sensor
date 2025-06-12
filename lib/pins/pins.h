// pins.h
#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// ---------- Default ESP12/Generic Board ----------
#if !defined(WITTY_BOARD)

  #define donePin     13
  #define switchPin   12     // trigger pin 
  #define ledPin      2      // Onboard blue LED
  #define MUX_Z       A0     // read Battery voltage or LDR 
  #define muxSel      14     // controls the MUX 
  #define configPin   3      // RX pin repurposed
  // Pin 5 = I2C SCL
  // Pin 4 = I2C SDA or defined as an input in sensor_DS18b20 and DHT22 

// ---------- Witty Board Pin Assignments ----------
#else

  #define RGB_RED_PIN     15
  #define RGB_GREEN_PIN   12
  #define RGB_BLUE_PIN    13
  #define OUTPUT_CTRL     14     // Use to control some other device 
  #define ledPin          2      // onboard blue LED
  #define MUX_Z          A0      // only reads LDR in this case, 
  #define switchPin       4      // trigger pin 
  #define sensorPin       5      // input pin for sensor_DS18b20 and _DHT22 
  #define configPin       3      // RX pin repurposed for config 

  // Define unused pins to avoid breaking shared code
  #define muxSel          -1     // Not used on Witty
  #define donePin         -1     // Not used on Witty
  
#endif

#endif

