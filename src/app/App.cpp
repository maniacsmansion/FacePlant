#include "App.h"

#include "../util/Version.h"
#include "../util/BootTrigger.h"

#include "../settings/Settings.h"
#include "../net/WiFiManager.h"
#include "../net/WebUI.h"
#include "../net/OtaService.h"
#include "../net/WebhookService.h"

#include "../ui/Display.h"
#include "../sensors/MoistureSensor.h"
#include "../ui/FaceRenderer.h"

static Settings settings;
static WiFiManager wifi;
static WebUI web;
static OtaService ota;
static WebhookService webhook;

static Display display;
static MoistureSensor moisture;
static FaceRenderer face;

static unsigned long bootMs = 0;

static FaceRenderer::Mood lastMood = FaceRenderer::HAPPY;
static bool lastDead = false;

static PlantEventType moodToEvent(FaceRenderer::Mood m) {
  if (m == FaceRenderer::DRY) return EVT_DRY;
  if (m == FaceRenderer::TOO_WET) return EVT_TOO_WET;
  return EVT_OK;
}

void App::setup() {
  bootMs = millis();

  settings.begin();
  bool forceSetup = BootTrigger::checkAndConsume();

  display.begin();
  display.showStatus("FacePlant", "Starting...");

  wifi.begin(settings, forceSetup);

  moisture.begin(settings);
  face.begin(display, settings);

  webhook.begin(settings);

  web.begin(settings, wifi, moisture, face, webhook, bootMs);
  ota.begin(web.server(), &display);

  lastMood = face.mood();
  lastDead = face.isDeadMode();
}

void App::loop() {
  BootTrigger::clearAfterStableUptime();

  wifi.loop();
  web.loop();

  moisture.loop();
  face.loop(moisture);

  // Webhook events on state transitions
  FaceRenderer::Mood m = face.mood();
  bool dead = face.isDeadMode();

  if (dead && !lastDead) {
    webhook.send(EVT_DEAD, moisture, dead, bootMs);
  } else if (!dead && lastDead) {
    webhook.send(EVT_OK, moisture, dead, bootMs);
  } else if (!dead && m != lastMood) {
    webhook.send(moodToEvent(m), moisture, dead, bootMs);
  }

  lastMood = m;
  lastDead = dead;
}
