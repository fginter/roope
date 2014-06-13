/* .h is a header file - it defines classes, functions, etc. Wherever you need
  to use these, you simply #include "thisfile.h". That copies the contents of this
  file verbatim to the place where you #include it. Now, the problem is what if
  it would end up getting included twice? So we use this little trick:
  
  #ifndef  XXXX   says "only read if macro XXXX is not defined
  #define  XXXX   says "define macro XXXX"
  
  So, the first time around, XXXX is not defined and the .h file gets included.
  If it is included again, XXXX is already defined and the contents of the .h is ignored
  */
  
#ifndef _step_motor_h_
#define _step_motor_h_

class StepMotor { //kinda obvious what this does
  public: // I think java has these too. Makes the members accessible from outside of the class
    int gearRatio, i1, i2, i3, i4; //defines some integer attributes
    unsigned long stepDelay; //Delay in microseconds between steps, for the given speed (as set by setSpeed())
    int phase; //0-7: phase in the cycle of the motor
    StepMotor(int gearRatio, int i1, int i2, int i3, int i4);
    void setSpeed(int speed);
    void turn(long step);
    void turnDegrees(long degrees);
    void deenergize(); //all coils off
    void energize(); //energize coils according to phase
    void singleStep(bool fwd);
};

//end of the #ifndef above
#endif 
