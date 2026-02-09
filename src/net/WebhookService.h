#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "../settings/Settings.h"
#include "../sensors/MoistureSensor.h"

enum PlantEventType {
  EVT_OK = 0,
  EVT_DRY = 1,
  EVT_TOO_WET = 2,
  EVT_DEAD = 3,
  EVT_TEST = 4
};

class WebhookService {
public:
  void begin(Settings& settings);
  bool send(PlantEventType type, const MoistureSensor& moisture, bool deadMode, unsigned long bootMs);

private:
  Settings* _settings = nullptr;
  const char* eventName(PlantEventType t) const;
};
