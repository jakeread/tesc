#include "vsens.h"
#include "kernel.h"
#include "rb.h"

/*
 * try this out, but major TODO is speed up analogRead speed
 * https://forum.pjrc.com/threads/407-Teensy-3-0-ADC-details
 * https://forum.pjrc.com/threads/38-Help-with-ADC-speed
 * https://bitbucket.org/cmason/teensy3/src/77cd4753e17b2a3b79be06398ec68e300fb046f5/main_midi.cpp?at=default&fileviewer=file-view-default
 * http://arduino-pi.blogspot.ca/2015/04/you-have-to-see-this-rich-adc-library.html
 * 
 * katz' sample rate is 10kHz, arduino on teensy will only push to 2.2kHz, so you need to f with teensy registers directly 
 * I imagine this is true for PWM timers as well, if you want to do fancy in-phase sampling !
 * setup kernel to run this ... only this... on main loop, and report speeds ?
 */

VSens::VSens(){

  this->rbau = new RingBuffer();
  this->rbav = new RingBuffer();
  
  this->rbvu = new RingBuffer();
  this->rbvv = new RingBuffer();
  this->rbvw = new RingBuffer();
  
}

void VSens::init(){
}


void VSens::readVNow(){
  rbvu->push(analogRead(PIN_VSENS_U)); // TODO: is this-> implied? obv is?
  rbvv->push(analogRead(PIN_VSENS_V));
  rbvw->push(analogRead(PIN_VSENS_W));
}



