#include "step_motor.h" //Definitions from the header file

/* This is how you do multi-line
   comments */
   
// and this is how you do a single-line comment

/* This function runs only once, upon boot
   void is the return type of the function, or in this case it means that the function doesn't return a value
   */
   
StepMotor left(512,9,10,11,12,13); //left motor, pins 9-12, indicator 13
StepMotor right(512,4,5,6,7,8);    //right motor, pins 4-7, indicator 8

void setup() {
  left.setSpeed(30);
  right.setSpeed(30);
}

/* The microcontroller just keeps running this function
  over and over again in an endless loop. This is where
  everything should happen.
  */
void loop() {
  //This one won't go straight, of course. :)
  left.turnDegrees(360);
  right.turnDegrees(360);
}

