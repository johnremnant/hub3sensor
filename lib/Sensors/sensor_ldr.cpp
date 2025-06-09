#include "sensor_ldr.h"
#include "pins.h"  // muxSel and MUX_Z must be defined here

void initLDR() {
  pinMode(muxSel, OUTPUT);
  digitalWrite(muxSel, LOW); // Default state
}

float readLDR() {
  Serial.println(F("LDR enabled — reading value..."));
  digitalWrite(muxSel, HIGH);  // Select LDR input on mux
  delay(5);                    // Allow MUX + signal to settle
  int raw = analogRead(MUX_Z);
  digitalWrite(muxSel, LOW);   // Deselect MUX after reading

  float light = map(raw, 0, 1023, 0, 100);
   Serial.print(F("Mapped LDR value: "));
  Serial.println(light);
  return light;
}
