/*
* Common mistakes & tips:
*   * Don't just connect the IR LED directly to the pin, it won't
*     have enough current to drive the IR LED effectively.
*   * Make sure you have the IR LED polarity correct.
*     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
*   * Typical digital camera/phones can be used to see if the IR LED is flashed.
*     Replace the IR LED with a normal LED if you don't have a digital camera
*     when debugging.
*   * Avoid using the following pins unless you really know what you are doing:
*     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
*     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
*     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
*   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
*     for your first time. e.g. ESP-12 etc.
*/
#ifndef NODEBUG_WEBSOCKETS
#ifdef DEBUG_ESP_PORT
#define DEBUG_WEBSOCKETS(...)               \
    {                                       \
        DEBUG_ESP_PORT.printf(__VA_ARGS__); \
        DEBUG_ESP_PORT.flush();             \
    }
#else
//#define DEBUG_WEBSOCKETS(...) os_printf( __VA_ARGS__ )
#endif
#endif

#ifndef DEBUG_WEBSOCKETS
#define DEBUG_WEBSOCKETS(...)
#ifndef NODEBUG_WEBSOCKETS
#define NODEBUG_WEBSOCKETS
#endif
#endif
#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <WebSocketsClient.h>

#include "utils.hpp"
#include "routes.hpp"
#include "initialSetup.hpp"
#include "server.hpp"
#include "homebridgeConnectionSetup.hpp"
#include "configManager.hpp"
#include "irScanner.hpp"

 void handleRoot() {
   server.send(200, "text/plain", "hello from esp8266!\r\n");
 }
 void handleNotFound() {
   String message = "File Not Found\n\n";
   message += "URI: ";
   message += server.uri();
   message += "\nMethod: ";
   message += (server.method() == HTTP_GET) ? "GET" : "POST";
   message += "\nArguments: ";
   message += server.args();
   message += "\n";
   for (int i = 0; i < server.args(); i++) {
     message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
   }
   server.send(404, "text/plain", message);
 }

// an IR detector/demodulator is connected to GPIO pin 2
uint16_t RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;
RuntimeConfig storedConfig{};

IRsend irsend(12);  // An IR LED is controlled by GPIO pin  (D6)

void setup(void) {
  // runNewBoardSetup();
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(2000);
  Serial.println("Starting.");
  if (!SPIFFS.begin()) {
    Serial.println("Failed initing spiffs.");
  }
  irrecv.enableIRIn();  // Start the receiver
  Serial.println("Enabled Serial communication, SPIFFS and the IR scanner.");
  Serial.println("Loading stored config.");
  loadConfig(storedConfig);
  delay(2000);
  Serial.println("Stored config config loaded.");
  Serial.println("Initializing SSID connection or setup.");
  initSSIDConnectionOrSetup(storedConfig);
  delay(200);
  Serial.println("SSID connection or setup finished.");
  if (storedConfig.registered) {
    Serial.println("Starting SocketIO client.");
    startIRCodeSocketStream(storedConfig);
    delay(200);
    Serial.println("SocketIO client started.");
  }
  server.begin();

//  RuntimeConfig storedConfig{}; // TODO: make this global
//  loadConfig(storedConfig);
//  startIRCodeSocketStream(&storedConfig);

  AppState appState;
  irsend.begin();
  RouteHandlers routeHandlers(appState, irsend);
//  server.on("/status", HTTP_POST, std::bind(&RouteHandlers::handleUpdateStatus, routeHandlers));
//  // server.on("/status", HTTP_GET, handleStatus);
//
//  // server.on("/inline", [](){
//  //   server.send(200, "text/plain", "this works as well");
//  // });
//
//  // server.onNotFound(handleNotFound);
//
  // irrecv.enableIRIn();  // Start the receiver

   // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(WiFi.status());
    Serial.print(".");
  }

  server.on("/register", handleRegister);
  server.on("/unregister", handleUnRegister);
  server.on("/reset", std::bind(&RouteHandlers::handleReset, routeHandlers));
  server.on("/deleteConfig", deleteConfigFile);

//  server.on("/ir", HTTP_POST, handleIR);
  server.on("/ir", HTTP_POST, std::bind(&RouteHandlers::handleIr, routeHandlers));
  server.on("/", [](){
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  Serial.println("Startup setup done.");
 }

void loop(void) {
  MDNS.update();

  if (storedConfig.registered) {
    handleSocketIOLoop();
  }
  server.handleClient();
  char stringifiedIRCode[64];
  if (irrecv.decode(&results)) {
    Serial.println("Detected IR value: ");
    serialPrintUint64(results.value, 16);

    sprintf(stringifiedIRCode, "%llu", results.value);
    irrecv.resume();  // Receive the next value
    if (storedConfig.registered) {
      emitIROnSocketIO(stringifiedIRCode, storedConfig);
    }
  }
}
