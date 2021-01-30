#include <ESP8266HTTPClient.h>
 #include <IRrecv.h>
 #include <IRutils.h>
// #include <EEPROM.h>
#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
#include "server.hpp"
#include "utils.hpp"
// #include <IRremoteESP8266.h>
#include <IRsend.h>
// #include <WiFiClient.h>
#ifndef IR_REMOTE_EMULATOR_NODEMCU_ROUTES_H
#define IR_REMOTE_EMULATOR_NODEMCU_ROUTES_H
#include "irScanner.hpp"
#include "configManager.hpp"
void handleStatus();

class RouteHandlers { // see PIMPL
  public:
    RouteHandlers(AppState &appState);
    void handleIr();
    void handleNotFound();
    void blinkLED();
    void handleReset();
    void send200Response(char*);
  // POST /status
    void handleUpdateStatus();
    void handleSocketIOLoop();
//    ~Handlers();
  private:
    AppState* appState;
};

#endif IR_REMOTE_EMULATOR_NODEMCU_ROUTES_H

// String readSerialLine();


// /**
//  * This should be called when a new device is added.
//  * Sets a device id and an API key in the local EEPROM memory.
//  * These will be used to authenticate all subsequent requests.
//  */
// void firstTimeRegister() {
//   Serial.println("\n");
//   Serial.println("*** First time boot register process ***");
//   Serial.println("Server backend name (will be used instead of its ip): ");
//   String serverBackendName = readSerialLine();
  
//   Serial.println("Remote emulator name (this device's name): ");
//   String deviceName = readSerialLine();

  
// }