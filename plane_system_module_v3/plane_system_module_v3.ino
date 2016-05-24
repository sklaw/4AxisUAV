#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <Wire.h>
#include <Servo.h>
#include <Arduino.h>

#include "m1_MPU_module.h"
#include "m2_motor_module.h"
#include "m3_bluetooth_module.h"
#include "m5_controlCenter_module.h"

void setup() {
  Serial.begin(115200);      // Set up bps for Serial communication

  motor_setup();
  bluetooth_setup();
  bluetooth_wait_for_connect();


  Serial3.print("=========welcom!========;");
  


  
  Serial3.print("start init MPU;");

  MPU_setup();

  Serial3.print("----------go!----------;");

}

void loop() {
  bluetooth_loop();

  motor_loop();

  MPU_loop();

  controlCenter_loop();


}


