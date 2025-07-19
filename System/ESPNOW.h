#ifndef ESPNOW_H
#define ESPNOW_H

#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 2

class ESPNOW {
public:
  ESPNOW();
  void sendRequest();

  static uint8_t slaveMac[];
  static bool receivedFlag;
  bool ledState = 0;
  static float receivedVal;

private:
  void startESPNow();
  static void onDataRecv(const uint8_t *mac, const uint8_t *data, int len);
  static void offESPNow();
};

#endif
