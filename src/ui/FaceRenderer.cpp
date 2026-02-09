#include "FaceRenderer.h"

static int clampInt(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

unsigned long FaceRenderer::randRange(unsigned long lo, unsigned long hi) {
  if (hi <= lo) return lo;
  return lo + (unsigned long)random((long)(hi - lo + 1UL));
}

int8_t FaceRenderer::randRangeI8(int8_t lo, int8_t hi) {
  if (hi <= lo) return lo;
  return (int8_t)(lo + random((int)(hi - lo + 1)));
}

void FaceRenderer::begin(Display& display, Settings& settings) {
  _display = &display;
  _settings = &settings;

  randomSeed((uint32_t)esp_random());

  unsigned long now = millis();
  _nextFrameMs = now;
  _nextBlinkMs = now + randRange(3000, 9000);
  _nextSillyMs = now + randRange(20000, 60000);
  _nextGazeMs  = now + randRange(800, 2000);
}

void FaceRenderer::loop(const MoistureSensor& moisture) {
  unsigned long now = millis();
  if ((long)(now - _nextFrameMs) < 0) return;
  _nextFrameMs = now + FRAME_MS;

  updateMood(moisture.percent());
  updateDeathMode(now);

  if (_deadMode) {
    renderDead(now);
    return;
  }

  if (_mood == HAPPY) updateHappy(now);
  else if (_mood == DRY) updateDry(now);
  else updateTooWet(now);

  renderNormal(now);
}

void FaceRenderer::updateMood(int moisturePct) {
  PlantThresholds t = _settings->thresholds();

  if (moisturePct >= t.tooWetPct) _mood = TOO_WET;
  else if (moisturePct <= t.dryPct) _mood = DRY;
  else _mood = HAPPY;
}

void FaceRenderer::updateDeathMode(unsigned long now) {
  if (_settings->kidsMode()) {
    _deadMode = false;
    _dryStartMs = 0;
    return;
  }

  if (_mood == DRY) {
    if (_dryStartMs == 0) _dryStartMs = now;
    if ((now - _dryStartMs) > DRY_DEADLINE_MS) {
      _deadMode = true;
      _nextDeadToggleMs = now + 3000;
    }
  } else {
    _dryStartMs = 0;
    _deadMode = false;
  }
}

void FaceRenderer::updateHappy(unsigned long now) {
  if ((long)(now - _nextGazeMs) >= 0) {
    _targetX = randRangeI8(-3, 3);
    _targetY = randRangeI8(-2, 2);
    _nextGazeMs = now + randRange(800, 2200);
  }

  _gazeX += clampInt(_targetX - _gazeX, -1, 1);
  _gazeY += clampInt(_targetY - _gazeY, -1, 1);

  bool sillyDue = (long)(now - _nextSillyMs) >= 0;
  if (!_silly && sillyDue) {
    _silly = true;
    _sillyVariant = random(0, 3);
    _sillyUntilMs = now + (_settings->kidsMode() ? 4000 : 2000);
    _nextSillyMs = now + (_settings->kidsMode() ? randRange(15000, 35000)
                                                : randRange(60000, 180000));
  }

  if (_silly && (long)(now - _sillyUntilMs) >= 0) _silly = false;

  if ((now - _lastBlinkMs) > 60000 || (long)(now - _nextBlinkMs) >= 0) {
    _blinking = true;
    _blinkUntilMs = now + 90;
    _lastBlinkMs = now;
    _nextBlinkMs = now + randRange(5000, 15000);
  }

  if (_blinking && (long)(now - _blinkUntilMs) >= 0) _blinking = false;
}

void FaceRenderer::updateDry(unsigned long now) {
  if ((now - _lastBlinkMs) > 60000) {
    _blinking = true;
    _blinkUntilMs = now + 120;
    _lastBlinkMs = now;
  }
  if (_blinking && (long)(now - _blinkUntilMs) >= 0) _blinking = false;
}

void FaceRenderer::updateTooWet(unsigned long now) {
  if ((now - _lastBlinkMs) > 60000) {
    _blinking = true;
    _blinkUntilMs = now + 90;
    _lastBlinkMs = now;
  }
  if (_blinking && (long)(now - _blinkUntilMs) >= 0) _blinking = false;
}

void FaceRenderer::renderDead(unsigned long now) {
  auto &d = _display->oled();
  d.clearDisplay();

  if ((long)(now - _nextDeadToggleMs) >= 0) {
    _deadShowTombstone = !_deadShowTombstone;
    _nextDeadToggleMs = now + 3000;
  }

  if (_deadShowTombstone) {
    d.drawRoundRect(38, 12, 52, 40, 8, 1);
    d.setCursor(52, 28);
    d.setTextSize(2);
    d.print("RIP");
  } else {
    drawXEye(40, 24);
    drawXEye(88, 24);
    drawMouthFlat();
  }

  d.display();
}

void FaceRenderer::renderNormal(unsigned long now) {
  auto &d = _display->oled();
  d.clearDisplay();

  if (_mood == HAPPY) {
    if (_silly) drawSilly(_sillyVariant);
    else if (_blinking) drawEyesClosed();
    else drawEyesOpen(_gazeX, _gazeY);
    drawMouthHappy();
    drawSparkles(now);
  }
  else if (_mood == DRY) {
    if (_blinking) drawEyesClosed();
    else drawEyesSmallPupils(6, 0, 0);
    drawBrowsVerySad();
    drawMouthFrown();
    drawBigWaterText();
  }
  else {
    if (_blinking) drawEyesClosed();
    else drawEyesOpen(0, 0);
    drawMouthFlat();
    drawBubbles((now / 100) % 12 - 6);
  }

  d.display();
}

/* --- Drawing helpers --- */

void FaceRenderer::drawEyesOpen(int dx, int dy) {
  auto &d = _display->oled();
  d.fillCircle(40, 24, 12, 1);
  d.fillCircle(88, 24, 12, 1);
  d.fillCircle(40 + dx, 24 + dy, 4, 0);
  d.fillCircle(88 + dx, 24 + dy, 4, 0);
}

void FaceRenderer::drawEyesClosed() {
  auto &d = _display->oled();
  d.drawLine(28, 24, 52, 24, 1);
  d.drawLine(76, 24, 100, 24, 1);
}

void FaceRenderer::drawEyesSmallPupils(int dy, int sx, int sy) {
  auto &d = _display->oled();
  d.fillCircle(40, 24, 12, 1);
  d.fillCircle(88, 24, 12, 1);
  d.fillCircle(40 + sx, 24 + dy, 2, 0);
  d.fillCircle(88 + sx, 24 + dy, 2, 0);
}

void FaceRenderer::drawBrowsVerySad() {
  auto &d = _display->oled();
  d.drawLine(26, 12, 48, 18, 1);
  d.drawLine(80, 18, 102, 12, 1);
}

void FaceRenderer::drawMouthHappy() {
  auto &d = _display->oled();
  for (int i = 0; i < 5; i++)
    d.drawLine(54 + i, 46 + i / 2, 74 - i, 46 + i / 2, 1);
}

void FaceRenderer::drawMouthFrown() {
  auto &d = _display->oled();
  for (int i = 0; i < 5; i++)
    d.drawLine(54 - i, 48 + i / 2, 74 + i, 48 + i / 2, 1);
}

void FaceRenderer::drawMouthFlat() {
  auto &d = _display->oled();
  d.drawLine(52, 48, 76, 48, 1);
}

void FaceRenderer::drawBubbles(int off) {
  auto &d = _display->oled();
  d.drawCircle(10 + off, 12, 4, 1);
  d.drawCircle(20 + off, 20, 3, 1);
  d.drawCircle(30 + off, 14, 5, 1);
}

void FaceRenderer::drawSparkles(unsigned long now) {
  auto &d = _display->oled();
  if ((now / 250) % 2 == 0) {
    d.drawPixel(10, 10, 1);
    d.drawPixel(118, 12, 1);
  }
}

void FaceRenderer::drawSilly(uint8_t v) {
  auto &d = _display->oled();
  if (v == 0) {
    drawEyesOpen(2, 2);
    d.drawCircle(64, 48, 6, 1);
  } else if (v == 1) {
    drawEyesClosed();
    d.drawRect(58, 44, 12, 8, 1);
  } else {
    drawEyesOpen(-2, 1);
    d.drawLine(52, 48, 76, 48, 1);
    d.drawPixel(64, 54, 1);
  }
}

void FaceRenderer::drawXEye(int cx, int cy) {
  auto &d = _display->oled();
  d.drawLine(cx - 6, cy - 6, cx + 6, cy + 6, 1);
  d.drawLine(cx - 6, cy + 6, cx + 6, cy - 6, 1);
}

void FaceRenderer::drawBigWaterText() {
  auto &d = _display->oled();
  d.setTextSize(1);
  d.setCursor(38, 56);
  d.print("WATER?");
}
