#include "WebhookService.h"
#include "../util/Version.h"

void WebhookService::begin(Settings& settings) {
  _settings = &settings;
}

const char* WebhookService::eventName(PlantEventType t) const {
  switch (t) {
    case EVT_OK: return "ok";
    case EVT_DRY: return "dry";
    case EVT_TOO_WET: return "too_wet";
    case EVT_DEAD: return "dead";
    case EVT_TEST: return "test";
    default: return "unknown";
  }
}

bool WebhookService::send(PlantEventType type, const MoistureSensor& moisture, bool deadMode, unsigned long bootMs) {
  if (!_settings) return false;
  if (!_settings->webhookEnabled()) return false;
  String url = _settings->webhookUrl();
  if (url.length() < 8) return false;
  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;
  http.setTimeout(2500);
  if (!http.begin(url)) return false;
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<768> doc;
  doc["device"] = "FacePlant";
  doc["hostname"] = String(PB_HOSTNAME) + ".local";
  doc["version"] = PB_VERSION;
  doc["event"] = eventName(type);
  doc["dead_mode"] = deadMode;
  doc["moisture_pct"] = moisture.percent();
  doc["moisture_raw"] = moisture.raw();
  doc["uptime_ms"] = (uint32_t)(millis() - bootMs);

  String body;
  serializeJson(doc, body);

  int code = http.POST((uint8_t*)body.c_str(), body.length());
  http.end();
  return (code >= 200 && code < 300);
}
