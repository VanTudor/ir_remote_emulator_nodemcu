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

#include <Arduino.h>
#include <ESP8266mDNS.h>

// #include "routes.cpp"
#include "utils.hpp"
#include "routes.hpp"
#include "initialSetup.hpp"
#include "server.hpp"
#include "homebridgeConnectionSetup.hpp"
#include "configManager.hpp"
#include "irScanner.hpp"

// // an IR detector/demodulator is connected to GPIO pin 2
// uint16_t RECV_PIN = 2;
// HTTPClient http;
// IRrecv irrecv(RECV_PIN);

// const char* ssid = "DIGI-vmG5";
// const char* password = "poiasd03";

// IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)

// //void generateStatus() {}
// #include <WiFiManager.h>

//extern server(80);
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

void setup(void) {
  // runNewBoardSetup();
  Serial.println("Starting.");
  Serial.begin(115200);
  SPIFFS.begin();
  irrecv.enableIRIn();  // Start the receiver
  Serial.println("Enabled Serial communication, SPIFFS and the IR scanner.");
  delay(200);
  initSSIDConnectionOrSetup();
  server.begin();

//  RuntimeConfig storedConfig{}; // TODO: make this global
//  loadConfig(storedConfig);
//  startIRCodeSocketStream(&storedConfig);

  AppState appState;
  RouteHandlers routeHandlers(appState);
  server.on("/ir", HTTP_POST, std::bind(&RouteHandlers::handleIr, routeHandlers));
  server.on("/status", HTTP_POST, std::bind(&RouteHandlers::handleUpdateStatus, routeHandlers));
//  // server.on("/status", HTTP_GET, handleStatus);
//
//  // server.on("/inline", [](){
//  //   server.send(200, "text/plain", "this works as well");
//  // });
//
//  // server.onNotFound(handleNotFound);
//
  // irsend.begin();
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
  server.on("/", [](){
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

 }

// void sendIrCodeToDB(uint64_t value) {
//   http.begin("http://192.168.100.11:3001/createIrValue");
//   http.addHeader("Content-Type", "application/json");
//   int httpCode = http.POST("{\"value\":\"" + uint64ToString(value, 16) + "\"}");
//   String payload = http.getString();
//   Serial.println("Sent new value to server:");
//   Serial.println(payload);
//   http.end();
// }

void loop(void) {
//  deleteConfigFile();
  MDNS.update();
  //   if (shouldRecordIr) {
  //       if (irrecv.decode(&results)) {
  //         Serial.println("DETECTED IR VALUE:!");
  //         serialPrintUint64(results.value, 16);
  //         // dump(&results);
  //         sendIrCodeToDB(results.value);
  //         Serial.println('Sent request to mofocking mofock.');
  //         irrecv.resume();  // Receive the next value
  //       }
  // }
   server.handleClient();
   handleSocketIOLoop();
    if (irrecv.decode(&results)) {
      serialPrintUint64(results.value, 16);
      recordIr(&results);
      irrecv.resume();  // Receive the next value
    }
//  Serial.println(beServerName);
//  Serial.println("LOCALIP: ");
//  Serial.println(WiFi.localIP());
//  Serial.println("WIFI connected: ");
//  Serial.println(WiFi.status() == WL_CONNECTED);
}
