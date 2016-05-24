#ifndef MOTOR_MODULE_H
#define MOTOR_MODULE_H

#include <Arduino.h>
#include <Servo.h>


extern int motor_value_0;
extern int motor_value_1;
extern int motor_value_2;
extern int motor_value_3;

void motor_setup();
void motor_loop();

extern int motor_killed;

void kill_motor();

#endif
