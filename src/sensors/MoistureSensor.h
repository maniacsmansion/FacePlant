#pragma once
#include <Arduino.h>
#include "../settings/Settings.h"

class MoistureSensor {
public:
  void begin(Settings& settings);
  void loop();

  int raw() const { return _avgRaw; }
  int percent() const { return _pct; }
  unsigned long lastUpdateMs() const { return _lastUpdateMs; }

private:
  static constexpr int PIN_MOISTURE = 34;

  Settings* _settings = nullptr;

  static constexpr int NUM_SAMPLES = 12;
  int _samples[NUM_SAMPLES]{};
  int _idx = 0;
  long _sum = 0;

  int _avgRaw = 0;
  int _pct = 0;
  unsigned long _lastUpdateMs = 0;

  static constexpr unsigned long INTERVAL_MS = 600;
  unsigned long _nextMs = 0;

  int rawToPercent(int raw) const;
  int clampInt(int v, int lo, int hi) const;
};
