//
// Created by vantudor on 30.09.2020.
//

#include "utils.hpp"

unsigned short getIntLength(int x) {
  /* fast way of finding the length of a signed int
   *  kudos @ https://stackoverflow.com/a/3069580
   *  added logic for negative numbers too
  **/

  // POSITIVE
  if(x >= 100000) {
    if(x >= 10000000) {
      if(x >= 1000000000) return 10;
      if(x >= 100000000) return 9;
      return 8;
    }
    if(x >= 1000000) return 7;
    return 6;
  } else {
    if (x >= 1000) {
      if (x >= 10000) return 5;
      return 4;
    } else {
      if (x >= 100) return 3;
      if (x >= 10) return 2;
      // NEGATIVE
      if (x < 0) {
        if (x <= -100000) {
          if (x <= -10000000) {
            if (x <= -1000000000) return 10;
            if (x <= -100000000) return 9;
            return 8;
          }
          if (x <= -1000000) return 7;
          return 6;
        } else {
          if (x <= -1000) {
            if (x <= -10000) return 5;
            return 4;
          } else {
            if (x <= -100) return 3;
            if (x <= -10) return 2;
            return 1;
          }
        }
      }
      return 1; // < 10, > 0
    }
  }
}

unsigned int getIntAsArraySize(int x) {
  return getIntLength(x) / sizeof(x);
}
//
//template <size_t desiredCapacity, class type>
//type getDefaultIfNull(const StaticJsonDocument<desiredCapacity>& x, type dfault) {
//  return x.isNull() ? dfault : x.template as<type>();
//}

//template <typename T>
//T getDefaultIfNull(const DynamicJsonDocument& x, T dfault) {
//  return x.isNull() ? dfault : x.as<T>();
//}
//
//const char* getDefaultIfNull(const DynamicJsonDocument& x, const char* dfault) {
//  return x.isNull() ? dfault : x.as<char*>();
//}