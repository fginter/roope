#include <Arduino.h>
#include "step_motor.h"

/* Constructor.

  this is a pointer to the current object, i.e. its address in the memory
  to dereference the pointer and get the object you can do (*this)
  if you only need to access a method/attribute, you can use ->
  so 
    this->i1
  and
    (*this).i1
  are equivalent.
*/
StepMotor::StepMotor(int gearRatio, int i1, int i2, int i3, int i4) {
  this->gearRatio=gearRatio;
  this->i1=i1;
  this->i2=i2;
  this->i3=i3;
  this->i4=i4;
  pinMode(i1,OUTPUT); //Tells arduino that you will use the pin as output
  pinMode(i2,OUTPUT);
  pinMode(i3,OUTPUT);
  pinMode(i4,OUTPUT);
  //switch all coils off so we don't burn the motor
  deenergize(); //same as this->deenergize()
  setSpeed(60); //1Hz
};

void StepMotor::setSpeed(int speed) {
  //Speed is given in RPM
  //Steps per minute: 8*gearRatio*speed
  //Steps per second: 8*gearRatio*speed/60
  //Steps per microsecond: 8*gearRatio*speed/60000000
  //Delay in microseconds between steps: 60000000/(8*gearRatio*speed)
  //                                   : 7500000/(gearRatio*speed)
  unsigned long delay=7500000UL/((unsigned long)gearRatio*(unsigned long)speed);
  this->stepDelay=(unsigned int) delay;
};

//it takes 8 * gearRatio steps for a full revolution of the output shaft
void StepMotor::turn(long steps) {
  bool fwd=true;
  if (steps<0) {
      fwd=false;
      steps*=-1;
  }
  long start=0L;
  while (steps>0) {
    singleStep(fwd);
    steps--;
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
  deenergize();
};

void StepMotor::turnDegrees(long degs) {
  //How many steps?
  long steps=(gearRatio*degs)/45L; //8/360 -> 1/45
  turn(steps);
}

void StepMotor::deenergize() {
  digitalWrite(i1, LOW);  //i1 is same as this->i1
  digitalWrite(i2, LOW); 
  digitalWrite(i3, LOW); 
  digitalWrite(i4, LOW); 
};

void StepMotor::singleStep(bool fwd) {
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
  energize();
}

void StepMotor::energize() {
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
