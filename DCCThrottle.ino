
#include "config.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <Encoder.h>

// LCD.h and LiquidCrystal_I2C.h come from: https://bitbucket.org/fmalpartida/new-liquidcrystal
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

// Keypad.h comes from: http://playground.arduino.cc/Code/Keypad
#include "Keypad/src/Keypad.h"

// MenuSystem.h comes from: http://blog.humblecoder.com/arduino-menu-system-library/
// modified to use "const __FlashStringHelper *" instead of "const char *" for menu item names.
#include "MenuSystem/MenuSystem.h"

#include "Locomotive.h"

#include "printf.h"

Locomotive locos[MAX_LOCOS];
int currentLocoIndex = 0;
Locomotive *currentLoco = &locos[currentLocoIndex];

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_ENABLE_PIN, LCD_RW_PIN, LCD_REGSELECT_PIN,
		LCD_DATA_4_PIN, LCD_DATA_5_PIN, LCD_DATA_6_PIN, LCD_DATA_7_PIN,
		LCD_BACKLIGHT_PIN, POSITIVE);

const char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
		{ '1', '2', '3' },
		{ '4', '5', '6' },
		{ '7', '8', '9' },
		{ '*', '0', '#' } };
byte rowPins[KEYPAD_ROWS] = {KEYPAD_ROW_1_PIN, KEYPAD_ROW_2_PIN, KEYPAD_ROW_3_PIN, KEYPAD_ROW_4_PIN};
byte colPins[KEYPAD_COLS] = {KEYPAD_COL_1_PIN, KEYPAD_COL_2_PIN, KEYPAD_COL_3_PIN};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

// since we are using INTERRUPTS we need to use pin 2/3
Encoder speedEncoder(2, 3);
int directionButtonState = 0;
int throttleId = -1;

MenuSystem menuSystem;
bool exitMenu = false;

int inputNumber(const __FlashStringHelper * prompt, int length) {
	int result = 0;
	int digitCount = 0;
	lcd.clear();
	lcd.home();
	lcd.print(prompt);
	bool done = false;
	do {
		char key = keypad.getKey();
		if (key == '*') {
			result = -1;
			done = true;
		} else if (key == '#') {
			done = true;
		} else if (key != NO_KEY) {
			digitCount++;
			if (digitCount > length) {
				done = true;
			}
			int digit = key - '0';
			result = result * 10 + digit;
			lcd.setCursor(0, 1);
			lcd.print(result);
		}
	} while (!done);
	return result;
}

void onSelectFunctions(MenuItem* p_menu_item) {
	int functionNumber = inputNumber(F("Enter function #:"), 2);
	if(functionNumber != -1) {
		if(!currentLoco->toggleFunction(functionNumber)) {
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print(F("Invalid function"));
			delay(500);
		}
		exitMenu = true;
	}
}

void onSelectLoco(MenuItem* p_menu_item) {
	int newLocoAddress = inputNumber(F("Acquire Loco # "), 4);
	if(currentLoco->getAddress() != newLocoAddress && newLocoAddress != -1) {
		currentLoco->setAddress(newLocoAddress);
		exitMenu = true;
	}
}

void onReleaseLoco(MenuItem* p_menu_item) {
	currentLoco->setAddress(0);
	exitMenu = true;
}

void onSelectTurnouts(MenuItem* p_menu_item) {

}

void onSetThrottleId(MenuItem* p_menu_item) {
	int newThrottleId = inputNumber(F("Enter new id:"), 2);
	if(throttleId != newThrottleId && newThrottleId != -1) {
		throttleId = newThrottleId;
		exitMenu = true;
	}
}

void onSelectPowerOffTrack(MenuItem* p_menu_item) {
	// TODO: convert to RS485 bus command
	printf(F("<0>"));
}

void onSelectPowerOnTrack(MenuItem* p_menu_item) {
	// TODO: convert to RS485 bus command
	printf(F("<1>"));
}

void printf(const __FlashStringHelper *fmt, ...) {
	char buf[128]; // resulting string limited to 128 chars
	va_list args;
	va_start(args, fmt);
	vsnprintf_P(buf, sizeof(buf), (const char *) fmt, args); // progmem for AVR
	va_end(args);
	Serial.print(buf);
}

