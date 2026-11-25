# ESP32-S3-WROOM-1-N16R8-Internet-Radio
# ESP32-S3 Internet Radio

Web radio player for the **ESP32-S3-WROOM-1 (N16R8)** with a **MAX98357A** I2S amplifier. It streams an MP3 internet radio station (default: RAI Radio 3) over WiFi using the [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) library.

## Why the S3 with PSRAM

The current ESP32-audioI2S releases (3.4.x) allocate a large input buffer (about 640 KB) for web streams and expect PSRAM. On a plain ESP32 without PSRAM (e.g. ESP32-WROOM-32 / `ESP32-D0WD-V3`), the allocation fails with `PSRAM not found` / `OOM`, and the stream is unusable. The N16R8 module has 8 MB of octal PSRAM, so the library works as designed and high-bitrate streams (256 to 320 kbps) are possible.

## Hardware

- ESP32-S3-WROOM-1 board, variant **N16R8** (16 MB flash, 8 MB octal PSRAM)
- MAX98357A I2S amplifier breakout
- 4 to 8 ohm speaker
- USB cable that carries data

### Wiring

| MAX98357A | ESP32-S3 |
|-----------|----------|
| BCLK      | GPIO5    |
| LRC       | GPIO6    |
| DIN       | GPIO7    |
| VIN       | 5V       |
| GND       | GND      |
| SD        | not connected, or tied to VIN (if tied to GND the amp is muted) |

Speaker between the amp's `+` and `-` outputs (not to GND).

**Avoid GPIO26 to 37** on this module: they are used by the flash and octal PSRAM. GPIO22 to 25 do not exist on the S3.

## Software setup

1. Install the **esp32** board package in Arduino IDE.
2. Install **ESP32-audioI2S** from the [GitHub releases page](https://github.com/schreibfaul1/ESP32-audioI2S/releases). Make sure no other copy of the library (for example an old 3.2.x folder) remains in `Arduino/libraries/`.
3. Create `secrets.hpp` next to the `.ino` file:

   ```cpp
   const char* ssid     = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```

4. Keep **one `.ino` file per sketch folder**, and name the file like the folder. Arduino IDE merges all `.ino` files in a folder, which causes "redefinition" errors.

### Arduino IDE settings (Tools menu)

| Setting | Value |
|---------|-------|
| Board | ESP32S3 Dev Module |
| PSRAM | **OPI PSRAM** |
| Flash Size | 16MB (128Mb) |
| Partition Scheme | one with a ~3 MB app area (e.g. "16M Flash (3MB APP/9.9MB FATFS)" or "Huge APP") |
| USB CDC On Boot | Enabled if using the native USB port, Disabled if using the UART port |

The wrong PSRAM mode can stop the board from booting. A too-small partition gives "Sketch too big" (the radio sketch is about 1.9 MB).


## Troubleshooting

| Symptom | Likely cause and fix |
|---------|----------------------|
| `PSRAM not found`, `OOM: failed to allocate ... AudioBuffer` | Board without PSRAM, or PSRAM not enabled. Use the N16R8 and set PSRAM to **OPI PSRAM**. |
| `gpio_func_sel(...): GPIO number error`, boot loop (`TG1WDT_SYS_RST`) | Pin numbers from the plain ESP32 (25/26/27) still in the sketch. Use GPIO5/6/7. |
| `redefinition of ...` at compile time | Two `.ino` files in the same folder. Give each sketch its own folder. |
| `Sketch too big` / `text section exceeds available space` | Partition scheme too small. Choose one with a ~3 MB app area and set Flash Size to 16MB. |
| Upload fails with `Invalid head of packet (0x1B)` | Hold BOOT, tap RESET, release BOOT, then upload. Check the cable and close the Serial Monitor. Try the other USB port (UART vs native). |
| No log output after "WiFi connected" | The callback is not registered. Make sure `Audio::audio_info_callback = my_audio_info;` is not commented out. |
| Log looks healthy (`stream ready`, bitrate shown) but no sound | Hardware path: check amp power and GND, the SD pin (not tied to GND), the BCLK/LRC/DIN wiring and the speaker connection. |
| `slow stream, dropouts are possible` | Weak WiFi or too small a buffer. Move closer to the router, keep `WiFi.setSleep(false)`, or use a lower-bitrate stream. |

## Notes

- The library's own documentation lists the S3 as a supported target; the plain ESP32 is supported only to a limited extent.
- Library updates can change the API. If a new release breaks compilation, check the examples shipped with the library.

## Credits

- [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) by schreibfaul1 (GPL-3.0)
- Based on the ESP32-Based-Internet-Radio-using-MAX98357A-I2S-Amplifier-Board project
