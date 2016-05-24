#include <string.h>

#define SLAVE_ADDR "2015,2,120462"

char *setNameCode = "AT+NAME=TEAM8_MASTER\r\n";
char *setPasswordCode = "AT+PSWD=9527\r\n";
char *setBAUDCode = "AT+UART=38400,0,0\r\n";
char *setRoleMasterCode = "AT+ROLE=1\r\n";
char *setRoleSlaveCode = "AT+ROLE=0\r\n";
char *setCModeCode = "AT+CMODE=0\r\n";
char *setBindCode = "AT+BIND="SLAVE_ADDR"\r\n";


char *getNameCode = "AT+NAME?\r\n";
char *getPasswordCode = "AT+PSWD?\r\n";
char *getAddrCode = "AT+ADDR?\r\n";
char *getRNameCode = "AT+RNAME?"SLAVE_ADDR"\r\n";
char *getRoleCode = "AT+ROLE?\r\n";
char *getBindCode = "AT+BIND?\r\n";
char *getCModeCode = "AT+CMODE?\r\n";
char *getStateCode = "AT+STATE?\r\n";

char *rebootCode = "AT+RESET\r\n";
char *testConection = "AT\r\n";
char *clearMatchListCode = "AT+RMAAD\r\n";

int set_up_mode = 0;
int key_pin = 7;

int send_a_command(char *command) {
  digitalWrite(key_pin, HIGH);
  delay(100);
  char reply[100];
  int n;

  Serial.print("command:");
  Serial.print(command);

  Serial3.write(command);
  Serial3.setTimeout(3000);

  do {
    n = Serial3.readBytesUntil('\x0a', reply, 99);
    reply[n] = '\0';

    Serial.print("reply:");
    Serial.println(reply);
    delay(100);
  } while (Serial3.available());

  digitalWrite(key_pin, LOW);
  delay(100);
}

void setup() {
  pinMode(key_pin, OUTPUT);

  Serial.begin(115200);
  Serial3.begin(38400);


  digitalWrite(key_pin, LOW);


  send_a_command(rebootCode);
  delay(5000);

  if (set_up_mode) {
    send_a_command(setNameCode);
    send_a_command(setPasswordCode);
    send_a_command(setBAUDCode);
    send_a_command(setBindCode);
    send_a_command(setCModeCode);

    //diff start
    send_a_command(setRoleMasterCode);
    //diff end


    send_a_command(clearMatchListCode);
    send_a_command(rebootCode);
    delay(5000);
  }
  Serial.print("go!;");
}

unsigned long current_time;
unsigned long prev_connectionCheck = 0;
int connection_check_interval = 500;

void bluetooth_loop() {
  char buf[1000];
  int n;

  current_time = millis();

  if (current_time - prev_connectionCheck > connection_check_interval) {
    Serial3.print("#;");
    prev_connectionCheck = current_time;
  }

  //display message from bluetooth port
  if (Serial3.available()) {
    n = Serial3.readBytesUntil(';', buf, 1000);
    buf[n] = '\0';

    Serial.print(buf);
    Serial.print(';');
  }

  //display message from pc and send them through bluetooth port
  if (Serial.available()) {
    n = Serial.readBytesUntil(';', buf, 1000);
    buf[n] = '\0';
    Serial.print("user:");
    Serial.print(buf);
    Serial.print(';');

    if (strcmp(buf, "get name") == 0) {
      send_a_command(getNameCode);
    }
    else if (strcmp(buf, "get password") == 0) {
      send_a_command(getPasswordCode);
    }
    else if (strcmp(buf, "get addr") == 0) {
      send_a_command(getAddrCode);
    }
    else if (strcmp(buf, "get rname") == 0) {
      send_a_command(getRNameCode);
    }
    else if (strcmp(buf, "get role") == 0) {
      send_a_command(getRoleCode);
    }
    else if (strcmp(buf, "get bind") == 0) {
      send_a_command(getBindCode);
    }
    else if (strcmp(buf, "get cmode") == 0) {
      send_a_command(getCModeCode);
    }
    else {
      Serial3.print(buf);
      Serial3.print(';');
    }

  }
}

void loop() {

  bluetooth_loop();

}
