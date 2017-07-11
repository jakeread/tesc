#include "shell.h"
#include "kernel.h"

#include "streamer.h"
#include "bldc.h"
#include "encoder_as5047.h"

Shell::Shell() {
  // ?
}

const Shell::command_entry Shell::command_table[] = {
  {"test",         Shell::cmd_test},
  {"streamer",     Shell::cmd_streamer},         // for starting, stopping, setting stream vars
  {"timers",       Shell::cmd_timers},
  {"tr",           Shell::cmd_tr},
  {"encoder",      Shell::cmd_encoder},

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
      Serial.print("> ");
      Serial.println(message);
      command_table[place].func(_argwords);
      _found = true;
      break;
    }
    place++;
  }
  if (!_found) {
    shellError();
  }
}

// --------------------------------------------------------------------------- CMD_TEST
void Shell::cmd_test(String args[ARGWORDS]) {
  Serial.println("Shell Test...");
}

// --------------------------------------------------------------------------- CMD_STREAMER
void Shell::cmd_streamer(String args[ARGWORDS]) {
  Serial.println("Shell Streamer: ");
  if (args[0] == "start") {                       // START
    if (KERNEL->streamer->start()) {
      Serial.println("Starting stream...");
    } else {
      Serial.println("Stream already started.");
    }
  } else if (args[0] == "stop") {                 // STOP
    if (KERNEL->streamer->stop()) {
      Serial.println("Stopping stream...");
    } else {
      Serial.println("Stream already stopped.");
    }
  } else if (args[0] == "hz") {                   // HZ
    int hzUpdate = args[1].toInt();
    if (hzUpdate) { // i.e. if not 0
      if (KERNEL->streamer->setHz(hzUpdate)) {
        Serial.print("Streamer HZ set to: ");
        Serial.println(hzUpdate);
      } else {
        Serial.print("Stream HZ is: ");
        Serial.println(KERNEL->streamer->getHz());
      }
    }
  } else if (args[0] == "setvars") {                  // SETVARS
    if (args[1] == "test") {
      KERNEL->streamer->setVarSet(VARSET_ID_TEST);
      Serial.println("Streaming test variables...");
    } else if (args[1] == "voltages") {
      KERNEL->streamer->setVarSet(VARSET_ID_VOLTAGES);
      Serial.println("Streaming voltages...");
    } else if (args[1] == "currents") {
      KERNEL->streamer->setVarSet(VARSET_ID_CURRENTS);
      Serial.println("Streaming currents...");
    } else {
      Serial.print("Streamer current varset is: ");
      Serial.println(KERNEL->streamer->getVarSet());
    }
  } else {                                            // DEFAULT
    Serial.print("HZ is: ");
    Serial.print(KERNEL->streamer->getHz());
    Serial.print(" varset is: ");
    Serial.println(KERNEL->streamer->getVarSet());
  }
}

// --------------------------------------------------------------------------- CMD_TIMERS

