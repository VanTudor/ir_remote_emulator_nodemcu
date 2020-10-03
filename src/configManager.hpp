//
// Created by vantudor on 25.09.2020.
//

//#ifndef IR_REMOTE_EMULATOR_NODEMCU_CONFIGMANAGER_H
//#define IR_REMOTE_EMULATOR_NODEMCU_CONFIGCONFIGMANAGER_H
//
//#endif //IR_REMOTE_EMULATOR_NODEMCU_CONFIGCONFIGMANAGER_H

#include <ArduinoJson.h>
#include <FS.h>
#include "utils.hpp"

class ServerConfig {
  public:
    static const char* filePath;
    static const char* apPassword;
};

struct RuntimeConfigDefaults {
  const char* name = "no name yet";
  const bool registered = false;
};

//extern Defaults serverDefaults;

class RuntimeConfig {
  public:
    static const RuntimeConfigDefaults defaults;
    bool registered;
    const char* name;
};

void saveConfig(RuntimeConfig &config);
void loadConfig(RuntimeConfig &config);
void deleteConfigFile();