void setup() {
	Menu * rootMenu = new Menu(F("DCC++ Throttle"));
	menuSystem.set_root_menu(rootMenu);
	rootMenu->add_item(new MenuItem(F("Acquire Loco")), &onSelectLoco);
	rootMenu->add_item(new MenuItem(F("Turnouts")), &onSelectTurnouts);
	rootMenu->add_item(new MenuItem(F("Release Loco")), &onReleaseLoco);
	rootMenu->add_item(new MenuItem(F("Functions")), &onSelectFunctions);
	rootMenu->add_item(new MenuItem(F("Power Off Track")), &onSelectPowerOffTrack);
	rootMenu->add_item(new MenuItem(F("Power On Track")), &onSelectPowerOnTrack);
	Menu *configMenu = new Menu(F("Throttle Config"));
	configMenu->add_item(new MenuItem(F("Throttle ID")), &onSetThrottleId);
	rootMenu->add_menu(configMenu);
	pinMode(DIRECTION_BUTTON_PIN, INPUT);
	lcd.begin(16,2);
	lcd.home(); // go home
	lcd.setBacklight(HIGH);  // Switch on the backlight
	Serial.begin(9600);
	lcd.clear();
	lcd.print(F("DCC++ Throttle"));
	lcd.setCursor(0, 1);
	lcd.print(F(__DATE__));
	lcd.print(F(" v"));
	lcd.print(F(THROTTLE_VERSION));

#ifdef _THROTTLE_DEBUG
	printf(F("DCC++ Throttle\n%s  version %s\n"), __DATE__, THROTTLE_VERSION);
#endif
	delay(250);
	throttleId = EEPROM.read(0);
	if ( throttleId <= 0 || throttleId > 10) {
		onSetThrottleId(NULL);
		EEPROM.write(0, throttleId);
	}
	lcd.clear();
	updateLCD();
}

void processMenu() {
	menuSystem.reset();
	exitMenu = false;
	char key = NO_KEY;
	bool showMenu = true;
	while (!exitMenu) {
		if (showMenu) {
			lcd.clear();
			lcd.home();
			lcd.print(menuSystem.get_current_menu()->get_name());
			lcd.setCursor(0, 1);
			lcd.print(menuSystem.get_current_menu()->get_selected()->get_name());
			showMenu = false;
		}
		key = keypad.getKey();
		switch (key) {
		case '2':
			menuSystem.prev();
			showMenu = true;
			break;
		case '8':
			menuSystem.next();
			showMenu = true;
			break;
		case '4':
			exitMenu = !menuSystem.back();
			showMenu = true;
			break;
		case '6':
			menuSystem.select();
			showMenu = true;
			break;
		case '*':
		case '#':
			exitMenu = true;
			break;
		default:
			key = NO_KEY;
		}
	}
}

void loop() {
	int speedEncoderValue = constrain(speedEncoder.read(), 0, 126);
	if (speedEncoderValue != currentLoco->getSpeed()) {
		currentLoco->setSpeed(speedEncoderValue);
		updateLCD();
	}
	directionButtonState = digitalRead(DIRECTION_BUTTON_PIN);
	if (directionButtonState == LOW) {
		delay(50);
		directionButtonState = digitalRead(DIRECTION_BUTTON_PIN); // check a 2nd time to be sure
		if (directionButtonState == LOW) { // check a 2nd time to be sure
			currentLoco->setDirection(!currentLoco->getDirection());
			updateLCD();
			do { // routine to stay here till button released & not toggle direction
				directionButtonState = digitalRead(DIRECTION_BUTTON_PIN);
			} while (directionButtonState == LOW);
		}
	}
	char key = keypad.getKey();
	if (key == '*') {
		processMenu();
		lcd.clear();
		updateLCD();
	} else if (key == '#') {
		currentLocoIndex++;
		currentLocoIndex %= MAX_LOCOS;
		currentLoco=&locos[currentLocoIndex];
		updateLCD();
	} else if (key != NO_KEY) {
		currentLoco->toggleFunction(key - '0');
		updateLCD();
	}
}

void updateLCD() {
	lcd.setCursor(0, 0);
	lcd.print(F("L="));
	String address = "0000" + String(currentLoco->getAddress(), DEC);
	lcd.print(address.substring(address.length() - 4));
	if (currentLoco->getDirection()) {
		lcd.print(F(" FWD "));
	} else {
		lcd.print(F(" REV "));
	}
	lcd.print(F("S="));
	String speed = String(F("0000")) + String(currentLoco->getSpeed(), DEC);
	lcd.print(speed.substring(speed.length() - 3));
	String functions5to8 = String(F("000")) + String(currentLoco->getFunctinos5To8(), BIN);
	lcd.setCursor(0, 1); // start of 2nd line
	lcd.print(functions5to8.substring(functions5to8.length() - 4));
	lcd.print(F(" "));
	String functions0to4 = String(F("0000")) + String(currentLoco->getFunctions0To4(), BIN);
	lcd.print(functions0to4.substring(functions0to4.length() - 5));
	lcd.print(F(" #="));
	lcd.print(currentLocoIndex + 1, DEC);
	lcd.print(F(":"));
	lcd.print(throttleId, DEC);
}