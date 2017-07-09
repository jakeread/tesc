#include "sampler.h"
#include "kernel.h"
#include "ringbuffer.h"

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

Sampler::Sampler(int pinva, int pinvb, int pinvc, int pinaa, int pinab, int pinac){
  
  this->rbva = new RingBuffer();
  this->rbvb = new RingBuffer();
  this->rbvc = new RingBuffer();
  this->rbaa = new RingBuffer();
  this->rbab = new RingBuffer();
  this->rbac = new RingBuffer();
  
  _pinva = pinva;
  _pinvb = pinvb;
  _pinvc = pinvc;
  _pinaa = pinaa;
  _pinab = pinab;
  _pinac = pinac;
  
}

void Sampler::init(){
  //analogReadRes(16); // set the whole adc to this
}

void Sampler::readANow(){
  rbaa->push(analogRead(_pinaa));
  rbab->push(analogRead(_pinab));
  rbac->push(analogRead(_pinac));
}

void Sampler::readVNow(){
  rbva->push(analogRead(_pinva)); // TODO: is this-> implied? obv is?
  rbvb->push(analogRead(_pinvb));
  rbvc->push(analogRead(_pinvc));
}


