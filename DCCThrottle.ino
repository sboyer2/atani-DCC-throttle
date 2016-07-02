
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
uint8_t rowPins[KEYPAD_ROWS] = {KEYPAD_ROW_1_PIN, KEYPAD_ROW_2_PIN, KEYPAD_ROW_3_PIN, KEYPAD_ROW_4_PIN};
uint8_t colPins[KEYPAD_COLS] = {KEYPAD_COL_1_PIN, KEYPAD_COL_2_PIN, KEYPAD_COL_3_PIN};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

Encoder speedEncoder(ENCODER_PIN_A, ENCODER_PIN_B);
int lastDirectionButtonState = HIGH;
uint8_t throttleId = -1;

MenuSystem menuSystem;
bool exitMenu = false;

long lastSpeedEncoderUpdate = 0L;

int inputNumber(const String prompt, int length) {
	return inputNumber(prompt, "", length);
}

uint16_t inputNumber(const String promptLine1, const String promptLine2, int length) {
	uint16_t result = 0;
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
			result = 0xFFFF;
			done = true;
		} else if (key == '#') {
			done = true;
		} else if (key != NO_KEY) {
			if (--length <= 0) {
				done = true;
			}
			int digit = key - '0';
			result = result * 10 + digit;
			lcd.setCursor(promptLine2.length(), 1);
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
	uint16_t functionNumber = inputNumber(F("Enter function #:"), String(F("(Max ")) + String(MAX_FUNCTIONS) + String(F("):")), 2);
	if(functionNumber != 0xFFFF) {
		if(functionNumber <= MAX_FUNCTIONS) {
			if(!currentLoco->toggleFunction(functionNumber)) {
				showErrorScreen(F("Invalid function"), String(F("Max function:"))+String(MAX_FUNCTIONS, DEC));
			}
		} else {
			showErrorScreen(F("Invalid function"), String(F("Max function:"))+String(MAX_FUNCTIONS, DEC));
		}
		exitMenu = true;
	}
}

void onSelectLoco(MenuItem* p_menu_item) {
	uint16_t newLocoAddress = inputNumber(F("Acquire Loco # "), 5);
	if(currentLoco->getAddress() != newLocoAddress && newLocoAddress != 0xFFFF) {
		currentLoco->setAddress(newLocoAddress);
		exitMenu = true;
	}
}

void onReleaseLoco(MenuItem* p_menu_item) {
	currentLoco->setAddress(0);
	exitMenu = true;
}

void onSelectTurnouts(MenuItem* p_menu_item) {
	uint16_t turnoutNumber = inputNumber(F("Turnout # "), 4);
	if(turnoutNumber != 0xFFFF) {
		// TODO: add input to set the expected direction, can we avoid throwing a
		// switch if it is already aligned correctly?
		exitMenu = true;
	}
}

void onSetThrottleId(MenuItem* p_menu_item) {
	uint16_t newThrottleId = inputNumber(F("Enter new ID"), String(F("(Max ")) + String(MAX_THROTTLE_ID) + String(F("):")), 2);
	if(throttleId != newThrottleId && newThrottleId != 0xFFFF) {
		if(newThrottleId <= MAX_THROTTLE_ID) {
			throttleId = newThrottleId;
		} else {
			showErrorScreen(String(F("ID ")) + String(newThrottleId, DEC) + String(F(" is invalid")), String(F("Max ID:"))+String(MAX_THROTTLE_ID, DEC));
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
	long currentmillis = millis();
	int8_t speedEncoderReading = speedEncoder.read();
	// check if the user has spun the rotary encoder very fast in either
	// direction and if so force it to be within the limits
	if(speedEncoderReading <= ENCODER_MINIMUM_LIMIT) {
		speedEncoderReading = ENCODER_MINIMUM_LIMIT;
		speedEncoder.write(speedEncoderReading);
	} else if(speedEncoderReading >= ENCODER_MAXIMUM_LIMIT) {
		speedEncoderReading = ENCODER_MAXIMUM_LIMIT;
		speedEncoder.write(speedEncoderReading);
	}
	// read the speed again and constrain it to the configured limits
	speedEncoderReading = constrain(speedEncoderReading, ENCODER_MINIMUM_LIMIT, ENCODER_MAXIMUM_LIMIT);
	// divide the speedEncoderReading to bring it down into range.
	speedEncoderReading /= ENCODER_MAXIMUM_LIMIT_DIVIDER;
	// check if the reading is different than our last recorded speed
	// and that it has been long enough since we updated it last.
	if (speedEncoderReading != currentLoco->getSpeed() &&
			(currentmillis > (lastSpeedEncoderUpdate + ENCODER_REPORTING_DELAY))) {
		lastSpeedEncoderUpdate = currentmillis;
		if(speedEncoderReading > 126) {
			speedEncoderReading = 126;
		}
		currentLoco->setSpeed(speedEncoderReading);
		updateLCD();
	}
	// check the direction button on the encoder
	int directionButtonState = digitalRead(DIRECTION_BUTTON_PIN);
	// if the direction button is not what we saw last time and
	// it has been longer than configured delay send it on to the
	// locomotive
	if(directionButtonState && !lastDirectionButtonState) {
		currentLoco->toggleDirection();
		updateLCD();
		// reset to default since we just toggled dirction
		lastDirectionButtonState = HIGH;
	} else {
		lastDirectionButtonState = directionButtonState;
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
