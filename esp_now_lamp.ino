
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Arduino.h>
#include <FastLED.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>

#define DATA_PIN 1
#define pir_sensor 3
#define relay 2

#define NUM_LEDS 8

CRGB leds[NUM_LEDS];
ESP8266WebServer server(80);

#include "effects.h"
#include "OTA.h"

typedef struct struct_message {

  bool OTA1;
  bool OTA2;
  bool OTA3;
  bool OTA4;

  int EFFECT; 

  int LIGHT; //send 0 = 0ff, 1 = auto, 2 = on

} struct_message;

struct_message myData;
struct_message incomingReadings;

bool OTA_LOCAL = false;
int EFFECT_LOCAL = 0;
int LIGHT_LOCAL = 0;

bool ota_setup_done = false;
bool motion = false;
unsigned long time_triggered;

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

  OTA_LOCAL = incomingReadings.OTA4;
  EFFECT_LOCAL = incomingReadings.EFFECT;
  LIGHT_LOCAL = incomingReadings.LIGHT;
}

void setup() {
  pinMode(pir_sensor, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);

  for (int x = 0; x != 5; x++) {
    for (int i = 0; i != NUM_LEDS; i++) {
      leds[i] = CRGB::Green;
      FastLED.show();
    }

    delay(200);

    for (int i = 0; i != NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
      FastLED.show();
    }

    delay(200);
  }

  //Fill the colorIndex array with random numbers
  for (int i = 0; i < NUM_LEDS; i++) {
    colorIndex[i] = random8();
  }
  pinMode(relay, OUTPUT);
}

void motion_functions() {

  if (digitalRead(pir_sensor) == HIGH) 
  {
    motion = true;
    time_triggered = millis();
    digitalWrite(relay, HIGH);

  } 
  else 
  {
    motion = false;
  }

  if (!motion && millis() - time_triggered > 5000)
  {
    digitalWrite(relay, LOW);
  }
}

void off() {

  for (int i = 0; i != NUM_LEDS; i++) 
  {
    leds[i] = CRGB::Black;
    FastLED.show();
  }
}

void fast_ball() {

  for (int i = 0; i != NUM_LEDS; i++) 
  {
    leds[i].setRGB(random(256),random(256), random(256));

    FastLED.show();

    delay(200);

    FastLED.clear();
    FastLED.show();

    delay(20);
  }
}

void slow_ball() {

}

void fast_ball_random() {

}

void loop() {
  if (OTA_LOCAL) {
    if (ota_setup_done == false) {
      esp_now_unregister_recv_cb();
      esp_now_deinit();
      WiFi.disconnect(true, false);
      delay(1000);
      setupOTA("ssid", "passwd");
      ota_setup_done = true;
    }
    server.handleClient();
    ElegantOTA.loop();
  } 
  else 
  {
    if (LIGHT_LOCAL == 0) {
      digitalWrite(relay, LOW);
    } 
    else if (LIGHT_LOCAL == 1) {
      motion_functions();
    } 
    else if (LIGHT_LOCAL == 2) {
      digitalWrite(relay, HIGH);
    }

    if (EFFECT_LOCAL == 0) {
      off();
    } else if (EFFECT_LOCAL == 1) {
      fast_ball();
    } else if (EFFECT_LOCAL == 2) {
      slow_ball();
    } else if (EFFECT_LOCAL == 3) {
      fast_ball_random();
    } else if (EFFECT_LOCAL == 4) {
      rainbow();
    } else if (EFFECT_LOCAL == 5) {
      rainbowWithGlitter();
    } else if (EFFECT_LOCAL == 6) {
      confetti();
    } else if (EFFECT_LOCAL == 7) {
      sinelon();
    } else if (EFFECT_LOCAL == 8) {
      bpm();
    } else if (EFFECT_LOCAL == 9) {
      juggle();
    } else if (EFFECT_LOCAL == 10) {
      lightning_flashes();
    } else if (EFFECT_LOCAL == 11) {
      fade_toward_solid();
    } else if (EFFECT_LOCAL == 12) {
      chase_target();
    } else if (EFFECT_LOCAL == 13) {
      breath_effect();
    } else if (EFFECT_LOCAL == 14) {
      blocks();
    } else if (EFFECT_LOCAL == 15) {
      blend_rainbow();
    } else if (EFFECT_LOCAL == 16) {
      phase_beat();
    } else if (EFFECT_LOCAL == 17) {
      moving_dot();
    } else if (EFFECT_LOCAL == 18) {
      gradient_beat();
    } else if (EFFECT_LOCAL == 19) {
      brightness_wave();
    } else if (EFFECT_LOCAL == 20) {
      adding_waves();
    }
  }
}