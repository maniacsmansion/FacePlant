#include "CaptivePortal.h"

void CaptivePortal::start(const IPAddress& redirectIp) {
  _ip = redirectIp;
  _dns.start(DNS_PORT, "*", _ip);
  _running = true;
}

void CaptivePortal::stop() {
  if (_running) {
    _dns.stop();
    _running = false;
  }
}

void CaptivePortal::loop() {
  if (_running) _dns.processNextRequest();
}
