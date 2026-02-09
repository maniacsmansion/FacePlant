#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

class Display {
public:
  void begin();
  Adafruit_SH1106G& oled() { return _oled; }

  void showStatus(const String& line1, const String& line2);
  void bootAnimation();

private:
  static constexpr int PIN_SDA = 21;
  static constexpr int PIN_SCL = 22;
  static constexpr uint8_t OLED_ADDR = 0x3C; // try 0x3D if blank
  Adafruit_SH1106G _oled = Adafruit_SH1106G(128, 64, &Wire, -1);
  bool _ok = false;
};
