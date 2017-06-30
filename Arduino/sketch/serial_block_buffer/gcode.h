/*
 * GCode block
 * 
 * all gcode-like lines turn into gcode blocks
 * blocks are 'dispatched' by the sbp object,
 * planner-specific blocks are passed to the robot for physics coordination
 * other gcodes, like on / offs, are handled more or less 'immediately'
 * 
 * or do you gcode -> blocks, wherein blocks are not necessarily gcodes
 * but can be issued by any planning / control system?
 * 
 */

#ifndef GCODE_H
#define GCODE_H

#include <Arduino.h>

class GCode{

  public:
  GCode(String message);
  ~GCode(); // destructor, seems important here

  void isExecuted();

  private:
  // struct gword;
  // gword words[];

};

#endif
