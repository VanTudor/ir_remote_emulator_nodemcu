// // #include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

// // #include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
// // #include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal

#include <cstdio>

#include "initialSetup.hpp"
#include "configManager.hpp"
#include "server.hpp"
//char beServerName[40];

using namespace std;

void serviceProbeResult(const String& p_pcServiceName,
                        const MDNSResponder::hMDNSService p_hMDNSService,
                        bool p_bProbeResult) {
  (void) p_hMDNSService;
  Serial.printf("MDNSServiceProbeResultCallback: Service %s probe %s\n", p_pcServiceName.c_str(), (p_bProbeResult ? "succeeded." : "failed!"));
}

void initMDNS() {
  MDNSResponder::hMDNSService hMDNSService;
  MDNS.end();
  MDNS = MDNSResponder();
  short mDNSPort = 80;

  const char* mDNSTXTServiceType = "esp8266RemoteEmulator";
  const int chipId = ESP.getChipId();
  Serial.println("ARRAY SIZE: " + String(getIntAsArraySize(chipId)));
  char* mDNSTXTChipId = (char *)malloc(getIntAsArraySize(chipId));
  sprintf(mDNSTXTChipId, "%d", chipId);
  const char* mDNSTXTRegistered;

  Serial.println();
  Serial.println("Initiating mDNS service.");
  RuntimeConfig storedConfig{};

  String mdnsHost = "remoteEmulator" + String(chipId);
  if (MDNS.begin(mdnsHost, WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  loadConfig(storedConfig);

  mDNSTXTRegistered = storedConfig.registered ? "true" : "false";
  Serial.println();
  Serial.println("Finished loading config. Config data:");
  Serial.println("REGISTERED: >" + String(storedConfig.registered));
  Serial.println("NAME: >" + String(storedConfig.name));
  Serial.println("Registering mDNS service...");
  hMDNSService = MDNS.addService(nullptr, "_http", "tcp", mDNSPort);
  if (hMDNSService) {
    MDNS.setServiceProbeResultCallback(hMDNSService, serviceProbeResult);
    MDNS.addServiceTxt(hMDNSService, "type", mDNSTXTServiceType);
    MDNS.setServiceName(storedConfig.name, "ESP8266 Remote Emulator");
    MDNS.setServiceName(hMDNSService, storedConfig.name);
    MDNS.addServiceTxt(hMDNSService, "chipId", mDNSTXTChipId);
    MDNS.addServiceTxt(hMDNSService, "registered", mDNSTXTRegistered);

    Serial.println();
    Serial.println("mDNS service registered. mDNS Data: ");
    Serial.println("Service: HTTP");

    Serial.println("Port: " + String(mDNSPort));
    Serial.println("Type: " + String(mDNSTXTServiceType));
    Serial.println("TXT.registered: " + String(mDNSTXTRegistered));
  } else {
    Serial.println("Failed starting MDNS host. Retrying in 500ms.");
    delay(500);
    initMDNS();
  }
}

void initSSIDConnectionOrSetup() {
  WiFiManager wifiManager;
  // Wait 180 seconds after a WiFi connection failure before re-entering new board setup mode
  wifiManager.setConfigPortalTimeout(180);

  String tmpString = "IR REMOTE EMULATOR" + String(ESP.getChipId());
  char charBuf[50];
  tmpString.toCharArray(charBuf, 50);
  if(!wifiManager.autoConnect(charBuf, ServerConfig::apPassword)) {
    Serial.println("WiFi hotspot init failed. Resetting...");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  Serial.println();
  Serial.print("\nConnected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  initMDNS();
}
