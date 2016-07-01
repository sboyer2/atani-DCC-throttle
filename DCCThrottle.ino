
#include "config.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <Encoder.h>

// LCD.h and LiquidCrystal_I2C.h come from: https://bitbucket.org/fmalpartida/new-liquidcrystal
#include "NewliquidCrystal/LCD.h"
#include "NewliquidCrystal/LiquidCrystal_I2C.h"

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

Encoder speedEncoder(ENCODER_PIN_A, ENCODER_PIN_B);
int directionButtonState = 0;
int throttleId = -1;

MenuSystem menuSystem;
bool exitMenu = false;

int inputNumber(const String prompt, int length) {
	return inputNumber(prompt, "", length);
}

int inputNumber(const String promptLine1, const String promptLine2, int length) {
	int result = 0;
	int digitCount = 0;
	lcd.clear();
	lcd.home();
	lcd.print(promptLine1);
	lcd.setCursor(0, 1);
	if(promptLine2.length()) {
		lcd.print(promptLine2);
	}
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

void showErrorScreen(const String line1, const String line2) {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(line1);
	if(line2.length()) {
		lcd.setCursor(0, 1);
		lcd.print(line2);
	}
	delay(ERROR_DISPLAY_TIMEOUT);
}

void onSelectFunctions(MenuItem* p_menu_item) {
	int functionNumber = inputNumber(F("Enter function #:"), F("(Max ") + String(MAX_FUNCTIONS) + F("):"), 2);
	if(functionNumber != -1) {
		if(functionNumber <= MAX_FUNCTIONS) {
			if(!currentLoco->toggleFunction(functionNumber)) {
				showErrorScreen(F("Invalid function"), F("Max function:")+String(MAX_FUNCTIONS, DEC));
			}
		} else {
			showErrorScreen(F("Invalid function"), F("Max function:")+String(MAX_FUNCTIONS, DEC));
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
	int turnoutNumber = inputNumber(F("Turnout # "), 4);
	if(turnoutNumber != -1) {
		// TODO: add input to set the expected direction, can we avoid throwing a
		// switch if it is already aligned correctly?
		exitMenu = true;
	}
}

void onSetThrottleId(MenuItem* p_menu_item) {
	int newThrottleId = inputNumber(F("Enter new ID"), F("(Max ") + String(MAX_THROTTLE_ID) + F("):"), 2);
	if(throttleId != newThrottleId && newThrottleId != -1) {
		if(newThrottleId <= MAX_THROTTLE_ID) {
			throttleId = newThrottleId;
		} else {
			showErrorScreen(F("ID ") + String(newThrottleId, DEC) + F(" is invalid"), F("Max ID:")+String(MAX_THROTTLE_ID, DEC));
		}
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

	delay(250);
	throttleId = EEPROM.read(EEPROM_INDEX_THROTTLE_ID);
	if (throttleId <= 0 || throttleId > MAX_THROTTLE_ID) {
		onSetThrottleId(NULL);
		EEPROM.write(EEPROM_INDEX_THROTTLE_ID, throttleId);
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
