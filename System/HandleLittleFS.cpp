#include "HandleLittleFS.h"

HandleLittleFS::HandleLittleFS() {}

void HandleLittleFS::begin() {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
  } else {
    Serial.println("LittleFS mounted successfully.");
    if (!LittleFS.exists("/user")) {
      LittleFS.mkdir("/user");
      Serial.println("Created root folder: /user");
    }

    getUserSpace();
  }
}

void HandleLittleFS::getUserSpace() {
  size_t totalFs = LittleFS.totalBytes();
  size_t staticUsed = 0;

  const char* staticFiles[] = {
    "/index.html",
    "/script.js",
    "/style.css",
    "/model.tflite"
  };

  for (const char* path : staticFiles) {
    if (LittleFS.exists(path)) {
      File file = LittleFS.open(path, "r");
      if (file) {
        staticUsed += file.size();
        file.close();
      }
    }
  }

  userSpaceSize = totalFs - staticUsed;
}

uint8_t HandleLittleFS::addUser(const String& userId, const String& name, const String& gender, const String& age, const String& hospital) {
  String userPath = "/user/" + userId;

  if (LittleFS.exists(userPath)) {
    return 2;
  }

  if (LittleFS.mkdir(userPath)) {
    Serial.println("Folder created: " + userPath);

    String dateFolder = userPath + "/date";
    if (!LittleFS.mkdir(dateFolder)) {
      Serial.println("Failed to create date folder: " + dateFolder);
      return 0;
    }
    Serial.println("Date folder created: " + dateFolder);

    String inforPath = userPath + "/infor.txt";
    File file = LittleFS.open(inforPath, "w");
    if (!file || file.isDirectory()) {
      Serial.println("Failed to create file: " + inforPath);
      return 0;
    }

    String content = userId + "\t" + name + "\t" + gender + "\t" + age + "\t" + hospital;
    file.print(content);
    file.close();
    Serial.println("File created and written: " + inforPath);

    return 1;
  }

  Serial.println("Failed to create folder: " + userPath);
  return 0;
}

bool HandleLittleFS::delUser(const String& userId) {
  return delRecursive("/user/" + userId);
}

bool HandleLittleFS::addLesson(const String& userId, const String& dateName, const String& mode, const String& level, const String& time) {
  String filePath = "/user/" + userId + "/date/" + dateName + ".txt";
  int totalLessons = 0;

  if (!LittleFS.exists(filePath)) {
    File file = LittleFS.open(filePath, "w");
    if (!file || file.isDirectory()) {
      Serial.println("Failed to create file: " + filePath);
      return false;
    }

    String content;
    if (mode == "free") {
      content = "lesson1\t" + mode + "\t\t" + time;
    } else {
      content = "lesson1\t" + mode + "\t" + level + "\t" + time;
    }

    file.print(content);
    file.close();
    Serial.println("Date created and lesson1 added: " + filePath);
    return true;
  }

  File file = LittleFS.open(filePath, FILE_READ);
  if (!file || file.isDirectory()) {
    Serial.println("Failed to read file: " + filePath);
    return false;
  }

  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.startsWith("lesson")) {
      totalLessons++;
    }
  }
  file.close();

  totalLessons++;
  String lessonName = "lesson" + String(totalLessons);

  file = LittleFS.open(filePath, "a");
  if (!file) {
    Serial.println("Failed to open file for appending: " + filePath);
    return false;
  }

  String content;
  if (mode == "free") {
    content = "\n" + lessonName + "\t" + mode + "\t\t" + time;
  } else {
    content = "\n" + lessonName + "\t" + mode + "\t" + level + "\t" + time;
  }

  file.print(content);
  file.close();
  Serial.println("Lesson added to existing date: " + filePath);
  return true;
}

