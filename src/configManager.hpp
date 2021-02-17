//
// Created by vantudor on 25.09.2020.
//

#ifndef IR_REMOTE_EMULATOR_NODEMCU_CONFIGMANAGER_H
#define IR_REMOTE_EMULATOR_NODEMCU_CONFIGMANAGER_H


#include <ArduinoJson.h>
#include <FS.h>
#include "utils.hpp"
#include <stdlib.h>

class ServerConfig {
  public:
    static const char* filePath;
    static const char* apPassword;
};

struct RuntimeConfigDefaults {
  const char* name = "no name yet";
  const bool registered = false;
  const char* backendServerPath = ""; // TODO: find a better name
  const long backendHTTPPort = 3001; // TODO: find a better name
  const long backendWSPort = 3002; // TODO: find a better name
  const bool shouldRecord = false;
  const char* id = "no id yet";
};

//extern Defaults serverDefaults;

class RuntimeConfig {
  public:
    static const RuntimeConfigDefaults defaults;
    bool registered;
    char name[40];
  char backendServerPath[128]; // TODO: find a better name
  long backendHTTPPort; // TODO: find a better name
  long backendWSPort; // TODO: find a better name
  bool shouldRecord;
  char id[40]; // RCE's DB id, issued at registration
};

void saveConfig(RuntimeConfig &config);
void loadConfig(RuntimeConfig &config);
void deleteConfigFile();
#endif //IR_REMOTE_EMULATOR_NODEMCU_CONFIGMANAGER_H