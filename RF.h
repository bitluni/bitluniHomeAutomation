/*
Released under Creative Commons Attribution 4.0
by bitluni 2016
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it by sharing http://youtube.com/bitlunislab
*/

//Works with BH9938 with t=200

inline void rfPreamble(int pin, int t)
{
	int m = micros();
	digitalWrite(pin, 1);
	while(micros() - m < t);
	digitalWrite(pin, 0);
	while(micros() - m < t * 32);
}

inline void rfWriteBit(int pin, int t, int b)
{
	int m = micros();
	if(b)
	{
		digitalWrite(pin, 1);
		while(micros() - m < t * 3);
		digitalWrite(pin, 0);
		while(micros() - m < t * 4);
	}
	else
	{
		digitalWrite(pin, 1);
		while(micros() - m < t);
		digitalWrite(pin, 0);
		while(micros() - m < t * 4);
	}
}

void rfWriteCode(int pin, int t, int code, int data)
{
	rfPreamble(pin, t);
	for(int i = 0; i < 20; i++)
	{
		rfWriteBit(pin, t, code & 1);
		code >>= 1;
	}
	for(int i = 0; i < 4; i++)
	{
		rfWriteBit(pin, t, data & 1);
		data >>= 1;
	}
}
