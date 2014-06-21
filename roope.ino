#include "step_motor.h" //Definitions from the header file

/* This is how you do multi-line
   comments */
   
// and this is how you do a single-line comment

/* This function runs only once, upon boot
   void is the return type of the function, or in this case it means that the function doesn't return a value
   */
   
int counter=0;
StepMotor left(513,7,8,9,10,11); //left motor, pins 9-12, indicator 13
StepMotor right(513,3,4,5,6,12);    //right motor, pins 4-7, indicator 8

void setup() {
  left.setSpeed(10);
  right.setSpeed(10);
  Serial.begin(9600);
}

/* The microcontroller just keeps running this function
  over and over again in an endless loop. This is where
  everything should happen.
  */
void loop() {
  left.singleStep(true);
  right.singleStep(true);
  delayMicroseconds(left.stepDelay);
}

