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
#include "mpu6050.h"
#include "step_motor.h" //Definitions from the header file

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
   
      
StepMotor left(513,7,8,9,10,11); //left motor, pins 9-12, indicator 13
StepMotor right(513,3,4,5,6,12);    //right motor, pins 4-7, indicator 8

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


void setup() {
  left.setSpeed(10);
  right.setSpeed(10);
  Serial.begin(9600);
  int err;
  err=MPU6050_init();
  #ifndef MPU6050_silent
    Serial.print(F("*** MPU6050 init exit status: "));
    Serial.println(err,DEC);
  #endif
  // MPU6050_set_angle_reference(); //only needed once
  MPU6050_read_angle_reference_from_eeprom(); //Set the reference angle of the MPU from EEPROM
}

/* MPU6050_get_angle() returns the angle relative to the position
   in which the reset button was pressed. The return value is double in the +/-180 degree range
   */

/* The microcontroller just keeps running this function
  over and over again in an endless loop. This is where
  everything should happen.
  */
void loop() {
  /*
  while(1) {
      Serial.println(MPU6050_get_angle());
      delay(200);
  }
  */
    
  move_both(360);
  delay(1000);
  turn_left(90);
  delay(1000);
} 

