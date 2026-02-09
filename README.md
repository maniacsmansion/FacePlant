# FacePlant (v1.0.0)

ESP32 + SH1106 OLED + soil moisture buddy with expressions.

## Features
- Happy / Dry (frown + droplet) / Too-wet (bubbles)
- Random blinks (>= 1/min) + silly faces (more frequent in Kids Mode)
- 72-hour "RIP" mode when dry (disabled in Kids Mode)
- Captive-portal setup AP, then joins home Wi-Fi
- mDNS hostname: http://faceplant.local
- Web UI at `/` + JSON at `/status`
- OTA update at `/update`
- Webhook (optional) posts JSON on events

## Wiring
- OLED (I2C): SDA=GPIO21, SCL=GPIO22, VCC=3.3V, GND=GND
- Moisture sensor analog out: GPIO34
