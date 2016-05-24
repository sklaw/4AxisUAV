#ifndef BLUETOOTH_MODULE_H
#define BLUETOOTH_MODULE_H

#include <Arduino.h>
#include <string.h>

//we use Serial3 to communicate

void bluetooth_setup();
void bluetooth_loop();
void bluetooth_wait_for_connect();
extern int bluetooth_connected;

#endif
