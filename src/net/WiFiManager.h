#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "../settings/Settings.h"
#include "CaptivePortal.h"

enum NetMode { NET_STA, NET_AP_SETUP };

class WiFiManager {
public:
  void begin(Settings& settings, bool forceSetupAP);
  void loop();

  NetMode mode() const { return _mode; }
  bool connected() const { return WiFi.status() == WL_CONNECTED; }

  IPAddress ip() const;
  String ssid() const;

  void requestSetupAP();

  IPAddress apIp() const { return _apIP; }
  const char* setupSsid() const { return _setupSsid; }

private:
  void startSetupAP();
  void startStation();
  void startStationAttempt();

  Settings* _settings = nullptr;
  NetMode _mode = NET_AP_SETUP;

  CaptivePortal _portal;

  IPAddress _apIP = IPAddress(192,168,4,1);
  IPAddress _apMask = IPAddress(255,255,255,0);
  const char* _setupSsid = "FacePlant-Setup";
  const char* _setupPass = "faceplant";

  unsigned long _bootConnectStartMs = 0;
  static constexpr unsigned long BOOT_CONNECT_TIMEOUT_MS = 20000;

  unsigned long _nextReconnectMs = 0;
  int _reconnectBackoffStep = 0;

  bool _setupRequested = false;
};
