#include "Display.h"

void Display::begin() {
  Wire.begin(PIN_SDA, PIN_SCL);
  _ok = _oled.begin(OLED_ADDR, true);
  if (!_ok) return;
  bootAnimation();
}

void Display::showStatus(const String& line1, const String& line2) {
  if (!_ok) return;
  _oled.clearDisplay();
  _oled.setTextSize(1);
  _oled.setTextColor(1);
  _oled.setCursor(0, 0);
  _oled.print(line1);
  _oled.setCursor(0, 12);
  _oled.print(line2);
  _oled.display();
}

void Display::bootAnimation() {
  if (!_ok) return;

  _oled.clearDisplay();
  _oled.fillCircle(40, 24, 8, 1);
  _oled.fillCircle(88, 24, 8, 1);
  _oled.display();
  delay(200);

  _oled.drawLine(32, 24, 48, 24, 1);
  _oled.drawLine(80, 24, 96, 24, 1);
  _oled.display();
  delay(120);

  _oled.clearDisplay();
  _oled.fillCircle(40, 24, 8, 1);
  _oled.fillCircle(88, 24, 8, 1);

  for (int i = 0; i < 6; i++) {
    _oled.drawLine(52+i, 44+i/2, 76-i, 44+i/2, 1);
    _oled.display();
    delay(60);
  }

  _oled.setTextSize(1);
  _oled.setTextColor(1);
  _oled.setCursor(34, 56);
  _oled.print("FacePlant");
  _oled.display();
  delay(350);
}
