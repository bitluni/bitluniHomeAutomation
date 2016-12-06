/*
Released under Creative Commons Attribution 4.0
by bitluni 2016
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it by sharing http://youtube.com/bitlunislab
*/

#include "LedFunction.h"

class RainbowFunction: public LedFunction
{
  public:
  RainbowFunction()
  {    
  }

  virtual void render()
  {
    int j = (millis() >> 4) & 255;
    for(int i = 0; i < state->count; i++)
    {
      int ij = 255 - ((i + j) & 255);
      if(ij < 85)
        state->setRgb(i, 255 - ij * 3, 0, ij * 3);
      else
      if(ij < 170)
        state->setRgb(i, 0, (ij - 85) * 3, 255 - (ij - 85) * 3);
      else
        state->setRgb(i, (ij -170) * 3, 255 - (ij -170) * 3, 0);
    }
  }
};

