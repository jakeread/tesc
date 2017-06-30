#include "gcode.h"

GCode::GCode(String message){

  Serial.print("GCode Object finds: ");
  Serial.println(message);
  
}

