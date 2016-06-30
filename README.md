DCCThrottle
===========
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

Using The Throttle
==================
Connect the serial write pin from the pro mini to the DCC++ Base Station serial input and
provide power to the pro mini.  On first startup the throttle should prompt for a throttle
id number, if it doesn't you can go into the menu and set the throttle id.

Setting Throttle ID
-------------------
Press "*" to enter the menu, scroll through the options to "Throttle Config" and select the
"Throttle ID" option. The Throttle ID is used as the register number in the messages sent to
the DCC++ Base Station.  Default range for registers is 1-12.

Using the Menu
--------------
Press the "*" key to enter the menu.  Exit the menu by pressing "*" or "#" from any screen.
The "2" key will scroll UP, "8" will scroll DOWN, "6" will select the current item.

Selecting Locomotive
--------------------
Press the "#" key to rotate between the known locomotives, by default up to 4 locomotives
can be controlled from the throttle.

Toggling Locomotive Functions
-----------------------------
Digits 0-9 will toggle those functions, 10-28 can be toggled via the menu under the "Functions"
section.