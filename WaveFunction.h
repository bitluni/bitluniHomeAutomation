/*
Released under Creative Commons Attribution 4.0
by bitluni 2016
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it by sharing http://youtube.com/bitlunislab
*/

#include <math.h>
#include "LedFunction.h"

class WaveFunction: public LedFunction
{
  public:
  long sinTab[256];
  uint8_t rgb[3] = {0, 0, 0};
  int start;
  WaveFunction()
  {
    for(int i = 0; i < 256; i++)
      sinTab[i] = sin(3.1415 / 128 * i) * 0x7fff + 0x8000;
    start = millis();
  }

  virtual bool init(ESP8266WebServer &server)
  {
    if(!loadRGBValues(server, rgb))
      return false;
  }
  
  virtual void render()
  {
    int j = ((millis() - start) / 63) & 255;
    int k = ((millis() - start) / 71) & 255;
    for(int i = 0; i < state->count; i++)
    {
      long s = (sinTab[(i*3 + j) & 255] >> 8) * (sinTab[-(i*4 + k) & 255] >> 8);
      state->setRgb(i, (rgb[0] * s) >> 16, (rgb[1] * s) >> 16, (rgb[2] * s) >> 16);
    }
  }
};

