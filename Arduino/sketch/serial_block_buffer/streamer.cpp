#include "streamer.h"
#include "kernel.h"

/*
 * ! these functions should all actually stay independent of shell, 
 * where shell is handling chat-with-user, streamer.cpp operates the stream output ONLY
 */

float Streamer::sinSpot = 0;
float Streamer::sinStep = PI/60;
double Streamer::dv1 = 0;
double Streamer::dv2 = 0;
double Streamer::dv3 = 0;

Streamer::Streamer(int hz){
  _hz = hz;
  _isRunning = false;
}

void Streamer::onStreamLoop(){

  dv1 = sin(sinSpot - 2/3 * PI);
  dv2 = sin(sinSpot);
  dv3 = sin(sinSpot + 2/3 * PI);
  sinSpot += sinStep; // seems like these are all not evaluating...
  
  if(sinSpot > TWO_PI){
    sinSpot = 0;
  }

  Serial.print("#");
  Serial.print(" dv1:");
  Serial.print(dv1);
  Serial.print(" dv2:");
  Serial.print(sin(sinSpot + 2/3 * PI));
  Serial.print(" dv3:");
  Serial.print(dv3);
  Serial.print(" sinSpot:");
  Serial.print(sinSpot);
  Serial.println("");
}

void Streamer::start(){
  _Stream_Timer.begin(Streamer::onStreamLoop, 1000000/_hz);
  _isRunning = true;
}

void Streamer::stop(){
  _Stream_Timer.end();
  _isRunning = false;
}

void Streamer::setHz(int hz){
  if(hz == 0){
    Serial.println("ERR: hz is zero");// KERN->printErr("errmessage");
  } else {
    _hz = hz;
    if(_isRunning){
      _Stream_Timer.end();
      Serial.print("STREAMER: Restarting at HZ: ");
      Serial.println(_hz);
      _Stream_Timer.begin(Streamer::onStreamLoop, 1000000/_hz);
    } else {
      Serial.print("STREAMER: Updating HZ to: ");
      Serial.println(_hz);
    }
  }
}

int Streamer::getHz(){
  return _hz;
}

void Streamer::pickVarSet(int varSetId){
  //
}



