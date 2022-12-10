# ESP32-ESP8266-TO-VRC-OSC

Arduino code allowing for sending parameters to VRC avatars using a ESP32 or ESP8266 over WiFi connection.

Allows user to have a pysical remote for avatar parameters in game, no in-game menu interaction required

Works with Bools and Floats. Two versions, one with Bool and Analog to Float and one with only Bool

Notes on expanding are listed in code

### Prerequisites
 - ESP32 or ESP8266
  - https://github.com/stahlnow/OSCLib-for-ESP8266
   - https://playground.arduino.cc/Code/Keypad/
### Board packages
  -  if using ESP8266
     - https://arduino.esp8266.com/stable/package_esp8266com_index.json
  - If using ESP32
    - https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json


### Getting Started
  - Set WiFi 
  ```Python
  char ssid[] = "*****";
  char pass[] = "*****";
  ```
  
  - Set IP of Computer Running VRC 
  ```Python
  const IPAddress outIp(192, 168, 1, 12);
  ```
    
   - Set address of Bool parameter(s)
    - Parameter addresses found in C:\Users\User\AppData\LocalLow\VRChat\VRChat\OSC\usr_****\Avatars\
     - Open the JOSN with the avatar ID you want control parameters of and copy the address of the desired Bool
       - parameter toggles will also work on any other avatar with a matching parameter address or addresses

  ```Python
  char toggle1[] = "/avatar/parameters/Audio_Link";
  ```
   
   - Set button/keypad pin assignments 
  ```Python
  const byte ROWS = 1;  //one rows
const byte COLS = 4;  //four columns

char keys[ROWS][COLS] = {
  { '1', '2', '3', '4' },

};

// tested, working pin assignments for 4 buttons on ESP32, add/edit as needed

byte rowPins[ROWS] = { 33 };              //connect to the row pinouts of the keypad
byte colPins[COLS] = { 17, 13, 14, 26 };  //connect to the column pinouts of the keypad

// tested, working pin assignments for 4 buttons on ESP8266, add/edit as needed

byte rowPins[ROWS] = { 0 };              //connect to the row pinouts of the keypad
byte colPins[COLS] = { 4, 12, 13, 15 };  //connect to the column pinouts of the keypad


  ```
 ### If only using Bool, Upload to Board (Analog to Float setup notes to be added)
 - Test buttons
   - Serial console will show what button was invoked 1-4 and the case run, example
   ```bash
    1 sendparameter1true
    ```
   - Press the same button again it it should give
   ```bash
    1 sendparameter1false
    ```
    
     ### Test in VRC
     - Profit?
       - if no connection try power cycling the ESP
