#include "ESPNOW.h"

uint8_t ESPNOW::slaveMac[] = { 0x98, 0x88, 0xE0, 0xC9, 0x97, 0xB8 };
bool ESPNOW::receivedFlag = false;
float ESPNOW::receivedVal;

ESPNOW::ESPNOW() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
}

void ESPNOW::startESPNow() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);

  esp_now_del_peer(slaveMac);
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, slaveMac, 6);
  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void ESPNOW::offESPNow() {
  esp_now_deinit();
}

void ESPNOW::onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  memcpy(&receivedVal, data, sizeof(receivedVal));
  receivedFlag = true;
  offESPNow();
}

void ESPNOW::sendRequest() {
  startESPNow();
  esp_err_t result = esp_now_send(slaveMac, (uint8_t *)"r", 1);
  if (result != ESP_OK) {
    Serial.printf("Send failed: %d\n", result);
  }
}
