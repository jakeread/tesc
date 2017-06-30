/*
 * holding for all 'shell' style commands
 */

   /*
   * grab 1st word (space-separated) from _inString
   * if is shell cmd, fire that command in shell 
   * with remainder of _inString as 'args'
   * 
   * shell commands, being not-movement or loop-closing related, run immediately
   * and are not buffered!
   * 
   * however, they can of course update / change vars in loops-that-are-running
   * 
   */

#ifndef SHELL_H
#define SHELL_H

#include <Arduino.h>

class Shell{

  public:

  Shell();
  void parseCommand(String message);
  // wants to use function pointers ... list of commands, functions

  private:

  bool _found;

  static void cmd_test(String args);
  static void cmd_streamer(String args);

  typedef void(*PFUNC)(String args);
  
  typedef struct {
    const char *cmd;    
    const PFUNC func; 
  } command_entry;                        // pointerfunction table entry
  
  static const command_entry command_table[];   // table of commands *borrowing from simpleshell, via smoothie

  // parsing temp vars

  String _command;
  String _args;  
};

#endif
