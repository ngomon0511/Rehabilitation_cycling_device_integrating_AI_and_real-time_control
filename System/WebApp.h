#ifndef WEB_APP_H
#define WEB_APP_H

#include <WiFi.h>
#include <DNSServer.h>
#include "CountTime.h"
#include "MyFs.h"

class WebApp {
public:
  String userID;
  String today = "";
  bool login = false;

  WebApp();
  void start();
  void stop();
  void begin();
  void handleClient();

private:
  static const byte DNS_PORT = 53;
  const char* ssid = "ESP32-Rehab-Bike";
  const char* password = "mon21134NT";
  int max_connection = 1;

  DNSServer dnsServer;
  WiFiServer server = WiFiServer(80);

  void handleRequest(const String& header, WiFiClient& client);
  void serveFile(WiFiClient& client, const char* path, const char* contentType);
  String urlDecode(const String& str);
  void addUserWeb(WiFiClient& client, const String& header);
  void checkIdWeb(WiFiClient& client, const String& header);
  void checkPassWeb(WiFiClient& client, const String& header);
  void getUsersWeb(WiFiClient& client);
  void getUserInfoWeb(WiFiClient& client, const String& header);
  void getUserHisWeb(WiFiClient& client, const String& header);
  void getDayLessonWeb(WiFiClient& client, const String& header);
  void delUserWeb(WiFiClient& client, const String& header);
  void getExportDataWeb(WiFiClient& client, const String& header);
};

#endif
