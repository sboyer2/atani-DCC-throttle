/*
 * printf.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: mdunston
 */

#include <Arduino.h>

// needed for printf usage
#include <stdarg.h>


void printf(const __FlashStringHelper *fmt, ...) {
	char buf[128]; // resulting string limited to 128 chars
	va_list args;
	va_start(args, fmt);
	vsnprintf_P(buf, sizeof(buf), (const char *) fmt, args); // progmem for AVR
	va_end(args);
	Serial.print(buf);
}
