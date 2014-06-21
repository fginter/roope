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
}

/* The microcontroller just keeps running this function
  over and over again in an endless loop. This is where
  everything should happen.
  */
void loop() {
  move_both(360);
  delay(1000);
  turn_left(90);
  delay(1000);
} 
