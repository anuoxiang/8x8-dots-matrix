#include "Arduino.h"
#include "LedControl.h"
#include "TimerOne.h"
#include "TomatoHourglass.h"

TomatoHourglass t;
void setup(void)
{
	t.Init();
	Timer1.initialize(50000);
	//[this](const String& s)->int{return doIt(s); }
	//Timer1.attachInterrupt([this](void)-> void {return blinkLED(); });
	Timer1.attachInterrupt(t.blinkLED);
	Serial.begin(9600);
}

void a(){

}

void loop(void)
{

}
