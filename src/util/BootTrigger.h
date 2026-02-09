#pragma once
#include <Arduino.h>

class BootTrigger {
public:
  // Best-effort 3 quick resets trigger (RTC memory).
  // May not survive full power removal.
  static bool checkAndConsume();

  // Clear burst state after stable uptime (~15s)
  static void clearAfterStableUptime();
};
