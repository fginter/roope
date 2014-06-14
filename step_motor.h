/* .h is a header file - it defines classes, functions, etc. Wherever you need
  to use these, you simply #include "thisfile.h". That copies the contents of this
  file verbatim to the place where you #include it. Now, the problem is what if
  it would end up getting included twice? So we use this little trick:
  
  #ifndef  XXXX   says "only read if macro XXXX is not defined
  #define  XXXX   says "define macro XXXX"
  
  So, the first time around, XXXX is not defined and the .h file gets included.
  If it is included again, XXXX is already defined and the contents of the .h is ignored

  There is no executable code in here. Just definitions.
  */
  
#ifndef _step_motor_h_
#define _step_motor_h_

class StepMotor { //kinda obvious what this does
  public: // I think java has these too. Makes the members accessible from outside of the class
    /*
    gearRatio: the ratio of the rotation of 8 steps to a full rotation of the shaft
    i1, i2, i3, i4: arduino pins (as integers) to which the four coils of the motor are connected
    */
    int gearRatio, i1, i2, i3, i4; //defines some integer attributes
    int energizedIndicatorPin; //pin to which HIGH will be written if any coil is energized, and LOW otherwise. Set to -1 if you don't have such a pin.
    unsigned long stepDelay; //Delay in microseconds between steps, for the given speed (as set by setSpeed()). Calculated in the constructor.
    int phase; //0-7: phase in the cycle of the motor
    StepMotor(int gearRatio, int i1, int i2, int i3, int i4, int energizedIndicatorPin); //Constructor. Its name is the name of the class.
    void setSpeed(int speed); // sets the speed in RPM with which the motor will rotate
    void turn(long steps); //Turn given number of steps. Positive forward. Negative backward. 
    void turnDegrees(long degrees); //Number of degrees to turn
    void deenergize(); //all coils off. Don't leave the coils on if the motor is not moving.
    void energize(); //energize coils according to the setting of phase
    void singleStep(bool fwd); //one step (adjusts phase and energizes)
}; //remember ; after the definition

//end of the #ifndef above
#endif 
