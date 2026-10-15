# E Line Door Display (ESP32 variant)

Tiny door-side display showing northbound METRO E Line departures in Minneapolis, on the same 2× 64×32 P2.5 HUB75 panels (128×32) as the Pi build. ESP32 instead of Pi: cheaper, boots instantly, polls the transit API directly.

## Stop

- **Hennepin & Spruce Station**, stop ID **17941** — Hennepin Ave S at Spruce Place, in front of Minneapolis College (1415 Hennepin Ave S)
- Southbound uses Laurel Ave across the street ("Spruce/Laurel" station name)
- Filter: E Line only (`route_id == "925"`, `direction_id == 0`, toward Westgate)

## Data

Free, no key. Poll every 30s:

```
https://svc.metrotransit.org/nextripv2/17941
```

Per departure: `departure_text` ("Due", "5 Min", "6:15"), `description` (destination), `actual` (true = real-time).

## Display format

Two static 8px rows, e.g. `E NB  DUE` / `E NB  15m`. No scrolling needed for one route.

## Hardware

- 2× 64×32 P2.5 HUB75 panels, chained (panel 1 OUT → panel 2 IN) → 128×32
- ESP32 with HUB75 driver (generic ESP32 + matrix shield, or Matrix Portal S3)
- 5V 6–8A supply powering the panels directly
- Cost estimate: ~$55–75 total (panels ~$26–35, controller $10–25, PSU ~$12–15)

## Firmware

[e-line-door-display.ino](e-line-door-display.ino) — Arduino sketch using [ESP32-HUB75-MatrixPanel-I2S-DMA](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-I2S-DMA) + ArduinoJson. Set `WIFI_SSID`/`WIFI_PASS` and flash.
