/*
 * config.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mdunston
 */

#ifndef CONFIG_H_
#define CONFIG_H_


#undef _THROTTLE_DEBUG

// button on rotary encoder
#define DIRECTION_BUTTON_PIN 13

#define THROTTLE_VERSION "1.0"

// defines the maximum number of locomotives that this throttle can control
#define MAX_LOCOS 4

// turn of interrupts since we are not using the encoder on ports 2/3
#define ENCODER_DO_NOT_USE_INTERRUPTS

#define LCD_I2C_ADDR 0x3F
#define LCD_BACKLIGHT_PIN 3
#define LCD_ENABLE_PIN 2
#define LCD_RW_PIN 1
#define LCD_REGSELECT_PIN 0
#define LCD_DATA_4_PIN 4
#define LCD_DATA_5_PIN 5
#define LCD_DATA_6_PIN 6
#define LCD_DATA_7_PIN 7

#define KEYPAD_ROWS 4 //four rows
#define KEYPAD_COLS 3 //three columns

#define KEYPAD_ROW_1_PIN 4
#define KEYPAD_ROW_2_PIN 5
#define KEYPAD_ROW_3_PIN 6
#define KEYPAD_ROW_4_PIN 7

#define KEYPAD_COL_1_PIN 10
#define KEYPAD_COL_2_PIN 11
#define KEYPAD_COL_3_PIN 12

#endif /* CONFIG_H_ */
