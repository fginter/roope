#include <math.h>
#include <EEPROM.h>
#include "mpu6050.h"

#define MPU6050_silent

/* 
   This code is extracted and only minimally modified by @fginter from
   the code by arduino.cc user Krodal published here:

   http://playground.arduino.cc/Main/MPU-6050#sketch
   
   The main modification was splitting the code into a separate .h and .cpp files
*/

double x_ref,y_ref; // normalized reference x and y values of the accelerometer


// calculate the unit vector (x,y) of the accelerometer and store it to (x_ref,y_ref)
int MPU6050_set_angle_reference() {
  int err;
  accel_t_gyro_union accel_t_gyro_reference;
  delay(200); //let everything stabilize a bit
  err=MPU6050_read_raw_data(&accel_t_gyro_reference); //TODO: should we average this properly, over few seconds of time?
  double mag=sqrt(pow((double)accel_t_gyro_reference.value.x_accel,2.0)+pow((double)accel_t_gyro_reference.value.y_accel,2.0));
  x_ref=(double)accel_t_gyro_reference.value.x_accel/mag;
  y_ref=(double)accel_t_gyro_reference.value.y_accel/mag;
  

  //Write the angle reference to EEPROM, this needs to be done one byte at a time
  int addr=0; //address into EEPROM
  /* 
      in the declaration below:

      &x_ref is pointer to x_ref
      (void *) makes it untyped pointer which we are free to cast any which way we like
      (byte *) makes this a pointer to byte values, which behaves as
      an array of bytes. This lets us access the bytes of the memory
      behind the float values and read/write them any which way we
      want

      We need the (void *) mid-step to tell the compiler we really
      know what we're doing, because normally you have no business
      casting a float pointer to a byte pointer.  So the compiler
      would refuse to do this. But the (void *) cast wipes any type
      information from the pointer, and it just becomes a generic
      memory as far as we are concerned.

      --thanks to arduino.cc user haley for this trick
  */

  byte *b=(byte *)(void *) &x_ref;
  for (int counter=0 ; counter < sizeof(x_ref) ; counter++) { //write as many bytes as there are in x_ref
    EEPROM.write(addr++, *b++); //write the current byte to address, and increment both the address and the pointer to which byte
  }
  b=(byte *)(void *) &y_ref;
  for (int counter=0 ; counter < sizeof(y_ref) ; counter++) { //repeats the exercise for y_ref
    EEPROM.write(addr++, *b++);
  }
  return err;
}

int MPU6050_read_angle_reference_from_eeprom() {
  int addr=0; //address into EEPROM
  byte *b=(byte *)(void *) &x_ref; 
  for (int counter=0 ; counter < sizeof(x_ref) ; counter++) { //read as many bytes as there are in x_ref
    *b++=EEPROM.read(addr++); //read the current byte from address into the relevant byte, and increment both the address and the pointer to which byte
  }
  b=(byte *)(void *) &y_ref;
  for (int counter=0 ; counter < sizeof(y_ref) ; counter++) {
    *b++=EEPROM.read(addr++);
  }
}


int MPU6050_init() {
    int error;
    uint8_t c; //one byte to be read/written
    Wire.begin(); //initializes the I2C bus to talk to MPU6050
    //This block of code comes from http://www.varesano.net/blog/fabio/how-disable-internal-arduino-atmega-pullups-sda-and-scl-i2c-bus
    //its purpose is to disable the AVR internal pull-up registers on the I2C bus, so we can run the MPU
    //from the 5V arduino without too much risk
    #ifndef cbi
      #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
    #endif
    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
      // deactivate internal pull-ups for twi
      // as per note from atmega8 manual pg167
      cbi(PORTC, 4);
      cbi(PORTC, 5);
    #else
      // deactivate internal pull-ups for twi
      // as per note from atmega128 manual pg204
      cbi(PORTD, 0);
      cbi(PORTD, 1);
    #endif
    //...and here continues code I borrowed from the MPU-6050 example
    error = MPU6050_read (MPU6050_WHO_AM_I, &c, 1); //check that MPU-6050 is alive and talks to us
    #ifndef MPU6050_silent 
    Serial.print(F("MPU6050 WHO_AM_I : ")); //The F macro puts the string into the Flash memory
    Serial.print(c,HEX);
    Serial.print(F(", error = "));
    Serial.println(error,DEC);
    #endif
    if (error != 0) {
      return error;
    }
    error=MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0); //clear the sleep bit, which starts the measurement
    if (error != 0) {
      return error;
    }
    #ifndef MPU6050_silent
    Serial.print(F("MPU6050 Wake up, error = "));
    Serial.println(error,DEC);
    #endif
    // @fginter config
    // based on this document: http://invensense.com/mems/gyro/documents/RM-MPU-6000A.pdf
    error=MPU6050_write_reg (MPU6050_CONFIG, 0x6); //aggressive filtering on -- DLPF_CFG table on page 13
    if (error != 0) {
      return error;
    }
    #ifndef MPU6050_silent
    Serial.print(F("MPU6050 FILTER SETUP, error = "));
    Serial.println(error,DEC);
    #endif

    return 0;    
}