void Shell::cmd_timers(String args[ARGWORDS]) {
  if (args[0] == "machine") {                         // MACHINE
    if (args[1] == "hz") {                            // MACHINE && HZ
      int hz = args[2].toInt();
      if (hz) {
        if (KERNEL->_Machine_T_isRunning) {
          KERNEL->_Machine_Timer.end();
          Serial.print("Restarting Machine Timer at HZ: ");
          Serial.println(hz);
          KERNEL->_Machine_T_hz = hz;
          KERNEL->_Machine_Timer.begin(Kernel::onMachineLoop, 1000000 / hz);
        } else {
          Serial.print("Starting Machine Timer at HZ: ");
          Serial.println(hz);
          KERNEL->_Machine_T_hz = hz;
          KERNEL->_Machine_Timer.begin(Kernel::onMachineLoop, 1000000 / hz);
        }
      }
    } else if (args[1] == "stop") {                 // MACHINE && STOP
      if (KERNEL->_Machine_T_isRunning) {
        KERNEL->_Machine_Timer.end();
        KERNEL->bldc->killAllPower();
        KERNEL->_Machine_T_isRunning = false;
#if IS_FOC_MACHINE
#warning NOT IMPLEMENTED
#endif
        Serial.println("RIP Machine Timer");
      } else {
        Serial.println("Machine Timer already stopped...");
      }
    } else if (args[1] == "start") {                // MACHINE && START
      if (KERNEL->_Machine_T_isRunning) {
        Serial.println("Machine Timer already started...");
      } else {
        KERNEL->_Machine_Timer.begin(Kernel::onMachineLoop, 1000000 / KERNEL->_Machine_T_hz); // TODO: this should be KERNEL->startMachineTimer();
        KERNEL->_Machine_T_isRunning = true;
        Serial.print("Starting Machine Timer at HZ: ");
        Serial.println(KERNEL->_Machine_T_hz);
      }
    } else {
      Serial.print("Machine Timer HZ is: ");
      Serial.print(KERNEL->_Machine_T_hz);
      Serial.print(" and is running: ");
      Serial.println(KERNEL->_Machine_T_isRunning);
    }
  } else if (args[0] == "sample") {                  // SAMPLE
    if (args[1] == "hz") {                           // SAMPLE && HZ
      int hz = args[2].toInt();
      if (hz) {
        if (KERNEL->_Sample_T_isRunning) {
          KERNEL->_Sample_Timer.end();
          Serial.print("Restarting Sample Timer at HZ: ");
          Serial.println(hz);
          KERNEL->_Sample_T_hz = hz;
          KERNEL->_Sample_Timer.begin(Kernel::onSampleLoop, 1000000 / hz);
        } else {
          Serial.print("Starting Sample Timer at HZ: ");
          Serial.println(hz);
          KERNEL->_Sample_T_hz = hz;
          KERNEL->_Sample_Timer.begin(Kernel::onSampleLoop, 1000000 / hz);
        }
      }
    } else if (args[1] == "stop") {                 // SAMPLE && STOP
      if (KERNEL->_Sample_T_isRunning) {
        KERNEL->_Sample_Timer.end();
        KERNEL->_Sample_T_isRunning = false;
        Serial.println("RIP Sample Timer");
      } else {
        Serial.println("Sample Timer already stopped...");
      }
    } else if (args[1] == "start") {                // SAMPLE && START
      if (KERNEL->_Sample_T_isRunning) {
        Serial.println("Sample Timer already started...");
      } else {
        KERNEL->_Sample_Timer.begin(Kernel::onSampleLoop, 1000000 / KERNEL->_Sample_T_hz); // TODO: this should be KERNEL->startMachineTimer();
        KERNEL->_Sample_T_isRunning = true;
        Serial.print("Starting Sample Timer at HZ: ");
        Serial.println(KERNEL->_Sample_T_hz);
      }
    } else {
      Serial.print("Sample Timer HZ is: ");
      Serial.print(KERNEL->_Sample_T_hz);
      Serial.print(" and is running: ");
      Serial.println(KERNEL->_Sample_T_isRunning);
    }
  } else {
    Serial.print("Machine Timer Status: ");
    Serial.print(KERNEL->_Machine_T_isRunning);
    Serial.print(", HZ: ");
    Serial.println(KERNEL->_Machine_T_hz);
    Serial.print("Sample Timer Status: ");
    Serial.print(KERNEL->_Sample_T_isRunning);
    Serial.print(", HZ: ");
    Serial.println(KERNEL->_Sample_T_hz);
  }
}

// --------------------------------------------------------------------------- CMD_TR

