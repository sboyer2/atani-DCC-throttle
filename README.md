DCCThrottle
-----------------
DCC++ Handheld Throttle implementation

Required Libraries
------------------
The following libraries are required for this to compile:
* EEPROM (included in Arduino Eclipse)
* Encoder (included in Arduino Eclipse)

Bundled Libraries
-----------------
* Keypad (http://playground.arduino.cc/Code/Keypad)
  - only modification was to replace #include <Key.h> with #include "Key.h" and same for Keypad.h.
* MenuSystem (http://blog.humblecoder.com/arduino-menu-system-library/)
  - only modification was to replace "const char *" on the "name" field/accessor methods to
  be "const __FlashStringHelper *" to reduce SRAM usage.

Build Environment
-----------------
The DCCThrottle project uses Arduino Eclipse (http://eclipse.baeyens.it/index.shtml). 

Compiling the DCCThrottle project can be done through the Project -> Build Project menu
option.  After compilation you can upload the resulting binary to the Arduino board via
Arduino -> Upload Sketch.