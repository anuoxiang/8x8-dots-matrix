#include "LedControl.h"
#include <TimerOne.h>

LedControl lc = LedControl(12, 10, 11, 1);
byte disp[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
byte _blink[8] = {0, 0, 0, 0, 0, 0, 0, 0};
byte _disp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
void setup(void)
{
  lc.shutdown(0, false);
  lc.setIntensity(0, 1);
  lc.clearDisplay(0);
  //pinMode(led, OUTPUT);
  Timer1.initialize(50000);
  Timer1.attachInterrupt(blinkLED); // blinkLED to run every 0.15 seconds
  Serial.begin(9600);
}


// The interrupt will blink the LED, and keep
// track of how many times it has blinked.
int ledState = LOW;
volatile unsigned long blinkCount = 1; // use volatile for shared variables
uint8_t blinkIndex = 0;//闪烁所在

void blinkLED(void)
{
  //固定区域复制给先是区域
  memcpy(_disp, disp, sizeof(disp));

  //叠加闪烁区
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
      _disp[i] |=  _blink[i];
  } else {
    ledState = LOW;
    for (byte i = 0; i < 8; i++)
      _disp[i] &= ~_blink[i];
  }

  blinkCount = blinkCount % 2;
  if (blinkCount == 0)//移动节点
  {
    int16_t h = fhigh(disp[blinkIndex]);
    int16_t l = flow(_blink[blinkIndex]);
    if (h == -1 & l == 0) {//固定区什么都没,闪烁已经到底
      disp[blinkIndex] += 1;
      _blink[blinkIndex] = 0;
    }
    else if (h >= 0 & l >= 0 & (h == l - 1)) {
      //侧滑
      //for (byte i = 0; i <= 4; i++) {
      if (blinkIndex>0 & (disp[blinkIndex - 1] & (1 << h)) == 0) {
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
        //固化
        disp[blinkIndex] += (1 << l);
        _blink[blinkIndex] = 0;
      }
      //}
    }
    else {
      _blink[blinkIndex] = _blink[blinkIndex] >> 1;
    }
  }
  blinkCount++;
  for (byte i = 0; i < 8; i++)
    lc.setRow(0, i, _disp[i]);
}
//找到最高位
int16_t fhigh(uint8_t m) {
  for (int16_t i = 7; i >= 0; i--)
    if ((m & 1 << i) == 1 << i) return i;
  return -1;
}
//找到最低位
int16_t flow(uint8_t m) {
  if (m % 2 == 1) return 0;
  for (byte i = 1; i < 8; i++)
    if ((m & 1 << i) == 1 << i) return i;
  return -1;
}

void make_a_point() {
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

void all_full() {
  for (byte i = 0; i < 8; i++)
    disp[i] = 0;
}

// The main program will print the blink count
// to the Arduino Serial Monitor
void loop(void)
{

}
