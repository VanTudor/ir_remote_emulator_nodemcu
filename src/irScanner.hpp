//
// Created by vantudor on 16.10.2020.
//

#ifndef IR_REMOTE_EMULATOR_NODEMCU_IRDETECTION_HPP
#define IR_REMOTE_EMULATOR_NODEMCU_IRDETECTION_HPP

#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <ArduinoJson.h>
#include <IRrecv.h>
#include <IRutils.h>

#include "server.hpp"
#include "configManager.hpp"

void socketIOEvent(socketIOmessageType_t, uint8_t *, size_t);
void startIRCodeSocketStream(RuntimeConfig *storedConfig);
void handleSocketIOLoop();
void recordIr(decode_results *results);
#endif //IR_REMOTE_EMULATOR_NODEMCU_IRDETECTION_HPP

