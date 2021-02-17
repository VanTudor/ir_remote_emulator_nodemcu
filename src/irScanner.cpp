//
// Created by vantudor on 16.10.2020.
//
#include "irScanner.hpp"

SocketIOclient socketIO;

void handleGetStatus() {
  StaticJsonDocument<200> doc;
  String JSONResponse;

  // figure out if we're recording or not

  serializeJson(doc, JSONResponse);
  server.send(200, "text/javascript", JSONResponse);
}

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case sIOtype_DISCONNECT:
      Serial.printf("[IOc] Disconnected!\n");
      break;
    case sIOtype_CONNECT:
      Serial.printf("[IOc] Connected to url: %s\n", payload);
      break;
    case sIOtype_EVENT:
      Serial.printf("[IOc] get event: %s\n", payload);
      break;
    case sIOtype_ACK:
      Serial.printf("[IOc] get ack: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_ERROR:
      Serial.printf("[IOc] get error: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_BINARY_EVENT:
      Serial.printf("[IOc] get binary: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_BINARY_ACK:
      Serial.printf("[IOc] get binary ack: %u\n", length);
      hexdump(payload, length);
      break;
  }
}

void startIRCodeSocketStream(RuntimeConfig storedConfig) {
  String wsConnectPath = "ws://" + String(storedConfig.backendServerPath);
  Serial.println("Starting websocket connection to " + wsConnectPath + String(storedConfig.backendWSPort)); // + String(storedConfig.backendServerPath) + '3000' + String(storedConfig.backendWSPort));
  //   socketIO.begin(storedConfig.backendServerPath, storedConfig.backendWSPort);
  socketIO.begin("ws://" + String(storedConfig.backendServerPath), storedConfig.backendWSPort);;
  delay(200);
  // event handler
  socketIO.onEvent(socketIOEvent);
  socketIO.sendEVENT("sloboz");
}

//void stopIRCodeSocketStream() {
//  socketIO.disconnect();
//}

void handleSocketIOLoop() {
  socketIO.loop();
}
void emitIROnSocketIO(char* irCode, RuntimeConfig runtimeConfig) {
  if (socketIO.isConnected()) {
//    socketIO.loop();

    uint64_t now = millis();
    unsigned long messageTimestamp = 0;
    if(now - messageTimestamp > 2000) {
      messageTimestamp = now;

      // create JSON message for Socket.IO (event)
      DynamicJsonDocument doc(1024);
      JsonArray array = doc.to<JsonArray>();

      // add evemt name
      // Hint: socket.on('event_name', ....
      array.add("RCE_IRCodeDetected");

      // add payload (parameters) for the event
      JsonObject param1 = array.createNestedObject();
      param1["RCEId"] = runtimeConfig.id;
      param1["IRCode"] = irCode;
      // JSON to String (serializion)
      String output;
      serializeJson(doc, output);

      // Send event
      socketIO.sendEVENT(output);
      Serial.println("Sent SocketIO message.");

      // Print JSON for debugging
      Serial.println(output);
    }
  } else {
    Serial.println("SocketIO not connected. Sending detected IR message failed.");
  }
}

void detectIr(decode_results *results) {
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
#define USE_SERIAL Serial1