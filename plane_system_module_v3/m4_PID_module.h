#ifndef PID_MODULE_H
#define PID_MODULE_H

#include <I2Cdev.h>


void pid_init(float init_raw_, float init_pitch_, float init_yaw_);
extern float kp, ki, kd;
void adjust(int* motor, float *ypr_array, int16_t *gyro_array, int16_t *acc_array);


#endif
