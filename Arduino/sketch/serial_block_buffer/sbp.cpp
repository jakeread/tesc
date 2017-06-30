#include "sbp.h"
#include "kernel.h"
#include "shell.h"

SBP::SBP(int baudRate) {
  _baudRate = baudRate;
  _status = SERIAL_STATUS_OFF;

}

void SBP::init() {
  Serial.begin(_baudRate);

  _inString.reserve(256);
  _inStringComplete = false;
  _inString = "";
  _status = SERIAL_STATUS_ON;
}

int SBP::status() {
  return _status;
}

void SBP::onMainLoop() {
  KERN->flashLed();
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

void SBP::newMessage() {
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

void SBP::newGCode() {
  Serial.println("ERR: gcodes not yet implemented");
  // do you build the block here and put the block into the planner?
}

void SBP::newShell() {
  KERN->shell->parseCommand(_inString);
}

void SBP::reportFormatError() {
  Serial.println("ERR: command or code not recognized");
}