void Shell::cmd_tr(String args[ARGWORDS]) {
  if (isAlpha(args[0].charAt(0))) {                              // NON-NUMERIC CMDS
    if (args[0] == "mode") {                                      // MODE
      if (args[1] == "pot") {                                     // MODE->POT
        if (KERNEL->bldc->setInputMode(BLDC_INPUTMODE_POT)) {
          KERNEL->bldc->duty(0);
          Serial.println("Set BLDC Input Mode to POT");
        } else {
          Serial.println("BLDC setInputMode Error");
        }
      } else if (args[1] == "shell") {                          // MODE->SHELL
        if (KERNEL->bldc->setInputMode(BLDC_INPUTMODE_SHELL)) {
          KERNEL->bldc->duty(0);
          Serial.println("Set BLDC Input Mode to Shell");
        } else {
          Serial.println("BLDC setInputMode Error");
        }
      } else {
        Serial.print("BLDC Mode is: ");
        Serial.println(KERNEL->bldc->getInputMode());
      }
    }
  } else {                                                      // NUMERIC
    int duty = args[0].toInt();
    int dir = args[1].toInt();
    if (duty == 0 && dir == 0) {                                // DEFAULT-REPORT
      // if neither of words exist in the command, report values
      Serial.print("TR: Duty: ");
      Serial.print(KERNEL->bldc->getDuty());
      Serial.print(" Dir: ");
      Serial.print(KERNEL->bldc->getDir());
      Serial.print(" Input: ");
      Serial.println(KERNEL->bldc->getInputMode());
    } else {
      if (KERNEL->bldc->duty(duty)) {
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

// --------------------------------------------------------------------------- CMD_ENCODER

void Shell::cmd_encoder(String args[ARGWORDS]) {
  if (args[0] == "offset") {                                  // OFFSET
    if (args[1] == "set") {                                   // OFFSET -> SET
      if (args[2].toInt()) {
        if (KERNEL->as5047->setEncoderOffset(args[2].toInt())) {
          Serial.print("Updated Encoder Offset to: ");
          Serial.println(KERNEL->as5047->getEncoderOffset());
        } else {
          Serial.println("Error on setting encoder offset");
        }
      } else {
        Serial.println("Error with encoder set value provided...");
      }
    } else if (args[1] == "find") {                           // OFFSET -> FIND
      Serial.println("ENCODER OFFSET SEARCH ROUTINE");
    } else {
      Serial.print("Encoder Offset is: ");
      Serial.println(KERNEL->as5047->getEncoderOffset());
    }
  } else if (args[0] == "reverse") {                          // REVERSE
    if (args[1] == "true") {          // REVERSE -> TRUE
      if (KERNEL->as5047->setEncoderReverse(true)) {
        Serial.println("Encoder Reverse set to True");
      } else {
        Serial.println("Error on setting encoder reverse");
      }
    } else if (args[1] == "false") {
      if (KERNEL->as5047->setEncoderReverse(false)) {         // REVERSE -> FALSE
        Serial.println("Encoder Reverse set to False");
      } else {
        Serial.println("Error on setting encoder reverse");
      }
    } else if (args[1] == "find") {                           // REVERSE -> FIND
      Serial.println("testing for encoder reverse...");
      bool result = KERNEL->findEncoderReverse(ENCODER_SEARCH_DEFAULT_DUTY);
      Serial.print("Result: ");
      Serial.println(result);
    } else {
      Serial.print("Encoder Reverse is: ");                   // REVERSE -> DEFAULT REPORT
      Serial.println(KERNEL->as5047->getEncoderReverse());
    }
  } else if (args[0] == "search") {
    Serial.println("testing for encoder settings...");
    bool result = KERNEL->findEncoderReverse(ENCODER_SEARCH_DEFAULT_DUTY);
    Serial.print("Result: ");
    Serial.println(result);
  } else if (args[0] == "print" && args[1] == "comzones") {
    KERNEL->bldc->prntCzc();
  } else {
    Serial.print("Encoder, Offset: ");
    Serial.print(KERNEL->as5047->getEncoderOffset());
    Serial.print(" Reverse: ");
    Serial.println(KERNEL->as5047->getEncoderReverse());
  }
}

void Shell::shellError() {
  Serial.println("unrecognized argument or command");
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

