#include "m1_MPU_module.h"
#include "m2_motor_module.h"
#include "m3_bluetooth_module.h"
#include "m4_PID_module.h"

int PID_on = 0;

int motor[4];
int16_t acc_array[3];

unsigned long m5_current_time;

unsigned long m5_previous_time_for_stats = 0;
int stats_report_interval = 3000;

unsigned long m5_previous_time_for_plotData = 0;
int plotData_report_interval = 100;

unsigned long connectionCheck = -1;

void PID_runner() {
  if (!PID_on) {
    return;
  }

  motor[0] = motor_value_0;
  motor[1] = motor_value_1;
  motor[2] = motor_value_2;
  motor[3] = motor_value_3;

  acc_array[0] = aaWorld.x;
  acc_array[1] = aaWorld.y;
  acc_array[2] = aaWorld.z;

  adjust(motor, ypr, rotation, acc_array);
}

unsigned long pt = 0;
unsigned long ct;

void listen_master_command() {
  static char buf[1000];
  static int n;
  if (Serial3.available()) {
    n = Serial3.readBytesUntil(';', buf, 1000);
    buf[n] = '\0';

    if (strcmp(buf, "#") == 0) {
      connectionCheck = millis();
    }
    else if (strcmp(buf, "a") == 0) {
      motor_value_0 += 100;
      motor_value_1 += 100;
      motor_value_2 += 100;
      motor_value_3 += 100;

    }
    else if (strcmp(buf, "s") == 0) {
      motor_value_0 -= 100;
      motor_value_1 -= 100;
      motor_value_2 -= 100;
      motor_value_3 -= 100;

    }
    else if (strcmp(buf, "d") == 0) {
      motor_value_0 = 700;
      motor_value_1 = 700;
      motor_value_2 = 700;
      motor_value_3 = 700;

    }
    else if (strcmp(buf, "start PID") == 0) {
      pid_init(ypr[2], ypr[1], ypr[0]);
      PID_on = 1;
    }
    else if (strcmp(buf, "kp") == 0) {
      n = Serial3.readBytesUntil(';', buf, 1000);
      buf[n] = '\0';
      float tmp;
      sscanf(buf, "%f", &tmp);
      kp += tmp;
    }
  }
}

void report_stats() {
  m5_current_time = millis();
  if (m5_current_time - m5_previous_time_for_stats > stats_report_interval) {
    m5_previous_time_for_stats = m5_current_time;
  }
  else {
    return;
  }

  //report for m1 MPU
  Serial3.print("#MPU_calibrated:");
  Serial3.print(MPU_calibrated);

  Serial3.print("#MPUDevStatus:");
  Serial3.print(MPUDevStatus);

  //report for m2 motor
  Serial3.print("#motor_killed:");
  Serial3.print(motor_killed);

  //report for m3 bluetooth
  Serial3.print("#bluetooth_connected:");
  Serial3.print(bluetooth_connected);

  //report for m4 PID
  Serial3.print("#kp:");
  Serial3.print(kp);
  Serial3.print("#ki:");
  Serial3.print(kp);
  Serial3.print("#kd:");
  Serial3.print(kp);

  //report for m5 controlCent
  Serial3.print("#PID_on:");
  Serial3.print(PID_on);

  Serial3.print(';');
}


void report_plot_data() {
  m5_current_time = millis();
  if (m5_current_time - m5_previous_time_for_plotData > plotData_report_interval) {
    m5_previous_time_for_plotData = m5_current_time;
  }
  else {
    return;
  }


  if (MPUDevStatus) {
    return;
  }

  Serial3.print("PLOT DATA;");



  Serial3.print(aaWorld.x);
  Serial3.print(" ");
  Serial3.print(aaWorld.y);
  Serial3.print(" ");
  Serial3.print(aaWorld.z);
  Serial3.print(" ");

  Serial3.print(rotation[0]);
  Serial3.print(" ");
  Serial3.print(rotation[1]);
  Serial3.print(" ");
  Serial3.print(rotation[2]);
  Serial3.print(" ");

  Serial3.print(ypr[0] * 1000);
  Serial3.print(" ");
  Serial3.print(ypr[1] * 1000);
  Serial3.print(" ");
  Serial3.print(ypr[2] * 1000);
  Serial3.print(" ");

  Serial3.print(motor_value_0);
  Serial3.print(" ");
  Serial3.print(motor_value_1);
  Serial3.print(" ");
  Serial3.print(motor_value_2);
  Serial3.print(" ");
  Serial3.print(motor_value_3);

  Serial3.print(";");
}


int MPU_motion_check() {
  return 0;
}
void safety_check() {
  if (MPUDevStatus != 0) {
    kill_motor();
  }
  if (!bluetooth_connected) {
    kill_motor();
  }
  if (MPU_motion_check) {
    kill_motor();
  }
}

void controlCenter_loop() {
  if (bluetooth_connected) {
    listen_master_command();
    report_stats();
    report_plot_data();
  }

  PID_runner();
}


