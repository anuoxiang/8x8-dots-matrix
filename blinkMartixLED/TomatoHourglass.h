#pragma once
#include "LedControl.h"
static class TomatoHourglass
{
public:
	TomatoHourglass();
	~TomatoHourglass();
	static void Init();
	static void Dispose();
	static void blinkLED();

private:
	LedControl lc = LedControl(12, 10, 11, 1);
	byte disp[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	byte _blink[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	byte _disp[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int ledState = LOW;
	volatile unsigned long blinkCount = 1;
	uint8_t blinkIndex = 0;


	static int16_t fhigh(uint8_t m);
	static int16_t flow(uint8_t m);
	static void make_a_point();
	static void all_full();
};

