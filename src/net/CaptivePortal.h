#pragma once
#include <DNSServer.h>
#include <IPAddress.h>

class CaptivePortal {
public:
  void start(const IPAddress& redirectIp);
  void stop();
  void loop();

private:
  DNSServer _dns;
  bool _running = false;
  IPAddress _ip;
  static constexpr byte DNS_PORT = 53;
};
