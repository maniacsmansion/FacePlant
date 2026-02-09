#include "WebUI.h"
#include "../util/Version.h"
#include <ArduinoJson.h>

void WebUI::begin(Settings& settings,
                  WiFiManager& wifi,
                  MoistureSensor& moisture,
                  FaceRenderer& face,
                  WebhookService& webhook,
                  unsigned long bootMs) {

  _server.on("/", HTTP_GET, [&]() {
    String page =
      "<h2>FacePlant</h2>"
      "<p>Firmware v" + String(PB_VERSION) + "</p>"
      "<form method='POST' action='/config'>"
      "<label>Plant profile</label><br>"
      "<select name='plant'>"
      "<option value='house'>Houseplant</option>"
      "<option value='succulent'>Succulent</option>"
      "<option value='herbs'>Herbs</option>"
      "<option value='fern'>Fern</option>"
      "<option value='tropical'>Tropical</option>"
      "</select><br><br>"
      "<label><input type='checkbox' name='kids' " + String(settings.kidsMode() ? "checked" : "") + "> Kids Mode</label><br><br>"
      "<label>Webhook URL</label><br>"
      "<input name='wh' size='40' value='" + settings.webhookUrl() + "'><br>"
      "<label><input type='checkbox' name='wh_en' " + String(settings.webhookEnabled() ? "checked" : "") + "> Enable webhook</label><br><br>"
      "<button type='submit'>Save</button>"
      "</form>"
      "<p><a href='/status'>Status (JSON)</a></p>"
      "<p><a href='/update'>OTA Update</a></p>";
    _server.send(200, "text/html", page);
  });

  _server.on("/config", HTTP_POST, [&]() {
    if (_server.hasArg("plant")) settings.setPlantProfile(_server.arg("plant"));
    settings.setKidsMode(_server.hasArg("kids"));
    settings.setWebhookEnabled(_server.hasArg("wh_en"));
    if (_server.hasArg("wh")) settings.setWebhookUrl(_server.arg("wh"));
    _server.sendHeader("Location", "/");
    _server.send(303);
  });

  _server.on("/status", HTTP_GET, [&]() {
    JsonDocument doc;
    doc["device"] = "FacePlant";
    doc["version"] = PB_VERSION;
    doc["ip"] = wifi.ip().toString();
    doc["ssid"] = wifi.ssid();
    doc["moisture_pct"] = moisture.percent();
    doc["raw"] = moisture.raw();
    doc["kids_mode"] = settings.kidsMode();
    doc["dead_mode"] = face.isDeadMode();
    doc["uptime_ms"] = millis() - bootMs;

    String out;
    serializeJson(doc, out);
    _server.send(200, "application/json", out);
  });

  _server.begin();
}

void WebUI::loop() {
  _server.handleClient();
}
