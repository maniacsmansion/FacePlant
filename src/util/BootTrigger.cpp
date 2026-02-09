#include "BootTrigger.h"

RTC_DATA_ATTR uint32_t fp_bootBurstCount = 0;
RTC_DATA_ATTR uint32_t fp_bootBurstMagic = 0xA5A5A5A5;

bool BootTrigger::checkAndConsume() {
  if (fp_bootBurstMagic != 0xA5A5A5A5) {
    fp_bootBurstMagic = 0xA5A5A5A5;
    fp_bootBurstCount = 1;
    return false;
  }
  fp_bootBurstCount++;
  if (fp_bootBurstCount >= 3) {
    fp_bootBurstCount = 0;
    return true;
  }
  return false;
}

void BootTrigger::clearAfterStableUptime() {
  if (millis() > 15000) fp_bootBurstCount = 0;
}
