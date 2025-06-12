#ifdef WITTY_BOARD
#ifndef SENSOR_WITTY_H
#define SENSOR_WITTY_H

#include <Arduino.h>

void initWitty();
bool readWitty(float& value);

#endif
#endif // WITTY_BOARD
