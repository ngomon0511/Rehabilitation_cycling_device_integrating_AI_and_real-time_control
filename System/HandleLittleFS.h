#ifndef HANDLE_LITTLEFS_H
#define HANDLE_LITTLEFS_H

#include <LittleFS.h>
#include <vector>

typedef struct {
  String lessonName;
  String mode;
  String level;
  String time;
} Lesson;

typedef struct {
  String id;
  String name;
  String gender;
  String age;
  String hospital;
} Infor;

class HandleLittleFS {
public:
  HandleLittleFS();
  void begin();
  
  void getUserSpace();
  uint8_t addUser(const String& userId, const String& name, const String& gender, const String& age, const String& hospital);
  bool delUser(const String& userId);
  bool addLesson(const String& userId, const String& dateName, const String& mode, const String& level, const String& time);
  std::vector<String> getUserList();
  std::vector<String> getDateList(const String& userId);
  size_t getFolderSize(const String& path);
  float getUsedPer();
  void printUsed();
  Infor getInfor(const String& userId, bool& success);
  std::vector<Lesson> getLessonList(String userId, String dateName);

private:
  size_t userSpaceSize = 0;
  bool delRecursive(const String& path);
};

#endif
