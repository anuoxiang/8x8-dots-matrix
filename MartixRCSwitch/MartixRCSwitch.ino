#include "LedControl.h"
LedControl lc = LedControl(12, 10, 11, 1);
byte disp[8] = {0xf0, 0xc0, 0xa0, 0x10, 0x0, 0x0, 0x0, 0x0};

bool isCol = false;
void (LedControl::*d)(int , int , byte ) ;


void setup() {
  // put your setup code here, to run once:
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  Serial.begin(9600);
}

void loop() {
  byte _disp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (byte r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      byte i = 1 << (7 - c);

      Serial.print("dp:\t");
      Serial.print(disp[r]);
      Serial.print("\ti:\t");
      Serial.println(i);
      if ((disp[r] & i) == i) {
        _disp[c] += 1 << (7 - r);
        Serial.println("hit i");
      }

    }
  }
  Serial.println("Done");

  if (isCol) {
    d = &LedControl::setColumn;
    isCol = false;
  }
  else {
    d = &LedControl::setRow;
    isCol = true;
  }

  memcpy(disp, _disp, sizeof(disp));

  for (byte i = 0; i < 8; i++)
    (lc.*(d))(0, i, disp[i]);

  delay(500);
}
