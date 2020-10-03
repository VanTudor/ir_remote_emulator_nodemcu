//
// Created by vantudor on 30.09.2020.
//

#ifndef IR_REMOTE_EMULATOR_NODEMCU_UTILS_HPP
#define IR_REMOTE_EMULATOR_NODEMCU_UTILS_HPP

#include <sstream>
#include <cstring>
#include <ArduinoJson.h>
#include <FS.h>
#include <Arduino.h>

unsigned short getIntLength(int x);
unsigned int getIntAsArraySize(int x);

//int getDefaultIfNull(const DynamicJsonDocument& x, int dfault);
//bool getDefaultIfNull(const DynamicJsonDocument& x, bool dfault);
//const char* getDefaultIfNull(const DynamicJsonDocument& x, const char* dfault);

//template <typename T>
//T getDefaultIfNull(const DynamicJsonDocument& x, T dfault) {
//  Serial.println("ISNULL:");
//  Serial.println(x.isNull());
//  Serial.println("TEMPLATE VAL:");
//  Serial.println(x.template as<T>());
//  Serial.println("DFAULT :");
//  Serial.println(dfault);
//  return x.isNull() ? dfault : x.as<T>();
//}

#endif //IR_REMOTE_EMULATOR_NODEMCU_UTILS_HPP