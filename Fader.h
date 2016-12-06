/*
Released under Creative Commons Attribution 4.0
by bitluni 2016
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it by sharing http://youtube.com/bitlunislab
*/

template<class T>
class Fader
{
  public:
  bool active = false;
  long startTime = 0;
  long duration = 0;
  T &from;
  T &to;
  
  Fader(T &fromState, T &toState)
    :from(fromState)
    ,to(toState)
  {
  }

  bool start(int duration)
  {
    if(active)
      return false;
    active = true;
    startTime = millis();
    this->duration = duration;
  }
  
  bool fade()
  {
    if(!active) 
      return false;
    long t = millis() - startTime;
    if(t >= duration || t < 0 || duration == 0)
    {
      from.setValues(to);
      active = false;
      return false;
    }
    float f1 = (float)t / duration;
    float f0 = 1 - f1;
    from.fade(to, long(f0 * 0x10000), long(f1 * 0x10000));
    return true;
  }
};

