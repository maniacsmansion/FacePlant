#pragma once
#include <Arduino.h>
#include "../ui/Display.h"
#include "../settings/Settings.h"
#include "../sensors/MoistureSensor.h"

class FaceRenderer {
public:
  enum Mood { HAPPY, DRY, TOO_WET };

  void begin(Display& display, Settings& settings);
  void loop(const MoistureSensor& moisture);

  bool isDeadMode() const { return _deadMode; }
  Mood mood() const { return _mood; }

private:
  Display* _display = nullptr;
  Settings* _settings = nullptr;

  Mood _mood = HAPPY;

  bool _deadMode = false;
  unsigned long _dryStartMs = 0;
  static constexpr unsigned long DRY_DEADLINE_MS = 72UL * 60UL * 60UL * 1000UL;
  unsigned long _nextDeadToggleMs = 0;
  bool _deadShowTombstone = false;

  unsigned long _nextFrameMs = 0;
  static constexpr unsigned long FRAME_MS = 80;

  int8_t _gazeX = 2, _gazeY = 2;
  int8_t _targetX = 2, _targetY = 2;
  unsigned long _nextGazeMs = 0;

  unsigned long _lastBlinkMs = 0;
  unsigned long _nextBlinkMs = 0;
  bool _blinking = false;
  uint8_t _blinkStep = 0;
  unsigned long _blinkUntilMs = 0;

  bool _silly = false;
  uint8_t _sillyVariant = 0;
  unsigned long _sillyUntilMs = 0;
  unsigned long _nextSillyMs = 0;
  unsigned long _lastSillyMs = 0;
  static constexpr unsigned long SILLY_MAX_GAP_MS = 180000UL;

  int8_t _shakeX = 0, _shakeY = 0;
  unsigned long _nextShakeMs = 0;

  void updateMood(int moisturePct);
  void updateDeathMode(unsigned long now);
  void updateHappy(unsigned long now);
  void updateDry(unsigned long now);
  void updateTooWet(unsigned long now);

  void renderDead(unsigned long now);
  void renderNormal(unsigned long now);

  void drawEyesOpen(int pupilDx, int pupilDy);
  void drawEyesClosed();
  void drawEyesSmallPupils(int pupilDy, int sx, int sy);
  void drawBrowsVerySad();
  void drawMouthHappy();
  void drawMouthFrown();
  void drawMouthFlat();
  void drawDroplet(int x, int y, bool pulse);
  void drawBubbles(int off);
  void drawSparkles(unsigned long now);
  void drawSilly(uint8_t v);
  void drawXEye(int cx, int cy);
  void drawBigWaterText();

  unsigned long randRange(unsigned long lo, unsigned long hi);
  int8_t randRangeI8(int8_t lo, int8_t hi);
};
