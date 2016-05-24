#include "m4_PID_module.h"

float kp, ki, kd;
float init_roll, init_pitch, init_yaw;

void pid_init(float init_roll_, float init_pitch_, float init_yaw_) {
  init_roll = init_roll_;
  init_pitch = init_pitch_;
  init_yaw = init_yaw_;
}

void adjust(int* motor, float *ypr_array, int16_t *gyro_array, int16_t *acc_array) {
  int throttle = 2000;
//  float ax = acc_array[0]/131.072, ay = acc_array[1]/131.072, az = acc_array[2]/131.072;
  float gx = gyro_array[0]/16384, gy = gyro_array[1]/16384, gz = gyro_array[2]/16384;

  float err, pid_p, pid_i, pid_d, out_roll, out_pitch, out_yaw;

  // x - roll axis
  err = init_roll - ypr_array[2];
  pid_p = kp * err;
  pid_d = kd * gx;
  out_roll = pid_p + pid_d;

  // y - pitch axis
  err = init_pitch - ypr_array[1];
  pid_p = kp * err;
  pid_d = kd * gy;
  out_pitch = pid_p + pid_d;

  // z - yaw axis
  err = init_yaw - ypr_array[0];
  pid_p = kp * err;
  pid_d = kd * gz;
  out_yaw = pid_p + pid_d;

  // motor
  // motor[2] = (int16_t)(throttle - output_pitch - output_roll - output_yaw );
  // motor[0] = (int16_t)(throttle + output_pitch + output_roll - output_yaw );
  // motor[3] = (int16_t)(throttle - output_pitch + output_roll + output_yaw );
  // motor[1] = (int16_t)(throttle + output_pitch - output_roll + output_yaw );
  motor[3] = (throttle - out_pitch - out_roll - out_yaw );
  motor[1] = (throttle + out_pitch + out_roll - out_yaw );
  motor[2] = (throttle + out_pitch - out_roll + out_yaw );
  motor[0] = (throttle - out_pitch + out_roll + out_yaw );
}
