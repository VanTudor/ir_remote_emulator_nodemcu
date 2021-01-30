# IR Remote emulator nodemcu

## Setup
1. Connect to remote_emulator.local
    - `http://remote_emulator1.local` or `http://remote_emulator2.local` ... `http://remote_emulatorn.local`
2. Enter the SSID credentials and Server name (mDNS name of the IR Remote Emulator backend server)
    - The backend server name is the same you're using to access it on the local network. E.g. `my_remote_emulator_backend` in `http://my_remote_emulator_backend.local` 
    - #TODO: Tip: If you're setting multiple units up at the same time, you can use the `Blink LED` button to identify the board.

### How to get yourself out of a mess
0. write some bad code and flash it to your esp8266
1. get stuck in a loop that doesn't stop even after subsequent updates
2. `pip install esptool`
3. `esptool.py erase_flash`
4. repeat.

### Wiring diagram
#### IR sensor
- I'm using a Vishay TSOP24 family IR sensor (https://www.vishay.com/docs/82459/tsop48.pdf).
The sensor's pins are (left to right, facing of the sensor) 1 OUT, 2 Vs (3.3V works), 3 GND
  - The code expects the output of the sensor to go to pin D4 (aka 2 in code)
## Dependencies
- https://github.com/tzapu/WiFiManager