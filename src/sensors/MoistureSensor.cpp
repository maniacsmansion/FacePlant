#include "MoistureSensor.h"

int MoistureSensor::clampInt(int v, int lo, int hi) const {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

int MoistureSensor::rawToPercent(int raw) const {
  int dry = _settings->dryRaw();
  int wet = _settings->wetRaw();
  long denom = (long)(dry - wet);
  if (denom == 0) return 0;
  long pct = (long)(dry - raw) * 100L / denom;
  return clampInt((int)pct, 0, 100);
}

void MoistureSensor::begin(Settings& settings) {
  _settings = &settings;
  analogReadResolution(12);

  int first = analogRead(PIN_MOISTURE);
  for (int i = 0; i < NUM_SAMPLES; i++) { _samples[i] = first; _sum += first; }
  _avgRaw = first;
  _pct = rawToPercent(_avgRaw);
  _nextMs = millis();
  _lastUpdateMs = millis();
}

void MoistureSensor::loop() {
  unsigned long now = millis();
  if ((long)(now - _nextMs) < 0) return;
  _nextMs = now + INTERVAL_MS;

  int raw = analogRead(PIN_MOISTURE);
  _sum -= _samples[_idx];
  _samples[_idx] = raw;
  _sum += raw;
  _idx = (_idx + 1) % NUM_SAMPLES;

  _avgRaw = (int)(_sum / NUM_SAMPLES);
  _pct = rawToPercent(_avgRaw);
  _lastUpdateMs = now;
}
