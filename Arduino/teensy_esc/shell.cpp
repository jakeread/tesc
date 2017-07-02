#include "shell.h"
#include "kernel.h"

#include "streamer.h"
#include "bldc.h"

Shell::Shell() {
  // ?
}

const Shell::command_entry Shell::command_table[] = {
  {"test",         Shell::cmd_test},
  {"streamer",     Shell::cmd_streamer},         // for starting, stopping, setting stream vars
  {"timers",       Shell::cmd_timers},
  {"tq",           Shell::cmd_tq},

  {NULL, NULL}
};


void Shell::parseCommand(String message) {

  _found = false;
  for (int i = 0; i < ARGWORDS; i ++) { // TODO
    _argwords[i] = "";
  }

  // ----------------- Split message into command, args
  uint8_t place = 0;
  while (place < message.length()) {
    if (message.charAt(place) == ' ') {
      _command = message.substring(0, place); // 1st piece (subtracts the separating space out)
      _args = message.substring(place + 1);       // 2nd piece -> further parsing of args done on a per-command basis?
      break;
    }
    place ++;
    if (place == message.length()) {
      _command = message;
      _args = "";
      break;
    }
  }

#define DEBUG_PARSECOMMAND 0
#if DEBUG_PARSECOMMAND
  Serial.print("Shell->parseCommand, command:|");
  Serial.print(_command);
  Serial.print("|args:|");
  Serial.print(_args);
  Serial.println("|");
#endif

  // ----------------- Split args into words
  uint8_t p0 = 0;
  uint8_t p1 = 0;
  place = 0;
  while (place < _args.length() && p0 < ARGWORDS) {
    if (_args.charAt(place) == ' ') {
      _argwords[p0] = _args.substring(p1, place);
      p1 = place + 1;
      p0++;
    }
    place++;
    if (place == _args.length()) {
      _argwords[p0] = _args.substring(p1, place);
    }
  }

#define DEBUG_PARSEARGS 0
#if DEBUG_PARSEARGS
  for (uint8_t i = 0; i < ARGWORDS; i++) {
    Serial.print("arg: ");
    Serial.print(i);
    Serial.print("|");
    Serial.print(_argwords[i]);
    Serial.println("|");
  }
#endif

  // ----------------- roll through command table and fire
  place = 0;
  while (command_table[place].cmd != NULL) {
    if ((String)command_table[place].cmd == _command) {
      command_table[place].func(_argwords);
      _found = true;
    }
    place++;
  }
  if (!_found) {
    Serial.println("Shell: command not found");
  }
}

void Shell::cmd_test(String args[ARGWORDS]) {
  Serial.println("CMD_TEST: ");
  //Serial.println(args);
}

/*
   stream->hz 1200
   stream->start
   stream->stop
*/

void Shell::cmd_streamer(String args[ARGWORDS]) {
  Serial.print("CMD_STREAM: ");
  if (args[0] == "start") {
    Serial.println("Starting stream...");
    KERNEL->streamer->start();
  } else if (args[0] == "stop") {
    KERNEL->streamer->stop();
    Serial.println("Stream stopped");
  } else if (args[0] == "hz") {
    int hzUpdate = args[1].toInt();
    if (hzUpdate) { // i.e. if not 0
      Serial.print("Setting Stream HZ to ");
      Serial.println(hzUpdate);
      KERNEL->streamer->setHz(hzUpdate);
    } else {
      Serial.print("Stream HZ is: ");
      Serial.println(KERNEL->streamer->getHz());
    }
  } else {
    Serial.println("ERR: args in stream not valid");
  }
}

/*
   TODO: This implementation is pretty garb, missing a lot
*/

void Shell::cmd_timers(String args[ARGWORDS]) {
  Serial.print("CMD_TIMERS: ");
  if (args[0] == "machine") {
    int hz = args[1].toInt();
    if (hz) {
      if (KERNEL->_Machine_T_isRunning) {
        KERNEL->_Machine_Timer.end();
        Serial.print("Restarting MACHINE Timer at HZ: ");
        Serial.println(hz);
        KERNEL->_Machine_T_hz = hz;
        KERNEL->_Machine_Timer.begin(Kernel::onMachineLoop, 1000000 / hz);
      } else {
        Serial.println("MACHINE timer is not running... this command is incomplete...");
      }
      /*
         ALSO:
         report current hz
         do 'default'
      */
    }
  }
  if (args[0] == "sample") {
    int hz = args[1].toInt();
    if (hz) {
      if (KERNEL->_Sample_T_isRunning) {
        KERNEL->_Sample_Timer.end();
        Serial.print("Restarting SAMPLE Timer at HZ: ");
        Serial.println(hz);
        KERNEL->_Sample_T_hz = hz;
        KERNEL->_Sample_Timer.begin(Kernel::onSample, 1000000 / hz);
      } else {
        Serial.println("SAMPLE timer is not running ... this command is incomplete...");
      }
    }
  }
}

void Shell::cmd_tq(String args[ARGWORDS]) {

  if (isAlpha(args[0].charAt(0))) {
    if (args[0] == "mode") {
      if (args[1] == "pot") {
        KERNEL->bldc->setInputMode(BLDC_INPUTMODE_POT);
        Serial.println("Set BLDC Input Mode to POT");
      } else if (args[1] == "shell") {
        KERNEL->bldc->setInputMode(BLDC_INPUTMODE_SHELL);
        Serial.println("Set BLDC Input Mode to Shell");
      } else {
        Serial.println("invalid mode argument");
      }
    }
  } else {
    int duty = args[0].toInt();
    int dir = args[1].toInt();
    if (duty == 0 && dir == 0) {
      // if neither of words exist in the command
      Serial.print("TQ: Duty: ");
      Serial.print(KERNEL->bldc->getDuty());
      Serial.print(" Dir: ");
      Serial.print(KERNEL->bldc->getDir());
      Serial.print(" Input: ");
      Serial.println(KERNEL->bldc->getInputMode());
    } else {
      if(KERNEL->bldc->duty(duty)){
        KERNEL->bldc->dir(dir);
        Serial.print("Set BLDC Duty: ");
        Serial.print(duty);
        Serial.print(" Dir: ");
        Serial.println(dir);
      } else {
        Serial.println("ERR Setting Duty // out of range");
      }
    }
  }


  // OK, now an operating Q
  // in this command, we should be able to say 'tq pot' or 'tq analog' or something... to switch input mode as well
  // how do we implement that on BLDC side? if *is_updating* or something? it would also be nice to get rid of the awk. once-in-a-while check at the moment
  // which is also potentially causing the knock
}

/*
   more like

   KERNEL->updateTimer(argwords[0],argwords[1]);
   does all "is on / is off" checks
   in fact, could wrap the IntervalTimer class ... in a class ... :|
*/

/*
   A good reference from smoothie... for the future when we are coordinating physics and need to pause to do this stuff...
  if(!THECONVEYOR->is_idle()) {
    stream->printf("upload not allowed while printing or busy\n");
    return;
  }
*/

