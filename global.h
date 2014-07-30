#define debug

typedef struct {
  char command;
  unsigned int steps;
  unsigned int angle;
  byte pen;
  byte backwards;
  char end;
} comm_t;