bool HandleLittleFS::delRecursive(const String& path) {
  File root = LittleFS.open(path);
  if (!root || !root.isDirectory()) {
    root.close();
    return LittleFS.remove(path);
  }

  File file = root.openNextFile();
  while (file) {
    String filePath = file.path();
    if (file.isDirectory()) {
      file.close();
      delRecursive(filePath);
    } else {
      file.close();
      LittleFS.remove(filePath);
    }
    file = root.openNextFile();
  }

  root.close();
  return LittleFS.rmdir(path);
}

std::vector<String> HandleLittleFS::getUserList() {
  std::vector<String> users;
  File root = LittleFS.open("/user");
  if (!root || !root.isDirectory()) {
    return users;
  }

  File entry = root.openNextFile();
  while (entry) {
    if (entry.isDirectory()) {
      String folderName = entry.name();
      folderName.replace("/user/", "");
      users.push_back(folderName);
    }
    entry = root.openNextFile();
  }

  root.close();
  return users;
}

std::vector<String> HandleLittleFS::getDateList(const String& userId) {
  std::vector<String> dates;
  String datePath = "/user/" + userId + "/date";
  File root = LittleFS.open(datePath);
  if (!root || !root.isDirectory()) {
    return dates;
  }

  File entry = root.openNextFile();
  while (entry) {
    if (!entry.isDirectory()) {
      String fileName = entry.name();
      fileName.replace(datePath + "/", "");
      fileName = fileName.substring(0, fileName.length() - 4);
      dates.push_back(fileName);
    }
    entry = root.openNextFile();
  }

  root.close();
  return dates;
}

size_t HandleLittleFS::getFolderSize(const String& path) {
  size_t totalSize = 0;

  File root = LittleFS.open(path);
  if (!root || !root.isDirectory()) return 0;

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      totalSize += getFolderSize(file.path());
    } else {
      totalSize += file.size();
    }
    file = root.openNextFile();
  }

  return totalSize;
}

float HandleLittleFS::getUsedPer() {
  size_t used = getFolderSize("/user");
  return (float)used * 100.0f / userSpaceSize;
}

void HandleLittleFS::printUsed() {
  Serial.printf("Used data in user space: %d byte \n", getFolderSize("/user"));
  Serial.printf("Storage percent: %.2f%%\n", getUsedPer());
}

Infor HandleLittleFS::getInfor(const String& userId, bool& success) {
  Infor info;
  success = false;

  String path = "/user/" + userId + "/infor.txt";
  File file = LittleFS.open(path, "r");

  if (!file) return info;

  String line = file.readStringUntil('\n');
  file.close();
  line.trim();

  int idx1 = line.indexOf('\t');
  int idx2 = line.indexOf('\t', idx1 + 1);
  int idx3 = line.indexOf('\t', idx2 + 1);
  int idx4 = line.indexOf('\t', idx3 + 1);

  if (idx1 == -1 || idx2 == -1 || idx3 == -1 || idx4 == -1) return info;

  info.id = line.substring(0, idx1);
  info.name = line.substring(idx1 + 1, idx2);
  info.gender = line.substring(idx2 + 1, idx3);
  info.age = line.substring(idx3 + 1, idx4);
  info.hospital = line.substring(idx4 + 1);

  success = true;
  return info;
}

std::vector<Lesson> HandleLittleFS::getLessonList(String userId, String dateName) {
  std::vector<Lesson> lessons;
  String path = "/user/" + userId + "/date/" + dateName + ".txt";

  if (!LittleFS.exists(path)) {
    Serial.println("Date file not found.");
    return lessons;
  }

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open date file.");
    return lessons;
  }

  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;

    int p1 = line.indexOf('\t');
    int p2 = line.indexOf('\t', p1 + 1);
    int p3 = line.indexOf('\t', p2 + 1);

    Lesson lesson;
    lesson.lessonName = line.substring(0, p1);
    lesson.mode = line.substring(p1 + 1, p2);
    lesson.level = line.substring(p2 + 1, p3);
    lesson.time = line.substring(p3 + 1);

    lessons.push_back(lesson);
  }

  file.close();
  return lessons;
}
