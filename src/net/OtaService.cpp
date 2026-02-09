#include "OtaService.h"
#include <Update.h>

static const char* OTA_FORM =
  "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
  "<title>PlanterBuddy OTA</title></head>"
  "<body style='font-family:system-ui;margin:18px;max-width:720px'>"
  "<h3>Firmware Update</h3>"
  "<p><small>Upload a .bin built for ESP32. Do not unplug during update.</small></p>"
  "<form method='POST' action='/update' enctype='multipart/form-data'>"
  "<input type='file' name='firmware' accept='.bin' required>"
  "<p><button type='submit' style='padding:10px 14px;border-radius:10px;border:0;background:#111;color:#fff;font-weight:700'>Upload</button></p>"
  "</form></body></html>";

void OtaService::begin(WebServer& server, Display* display) {
  _display = display;

  server.on("/update", HTTP_GET, [&server]() {
    server.send(200, "text/html", OTA_FORM);
  });

  server.on("/update", HTTP_POST,
    [&server]() { /* handled by upload */ },
    [&server, this]() {
      HTTPUpload& upload = server.upload();

      if (upload.status == UPLOAD_FILE_START) {
        if (_display) _display->showStatus("Updating...", "Do not unplug");
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
          server.send(500, "text/plain", "Update begin failed");
          return;
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        Update.write(upload.buf, upload.currentSize);
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          server.send(200, "text/plain", "Update complete. Rebooting...");
          delay(300);
          ESP.restart();
        } else {
          server.send(500, "text/plain", "Update failed.");
        }
      }
    }
  );
}
