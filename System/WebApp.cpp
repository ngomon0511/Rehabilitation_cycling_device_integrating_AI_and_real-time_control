#include "WebApp.h"

WebApp::WebApp() {}

void WebApp::begin() {
  MyFs.begin();
}

void WebApp::start() {
  WiFi.softAP(ssid, password, 1, 0, max_connection);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  server.begin();
}

void WebApp::stop() {
  dnsServer.stop();
  server.stop();
  WiFi.mode(WIFI_STA);
}

void WebApp::handleClient() {
  dnsServer.processNextRequest();

  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("New Client.");
  String header, currentLine;

  uint64_t startTime = CountTime::read();     
  const uint64_t timeoutUs = 5000000;  

  while (client.connected()) {
    if (CountTime::read() - startTime > timeoutUs) {
      Serial.println("Client timeout.");
      break;
    }

    if (!client.available()) continue;

    char c = client.read();
    header += c;
    startTime = CountTime::read(); 

    if (c == '\n') {
      if (currentLine.length() == 0) {
        handleRequest(header, client);
        break;
      } else {
        currentLine = "";
      }
    } else if (c != '\r') {
      currentLine += c;
    }
  }

  client.stop();
  Serial.println("Client disconnected.");
}

void WebApp::handleRequest(const String& header, WiFiClient& client) {
  if (header.startsWith("GET / ")) {
    serveFile(client, "/index.html", "text/html");

  } else if (header.startsWith("GET /style.css")) {
    serveFile(client, "/style.css", "text/css");

  } else if (header.startsWith("GET /script.js")) {
    serveFile(client, "/script.js", "application/javascript");

  } else if (header.startsWith("GET /regis")) {
    addUserWeb(client, header);

  } else if (header.startsWith("GET /checkId")) {
    checkIdWeb(client, header);

  } else if (header.startsWith("GET /checkPass")) {
    checkPassWeb(client, header);

  } else if (header.startsWith("GET /getUsers")) {
    getUsersWeb(client);

  } else if (header.startsWith("GET /getUserInfo")) {
    getUserInfoWeb(client, header);

  } else if (header.startsWith("GET /getUserHis")) {
    getUserHisWeb(client, header);

  } else if (header.startsWith("GET /getDayLesson")) {
    getDayLessonWeb(client, header);

  } else if (header.startsWith("GET /delUser")) {
    delUserWeb(client, header);

  } else if (header.startsWith("GET /getExportData")) {
    getExportDataWeb(client, header);

  } else {
    client.println("HTTP/1.1 302 Found");
    client.print("Location: http://");
    client.print(WiFi.softAPIP());
    client.println("/");
  }
}

void WebApp::serveFile(WiFiClient& client, const char* path, const char* contentType) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    client.println("HTTP/1.1 500 Internal Server Error");
    client.println("Content-Type: text/plain");
    client.println("Connection: close\n");
    client.println("File not found");
    return;
  }

  client.println("HTTP/1.1 200 OK");
  client.print("Content-Type: ");
  client.println(contentType);
  client.println("Connection: close\n");

  while (file.available()) {
    client.write(file.read());
  }

  file.close();
}

String WebApp::urlDecode(const String& str) {
  String decoded = "";
  char c;
  for (int i = 0; i < str.length(); i++) {
    c = str[i];
    if (c == '+') {
      decoded += ' ';
    } else if (c == '%' && i + 2 < str.length()) {
      String hex = str.substring(i + 1, i + 3);
      decoded += (char)strtol(hex.c_str(), nullptr, 16);
      i += 2;
    } else {
      decoded += c;
    }
  }
  return decoded;
}

void WebApp::addUserWeb(WiFiClient& client, const String& header) {
  int qIndex = header.indexOf("/regis?");
  if (qIndex == -1) {
    client.println("HTTP/1.1 400 Bad Request");
    client.println("Content-Type: application/json");
    client.println("Connection: close\n");
    client.println("{\"status\":\"invalid\"}");
    return;
  }

  int httpIndex = header.indexOf("HTTP/1.1", qIndex);
  String paramString = header.substring(qIndex + 7, httpIndex - 1);

  String id, name, gender, age, hospital;
  while (paramString.length()) {
    int eq = paramString.indexOf('=');
    int andIdx = paramString.indexOf('&');

    if (eq == -1) break;

    String key = paramString.substring(0, eq);
    String value;
    if (andIdx == -1) {
      value = paramString.substring(eq + 1);
      paramString = "";
    } else {
      value = paramString.substring(eq + 1, andIdx);
      paramString = paramString.substring(andIdx + 1);
    }

    value = urlDecode(value);
    if (key == "id") id = value;
    else if (key == "name") name = value;
    else if (key == "gender") gender = value;
    else if (key == "age") age = value;
    else if (key == "hospital") hospital = value;
  }

  if (id == "" || name == "" || gender == "" || age == "" || hospital == "") {
    client.println("HTTP/1.1 400 Bad Request");
    client.println("Content-Type: application/json");
    client.println("Connection: close\n");
    client.println("{\"status\":\"invalid\"}");
    return;
  }

  uint8_t result = MyFs.addUser(id, name, gender, age, hospital);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close\n");

  if (result == 1) {
    client.print("{\"status\":\"ok\"}");
  } else if (result == 0) {
    client.print("{\"status\":\"error\"}");
  } else {
    client.print("{\"status\":\"exists\"}");
  }
}

