#include "routes.hpp"
IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)

RouteHandlers::RouteHandlers(AppState &externalAppState) {
  appState = &externalAppState;
}

void RouteHandlers::handleReset() {
  Serial.println("Resetting...");
  WiFi.disconnect();
  Serial.println("Disconnecting from WiFi...");
  ESP.reset();
  Serial.println("Erasing stored config...");
  delay(1000);
  Serial.println("Successfully reset!");
  server.send(200, "text/plain", "Successfully reset!\r\n");
}

void RouteHandlers::handleUpdateStatus() {
  RuntimeConfig storedConfig{}; // TODO: make this global
  DynamicJsonDocument doc(2048);

  deserializeJson(doc, server.arg("plain"));
  loadConfig(storedConfig);
//  const boolean shouldRecord = deserializedConfigJSON["shouldRecord"].as<bool>(); // "Bret"
  const boolean shouldRecordIr = doc["shouldRecord"].as<bool>();
  storedConfig.shouldRecord = shouldRecordIr;
  appState->shouldRecordIr = shouldRecordIr;
  saveConfig(storedConfig);

  if (shouldRecordIr) {
    startIRCodeSocketStream(&storedConfig);
  } else {
//    stopIRCodeSocketStream();
  }

  // add other config handlers/modifiers here
}
//void handleStatus(ESP8266WebServer server) {
// StaticJsonDocument<200> doc;
// String JSONResponse;
//
// doc["isRecording"] = shouldRecordIr;
//
// serializeJson(doc, JSONResponse);
// server.send(200, "text/javascript", JSONResponse);
//}

void RouteHandlers::send200Response(char* responseText) {
//   server.send(200, "text/html",
//               "<html>" \
//                 "<head><title>ESP8266 Demo</title></head>" \
//                 "<body>" \
//                   "<h1>Hello from ESP8266, you can send NEC encoded IR" \
//                       "signals from here!</h1>" \
//                   "<p><a href=\"ir?code=551489775\">Send 20DF10EF</a></p>" \
//                   "<p><a href=\"ir?code=16429347\">Send 0xFAB123</a></p>" \
//                   "<p><a href=\"ir?code=16771222\">Send 0xFFE896</a></p>" \
//                 "</body>" \
//               "</html>");

  Serial.println("Sending 200 response...");
  return server.send(200, "text/html", responseText);
 }

void RouteHandlers::handleIr() {
  Serial.println("Handling ir...");
  /*for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      uint32_t code = strtoul(server.arg(i).c_str(), NULL, 10);
      irsend.sendNEC(code, 32);
    }
    if (server.argName(i) == "record") {

      uint32_t code = strtoul(server.arg(i).c_str(), NULL, 10);
      irsend.sendNEC(code, 32);
    }
  }*/
  // Parsing
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, server.arg("plain"));

  const char *irCode = doc["irCode"].as<const char *>(); // "Leanne Graham"
  const boolean shouldRecord = doc["shouldRecord"].as<bool>(); // "Bret"

  appState->shouldRecordIr = shouldRecord;
  if (irCode) {
    uint32_t code = strtoul(irCode, NULL, 10);
    irsend.sendNEC(code, 32);
  }
  // Output to serial monitor
  Serial.print("JSON irCode:");
  Serial.println(irCode);
  Serial.print("JSON shouldRecord:");
  Serial.println(shouldRecord);
  Serial.println("JSON shouldRecord type: ");
  //  Serial.println(doc["shouldRecord"].is<bool>());
  //  Serial.println(doc["shouldRecord"].is<const char*>());
  send200Response(strcat("Successfully issued ", irCode));
}

 void RouteHandlers::handleNotFound() {
   String message = "File Not Found\n\n";
   message += "URI: ";
   message += server.uri();
   message += "\nMethod: ";
   message += (server.method() == HTTP_GET)?"GET":"POST";
   message += "\nArguments: ";
   message += server.args();
   message += "\n";
   for (uint8_t i = 0; i < server.args(); i++)
     message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
   server.send(404, "text/plain", message);
 }

 /**
  * Simple utility function to read a serial monitor line. Reads char by char until it gets a '\n',
  * when it stops and returns the concatenated chars under the form of a string.
  **/
 String readSerialLine() {
   String line = "";
   while (Serial.available() > 0) {
     char received = Serial.read();
     if (received == '\n') {
       return line;
     }
     line += received;
   }
 }


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
