/*
 * config.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mdunston
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#undef _THROTTLE_DEBUG

// DCC Throttle version number
#define THROTTLE_VERSION "1.0"

#define EEPROM_INDEX_THROTTLE_ID 0

// defines the maximum number of locomotives that this throttle can control
#define MAX_LOCOS 4

// defines the maximum number of registers that the DCC++ Base Station
// supports.
#define MAX_THROTTLE_ID 12

// defines the maximum number of functions that can be toggled via the menu
#define MAX_FUNCTIONS 28

// defines the number of milliseconds that error messages will be displayed
// on the lcd before the throttle returns to normal operations.
#define ERROR_DISPLAY_TIMEOUT 1500

// turn off interrupts since we are not using the encoder on ports 2/3
#define ENCODER_DO_NOT_USE_INTERRUPTS

// defines the I2C address of the LCD screen, you will need to adjust this
// to match your LCD screen.  Use the I2C scanner to find the correct value
#define LCD_I2C_ADDR 0x3F

// defines which pins LiquidCrystal_I2C should use for various functions
// these may or may not need adjustment based on your LCD screen
#define LCD_BACKLIGHT_PIN 3
#define LCD_ENABLE_PIN 2
#define LCD_RW_PIN 1
#define LCD_REGSELECT_PIN 0
#define LCD_DATA_4_PIN 4
#define LCD_DATA_5_PIN 5
#define LCD_DATA_6_PIN 6
#define LCD_DATA_7_PIN 7

// defines the size of the keypad as 4 rows of 3 buttons
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3

// defines which pins the keypad rows are connected to
#define KEYPAD_ROW_1_PIN 6
#define KEYPAD_ROW_2_PIN 7
#define KEYPAD_ROW_3_PIN 8
#define KEYPAD_ROW_4_PIN 9

// defines which pins the keypad columns are connected to
#define KEYPAD_COL_1_PIN 10
#define KEYPAD_COL_2_PIN 11
#define KEYPAD_COL_3_PIN 12

// defines the button on rotary encoder is connected to
#define DIRECTION_BUTTON_PIN 5

// defines the pins that the rotary encoder is connected to
#define ENCODER_PIN_A 3
#define ENCODER_PIN_B 4

// minimum number of milliseconds that must pass before reporting
// a new speed value to the base station
#define ENCODER_REPORTING_DELAY 100

// defines the minimum limit for the rotary encoder to allow
#define ENCODER_MINIMUM_LIMIT 0

// defines the maximum limit for the rotary encoder to allow
#define ENCODER_MAXIMUM_LIMIT 126

// defines a divider for the maximum limit, this should be set to
// ENCODER_MAXIMUM_LIMIT/126 (ie: 100ppm encoder that provides 4
// detents per rotation should use 504 for ENCODER_MAXIMUM_LIMIT
// and set this value to 4
#define ENCODER_MAXIMUM_LIMIT_DIVIDER 1

#endif /* CONFIG_H_ */
