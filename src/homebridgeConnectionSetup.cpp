#include "homebridgeConnectionSetup.hpp"
#include "configManager.hpp"
#include "server.hpp"
#include "initialSetup.hpp"

bool shouldSaveConfig = false;

void handleRegister() {
  RuntimeConfig config{};
//
//  // TODO: compare with previous config
//  Serial.println("Registering board...");
//  RuntimeConfig existingConfig{};
//  loadConfig(existingConfig);
//
//  Serial.println("Existing config.registered value: ");
//  Serial.print(existingConfig.registered);
//
//  Serial.println();
//  Serial.println("Reading from supplied JSON.");
  StaticJsonDocument<256> doc;
  doc.clear();
  doc.to<JsonObject>();
  // TODO: add a try catch here.
  Serial.println("----Registering----");
  Serial.println("Raw JSON received:");
  Serial.println(server.arg("plain"));
  deserializeJson(doc, server.arg("plain"));

  Serial.println("----");
  Serial.println(doc["name"].isNull());
  Serial.println(doc["registered"].isNull());
  Serial.println(doc["cacatoa"].isNull());
  Serial.println(doc["mazga_in_beci"].isNull());
  Serial.println("----");
  config.registered = doc["registered"].isNull() ? RuntimeConfig::defaults.registered : doc["registered"];
//  config.name = doc["name"].isNull() ? RuntimeConfig::defaults.name : doc["name"];
  strlcpy(config.name, doc["name"] | RuntimeConfig::defaults.name, sizeof(config.name));
//  config.backendServerPath = doc["backendServerPath"].isNull() ? RuntimeConfig::defaults.backendServerPath : doc["backendServerPath"]; // TODO: add error handling
  strlcpy(config.backendServerPath, doc["backendServerPath"] | RuntimeConfig::defaults.backendServerPath, sizeof(config.backendServerPath));
  config.backendHTTPPort = doc["backendHTTPPort"].isNull() ? RuntimeConfig::defaults.backendHTTPPort : doc["backendHTTPPort"]; // TODO: add error handling
  config.backendWSPort = doc["backendWSPort"].isNull() ? RuntimeConfig::defaults.backendWSPort : doc["backendWSPort"]; // TODO: add error handling

  strlcpy(config.id, doc["id"] | RuntimeConfig::defaults.id, sizeof(config.id));
//  config.id = doc["id"]; // TODO: add error handling

  Serial.println("Deserialized JSON. Values: ");
  Serial.println("Registered: " + String(config.registered));
  Serial.println("Name: " + String(config.name));
  Serial.println("BackendServerPath: " + String(config.backendServerPath));
  Serial.println("ID: " + String(config.id));

  saveConfig(config);
  Serial.println("Restarting mDNS service...");
  server.send(200, "text/plain", "Updated registration!\r\n");
  delay(1000);
  Serial.println("----Finished registering----");
  ESP.restart(); // do this instead of initMDNS(), to avoid passing the config from main.cpp down to this handler.
  delay(1000);
}

void handleUnRegister() {
//  SPIFFS.begin();
  Serial.println("De-registering...");
  deleteConfigFile();
  delay(500);
  Serial.println("Resetting mDNS server...");
  server.send(200, "text/plain", "Successfully unregistered!\r\n"); // naughty liar, hehe
  delay(500);
  ESP.restart();
//  MDNS = MDNSResponder();
//  delay(500);
//  initMDNS();
//
//  Serial.println("De-registration successful!");
//  SPIFFS.end();
}
