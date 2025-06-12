#pragma once
#include <Arduino.h>

extern bool debugEnabled;

inline void debugPrint(const char* msg) {
  if (debugEnabled) Serial.print(msg);
}

inline void debugPrintln(const char* msg) {
  if (debugEnabled) Serial.println(msg);
}

template<typename T>
inline void debugPrint(const T& value) {
  if (debugEnabled) Serial.print(value);
}

template<typename T>
inline void debugPrintln(const T& value) {
  if (debugEnabled) Serial.println(value);
}
