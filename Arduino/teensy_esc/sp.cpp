#include "sp.h"
#include "kernel.h"
#include "shell.h"

SP::SP() { // could just read-direct from config, here
  _status = SERIAL_STATUS_OFF;

}

void SP::init() {
  Serial.begin(THE_BAUDRATE);

  _inString.reserve(256);
  _inStringComplete = false;
  _inString = "";
  _status = SERIAL_STATUS_ON;
}

int SP::status() {
  return _status;
}

void SP::onMainLoop() {
  KERNEL->flashLed();
  while (Serial.available()) { // read all chars available from 64-byte arduino serial buffer
    _inChar = (char)Serial.read();
    if (_inChar == '\n') {
      _inStringComplete = true;
      this->newMessage();
    } else {
      _inString += _inChar;
    }
  }
}

void SP::newMessage() {
  // dish string to repsective parsers

  if (isUpperCase(_inString.charAt(0))) {
    // we have a to-be-buffered move
    this->newGCode();
  } else if (isLowerCase(_inString.charAt(0))) {
    this->newShell();
  } else {
    this->reportFormatError();
  }

  _inString = "";
  _inStringComplete = false;
}

void SP::newGCode() {
  Serial.println("ERR: gcodes not yet implemented");
  // do you build the block here and put the block into the planner?
}

void SP::newShell() {
  KERNEL->shell->parseCommand(_inString);
}

void SP::reportFormatError() {
  Serial.println("ERR: command or code not recognized");
}

