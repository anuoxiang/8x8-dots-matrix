#include "LedControl.h"

LedControl lc = LedControl(12, 10, 11, 1);
byte a[] = {0x3f, 0x48, 0x48, 0x3f, 0x80};
byte r[] = {0x1f, 0x10, 0x10, 0x8, 0x80};
byte d[] = {0xe, 0x11, 0x11, 0x7f, 0x80};
byte u[] = {0x1e, 0x1, 0x1, 0x1f, 0x80};
byte i[] = {0x8, 0x2f, 0x80};
byte n[] = {0x1f, 0x10, 0x10, 0xf, 0x80};
byte o[] = {0xe, 0x11, 0x11, 0xe, 0x80};
byte * str[7] = {a, r, d, u, i, n, o};

byte disp[8] = {0xf0, 0xc0, 0xa0, 0x10, 0x0, 0x0, 0x0, 0x0};
void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  Serial.begin(9600);
}

const byte delaytime = 150;

/* char pointer.
   point to which char is displaying.
*/
byte c_pointer = 0;

/* char position pointer
   when led is scrolling, it will be descending from 7 to 0
   but when 7-c_p_pointer > length of char then c_pointer++

*/
byte c_p_pointer = 0;//char position pointer

bool IsBlank() {
  bool f = true;
  for (byte i = 0; i < 8; i++)
    if (disp[i] != 0) {
      f = false;
      break;
    }
  return f;
}

void loop() {

  //Serial.println(c_pointer);
  //c_pointer = c_pointer<<1;

  //first screen is blank.
  if (str[c_pointer][c_p_pointer] == 0x80) {

    //if ( c_p_pointer >= 4) { //sizeof(&str[c_pointer]) / sizeof(&str[c_pointer][0])) {
    //Serial.println("reset");



    if (c_pointer+1 == (sizeof(str) / sizeof(str[0])))
    {
      if (IsBlank()) {
        c_pointer = 0;
        c_p_pointer = 0;
      }
    }
    else {
      c_p_pointer = 0;
      c_pointer++;
    }
  }
  else {
    //Serial.print("run here 2.");
    disp[0] += (str[c_pointer][c_p_pointer] & 0x80 ? 1 : 0);
    disp[1] += (str[c_pointer][c_p_pointer] & 0x40 ? 1 : 0);
    disp[2] += (str[c_pointer][c_p_pointer] & 0x20 ? 1 : 0);
    disp[3] += (str[c_pointer][c_p_pointer] & 0x10 ? 1 : 0);
    disp[4] += (str[c_pointer][c_p_pointer] & 0x08 ? 1 : 0);
    disp[5] += (str[c_pointer][c_p_pointer] & 0x04 ? 1 : 0);
    disp[6] += (str[c_pointer][c_p_pointer] & 0x02 ? 1 : 0);
    disp[7] += (str[c_pointer][c_p_pointer] & 0x01 ? 1 : 0);
    c_p_pointer++;
  }

  //set led to display
  for (byte i = 0; i < 8; i++)
    lc.setRow(0, i, disp[i]);

  for (byte i = 0; i < 8; i++)
    disp[i] = disp[i] << 1;

  delay(delaytime);
}
