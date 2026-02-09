#include "WiFiManager.h"
#include <ESPmDNS.h>
#include "../util/Version.h"

static unsigned long reconnectDelayMs(int step) {
  if (step > 6) step = 6;
  return (30000UL << step); // 30s * 2^step
}

IPAddress WiFiManager::ip() const {
  if (_mode == NET_AP_SETUP) return _apIP;
  return WiFi.localIP();
}

String WiFiManager::ssid() const {
  if (WiFi.status() == WL_CONNECTED) return WiFi.SSID();
  return "";
}

void WiFiManager::begin(Settings& settings, bool forceSetupAP) {
  _settings = &settings;

  WiFi.persistent(false);
  WiFi.setAutoReconnect(false);

  if (forceSetupAP) { startSetupAP(); return; }

  if (_settings->hasWiFi()) {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(PB_HOSTNAME);
    WiFi.begin(_settings->wifiSsid().c_str(), _settings->wifiPass().c_str());
    _bootConnectStartMs = millis();
    _mode = NET_STA;
  } else {
    startSetupAP();
  }
}

void WiFiManager::requestSetupAP() { _setupRequested = true; }

void WiFiManager::startSetupAP() {
  _mode = NET_AP_SETUP;
  _setupRequested = false;

  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(_apIP, _apIP, _apMask);
  WiFi.softAP(_setupSsid, _setupPass);

  _portal.start(_apIP);

  MDNS.end();
  MDNS.begin(PB_HOSTNAME);
  MDNS.addService("http", "tcp", 80);

  WiFi.scanDelete();
  WiFi.scanNetworks(true, true);

  _reconnectBackoffStep = 0;
  _nextReconnectMs = millis() + 30000UL;
}

void WiFiManager::startStation() {
  _mode = NET_STA;
  _setupRequested = false;

  _portal.stop();

  WiFi.softAPdisconnect(true);
  WiFi.enableAP(false);

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(PB_HOSTNAME);

  MDNS.end();
  if (MDNS.begin(PB_HOSTNAME)) MDNS.addService("http", "tcp", 80);

  _reconnectBackoffStep = 0;
  _nextReconnectMs = millis() + 30000UL;
}

void WiFiManager::startStationAttempt() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(PB_HOSTNAME);
  WiFi.begin(_settings->wifiSsid().c_str(), _settings->wifiPass().c_str());
}

void WiFiManager::loop() {
  if (_mode == NET_AP_SETUP) _portal.loop();

  if (_setupRequested) { startSetupAP(); return; }

  if (WiFi.status() == WL_CONNECTED) {
    if (_mode != NET_STA || WiFi.getMode() != WIFI_STA) startStation();
    if (!_settings->everConnected()) _settings->setEverConnected(true);
    return;
  }

  if (!_settings->hasWiFi()) {
    if (_mode != NET_AP_SETUP) startSetupAP();
    return;
  }

  if (!_settings->everConnected() && _bootConnectStartMs != 0 &&
      (millis() - _bootConnectStartMs) > BOOT_CONNECT_TIMEOUT_MS) {
    _bootConnectStartMs = 0;
    startSetupAP();
    return;
  }

  if (_settings->everConnected()) {
    unsigned long now = millis();
    if ((long)(now - _nextReconnectMs) >= 0) {
      startStationAttempt();
      _nextReconnectMs = now + reconnectDelayMs(_reconnectBackoffStep++);
    }
  }
}
