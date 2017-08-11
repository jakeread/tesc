#include "kernel.h"
#include "config.h"

/*
 * timers, should perhaps belong to kernel, as we will want to be able to command-line change their hz rates?
 */

void setup() {
  Kernel* kernel = new Kernel();
  KERNEL->init();
}

void loop() { 
/*
 * most importantly, checks the Serial Buffer. 
 * this will run as often as possible ..
 * whenever other timers' functions and follow-throughs are finished
 */ 
  KERNEL->onMainLoop(); 
  // TODO: on teensy, large amounts of data become available very fast -> to re-write serial such that other interrupts don't get blocked when, say, 256 bytes show up at once!
}


