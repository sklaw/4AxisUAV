#include "m1_MPU_calibrator.h"

int MPU_calibrated = 0;

int16_t ax_offset = 0;
int16_t ay_offset = 0;
int16_t az_offset = 0;

int16_t gx_offset = 0;
int16_t gy_offset = 0;
int16_t gz_offset = 0;

long buff_count = 0, buff_ax = 0, buff_ay = 0, buff_az = 0, buff_gx = 0, buff_gy = 0, buff_gz = 0;
  
int16_t mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz;
int buffersize = 1000;

int acel_deadzone=1; 
int giro_deadzone=1;

void calibrate_acc_gyro(VectorInt16 *aaWorld, int16_t *rotation) {

  aaWorld->x += ax_offset;
  aaWorld->y += ay_offset;
  aaWorld->z += az_offset;

  rotation[0] += gx_offset;
  rotation[1] += gy_offset;
  rotation[2] += gz_offset;
  
  if (MPU_calibrated) {
    return;
  }



  buff_ax = buff_ax + aaWorld->x;
  buff_ay = buff_ay + aaWorld->y;
  buff_az = buff_az + aaWorld->z;
  buff_gx = buff_gx + rotation[0];
  buff_gy = buff_gy + rotation[1];
  buff_gz = buff_gz + rotation[2];

  buff_count++;

  if (buff_count == buffersize) {
    int ready = 0;
    mean_ax = buff_ax / buffersize;
    mean_ay = buff_ay / buffersize;
    mean_az = buff_az / buffersize;
    mean_gx = buff_gx / buffersize;
    mean_gy = buff_gy / buffersize;
    mean_gz = buff_gz / buffersize;

    Serial3.print("\nSensor readings with offsets:\t");
    Serial3.print(mean_ax); 
    Serial3.print("\t");
    Serial3.print(mean_ay); 
    Serial3.print("\t");
    Serial3.print(mean_az); 
    Serial3.print("\t");
    Serial3.print(mean_gx); 
    Serial3.print("\t");
    Serial3.print(mean_gy); 
    Serial3.print("\t");
    Serial3.print(mean_gz);
    Serial3.print(';');

    if (abs(mean_ax)<=acel_deadzone) ready++;
    else ax_offset=ax_offset-mean_ax/acel_deadzone;

    if (abs(mean_ay)<=acel_deadzone) ready++;
    else ay_offset=ay_offset-mean_ay/acel_deadzone;

    if (abs(mean_az)<=acel_deadzone) ready++;
    else az_offset=az_offset-mean_az/acel_deadzone;

    if (abs(mean_gx)<=giro_deadzone) ready++;
    else gx_offset=gx_offset-mean_gx/(giro_deadzone+1);

    if (abs(mean_gy)<=giro_deadzone) ready++;
    else gy_offset=gy_offset-mean_gy/(giro_deadzone+1);

    if (abs(mean_gz)<=giro_deadzone) ready++;
    else gz_offset=gz_offset-mean_gz/(giro_deadzone+1);


    if (ready==6) {
      MPU_calibrated = 1;
    }
    else {
      buff_ax = 0, buff_ay = 0, buff_az = 0, buff_gx = 0, buff_gy = 0, buff_gz = 0;
      buff_count = 0;
    }
  }


}

