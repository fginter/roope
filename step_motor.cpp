/*
  #include <xxx.h>  standard headers, taken from system locations
  #include "xxx.h"  own headers, taken from project locations
  */

#include <Arduino.h> //Header file of the Arduino API
#include "step_motor.h" //Include the definitions so we can compile the code below

/* This file actually implements what step_motor.h defines */

/* Constructor.

  'this' is a pointer to the current object, i.e. its address in the memory
  to dereference the pointer and get the object you can do (*this)
  if you only need to access a method/attribute, you can use ->
  so 
    this->i1
  and
    (*this).i1
  are equivalent.
*/
StepMotor::StepMotor(int gearRatio, int i1, int i2, int i3, int i4, int energizedIndicatorPin) { //constructor has no return type (not even void)
  this->gearRatio=gearRatio;
  this->i1=i1; //this-i1 is the instance attribute, while i1 is the local variable (argument of the function)
  this->i2=i2;
  this->i3=i3;
  this->i4=i4;
  this->energizedIndicatorPin;
  pinMode(i1,OUTPUT); //Tells arduino that you will use the pin as output
  pinMode(i2,OUTPUT);
  pinMode(i3,OUTPUT);
  pinMode(i4,OUTPUT);
  if (energizedIndicatorPin>=0) {
    pinMode(energizedIndicatorPin,OUTPUT);
  }
  //The output value of the pins is undefined now, so
  //switch all coils off so we don't burn the motor
  deenergize(); //same as this->deenergize()
  //The lower this value, the less likely the motor is to skip a step, but the slower it is
  setSpeed(60); //60RPM, i.e. 1Hz
};

void StepMotor::setSpeed(int speed) {
  //Speed is given in RPM
  //There is 8 steps to a single rotation of the inner shaft of the motor
  //Steps per minute: 8*gearRatio*RPM
  //Steps per second: 8*gearRatio*RPM/60
  //Steps per microsecond: 8*gearRatio*RPM/60000000
  //Delay in microseconds between steps: 60000000/(8*gearRatio*RPM)
  //                                   : 7500000/(gearRatio*RPM)
  // unsigned long: non-negative integer, as large as possible here
  /*
  7500000UL  -> the UL says what type this constant should be (UL = unsigned long), without that, it would be a signed integer
  (unsigned long)gearRatio -> retype gearRatio from whatever it is to an unsigned long
  */
  unsigned long delay=7500000UL/((unsigned long)gearRatio*(unsigned long)speed);
  this->stepDelay=(unsigned int) delay; //Remember the delay in microseconds between steps as an attribute
};

//Note: it takes 8 * gearRatio steps for a full revolution of the output shaft
// steps can be negative for rotating in the other direction
void StepMotor::turn(long steps) {
  //Translate from +/- steps to a true/false and +steps
  bool fwd=true; //defines a true/false (bool) variable and sets it to true
  if (steps<0) { //if condition must always be closed betwen parentheses (...)
      fwd=false;
      steps*=-1;
  }
  long start=0L;
  while (steps>0) {
    singleStep(fwd); //do one step, then decrease steps counter, and then wait until the next step
    steps--; // More or less same as steps=steps-1 in this context
    /*This was meant to give the motor a slow start, let's see if we need it, now ineffective (start always 0, i.e. false)*/
    if (start) {
      delayMicroseconds(stepDelay*start);
      if (steps%30 == 0) {
        start--;
      }
    }
    else {
      delayMicroseconds(stepDelay);
    }
  }
  deenergize(); //When done turning, deenergize the coils. I wonder how important this is... but let's play it safe
};

//degs can also be negative and is not restricted to 0-360 range
void StepMotor::turnDegrees(long degs) {
  //How many steps?
  long steps=(gearRatio*degs)/45L; //8/360 -> 1/45
  turn(steps);
}

void StepMotor::deenergize() {
  //Drop all outputs to 0V
  if (energizedIndicatorPin>=0) {
    //Indicate that some coil is energized
     digitalWrite(energizedIndicatorPin, HIGH);
  }
 
 
  digitalWrite(i1, LOW);  //i1 is same as this->i1
  digitalWrite(i2, LOW); 
  digitalWrite(i3, LOW); 
  digitalWrite(i4, LOW); 
};


void StepMotor::singleStep(bool fwd) {
  //phase is a counter from 0-7 going through the 8 possible positions of the stepper
  if (fwd) {
    phase++;
    if (phase==8) {
      phase=0;
    }
  }
  else {
    phase--;
    if (phase==-1) {
      phase=7;
    }
  }
  //Now that the phase has been updated, energize the coils accordingly
  energize();
}

void StepMotor::energize() {
  /*Hard-coded outputs for the eight steps. The motor must of course be connected correctly
  to the pins.
  
  switch(variable) {
    case X1:
      statements...
      break;
    case X2
      statements...
      break;
  }
  
  Sort of like if ... elif ... elif ... but faster
  once it enters trough a case, all statements to the end will be carried out not just those under the case
  so one needs to put there the break;
  */
  
    if (energizedIndicatorPin>=0) {
      //Indicate that some coil is energized
      digitalWrite(energizedIndicatorPin, HIGH);
    }
  
   switch(phase){ 
   case 0: 
     digitalWrite(i1, LOW);  
     digitalWrite(i2, LOW); 
     digitalWrite(i3, LOW); 
     digitalWrite(i4, HIGH); 
     break;  
   case 1: 
     digitalWrite(i1, LOW);  
     digitalWrite(i2, LOW); 
     digitalWrite(i3, HIGH); 
     digitalWrite(i4, HIGH); 
     break;  
   case 2: 
     digitalWrite(i1, LOW);  
     digitalWrite(i2, LOW); 
     digitalWrite(i3, HIGH); 
     digitalWrite(i4, LOW); 
     break;  
   case 3: 
     digitalWrite(i1, LOW);  
     digitalWrite(i2, HIGH); 
     digitalWrite(i3, HIGH); 
     digitalWrite(i4, LOW); 
     break;  
   case 4: 
     digitalWrite(i1, LOW);  
     digitalWrite(i2, HIGH); 
     digitalWrite(i3, LOW); 
     digitalWrite(i4, LOW); 
     break;  
   case 5: 
     digitalWrite(i1, HIGH);  
     digitalWrite(i2, HIGH); 
     digitalWrite(i3, LOW); 
     digitalWrite(i4, LOW); 
     break;  
   case 6: 
     digitalWrite(i1, HIGH);  
     digitalWrite(i2, LOW); 
     digitalWrite(i3, LOW); 
     digitalWrite(i4, LOW); 
     break;  
   case 7: 
     digitalWrite(i1, HIGH);  
     digitalWrite(i2, LOW); 
     digitalWrite(i3, LOW); 
     digitalWrite(i4, HIGH); 
     break;
   }
}

