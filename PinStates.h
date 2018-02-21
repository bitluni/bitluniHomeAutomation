/*
Released under Creative Commons Attribution 4.0
by bitluni 2016
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it by sharing http://youtube.com/bitlunislab
*/

#include <ESP8266WebServer.h>

//If you encounter compile error in this file 
//you have to select the board WeMos D1 R2 Mini in the tools
//If you are using this code elswhere use the wemos D1 mini pin configuration
//from this comment
/*const int D0 = 16;
const int D1 = 5;
const int D2 = 4;
const int D3 = 0;
const int D4 = 2;
const int D5 = 14;
const int D6 = 12;
const int D7 = 13;
const int D8 = 15;*/

const int PIN_COUNT = 9;
const char *pinNames[PIN_COUNT] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8"};
const int pinNumbers[PIN_COUNT] = {D0, D1, D2, D3, D4, D5, D6, D7, D8};

class PinStates
{
  public:
  int values[PIN_COUNT] = {-1};
  
  PinStates()
  {
  }
  
  bool loadValues(ESP8266WebServer &server)
  {
    for(int i = 0; i < PIN_COUNT; i++)
      if(server.hasArg(pinNames[i]))
      {
        int value = server.arg(pinNames[i]).toInt();
        if(value < 0 || value > 1023)
        {
          String message = "Illegal value:\n\n";
          message += pinNames[i];
          message += "=";
          message += server.arg(pinNames[i]);
          message += "\nOnly values 0 to 1023 are allowed. 0 is off and 1023 is on.";
          server.send(200, "text/plain", message);
          return false;
        }
        values[i] = value;
      }
    return true;
  }

  void commit()
  {
    for(int i = 0; i < PIN_COUNT; i++)
      if(values[i] > -1)
        analogWrite(pinNumbers[i], values[i]);
  }

  void setAllTo(int value)
  {
    for(int i = 0; i < PIN_COUNT; i++)
      if(values[i] > -1)
        values[i] = value;
  }

  void toggle()
  {
    for(int i = 0; i < PIN_COUNT; i++)
      if(values[i] > -1)
        values[i] = values[i] ^ 1023;
  }

  void setValues(PinStates &to)
  {
    for(int i = 0; i < PIN_COUNT; i++)
      values[i] = to.values[i];
  }

  void fade(PinStates &to, long f0, long f1)
  {
    for(int i = 0; i < PIN_COUNT; i++)
      if(values[i] > -1)
        analogWrite(pinNumbers[i], ((to.values[i] * f1) + (values[i] * f0)) >> 16);
  }
};

