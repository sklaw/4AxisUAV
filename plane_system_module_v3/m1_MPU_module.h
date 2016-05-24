#ifndef MPU_MODULE_H
#define MPU_MODULE_H

#include <Arduino.h>
#include <MPU6050_6Axis_MotionApps20.h>

extern int MPU_calibrated;
extern MPU6050 mpu;

extern uint8_t MPUDevStatus;

void MPU_setup();
void MPU_loop();
extern float ypr[3];
extern int16_t rotation[3];
extern VectorInt16 aaWorld;

#endif

