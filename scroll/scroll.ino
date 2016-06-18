#include "LedControl.h"
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;
LedControl lc = LedControl(12, 10, 11, 1);
#define TIMESTYLE 3   //3=h:m:s,2=h:s
#define SPLIT 0x24    //split :
uint8_t chars[] = {
  0x3e, 0x41, 0x41, 0x3e, 0x80,       //0
  0x21, 0x7f, 0x1, 0x80,              //1
  0x27, 0x49, 0x49, 0x33, 0x80,       //2
  0x22, 0x49, 0x49, 0x36, 0x80,       //3
  0xc, 0x14, 0x24, 0x7f, 0x4, 0x80,   //4
  0x79, 0x49, 0x49, 0x46, 0x80,       //5
  0x3e, 0x49, 0x49, 0x6, 0x80,        //6
  0x40, 0x47, 0x58, 0x60, 0x80,       //7
  0x36, 0x49, 0x49, 0x36, 0x80,       //8
  0x32, 0x49, 0x49, 0x3e, 0x80,       //9
};

const byte delaytime = 100;

/* char pointer.
   point to which char is displaying.
*/
byte c_pointer = 0;

/* char position pointer
   when led is scrolling, it will be descending from 7 to 0
   but when 7-c_p_pointer > length of char then c_pointer++
*/
uint16_t c_p_pointer = 0;//char position pointer

/* some words is pringing, but not done
    char or words flag.
*/
bool charPrinting = false;
bool wordPrinting = false;
uint8_t words = 0;            //当前要现实的数字
int8_t hi = 0;               //显示数字的位数
uint8_t hms = 0;              //时分秒,逐个显示,最后清屏重来

byte disp[8] = {0xf0, 0xc0, 0xa0, 0x10, 0x0, 0x0, 0x0, 0x0};

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  //RTC.adjust(DateTime(__DATE__, __TIME__));
}

bool IsBlank() {
  bool f = true;
  for (byte i = 0; i < 8; i++)
    if (disp[i] != 0) {
      f = false;
      break;
    }
  return f;
}

void printingchar() {
  disp[0] += (chars[c_p_pointer] & 0x80 ? 1 : 0);
  disp[1] += (chars[c_p_pointer] & 0x40 ? 1 : 0);
  disp[2] += (chars[c_p_pointer] & 0x20 ? 1 : 0);
  disp[3] += (chars[c_p_pointer] & 0x10 ? 1 : 0);
  disp[4] += (chars[c_p_pointer] & 0x08 ? 1 : 0);
  disp[5] += (chars[c_p_pointer] & 0x04 ? 1 : 0);
  disp[6] += (chars[c_p_pointer] & 0x02 ? 1 : 0);
  disp[7] += (chars[c_p_pointer] & 0x01 ? 1 : 0);
  c_p_pointer++;
  charPrinting = (chars[c_p_pointer] != 0x80);
}

void printsplit() {
  disp[0] += (SPLIT & 0x80 ? 1 : 0);
  disp[1] += (SPLIT & 0x40 ? 1 : 0);
  disp[2] += (SPLIT & 0x20 ? 1 : 0);
  disp[3] += (SPLIT & 0x10 ? 1 : 0);
  disp[4] += (SPLIT & 0x08 ? 1 : 0);
  disp[5] += (SPLIT & 0x04 ? 1 : 0);
  disp[6] += (SPLIT & 0x02 ? 1 : 0);
  disp[7] += (SPLIT & 0x01 ? 1 : 0);
}

void printingword() {
  if (!charPrinting) {
    if (hi >= 0) {
      Serial.print("print a number. bit of:");
      Serial.println(hi);

      uint8_t m;
      /*
        words = 321
        hi = 2
        1:321 / 10 ^ 2 = 3 - (321 / 10 ^ 3) * 10 ^ 3 = 3
        2:321 / 10 ^ 1 = 32 - (321 / 10 ^ 2) * 10 ^ 2 = 2
        3:321 / 10 ^ 0 = 321 - (321 / 10 ^ 1) * 10 ^ 1 =  1
      */
      //Serial.print("hi:");
      //Serial.print((words / pow(10, hi + 1)) * pow(10, hi + 1));
      //Serial.println((uint16_t)(words / pow(10, hi)));
      //Serial.println(((uint16_t)(words / pow(10, hi + 1))) * pow(10, hi + 1));
      m = (uint16_t)(words / pow(10, hi)) - ((uint16_t)(words / pow(10, hi + 1))) * pow(10, hi + 1);
      //Serial.println(m);
      c_p_pointer = 0;
      while (m > 0) {
        m -= chars[c_p_pointer] == 0x80 ? 1 : 0;
        c_p_pointer++;
      }
      charPrinting = true;
      hi--;
      return;
    }
    else {
      Serial.println("done all numbers.");
      wordPrinting = false;
      return;
    }
  }
  printingchar();
}

void loop() {
  DateTime now = RTC.now();
  if (!wordPrinting & hms == TIMESTYLE) {
    if (IsBlank()) {
      hms = 0;
    }
  }
  else if (!wordPrinting) { //处理下一个时间域
    switch (hms) {
      case 0:
        words = now.hour();
        hi = 1;
        break;
      case 1:
        words = now.minute();
        hi = 1;
        printsplit();
        break;
      case 2:
        words = now.second();
        hi = 1;
        printsplit();
        break;
    }
    
    hms++;
    wordPrinting = true;
    //printingword();
  }
  else {
    printingword();
  }

  //set led to display
  for (byte i = 0; i < 8; i++)
    lc.setRow(0, i, disp[i]);

  for (byte i = 0; i < 8; i++)
    disp[i] = disp[i] << 1;

  delay(delaytime);
}


