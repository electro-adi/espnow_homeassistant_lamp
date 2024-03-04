
unsigned long ota_progress_millis = 0;

void onOTAStart() {
  for (int i = 0; i != NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
    FastLED.show();
  }
  delay(500);
}

void onOTAProgress(size_t current, size_t final) {
  if (millis() - ota_progress_millis > 1000) {

    for (int i = 0; i != NUM_LEDS; i++) {
      leds[i] = CRGB::Green;
      FastLED.show();
    }

    delay(700);

    for (int i = 0; i != NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
      FastLED.show();
    }
    delay(700);
  }
}

void onOTAEnd(bool success) {
  if (success) {

    for (int i = 0; i != NUM_LEDS; i++) {
      leds[i] = CRGB::Green;
      FastLED.show();
    }
  } else {
    for (int i = 0; i != NUM_LEDS; i++) {
      leds[i] = CRGB::Red;
      FastLED.show();
    }
  }
}

void setupOTA(const char* ssid, const char* password) 
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }

  server.on("/", []() {
    server.send(200, "text/plain", "Hi! This is PORCH LIGHT 4.");
  });

  ElegantOTA.begin(&server);
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
}