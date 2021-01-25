//
// Created by vantudor on 16.10.2020.
//
#include "irScanner.hpp"

SocketIOclient socketIO;

#define USE_SERIAL Serial1

bool shouldRecordIr = false;
void handleGetStatus() {
  StaticJsonDocument<200> doc;
  String JSONResponse;

  doc["isRecording"] = shouldRecordIr;

  serializeJson(doc, JSONResponse);
  server.send(200, "text/javascript", JSONResponse);
}

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case sIOtype_DISCONNECT:
      USE_SERIAL.printf("[IOc] Disconnected!\n");
      break;
    case sIOtype_CONNECT:
      USE_SERIAL.printf("[IOc] Connected to url: %s\n", payload);
      break;
    case sIOtype_EVENT:
      USE_SERIAL.printf("[IOc] get event: %s\n", payload);
      break;
    case sIOtype_ACK:
      USE_SERIAL.printf("[IOc] get ack: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_ERROR:
      USE_SERIAL.printf("[IOc] get error: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_BINARY_EVENT:
      USE_SERIAL.printf("[IOc] get binary: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_BINARY_ACK:
      USE_SERIAL.printf("[IOc] get binary ack: %u\n", length);
      hexdump(payload, length);
      break;
  }
}

void irRecordingStatusUpdateHandler(DynamicJsonDocument deserializedConfigJSON, RuntimeConfig* storedConfig) {
  const boolean shouldRecord = deserializedConfigJSON["shouldRecord"].as<bool>(); // "Bret"
  storedConfig->shouldRecord = shouldRecord;
  if (shouldRecordIr) { // TODO: maybe always do a socketIO.end(); or how whatever its name is before
    //   socketIO.begin(storedConfig.backendServerPath, storedConfig.backendServerPort);
    socketIO.begin(storedConfig->backendServerPath, storedConfig->backendServerPort);
    // event handler
    socketIO.onEvent(socketIOEvent);
  } else {
    // TODO: add a socketIO.end(); or how whatever its name is
  }
}

void handleSocketIOLoop() {
  if (shouldRecordIr && socketIO.isConnected()) {
    socketIO.loop();

    uint64_t now = millis();
    unsigned long messageTimestamp = 0;
    if(now - messageTimestamp > 2000) {
      messageTimestamp = now;

      // create JSON message for Socket.IO (event)
      DynamicJsonDocument doc(1024);
      JsonArray array = doc.to<JsonArray>();

      // add evnet name
      // Hint: socket.on('event_name', ....
      array.add("event_name");

      // add payload (parameters) for the event
      JsonObject param1 = array.createNestedObject();
      param1["now"] = (uint32_t) now;

      // JSON to String (serializion)
      String output;
      serializeJson(doc, output);

      // Send event
      socketIO.sendEVENT(output);

      // Print JSON for debugging
      USE_SERIAL.println(output);
    }
  }
}

#define USE_SERIAL Serial1