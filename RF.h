/*
Released under Creative Commons Attribution 4.0
by bitluni 2016
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it by sharing http://youtube.com/bitlunislab
*/

//Works with BH9938 with t=200

inline void rfPreamble(const int pin, const int t)
{
	int m = micros();
	digitalWrite(pin, 1);
	while(micros() - m < t);
	digitalWrite(pin, 0);
	while(micros() - m < t * 32);
}

inline void rfWriteBit(const int pin, const int t, int b)
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

inline void rfWriteByte(const int pin, const int t, int b)
{
		rfWriteBit(pin, t, b & 128);
		rfWriteBit(pin, t, b & 64);
		rfWriteBit(pin, t, b & 32);
		rfWriteBit(pin, t, b & 16);
		rfWriteBit(pin, t, b & 8);
		rfWriteBit(pin, t, b & 4);
		rfWriteBit(pin, t, b & 2);
		rfWriteBit(pin, t, b & 1);
}

void rfWriteCode(const int pin, const int t, int byte1, int byte2, int byte3)
{
	rfPreamble(pin, t);
	rfWriteByte(pin, t, byte1);
	rfWriteByte(pin, t, byte2);
	rfWriteByte(pin, t, byte3);
}




