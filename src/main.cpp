#include <ArduinoJson.h>
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
 
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>
#include <IRrecv.h>
#include <IRutils.h>

// an IR detector/demodulator is connected to GPIO pin 2
uint16_t RECV_PIN = 2;
HTTPClient http;
IRrecv irrecv(RECV_PIN);

decode_results results;

const char* ssid = "DIGI-vmG5";
const char* password = "poiasd03";
bool shouldRecordIr = false;

MDNSResponder mdns;

ESP8266WebServer server(80);

IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)

void handleRoot() {
  server.send(200, "text/html",
              "<html>" \
                "<head><title>ESP8266 Demo</title></head>" \
                "<body>" \
                  "<h1>Hello from ESP8266, you can send NEC encoded IR" \
                      "signals from here!</h1>" \
                  "<p><a href=\"ir?code=551489775\">Send 20DF10EF</a></p>" \
                  "<p><a href=\"ir?code=16429347\">Send 0xFAB123</a></p>" \
                  "<p><a href=\"ir?code=16771222\">Send 0xFFE896</a></p>" \
                "</body>" \
              "</html>");
}

//void generateStatus() {}

void handleStatus() {
 StaticJsonDocument<200> doc;
 String JSONResponse;

 doc["isRecording"] = shouldRecordIr;

 serializeJson(doc, JSONResponse);
 server.send(200, "text/javascript", JSONResponse);
}

void handleIr() {
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
  
  const char* irCode = doc["irCode"].as<const char*>(); // "Leanne Graham"
  const boolean shouldRecord = doc["shouldRecord"].as<bool>(); // "Bret"
  
  shouldRecordIr = shouldRecord;
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
  handleRoot();
}

void handleNotFound() {
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

void setup(void) {
  irsend.begin();
  irrecv.enableIRIn();  // Start the receiver
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ir", HTTP_POST, handleIr);
  server.on("/status", HTTP_GET, handleStatus);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void dump(decode_results *results) {
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  uint16_t count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  } else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  } else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  } else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  } else if (results->decode_type == RC5X) {
    Serial.print("Decoded RC5X: ");
  } else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  } else if (results->decode_type == RCMM) {
    Serial.print("Decoded RCMM: ");
  } else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address, HEX);
    Serial.print(" Value: ");
  } else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  } else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  } else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  } else if (results->decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  }
  serialPrintUint64(results->value, 16);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (uint16_t i = 1; i < count; i++) {
    if (i % 100 == 0)
      yield();  // Preemptive yield every 100th entry to feed the WDT.
    if (i & 1) {
      Serial.print(results->rawbuf[i] * RAWTICK, DEC);
    } else {
      Serial.write('-');
      Serial.print((uint32_t) results->rawbuf[i] * RAWTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
}

void sendIrCodeToDB(uint64_t value) {
  http.begin("http://192.168.100.11:3001/createIrValue");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{\"value\":\"" + uint64ToString(value, 16) + "\"}");
  String payload = http.getString();
  Serial.println("Sent new value to server:");
  Serial.println(payload);
  http.end();
}

void loop(void) {
    if (shouldRecordIr) {
        if (irrecv.decode(&results)) {
          Serial.println("DETECTED IR VALUE:!");
          serialPrintUint64(results.value, 16);
          // dump(&results);
          sendIrCodeToDB(results.value);
          Serial.println('Sent request to mofocking mofock.');
          irrecv.resume();  // Receive the next value
        }
  }
  server.handleClient();
}