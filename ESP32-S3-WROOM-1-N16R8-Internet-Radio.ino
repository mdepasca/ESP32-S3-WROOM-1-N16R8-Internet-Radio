#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"
#include "secrets.hpp"

// Digital I/O used (ESP32-S3-WROOM-1 N16R8: avoid GPIO26-37)
#define I2S_BCLK 5
#define I2S_LRC  6
#define I2S_DOUT 7

const char* stream_url = "http://icestreaming.rai.it/3.mp3";
const uint8_t start_volume = 10;  // 0...21

Audio audio;

// detailed callback output (from the library README)
void my_audio_info(Audio::msg_t m) {
  switch (m.e) {
    case Audio::evt_info:        Serial.printf("info: ....... %s\n", m.msg); break;
    case Audio::evt_eof:         Serial.printf("end of file:  %s\n", m.msg); break;
    case Audio::evt_bitrate:     Serial.printf("bitrate: .... %s\n", m.msg); break;
    case Audio::evt_lasthost:    Serial.printf("last URL: ... %s\n", m.msg); break;
    case Audio::evt_name:        Serial.printf("station name: %s\n", m.msg); break;
    case Audio::evt_streamtitle: Serial.printf("stream title: %s\n", m.msg); break;
    case Audio::evt_log:         Serial.printf("audio_logs:   %s\n", m.msg); break;
    default:                     Serial.printf("message:..... %s\n", m.msg); break;
  }
}

void setup() {
    // Turn off the onboard RGB LED
#ifdef RGB_BUILTIN
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
#else
  neopixelWrite(48, 0, 0, 0);  // try 38 if your board revision uses that pin
#endif

  Audio::audio_info_callback = my_audio_info;  // register before use
  Serial.begin(115200);
  delay(500);

  Serial.printf("PSRAM: %s, size %u bytes\n",
                psramFound() ? "found" : "NOT found", ESP.getPsramSize());

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);  // avoids power-save latency while streaming
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi connection failed!");
    while (1) delay(1000);
  }
  Serial.println("\nWiFi connected");

  // Audio
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(start_volume);
  audio.connecttohost(stream_url);
}

void loop() {
  audio.loop();
  vTaskDelay(1);
}