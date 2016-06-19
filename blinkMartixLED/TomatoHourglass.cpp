#include "TomatoHourglass.h"
#include "TimerOne.h"

TomatoHourglass::TomatoHourglass()
{
}


TomatoHourglass::~TomatoHourglass()
{
}

void TomatoHourglass::Init() {
	lc.shutdown(0, false);
	lc.setIntensity(0, 1);
	lc.clearDisplay(0);


}

void TomatoHourglass::Dispose() {

}

void TomatoHourglass::blinkLED() {
	memcpy(_disp, disp, sizeof(disp));

	bool noblink = true;
	for (byte i = 0; i < 8; i++)
		if (_blink[i] > 0) {
			noblink = false;
			break;
		}
	if (noblink) make_a_point();

	if (ledState == LOW) {
		ledState = HIGH;
		for (byte i = 0; i < 8; i++)
			_disp[i] |= _blink[i];
	}
	else {
		ledState = LOW;
		for (byte i = 0; i < 8; i++)
			_disp[i] &= ~_blink[i];
	}

	blinkCount = blinkCount % 20;
	if (blinkCount == 0)
	{
		int16_t h = fhigh(disp[blinkIndex]);
		int16_t l = flow(_blink[blinkIndex]);
		if (h == -1 & l == 0) {
			disp[blinkIndex] += 1;
			_blink[blinkIndex] = 0;
		}
		else if (h >= 0 & l >= 0 & (h == l - 1)) {
			if (blinkIndex > 0 & (disp[blinkIndex - 1] & (1 << h)) == 0) {
				_blink[blinkIndex] -= 1 << l;
				blinkIndex -= 1;
				_blink[blinkIndex] += 1 << h;
			}
			else if (blinkIndex < 7 & (disp[blinkIndex + 1] & (1 << h)) == 0) {
				_blink[blinkIndex] -= 1 << l;
				blinkIndex += 1;
				_blink[blinkIndex] += 1 << h;
			}
			else {
				disp[blinkIndex] += (1 << l);
				_blink[blinkIndex] = 0;
			}
		}
		else {
			_blink[blinkIndex] = _blink[blinkIndex] >> 1;
		}
	}
	blinkCount++;
	for (byte i = 0; i < 8; i++)
		lc.setRow(0, i, _disp[i]);
}

int16_t TomatoHourglass::fhigh(uint8_t m) {
	for (int16_t i = 7; i >= 0; i--)
		if ((m & 1 << i) == 1 << i) return i;
	return -1;
}

int16_t TomatoHourglass::flow(uint8_t m) {
	for (byte i = 0; i < 8; i++)
		if ((m & 1 << i) == 1 << i) return i;
	return -1;
}

void TomatoHourglass::make_a_point() {
	blinkIndex = 4;
	for (byte i = 0; i <= 4; i++) {
		if ((disp[blinkIndex - i] & 0x80) != 0x80) {
			blinkIndex -= i;
			break;
		}
		else if (blinkIndex + i == 8) {
			all_full();
			break;
		}
		else if ((disp[blinkIndex + i] & 0x80) != 0x80) {
			blinkIndex += i;
			break;
		}
	}
	_blink[blinkIndex] = 0x80;
}

void TomatoHourglass::all_full() {
	for (byte i = 0; i < 8; i++)
		disp[i] = 0;
}
