void handleStatus();


void handleIr();
void handleNotFound();
void blinkLED();
void handleReset();

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