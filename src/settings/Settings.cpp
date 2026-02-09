#include "Settings.h"

static PlantThresholds thresholdsForProfile(const String& key) {
  if (key == "succulent") return {20, 28, 80};
  if (key == "herbs")     return {35, 42, 88};
  if (key == "tropical")  return {45, 52, 90};
  if (key == "fern")      return {55, 62, 92};
  if (key == "veg")       return {40, 48, 90};
  return {35, 42, 85}; // houseplant default
}

void Settings::begin() {
  _prefs.begin("faceplant", false);
}

bool Settings::everConnected() const { return _prefs.getBool("ever_ok", false); }
void Settings::setEverConnected(bool v) { _prefs.putBool("ever_ok", v); }

bool Settings::hasWiFi() const { return _prefs.getString("wifi_ssid", "").length() > 0; }
String Settings::wifiSsid() const { return _prefs.getString("wifi_ssid", ""); }
String Settings::wifiPass() const { return _prefs.getString("wifi_pass", ""); }
void Settings::saveWiFi(const String& ssid, const String& pass) {
  _prefs.putString("wifi_ssid", ssid);
  _prefs.putString("wifi_pass", pass);
}
void Settings::clearWiFi() { _prefs.remove("wifi_ssid"); _prefs.remove("wifi_pass"); }

String Settings::plantProfile() const { return _prefs.getString("plant_profile", "house"); }
void Settings::setPlantProfile(const String& key) { _prefs.putString("plant_profile", key); }
PlantThresholds Settings::thresholds() const { return thresholdsForProfile(plantProfile()); }

int Settings::dryRaw() const { return _prefs.getInt("cal_dry_raw", 3000); }
int Settings::wetRaw() const { return _prefs.getInt("cal_wet_raw", 1600); }
void Settings::setCalibration(int dryRaw, int wetRaw) {
  _prefs.putInt("cal_dry_raw", dryRaw);
  _prefs.putInt("cal_wet_raw", wetRaw);
}

bool Settings::kidsMode() const { return _prefs.getBool("kids_mode", false); }
void Settings::setKidsMode(bool v) { _prefs.putBool("kids_mode", v); }

bool Settings::webhookEnabled() const { return _prefs.getBool("wh_en", false); }
void Settings::setWebhookEnabled(bool v) { _prefs.putBool("wh_en", v); }

String Settings::webhookUrl() const { return _prefs.getString("wh_url", ""); }
void Settings::setWebhookUrl(const String& url) { _prefs.putString("wh_url", url); }
