#include <Arduino.h>
#include "connection.h"
#include "global.h"

// Constructor
Connection::Connection() {
  // empty
}

void Connection::setup_connection() {
  // TODO
}

int Connection::fetch_command(char *message, int maxlen) {
  //Serial.println("fetching new command");
  int len=0;
  if (Serial.available()>0) {
    len=Serial.readBytesUntil(';',message,maxlen);
    if (len>0) {
      message[len]='\0';
      #ifndef debug
        Serial.println(message);
      #endif
      Serial.write("k");
      Serial.flush();
      return 0;
    }
  }
  
  return 1;  
}
