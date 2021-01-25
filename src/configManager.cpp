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

  Serial.println("loading registered");
  config.registered = doc["registered"].isNull() ? RuntimeConfig::defaults.registered : doc["registered"];
  Serial.println("loading doc");
  config.name = doc["name"].isNull() ? RuntimeConfig::defaults.name : doc["name"];
  Serial.println("loading backendServerPath");
  config.backendServerPath = doc["backendServerPath"].isNull() ? RuntimeConfig::defaults.backendServerPath : doc["backendServerPath"];
  Serial.println("loading backendServerPort");
  long backendServerPort;
  Serial.println("11111");
  delay(100);
  Serial.println(doc["backendServerPort"].as<String>());
  char charBuff[50];
  doc["backendServerPort"].as<String>().toCharArray(charBuff, 50);
  backendServerPort = strtol(charBuff, nullptr, 10);
  Serial.println("222222");
  config.backendServerPort = doc["backendServerPort"].isNull() ? RuntimeConfig::defaults.backendServerPort : backendServerPort;

  Serial.println("loading DB issued id");
  config.id = doc["id"].isNull() ? RuntimeConfig::defaults.id : doc["id"]; // if you don't use the defaults, everything crashes later down the line when advertising the service on mdns. TODO: handle this more beautifully
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
  doc["id"] = config.id;
  Serial.println("name  " + doc["name"].as<String>());
  Serial.println("registered  " + doc["registered"].as<String>());
  Serial.println("backendServerPath  " + doc["backendServerPath"].as<String>());
  Serial.println("backendServerPort  " + doc["backendServerPort"].as<String>());
  Serial.println("id  " + doc["id"].as<String>());

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
  Serial.println("Removing config file...");
  if (SPIFFS.exists(ServerConfig::filePath)) {
    SPIFFS.remove(ServerConfig::filePath);
    delay(500);
    Serial.println("Removed config file.");
  } else {
    Serial.println("No config file stored. Continuing...");
  }

}