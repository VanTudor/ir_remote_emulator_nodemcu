# IR Remote emulator nodemcu

## Setup
1. Connect to remote_emulator.local
    - `http://remote_emulator1.local` or `http://remote_emulator2.local` ... `http://remote_emulatorn.local`
2. Enter the SSID credentials and Server name (mDNS name of the IR Remote Emulator backend server)
    - The backend server name is the same you're using to access it on the local network. E.g. `my_remote_emulator_backend` in `http://my_remote_emulator_backend.local` 
    - #TODO: Tip: If you're setting multiple units up at the same time, you can use the `Blink LED` button to identify the board.

## Dependencies
- https://github.com/tzapu/WiFiManager