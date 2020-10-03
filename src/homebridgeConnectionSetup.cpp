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
  DynamicJsonDocument doc(2048);
  doc.clear();
  doc.to<JsonObject>();
  // TODO: add a try catch here.
  Serial.println("----");
  Serial.println(server.arg("plain"));
  Serial.println("----");
  deserializeJson(doc, server.arg("plain"));

  Serial.println("----");
  Serial.println(doc["name"].isNull());
  Serial.println(doc["registered"].isNull());
  Serial.println(doc["cacatoa"].isNull());
  Serial.println(doc["mazga_in_beci"].isNull());
  Serial.println("----");
  config.registered = doc["registered"].isNull() ? RuntimeConfig::defaults.registered : doc["registered"];
  config.name = doc["name"].isNull() ? RuntimeConfig::defaults.name : doc["name"];

  Serial.println("Deserialized JSON. Values: ");
  Serial.println("Registered: " + String(config.registered));
  Serial.println("Name: " + String(config.name));

  saveConfig(config);
  Serial.println("Restarting mDNS service...");
  initMDNS();
  server.send(200, "text/plain", "Updated registration!\r\n");
}

void handleUnRegister() {
//  SPIFFS.begin();
  Serial.println("De-registering...");
  if (SPIFFS.exists(ServerConfig::filePath)) {
    SPIFFS.remove(ServerConfig::filePath);
    Serial.println("Removed config file...");
  } else {
    Serial.println("No config file stored. Continuing...");
  }
  Serial.println("Resetting mDNS server...");
  delay(500);
  MDNS = MDNSResponder();
  initMDNS();

  Serial.println("De-registration successful!");
  server.send(200, "text/plain", "Successfully unregistered!\r\n");
//  SPIFFS.end();
}