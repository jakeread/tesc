#include "shell.h"
#include "kernel.h"

#include "streamer.h"

Shell::Shell() {
  // ?
}

const Shell::command_entry Shell::command_table[] = {
  {"test",         Shell::cmd_test},
  {"streamer",       Shell::cmd_streamer},         // for starting, stopping, setting stream vars

  {NULL, NULL}
};


void Shell::parseCommand(String message) {

  _found = false;
  /*
     get 1st 'word' (separated by space)
     check if is command in command table,
     run that pfunc with the remainder of the String as 'args' var
  */

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

  int eval = 0;
  while (command_table[eval].cmd != NULL) {
    if ((String)command_table[eval].cmd == _command) {
      command_table[eval].func(_args);
      _found = true;
    }
    eval++;
  }

  if(!_found){
    Serial.println("Shell: command not found");
  }

}

void Shell::cmd_test(String args) {
  Serial.print("CMD_TEST: ");
  Serial.println(args);
}

/*
   stream->hz 1200
   stream->start
   stream->stop
*/

void Shell::cmd_streamer(String args) {
  Serial.print("CMD_STREAM: ");
  Serial.println(args);

#define ARGWORDS 4

  // a very simple split, into up-to 8 words, to fire in 'cases' below
  // should be a f'n -> figure how to pass array ptr, string ptr to fn. *facepalm*
  uint8_t p0 = 0;
  uint8_t p1 = 0;
  uint8_t place = 0;
  String argwords[ARGWORDS];
  while (place < args.length() && p0 < ARGWORDS) {
    if (args.charAt(place) == ' ') {
      argwords[p0] = args.substring(p1, place);
      p1 = place + 1;
      p0++;
    }
    place++;
    if (place == args.length()) {
      argwords[p0] = args.substring(p1, place);
    }
  }

  if (argwords[0] == "start") {
    Serial.println("Starting stream...");
    KERN->streamer->start();
  } else if (argwords[0] == "stop") {
    KERN->streamer->stop();
    Serial.println("Stream stopped");
  } else if (argwords[0] == "hz") {
    int hzUpdate = argwords[1].toInt();
    if(hzUpdate){ // i.e. if not 0
      Serial.print("Setting Stream HZ to ");
      Serial.println(hzUpdate);
      KERN->streamer->setHz(hzUpdate);
    } else {
      Serial.print("Stream HZ is: ");
      Serial.println(KERN->streamer->getHz());
    }
  } else {
    Serial.println("ERR: args in stream not valid");
  }

#define DEBUG_PARSEARGS 0
#if DEBUG_PARSEARGS
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("arg: ");
    Serial.print(i);
    Serial.print("|");
    Serial.print(argwords[i]);
    Serial.println("|");
  }
#endif
}



/*
   A good reference from smoothie... for the future when we are coordinating physics and need to pause to do this stuff...
  if(!THECONVEYOR->is_idle()) {
    stream->printf("upload not allowed while printing or busy\n");
    return;
  }
*/

