#include "m1_MPU_module.h"
#include "m2_motor_module.h"
#include "m3_bluetooth_module.h"


Servo motor0;
Servo motor1;
Servo motor2;
Servo motor3;

int motor_value_0;
int motor_value_1;
int motor_value_2;
int motor_value_3;

void motor_setup() {
  // put your setup code here, to run once:
  int start_pin = 9;
  motor0.attach(start_pin);
  motor1.attach(start_pin + 1);
  motor2.attach(start_pin + 2);
  motor3.attach(start_pin + 3);

  motor_value_0 = 700;
  motor_value_1 = 700;
  motor_value_2 = 700;
  motor_value_3 = 700;

  motor0.writeMicroseconds(motor_value_0);
  motor1.writeMicroseconds(motor_value_1);
  motor2.writeMicroseconds(motor_value_2);
  motor3.writeMicroseconds(motor_value_3);
}

int motor_killed = 0;

void kill_motor() {
  motor0.writeMicroseconds(700);
  motor1.writeMicroseconds(700);
  motor2.writeMicroseconds(700);
  motor3.writeMicroseconds(700);
  motor_killed = 1;
}


void motor_loop() {
  if (motor_killed) {
    motor0.writeMicroseconds(700);
    motor1.writeMicroseconds(700);
    motor2.writeMicroseconds(700);
    motor3.writeMicroseconds(700);
    return;
  }
  

  motor0.writeMicroseconds(motor_value_0);
  motor1.writeMicroseconds(motor_value_1);
  motor2.writeMicroseconds(motor_value_2);
  motor3.writeMicroseconds(motor_value_3);

}


