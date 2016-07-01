/*
 * Locomotive.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: mdunston
 */


#include <Arduino.h>
#include "Locomotive.h"
#include "printf.h"

Locomotive::Locomotive() : _address(3), _direction(1), _speed(0), _function0to4(0), _function5to8(0), _function9to12(0), _function13to20(0), _function21to28(0) {
	// TODO Auto-generated constructor stub

}

Locomotive::~Locomotive() {
	// TODO Auto-generated destructor stub
}

void Locomotive::setAddress(short address) {
	short oldAddress = _address;
	_address = address;
	if(address != 0) {
		// TODO: send aquire loco command to base station.
		_function0to4 = B00000000;
		_function5to8 = B00000000;
		_function9to12 = B00000000;
		_function13to20 = B00000000;
		_function21to28 = B00000000;
		sendLocoFunctionCommand(FUNCTION_0_TO_4);
		sendLocoFunctionCommand(FUNCTION_5_TO_8);
		sendLocoFunctionCommand(FUNCTION_9_TO_12);
		sendLocoFunctionCommand(FUNCTION_13_TO_20);
		sendLocoFunctionCommand(FUNCTION_21_TO_28);
	} else {
		// TODO: send release loco command to base station.
	}
}

bool Locomotive::toggleFunction(int functionNumber) {
#ifdef _THROTTLE_DEBUG
	printf(F("Toggling function: %d\n"), functionNumber);
#endif
	if (functionNumber <= 4) {
		if (functionNumber == 0) {
			bitWrite(_function0to4, 4, !bitRead(_function0to4, 4));
		} else {
			functionNumber--;
			bitWrite(_function0to4, functionNumber, !bitRead(_function0to4, functionNumber));
		}
		sendLocoFunctionCommand(FUNCTION_0_TO_4);
	} else if (functionNumber <= 8) {
		functionNumber -= 5;
		bitWrite(_function5to8, functionNumber, !bitRead(_function5to8, functionNumber));
		sendLocoFunctionCommand(FUNCTION_5_TO_8);
	} else if (functionNumber <= 12) {
		functionNumber -= 9;
		bitWrite(_function9to12, functionNumber, !bitRead(_function9to12, functionNumber));
		sendLocoFunctionCommand(FUNCTION_9_TO_12);
	} else if (functionNumber <= 20) {
		functionNumber -= 13;
		bitWrite(_function13to20, functionNumber, !bitRead(_function13to20, functionNumber));
		sendLocoFunctionCommand(FUNCTION_13_TO_20);
	} else if (functionNumber <= 28) {
		functionNumber -= 21;
		bitWrite(_function21to28, functionNumber, !bitRead(_function21to28, functionNumber));
		sendLocoFunctionCommand(FUNCTION_21_TO_28);
	} else {
		return false;
	}
	return true;
}

void Locomotive::sendLocoCommand() {
	printf(F("<t1 %d %d %d>"), _address, _speed, _direction);
}

void Locomotive::sendLocoFunctionCommand(LOCO_FUNCTION_SET set) {
		switch (set) {
		case FUNCTION_0_TO_4:
			printf(F("<f %d %d>"), _address, _function0to4 + 128);
			break;
		case FUNCTION_5_TO_8:
			printf(F("<f %d %d>"), _address, _function5to8 + 176);
			break;
		case FUNCTION_9_TO_12:
			printf(F("<f %d %d>"), _address, _function9to12 + 160);
			break;
		case FUNCTION_13_TO_20:
			printf(F("<f %d 222 %d>"), _address, _function13to20);
			break;
		case FUNCTION_21_TO_28:
			printf(F("<f %d 223 %d>"), _address, _function21to28);
			break;
		}
	}
