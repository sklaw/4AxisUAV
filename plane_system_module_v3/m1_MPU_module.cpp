#include "m1_MPU_module.h"
#include "m3_bluetooth_module.h"
#include "m1_MPU_calibrator.h"

#include "I2Cdev.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

extern int MPU_calibrated;

MPU6050 mpu;

#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t MPUDevStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
int16_t rotation[3];



unsigned long m1_currentTime;
unsigned long m1_previousTime_for_mpuInt = 0;
unsigned long m1_interval_for_waitMpuInt = 1000;

unsigned long m1_previousTime_for_devIDCheck = 0;
unsigned long m1_interval_for_devIDCheck = 1000;


// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}




// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void MPU_setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif



  while (!Serial3); // wait for Leonardo enumeration, others continue immediately

  // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3v or Ardunio
  // Pro Mini running at 3.3v, cannot handle this baud rate reliably due to
  // the baud timing being too misaligned with processor ticks. You must use
  // 38400 or slower in these cases, or use some kind of external separate
  // crystal solution for the UART timer.

  // initialize device
  Serial3.print("Initializing I2C devices...;");
  mpu.initialize();


  // load and configure the DMP
  Serial3.print("Initializing DMP...;");
  MPUDevStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);

  // make sure it worked (returns 0 if so)
  if (MPUDevStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial3.print("Enabling DMP...;");
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial3.print("Enabling interrupt detection (Arduino external interrupt 0)...;");
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial3.print("DMP ready! Waiting for first interrupt...;");
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();

    m1_previousTime_for_mpuInt = millis();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial3.print("DMP Initialization failed (code ");
    Serial3.print(MPUDevStatus);
    Serial3.print(");");
  }


}






void MPU_loop() {
  // if programming failed, don't try to do anything
  if (MPUDevStatus != 0) return;

  m1_currentTime = millis();

  if (m1_currentTime - m1_previousTime_for_devIDCheck > m1_interval_for_devIDCheck) {
    int devID = mpu.getDeviceID();
    if (devID != 56) {
      Serial.print("MPU down;");
      Serial3.print("MPU down;");
      MPUDevStatus = 1;
      return;
    }
    else {
      Serial.print("devID:");
      Serial.println(devID);
      m1_previousTime_for_devIDCheck = m1_currentTime;
    }
  }


  

  if (!mpuInterrupt) {
    if (m1_currentTime - m1_previousTime_for_mpuInt > m1_interval_for_waitMpuInt) {
      Serial.print("MPU down;");
      Serial3.print("MPU down;");
      MPUDevStatus = 1;
    }
    return;
  }

  

  m1_previousTime_for_mpuInt = m1_currentTime;


  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait

    while (fifoCount < packetSize) {
      fifoCount = mpu.getFIFOCount();

    }

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;


    //reading starts here.
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
    mpu.getRotation(&rotation[0], &rotation[1], &rotation[2]);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    calibrate_acc_gyro(&aaWorld, rotation);
  }

//  Serial.print("PLOT DATA;");
//
//  Serial.print(aaWorld.x);
//  Serial.print(" ");
//  Serial.print(aaWorld.y);
//  Serial.print(" ");
//  Serial.print(aaWorld.z);
//  Serial.print(" ");
//
//  Serial.print(rotation[0]);
//  Serial.print(" ");
//  Serial.print(rotation[1]);
//  Serial.print(" ");
//  Serial.print(rotation[2]);
//  Serial.print(" ");
//
//  Serial.print(ypr[0] * 1000);
//  Serial.print(" ");
//  Serial.print(ypr[1] * 1000);
//  Serial.print(" ");
//  Serial.print(ypr[2] * 1000);
//  Serial.print(" ");
//
//  Serial.print(500);
//  Serial.print(" ");
//  Serial.print(1000);
//  Serial.print(" ");
//  Serial.print(1500);
//  Serial.print(" ");
//  Serial.print(2000);
//  Serial.print(" ");
//
//  Serial.println(";");

}


