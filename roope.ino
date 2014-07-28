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

#define MPU6050_silent

/* This is how you do multi-line
   comments */
   
// and this is how you do a single-line comment

/* This function runs only once, upon boot
   void is the return type of the function, or in this case it means that the function doesn't return a value
   */
   
/* Specs of our little prototype device:
Axel: 94mm --> perimeter: 590.6mm
Wheel diameter: 25mm --> perimeter: 78.5mm
...1 degree is about 8*11 steps...
*/
   
      
StepMotor left(513,10,9,8,7,-1); //left motor, pins 9-12, indicator 13
StepMotor right(513,6,5,4,3,-1);    //right motor, pins 4-7, indicator 8

// Move both tires simultaneously
void move_both(long degs) {
  long steps=(left.gearRatio*degs)/45L;
  while (steps>0) {
    left.singleStep(true);
    right.singleStep(true);
    steps--;
    delayMicroseconds(left.stepDelay); // without this magnet is turning too fast?
  }
  left.deenergize(); // blinks led
  right.deenergize();
}

void move_angle(double angle) {
  while (true) {
    double accel_a;
    accel_a=MPU6050_get_angle();
    double diff=calculate_diff(angle,accel_a);
    if (abs(diff)>1.0) {
      // force to correct angle
        correct(angle,diff);
    }
    move_both(5);
    
  }
  
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

void correct(double target, int dir) {
  #ifndef debug
    Serial.println("**");
    Serial.println(dir);
    Serial.println("**");
  #endif
  while (true) {
    if (dir>0) { // turn right
      right.singleStep(true);
    }
    else { // turn left
      left.singleStep(true);
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


void turn_left(long degs) {
  long steps=degs/2*8*11;
  while (steps>0) {
    left.singleStep(false);
    right.singleStep(true);
    steps--;
    delayMicroseconds(left.stepDelay);
  }
  left.deenergize();
  right.deenergize();
}

Servo myservo;
Connection c;
void setup() {
  myservo.attach(11);
  myservo.write(70);
  left.setSpeed(8);
  right.setSpeed(8);
  
  Serial.begin(9600);
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

/* The microcontroller just keeps running this function
  over and over again in an endless loop. This is where
  everything should happen.
  */
void loop() {
  #ifndef debug
    Serial.println("**Arduino connected**");
  #endif
  /*while (true) {
    char msg[60];
    int err=c.fetch_command(msg,59);
    if (err==0) {
      Serial.println("new command received:");
      Serial.println(msg);
    }
    //Serial.println(err);
  }*/
  while (true) {
    long start=micros();
    
    char msg[10];
    int err=c.fetch_command(msg,9);
    #ifndef debug
    if (err==0) {
      //Serial.println(msg);
    }
    #endif
    
    myservo.write(80);
    double f;
    f=MPU6050_get_angle();
    #ifndef debug
      Serial.println(f);
      Serial.println("");
      Serial.println(micros()-start);
      Serial.println("");
      Serial.println(left.stepDelay);
      Serial.println("--------");
    #endif
    //delay(1000);
  }
  
    
  //double f;
  //f=MPU6050_get_angle();
  //Serial.println(f);

  //move_angle(180.0);
  /*delay(1000);
  turn_left(90);
  delay(1000);*/
} 

