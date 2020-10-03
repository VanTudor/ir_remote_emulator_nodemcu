//
// Created by vantudor on 25.09.2020.
//
#include "configManager.hpp"

const char* ServerConfig::filePath = "beconfig.json";
const char* ServerConfig::apPassword = "iriririr";
const RuntimeConfigDefaults RuntimeConfig::defaults;

void loadConfig(RuntimeConfig &config) {
  Serial.println();
  Serial.println("Loading runtime config from disk.");
  DynamicJsonDocument doc(1024);
  doc.clear();

  if (SPIFFS.exists(ServerConfig::filePath)) {
    File configFile = SPIFFS.open(ServerConfig::filePath, "r");
    delay(1000); // Don't remove this. Apparently SPIFFS.open is async-ish. Didn't look it up thoroughly

    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      Serial.println();
      Serial.println("Failed reading from config file. Fail reason: ");
      Serial.print(error.c_str());
      Serial.println("Using default configuration");
    }
    configFile.close();
  } else {
    Serial.println("Config file not found. Using default configuration.");
  }

  config.registered = doc["registered"].isNull() ? RuntimeConfig::defaults.registered : doc["registered"];
  config.name = doc["name"].isNull() ? RuntimeConfig::defaults.name : doc["name"];

}

void saveConfig(RuntimeConfig &config) {
  Serial.println();
  Serial.println("----- In saveConfig -----");
  File configFile = SPIFFS.open(ServerConfig::filePath, "w");
  delay(1000); // Don't remove this. Apparently SPIFFS.open is async-ish. Didn't look it up thoroughly tho.
  Serial.println("File exists: " + (String)SPIFFS.exists(ServerConfig::filePath));
  if (configFile) {
    Serial.println("Opened file.");
  } else {
    Serial.println("Failed loading file. Exiting...");
    return;
  }

  DynamicJsonDocument doc(1024);
  doc.clear();
  Serial.println("IS DOC NULL: "+ String(doc.isNull()));

  doc["registered"] = config.registered;
  doc["name"] = config.name;
  Serial.println("name  "+ doc["name"].as<String>());
  Serial.println("registered  "+ doc["registered"].as<String>());
  Serial.println("Added data to json.");
  String serializedJSON;
  serializeJson(doc, serializedJSON);
  Serial.println("JSON to be written: " + serializedJSON);

  if (configFile.print(serializedJSON) > 0) { // if bytes written > 0
    Serial.println("Saved serialized json.");
  } else {
    Serial.println();
    Serial.println("Failed writing to config file.");
  }

  Serial.println("Closing file...");
  configFile.close();
}

void deleteConfigFile() {
  Serial.println("Removing config file.");
  SPIFFS.remove(ServerConfig::filePath);
  delay(500);
  Serial.println("Config file removed.");
}