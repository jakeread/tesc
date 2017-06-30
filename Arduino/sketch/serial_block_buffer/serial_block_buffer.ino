#include "kernel.h"
#include "sbp.h"
#include "shell.h"
#include "streamer.h"

void setup() {

  Kernel* kernel = new Kernel();
  // put your setup code here, to run once:
  KERN->init();
}

void loop() {
  // put your main code here, to run repeatedly:
  KERN->onMainLoop();
}

// do TIMER that pulls command blocks out at 1000hz?
