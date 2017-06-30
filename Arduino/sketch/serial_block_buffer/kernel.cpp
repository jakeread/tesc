#include "kernel.h"

#include "sbp.h"
#include "shell.h"
#include "streamer.h"

Kernel* Kernel::instance;

Kernel::Kernel(){
  
  instance = this;
  
  this->sbp = new SBP(115200);
  this->shell = new Shell();
  this->streamer = new Streamer();
}

void Kernel::init(){
  pinMode(13, OUTPUT);
  this->sbp->init();
}

void Kernel::onMainLoop(){
  this->sbp->onMainLoop();
}

void Kernel::flashLed(){
  digitalWrite(13, !digitalRead(13));
}

