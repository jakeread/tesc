/*
 * Serial Block & Parse
 * gcode-like command / reply formatting layer
 * 
 * leaning heavy on the 'internal' Arduino Serial layer, which buffers 64 characters ! automatically from hardware
 * .available() returns the # of bytes available to read from this buffer
 * 
 * & good to note that serialEvent is actually not an ISR or anything.. it is just called at the end of each loop
 */

#ifndef SP_H
#define SP_H

#include <Arduino.h>

#define SERIAL_STATUS_OFF   0
#define SERIAL_STATUS_ON    1

class SP{

  public:
  SP(int baudRate);

  void init();
  int status();

  void onMainLoop();
  void newMessage();
  void newGCode();
  void newShell();
  void reportFormatError();

  private:
  int _baudRate;
  int _status;

  char _inChar;
  String _inString;
  bool _inStringComplete;
  
};

#endif
