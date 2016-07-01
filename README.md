# DCCThrottle
DCC++ Handheld Throttle implementation

## Required Libraries
The following libraries are required for this to compile:
* EEPROM (included in Arduino Eclipse)
* Encoder (included in Arduino Eclipse)

## Bundled Libraries
* Keypad (http://playground.arduino.cc/Code/Keypad)
  - only modification was to replace #include <Key.h> with #include "Key.h" and same for Keypad.h.
* MenuSystem (http://blog.humblecoder.com/arduino-menu-system-library/)
  - only modification was to replace "const char *" on the "name" field/accessor methods to
  be "const __FlashStringHelper *" to reduce SRAM usage.
* New LiquidCrystal (https://bitbucket.org/fmalpartida/new-liquidcrystal)
  - no modifications, included to make building easier since it is not available
  by default in Arduino Eclipse.

## Build Environment
The DCCThrottle project uses Arduino Eclipse (http://eclipse.baeyens.it/index.shtml). 

Compiling the DCCThrottle project can be done through the Project -> Build Project menu
option.  After compilation you can upload the resulting binary to the Arduino board via
Arduino -> Upload Sketch.

### Disabling Arduino Eclipse's built in LiquidCrystal library
Before you can get a successful compilation of DCC Throttle you must disable the default
LiquidCrystal library as Arduino Eclipse will insist on adding it to the project even
though the NewliquidCrystal library has been included already.  You can disable this via:
Window -> Preferences -> Arduino -> Libraries.  Locate the Displays / LiquidCrystal entry
and select "remove" from the drop down list in the third column.  Click OK to save this
change.  Go to the Arduino menu and select "Add a library to the selected project" and
click OK to force sync the libraries picked up by the project.

### Enabling Arduino Eclipse's built in Encoder library
The Encoder library is not enabled by default and can be enabled via the above steps for
disabling the LiquidCrystal library but instead locating "Signal Input/Output / Encoder"
and selecting the latest version from the dropdown list.

# Using The Throttle
Connect the serial write pin from the pro mini to the DCC++ Base Station serial input and
provide power to the pro mini.  On first startup the throttle should prompt for a throttle
id number, if it doesn't you can go into the menu and set the throttle id.

## Setting Throttle ID
Press __*__ to enter the menu, scroll through the options to "Throttle Config" and select the
"Throttle ID" option. The Throttle ID is used as the register number in the messages sent to
the DCC++ Base Station.  Default range for registers is 1-12.

## Using the Menu
Press the __*__ key to enter the menu.  Exit the menu by pressing __*__ or __#__ from any screen.
The __2__ key will scroll UP, __8__ will scroll DOWN, __6__ will select the current item.

## Selecting Locomotive
Press the __#__ key to rotate between the known locomotives, by default up to 4 locomotives
can be controlled from the throttle.

## Toggling Locomotive Functions
Digits __0__-__9__ will toggle those functions, __10__-__28__ can be toggled via the menu under the "Functions"
section.