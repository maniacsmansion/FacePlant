#pragma once
#include <WebServer.h>
#include "../settings/Settings.h"
#include "../net/WiFiManager.h"
#include "../sensors/MoistureSensor.h"
#include "../ui/FaceRenderer.h"
#include "../net/WebhookService.h"

class WebUI {
public:
  void begin(Settings& settings,
             WiFiManager& wifi,
             MoistureSensor& moisture,
             FaceRenderer& face,
             WebhookService& webhook,
             unsigned long bootMs);

  void loop();
  WebServer& server() { return _server; }

private:
  WebServer _server{80};
};