int MPU6050_read_raw_data(accel_t_gyro_union *accel_t_gyro) {
  // Read the raw values into accel_t_gyro
  // Read 14 bytes at once, 
  // containing acceleration, temperature and gyro.
  // With the default settings of the MPU-6050,
  // there is no filter enabled, and the values
  // are not very stable.
  int err = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) accel_t_gyro, 6);//sizeof(*accel_t_gyro));
  uint8_t swap;
  #define SWAP(x,y) swap = x; x = y; y = swap

  SWAP (accel_t_gyro->reg.x_accel_h, accel_t_gyro->reg.x_accel_l);
  SWAP (accel_t_gyro->reg.y_accel_h, accel_t_gyro->reg.y_accel_l);
  SWAP (accel_t_gyro->reg.z_accel_h, accel_t_gyro->reg.z_accel_l);
  /*SWAP (accel_t_gyro->reg.t_h, accel_t_gyro->reg.t_l);
  SWAP (accel_t_gyro->reg.x_gyro_h, accel_t_gyro->reg.x_gyro_l);
  SWAP (accel_t_gyro->reg.y_gyro_h, accel_t_gyro->reg.y_gyro_l);
  SWAP (accel_t_gyro->reg.z_gyro_h, accel_t_gyro->reg.z_gyro_l);*/
  return err;
}

double MPU6050_get_angle() {
  int err;
  double x,y,mag,fi,dir;
  accel_t_gyro_union accel_t_gyro;
  err=MPU6050_read_raw_data(&accel_t_gyro);
  mag=sqrt(((double)accel_t_gyro.value.x_accel)*accel_t_gyro.value.x_accel+((double)accel_t_gyro.value.y_accel)*accel_t_gyro.value.y_accel);
//  Serial.println(accel_t_gyro.value.x_accel*accel_t_gyro.value.x_accel+accel_t_gyro.value.y_accel*accel_t_gyro.value.y_accel);
/*  Serial.println(mag);
  Serial.println(accel_t_gyro.value.x_accel);
  Serial.println(accel_t_gyro.value.y_accel);
  Serial.println();*/
  x=(double)accel_t_gyro.value.x_accel/mag;
  y=(double)accel_t_gyro.value.y_accel/mag;
  fi=acos(x*x_ref+y*y_ref)*180.0/3.14159265; //dot product is cosine of the angle, acos gives radians, *180/PI gives degrees
  //this comes from here:
  //http://www.cs.cmu.edu/~quake/robust.html
  //we're only interested in the sign of dir, to distinguish whether we're deviating left or right
  //I bet there's a better way to do this...
  dir=(y*(x_ref-x))-x*(y_ref-y);
  if (dir<0) {
      return 360-fi;
  }
  else {
      return fi;
  }
}


// --------------------------------------------------------
// MPU6050_read
//
// This is a common function to read multiple bytes 
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus. 
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read. 
// There is no function for a single byte.
//

int MPU6050_read(int start, uint8_t *buffer, int size)
{
  int i, n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);
  if (n != 1)
    return (-10);

  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
    return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
    return (-11);

  return (0);  // return : no error
}


// --------------------------------------------------------
// MPU6050_write
//
// This is a common function to write multiple bytes to an I2C device.
//
// If only a single register is written,
// use the function MPU_6050_write_reg().
//
// Parameters:
//   start : Start address, use a define for the register
//   pData : A pointer to the data to write.
//   size  : The number of bytes to write.
//
// If only a single register is written, a pointer
// to the data has to be used, and the size is
// a single byte:
//   int data = 0;        // the data to write
//   MPU6050_write (MPU6050_PWR_MGMT_1, &c, 1);
//
int MPU6050_write(int start, const uint8_t *pData, int size)
{
  int n, error;
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);        // write the start address
  if (n != 1)
    return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
    return (-21);

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);

  return (0);         // return : no error
}

// --------------------------------------------------------
// MPU6050_write_reg
//
// An extra function to write a single register.
// It is just a wrapper around the MPU_6050_write()
// function, and it is only a convenient function
// to make it easier to write a single register.
//
int MPU6050_write_reg(int reg, uint8_t data)
{
  int error;

  error = MPU6050_write(reg, &data, 1);

  return (error);
}


