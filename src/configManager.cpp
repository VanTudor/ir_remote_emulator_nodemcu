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
  StaticJsonDocument<256> doc;
  File configFile;
  bool fileExistsAndNoError = true;
  if (SPIFFS.exists(ServerConfig::filePath)) {
    configFile = SPIFFS.open(ServerConfig::filePath, "r");
    delay(1000); // Don't remove this. Apparently SPIFFS.open is async-ish. Didn't look it up thoroughly
//    Serial.println("Raw file content: ");
//    while(configFile.available()) {
//      Serial.println(configFile.read());
//    }

//    size_t size = configFile.size();
//    if (size > 1024) {
//      Serial.println("Config file size is too large");
//      return;
//    }
//     Allocate a buffer to store contents of the file.
//    std::unique_ptr<char[]> buf(new char[size]);
//
//    // We don't use String here because ArduinoJson library requires the input
//    // buffer to be mutable. If you don't use ArduinoJson, you may as well
//    // use configFile.readString instead.
//    configFile.readBytes(buf.get(), size);
//    DeserializationError error = deserializeJson(doc, buf.get());
    DeserializationError error = deserializeJson(doc, configFile);
//
//    String output;
//    serializeJson(doc, output);
//    Serial.println("Loaded json: ");
//    Serial.println(output);
    if (error) {
      Serial.println();
      Serial.println("Failed reading from config file. Fail reason: ");
      Serial.print(error.c_str());
      Serial.println("Using default configuration");
      fileExistsAndNoError = false;
    }
  } else {
    Serial.println("Config file not found. Using default configuration.");
    fileExistsAndNoError = false;
  }

  Serial.println("loading registered");
  Serial.println("registered.isNull: ");
  Serial.println(doc["registered"].isNull());
  config.registered = doc["registered"].isNull() ? RuntimeConfig::defaults.registered : doc["registered"].as<boolean>();
  Serial.println("Config.registered loaded value: ");
  Serial.println(config.registered);

  Serial.println("loading name");
  Serial.println("name.isNull: ");
  Serial.println(doc["name"].isNull());
  strlcpy(config.name, doc["name"] | RuntimeConfig::defaults.name, sizeof(config.name));
//  config.name = doc["name"].isNull() ? RuntimeConfig::defaults.name : doc["name"];
  Serial.println("Config.name loaded value: ");
  Serial.println(config.name);

  Serial.println("loading backendServerPath");
  Serial.println("backendServerPath.isNull: ");
  Serial.println(doc["backendServerPath"].isNull());
  strlcpy(config.backendServerPath, doc["backendServerPath"] | RuntimeConfig::defaults.backendServerPath, sizeof(config.backendServerPath));
//  config.backendServerPath = doc["backendServerPath"].isNull() ? RuntimeConfig::defaults.backendServerPath : doc["backendServerPath"];
  Serial.println("Config.backendServerPath loaded value: ");
  Serial.println(config.backendServerPath);

  Serial.println("loading backendHTTPPort");
  Serial.println("backendHTTPPort.isNull: ");
  Serial.println(doc["backendHTTPPort"].isNull());
  long backendHTTPPort, backendWSPort;
  Serial.println(doc["backendHTTPPort"].as<String>());
  Serial.println(doc["backendWSPort"].as<String>());
  char httpCharBuff[10], wsCharBuff[10];
  doc["backendHTTPPort"].as<String>().toCharArray(httpCharBuff, 10);
  backendHTTPPort = strtol(httpCharBuff, nullptr, 10);
  doc["backendWSPort"].as<String>().toCharArray(wsCharBuff, 10);
  backendWSPort = strtol(wsCharBuff, nullptr, 10);
  config.backendHTTPPort = doc["backendHTTPPort"].isNull() ? RuntimeConfig::defaults.backendHTTPPort : backendHTTPPort;
  config.backendWSPort = doc["backendWSPort"].isNull() ? RuntimeConfig::defaults.backendWSPort : backendWSPort;
  Serial.println("Config.backendHTTPPort loaded value: ");
  Serial.println(config.backendHTTPPort);
  Serial.println("Config.backendWSPort loaded value: ");
  Serial.println(config.backendWSPort);

  Serial.println("loading DB issued id");
  Serial.println("loading id");
  Serial.println("id.isNull: ");
  Serial.println(doc["id"].isNull());
//  Serial.println(doc["id"]);

  strlcpy(config.id, doc["id"] | RuntimeConfig::defaults.id, sizeof(config.id));
//  config.id = doc["id"].isNull() ? RuntimeConfig::defaults.id : doc["id"]; // if you don't use the defaults, everything crashes later down the line when advertising the service on mdns. TODO: handle this more beautifully
  Serial.println("Config.id loaded value: ");
  Serial.println(config.id);

  if(fileExistsAndNoError) {
    configFile.close();
  }
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

  DynamicJsonDocument doc(2048);
  doc.clear();
  Serial.println("IS DOC NULL: "+ String(doc.isNull()));

  doc["registered"] = config.registered;
  doc["name"] = config.name;
  doc["id"] = config.id;
  doc["backendHTTPPort"] = config.backendHTTPPort;
  doc["backendWSPort"] = config.backendWSPort;
  doc["backendServerPath"] = config.backendServerPath;
  Serial.println("name  " + doc["name"].as<String>());
  Serial.println("registered  " + doc["registered"].as<String>());
  Serial.println("backendServerPath  " + doc["backendServerPath"].as<String>());
  Serial.println("backendHTTPPort  " + doc["backendHTTPPort"].as<String>());
  Serial.println("backendWSPort  " + doc["backendWSPort"].as<String>());
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