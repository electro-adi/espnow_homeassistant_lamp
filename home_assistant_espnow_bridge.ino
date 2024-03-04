//----------------------------------------------------------------------------------------------------------------------------------------------




// PORCH LIGHT BRIDGE




//----------------------------------------------------------------------------------------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
#include <ArduinoHA.h>

#define WIFI_SSID       "ssid"
#define WIFI_PASSWORD   "passwd"

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);
ESP8266WebServer server(80);

HAButton ota("OTA");
HASelect warmwhite("WarmWhite");
HASelect effects("Effects");

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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
bool ota_setup_done = false;
bool need_to_send = false;

void onSelectCommand1(int8_t index, HASelect* sender)
{
  need_to_send = true;
  switch (index) 
  {
    case 0:
      myData.LIGHT = 0;
      break;
    case 1:
      myData.LIGHT = 1;
      break;
    case 2:
      myData.LIGHT = 2;
      break;
  }
  sender->setState(index);
}

void onSelectCommand2(int8_t index, HASelect* sender)
{
  need_to_send = true;
  myData.EFFECT = index;
  sender->setState(index);
}

void onButtonCommand(HAButton* sender)
{
  OTA_LOCAL = true;
}

void setup_mqtt()
{
  byte mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  device.setUniqueId(mac, sizeof(mac));

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
  }

  device.setName("PorchLightsBridge");

  ota.setName("OTA");
  ota.onCommand(onButtonCommand);

  warmwhite.setOptions("OFF;AUTO;ON");
  warmwhite.onCommand(onSelectCommand1);
  warmwhite.setName("Warm White");

  effects.setOptions("off;fast_ball;slow_ball;fast_ball_random;rainbow;rainbowWithGlitter;confetti;sinelon;bpm;juggle;lightning_flashes;fade_toward_solid;chase_target;breath_effect;blocks;blend_rainbow;phase_beat;moving_dot;gradient_beat;brightness_wave;adding_waves");
  effects.onCommand(onSelectCommand2);
  effects.setName("Effects");


  mqtt.begin("192.168.29.141", "homeassistant", "passwordofurhmqttbrokerinstance");
}

void setup() {
  Serial.begin(9600);
  setup_mqtt();
}

void loop() {

  if(need_to_send && !OTA_LOCAL)
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_now_init();
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 0, NULL, 0);
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    setup_mqtt();
    need_to_send = false;
  }
  else
  {
    if(OTA_LOCAL)
    {
      if (ota_setup_done == false) 
      {
        esp_now_del_peer(broadcastAddress);
        esp_now_deinit();
        WiFi.disconnect(true, false);
        delay(1000);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        while (WiFi.status() != WL_CONNECTED) 
        {
          delay(500);
        }
        server.on("/", []() {server.send(200, "text/plain", "Hi! This is PORCH LIGHT MASTER.");});
        ElegantOTA.begin(&server);
        server.begin();
        ota_setup_done = true;
      }
      server.handleClient();
      ElegantOTA.loop();
    }
  }
  if(!OTA_LOCAL) mqtt.loop();
}