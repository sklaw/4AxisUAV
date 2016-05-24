#include "m3_bluetooth_module.h"
#include "m5_controlCenter_module.h"

#define MASTER_ADDR "2015,2,120472"



char *setNameCode = "AT+NAME=TEAM8_SLAVE\r";
char *setPasswordCode = "AT+PSWD=9527\r";
char *setBAUDCode = "AT+UART=38400,0,0\r";
char *setRoleSlaveCode = "AT+ROLE=0\r";
char *setCModeCode = "AT+CMODE=0\r";
char *setBindCode = "AT+BIND="MASTER_ADDR"\r";


char *getNameCode = "AT+NAME?\r";
char *getPasswordCode = "AT+PSWD?\r";
char *getAddrCode = "AT+ADDR?\r";
char *getRNameCode = "AT+RNAME?"MASTER_ADDR"\r";
char *getRoleCode = "AT+ROLE?\r";
char *getBindCode = "AT+BIND?\r";
char *getCModeCode = "AT+CMODE?\r";
char *getStateCode = "AT+STATE?\r";

char *rebootCode = "AT+RESET\r";
char *testConection = "AT\r";
char *clearMatchListCode = "AT+RMAAD\r";


int key_pin = 6;
int set_up_mode = 0;
int bluetooth_connected = 0;


unsigned long m3_previousTime = 0;
unsigned long m3_currentTime;

long m3_pminus1_interval = 8000;
long m3_p0_interval = 500;
long m3_p1_interval = 100;
long m3_p2_interval = 1000;
long m3_p3_interval = 100;

int send_a_command(char *command) {
  digitalWrite(key_pin, HIGH);
  delay(100);
  char reply[100];
  int n;

  Serial.println("command:");
  Serial.println(command);

  Serial3.write(command);
  Serial3.setTimeout(3000);

  do {
    n = Serial3.readBytesUntil('\x0a', reply, 99);
    reply[n] = '\0';

    Serial.println("reply:");
    Serial.println(reply);
    delay(100);
  } while (Serial3.available());

  digitalWrite(key_pin, LOW);
  delay(100);
}

//for setup use
void bluetooth_setup() {
  pinMode(key_pin, OUTPUT);

  Serial3.begin(38400);

  send_a_command(rebootCode);
  delay(5000);

  if (set_up_mode) {
    send_a_command(setNameCode);
    send_a_command(setPasswordCode);
    send_a_command(setBAUDCode);
    send_a_command(setBindCode);
    send_a_command(setCModeCode);

    //diff start
    send_a_command(setRoleSlaveCode);
    //diff end


    send_a_command(clearMatchListCode);
    send_a_command(rebootCode);
    delay(5000);
  }


}

void bluetooth_wait_for_connect() {
  static char buf[1000];
  static int n;
  while (connectionCheck == -1) {
    if (Serial3.available()) {
      n = Serial3.readBytesUntil(';', buf, 1000);
      buf[n] = '\0';

      Serial3.print(buf);

      if (strcmp(buf, "#") == 0) {
        connectionCheck = millis();
      }
    }
  }
  bluetooth_connected = 1;
}


void bluetooth_loop() {
  if (millis() - connectionCheck > 3000) {
    bluetooth_connected = 0;
  }
}
