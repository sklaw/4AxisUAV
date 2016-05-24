#ifndef MPU_CALIBRATOR_H
#define MPU_CALIBRATOR_H

#include "MPU6050_6Axis_MotionApps20.h"

extern int MPU_calibrated;
void calibrate_acc_gyro(VectorInt16 *aaWorld, int16_t *rotation);

#endif
