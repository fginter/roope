/*
  MPU-6050 connection
  
  MPU           ARDUINO
  =========================
  VDD,VIO  --   3V3 (+3.3V)
  GND      --   GND
  SCL      --   Analog 5
  SDA      --   Analog 4
*/

//This would strictly be only needed in mpu6050.cpp, but due to Arduino's
//extra processing, it also needs to be in the main .ino
#include <EEPROM.h>

#include <Wire.h>
#include <Servo.h>
#include "mpu6050.h"
#include "step_motor.h" //Definitions from the header file
#include "connection.h"
#include "global.h"

//#define MPU6050_silent

/* This is how you do multi-line
   comments */
   
// and this is how you do a single-line comment

/* This function runs only once, upon boot
   void is the return type of the function, or in this case it means that the function doesn't return a value
   */
   
      
StepMotor left(513,7,8,9,10,-1); //left motor, pins 7-10, no indicator LED
StepMotor right(513,3,4,5,6,-1);  
Servo myservo;
Servo penlift_servo; //115 degrees pen up, 130 degrees, pen down
Connection c;
int S_LOW = 60; // lowest possible servo angle
int S_HIGH = 80; // highest possible servo angle
int S_MIDDLE = S_LOW+(S_HIGH-S_LOW)/2;
int WHITE = 0; // 

void setup() {
  myservo.attach(11);
  penlift_servo.attach(12);
  penlift_servo.write(115);
  myservo.write(70);
  left.setSpeed(8);
  right.setSpeed(8);
  
  Serial.begin(9600);
  #ifndef debug
    Serial.println("**Arduino connected**");
  #endif
  
  int err;
  err=MPU6050_init();
  #if !defined(MPU6050_silent) && !defined(debug)
    Serial.print(F("*** MPU6050 init exit status: "));
    Serial.println(err,DEC);
  #endif
  //MPU6050_set_angle_reference(); //only needed once
  MPU6050_read_angle_reference_from_eeprom(); //Set the reference angle of the MPU from EEPROM
  
}

/* MPU6050_get_angle() returns the angle relative to the position
   in which the reset button was pressed. The return value is double in the 0-360 degree range
   */


void follow_command(comm_t cmm) { 
  Serial.println(cmm.steps);
  
  unsigned int i = 0;
  boolean f = true;
  if (cmm.backwards==1) {
    f=false;
  }
  if (cmm.pen==0) {
    penlift_servo.write(115); // pen up
  }
  else {
    myservo.write(S_MIDDLE); // make it to be middle
    penlift_servo.write(130); // pen down
  }
  while (i<=cmm.steps) {
    //unsigned long start = micros(); // start time
    
    if (cmm.pen>WHITE && i%2==0) {
      myservo.write(S_LOW);
    }
    else if (cmm.pen>WHITE) {
      myservo.write(S_HIGH);
    }
    double accel_a;
    accel_a=MPU6050_get_angle();
    double diff=calculate_diff((double)cmm.angle,accel_a);
    if (abs(diff)>1.0) {
      // force to correct angle
      correct((double)cmm.angle,diff,true);
    }
    left.singleStep(f);
    right.singleStep(f);
    
    //long left = (long)(left.stepDelay-(micros()-start));
    //if (left>0) {
    delayMicroseconds(left.stepDelay);
    //}
    i++;
  }
  Serial.println(";");
}

double calculate_diff(double target, double robot) {
  double diff = target-robot; // both between 0-360
  boolean reversed = false;
  int dir = 1;
  if (abs(diff)>180) { // okay, we don't want this...
    if (diff>0) {
      dir = -1;
    }
    diff=360-abs(diff);
  }
  diff=diff*dir;
  return diff;
}

void correct(double target, int dir, boolean forward) {
  #ifndef debug
    Serial.println("**");
    Serial.println(dir);
    Serial.println("**");
  #endif
  while (true) {
    if (dir>0 && forward==true) { // turn right
      left.singleStep(true);
    }
    else if (dir<0 && forward==true){ // turn left
      right.singleStep(true);
    }
    else if (dir>0 && forward==false) {
      left.singleStep(false);
    }
    else {
      right.singleStep(false);
    }
    // ready yet?
    double new_a=MPU6050_get_angle(); // get new angle
    double diff=calculate_diff(target,new_a);
    if (abs(diff)<0.001) {
      break;
    }
    else if ((dir<0 && diff>0) || (dir>0 && diff<0)) {
      break;  
    }
    
  }
    
}


void move_horizontal(double orig_angle, boolean forward) {
  double target;
  if (forward==true) {
    target = 340.0;
  }
  else {
    target = 20;
  }
  double robot = MPU6050_get_angle();
  double diff = calculate_diff(target,robot);
  correct(target,diff,forward);
  
  robot = MPU6050_get_angle();
  diff = calculate_diff(orig_angle,robot);
  correct(orig_angle,diff,forward);
}



/* The microcontroller just keeps running this function
  over and over again in an endless loop. This is where
  everything should happen.
  */

void loop() {
  
  
  //long start=micros();
    
  comm_t msg;
  
  int err=c.fetch_command((char*)(void*)&msg);
  //Serial.println(err);
  if (err==0) {
    follow_command(msg);
  }
    
  
  /*  
  myservo.write(80);
  double f;
  f=MPU6050_get_angle();
  #ifndef debug
    Serial.print("angle:");
    Serial.print(f);
    Serial.println("");
    Serial.print("time used:");
    Serial.print(micros()-start);
    Serial.println("");
    Serial.print("time avail:");
    Serial.print(left.stepDelay);
    Serial.println("--------");
  #endif
  */
  
  //delay(1000);
  
    
} 

