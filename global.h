#define debug
#define TWO_W_C 2

//#define S_LOW 55 // lowest possible servo angle
//#define S_HIGH 75 // highest possible servo angle
#define S_LOW 63 // lowest possible servo angle
#define S_HIGH 68 // highest possible servo angle
#define WHITE 100  

typedef struct {
  char command;
  unsigned int steps;
  unsigned int angle;
  byte pen;
  byte backwards;
  char end;
} comm_t;
