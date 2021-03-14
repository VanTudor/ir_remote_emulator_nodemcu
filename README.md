# IR Remote emulator nodemcu

## Setting up a fresh Remote Control Emulator (RCE)

1. Make sure you've went through the prerequisites (RCE BE and FE should already have been installed and running).
2. Burn this code on your ESP8266 board. (Tested with NodeMCU AMICA, but all esp8266 boards should work).
3. On board boot-up, look for `remoteControlEmulator${chipId}` in your WiFi networks list and connect to it.
4. Wait until the captive portal pops up, then select your WiFi network and enter its password. The RCE will connect to it.
5. Navigate to RCE FE page (link here), where you should see your RCE in the list of unregistered devices. 

### Wiring diagram

#### IR sensor
- During development, I've used a Vishay TSOP24 family IR sensor (https://www.vishay.com/docs/82459/tsop48.pdf).
  The sensor's pins are (left to right, facing of the sensor) 1 OUT, 2 Vs (3.3V works), 3 GND
    - The code expects the output of the sensor to go to pin D4 (aka 2 in code)

#### IR emitter
- IR LED: + (long leg) to 3V3
- transistor (2N2222, facing the flat side):
    - right leg to GND
    - middle leg to D6
    - left leg to IR LED - (short leg)

#### TO DO:
- actual diagrams

## Development

### How to get yourself out of a mess
0. write some bad code and flash it to your esp8266
1. get stuck in a loop that doesn't stop even after subsequent updates
2. `pip install esptool`
3. `esptool.py erase_flash`
4. repeat.


### Debugging

#### Platformio
Go to Projects -> Your project -> New option `build_flags` and add `-DDEBUG_ESP_PORT=Serial` to the new input that appeared on the bottom of the page.

## Dependencies
	tzapu/WifiManager@^0.15.0
	crankyoldgit/IRremoteESP8266@^2.7.10
	bblanchon/ArduinoJson@6.15.2
	links2004/WebSockets@^2.3.3
 