void WebApp::checkIdWeb(WiFiClient& client, const String& header) {
  int idx = header.indexOf("id=");
  userID = urlDecode(header.substring(idx + 3, header.indexOf("&", idx)));

  int dateIdx = header.indexOf("date=");
  String date = "";
  if (dateIdx != -1) {
    date = urlDecode(header.substring(dateIdx + 5, header.indexOf(" ", dateIdx)));
    Serial.printf("Client login on date: %s\n", date.c_str());
    today = date;
  } else {
    Serial.printf("Client login (no date sent) !\n");
  }

  bool exists = false;
  for (String uid : MyFs.getUserList()) {
    if (uid == userID) {
      exists = true;
      break;
    }
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close\n");
  client.print("{\"exists\": ");
  client.print(exists ? "true" : "false");
  client.println("}");

  login = exists;
}

void WebApp::checkPassWeb(WiFiClient& client, const String& header) {
  int idx = header.indexOf("pass=");
  String pass = urlDecode(header.substring(idx + 5, header.indexOf(" ", idx)));

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close\n");
  client.print("{\"fit\": ");
  client.print((pass == "monAdmin") ? "true" : "false");
  client.println("}");
}

void WebApp::getUsersWeb(WiFiClient& client) {
  std::vector<String> users = MyFs.getUserList();
  float percent = MyFs.getUsedPer();

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close\n");

  client.print("{\"users\":[");
  for (size_t i = 0; i < users.size(); i++) {
    client.print("{\"id\":\"" + users[i] + "\"}");
    if (i != users.size() - 1) client.print(",");
  }

  client.printf("],\"total_percent\":%.2f}", percent);
}

void WebApp::getUserInfoWeb(WiFiClient& client, const String& header) {
  int idx = header.indexOf("id=");
  String id = urlDecode(header.substring(idx + 3, header.indexOf(" ", idx)));

  bool ok = false;
  Infor info = MyFs.getInfor(id, ok);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close\n");

  if (!ok) {
    client.println("{\"error\":\"not found\"}");
  } else {
    client.printf("{\"id\":\"%s\",\"name\":\"%s\",\"gender\":\"%s\",\"age\":\"%s\",\"hospital\":\"%s\"}\n",
                  info.id.c_str(), info.name.c_str(), info.gender.c_str(), info.age.c_str(), info.hospital.c_str());
  }
}

void WebApp::getUserHisWeb(WiFiClient& client, const String& header) {
  int idx = header.indexOf("id=");
  String userId = urlDecode(header.substring(idx + 3, header.indexOf(" ", idx)));

  std::vector<String> dates = MyFs.getDateList(userId);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close\n");

  client.print("{\"history\":[");
  for (size_t i = 0; i < dates.size(); i++) {
    client.print("\"" + dates[i] + "\"");
    if (i != dates.size() - 1) client.print(",");
  }
  client.println("]}");
}

void WebApp::getDayLessonWeb(WiFiClient& client, const String& header) {
  int idIdx = header.indexOf("id=");
  int dateIdx = header.indexOf("date=");

  String userId = urlDecode(header.substring(idIdx + 3, header.indexOf("&", idIdx)));
  String date = urlDecode(header.substring(dateIdx + 5, header.indexOf(" ", dateIdx)));

  std::vector<Lesson> lessons = MyFs.getLessonList(userId, date);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close\n");

  client.print("[");
  for (size_t i = 0; i < lessons.size(); i++) {
    client.printf("{\"no\":\"%s\",\"mode\":\"%s\",\"level\":\"%s\",\"time\":\"%s\"}",
                  lessons[i].lessonName.c_str(),
                  lessons[i].mode.c_str(),
                  lessons[i].level.length() > 0 ? lessons[i].level.c_str() : "null",
                  lessons[i].time.c_str());
    if (i != lessons.size() - 1) client.print(",");
  }
  client.println("]");
}

void WebApp::delUserWeb(WiFiClient& client, const String& header) {
  int idx = header.indexOf("id=");
  String userId = urlDecode(header.substring(idx + 3, header.indexOf(" ", idx)));

  bool success = MyFs.delUser(userId);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close\n");

  client.printf("{\"del\": %d}\n", success ? 1 : 0);
}

void WebApp::getExportDataWeb(WiFiClient& client, const String& header) {
  int idx = header.indexOf("id=");
  String userId = urlDecode(header.substring(idx + 3, header.indexOf(" ", idx)));

  std::vector<String> dates = MyFs.getDateList(userId);
  std::vector<Lesson> allLessons;

  for (const String& date : dates) {
    std::vector<Lesson> lessons = MyFs.getLessonList(userId, date);
    allLessons.insert(allLessons.end(), lessons.begin(), lessons.end());
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close\n");

  client.print("[");
  for (size_t i = 0; i < allLessons.size(); i++) {
    client.printf("{\"no\":\"%s\",\"mode\":\"%s\",\"level\":\"%s\",\"time\":\"%s\"}",
                  allLessons[i].lessonName.c_str(),
                  allLessons[i].mode.c_str(),
                  allLessons[i].level.length() > 0 ? allLessons[i].level.c_str() : "null",
                  allLessons[i].time.c_str());
    if (i != allLessons.size() - 1) client.print(",");
  }
  client.println("]");
}
