#pragma once
#include <Arduino.h>
#include <Preferences.h>

struct PlantThresholds {
  int dryPct;
  int okPct;
  int tooWetPct;
};

class Settings {
public:
  void begin();

  // Product state
  bool everConnected() const;
  void setEverConnected(bool v);

  // Wi-Fi
  bool hasWiFi() const;
  String wifiSsid() const;
  String wifiPass() const;
  void saveWiFi(const String& ssid, const String& pass);
  void clearWiFi();

  // Plant profile
  String plantProfile() const;
  void setPlantProfile(const String& key);
  PlantThresholds thresholds() const;

  // Calibration
  int dryRaw() const;
  int wetRaw() const;
  void setCalibration(int dryRaw, int wetRaw);

  // Kids mode
  bool kidsMode() const;
  void setKidsMode(bool v);

  // Webhook
  bool webhookEnabled() const;
  void setWebhookEnabled(bool v);
  String webhookUrl() const;
  void setWebhookUrl(const String& url);

private:
  mutable Preferences _prefs;
};
