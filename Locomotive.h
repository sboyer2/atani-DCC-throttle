/*
 * Locomotive.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mdunston
 */

#ifndef LOCOMOTIVE_H_
#define LOCOMOTIVE_H_

class Locomotive {
public:
	Locomotive();
	virtual ~Locomotive();

	void setAddress(uint16_t address);
	uint16_t getAddress() {
		return _address;
	}

	void setSpeed(uint8_t speed) {
		if(speed > 126) {
			_speed = 126;
		} else if (speed < 0) {
			_speed = 0;
		} else {
			_speed = speed;
		}
		sendLocoCommand();
	}

	uint8_t getSpeed() {
		return _speed;
	}

	void toggleDirection() {
		_direction = !_direction;
	}

	void setDirection(uint8_t direction) {
		_direction = direction;
	}

	uint8_t getDirection() {
		return _direction;
	}

	uint8_t getFunctions0To4() {
		return _function0to4;
	}

	uint8_t getFunctinos5To8() {
		return _function5to8;
	}

	bool toggleFunction(int functionNumber);
private:
	enum LOCO_FUNCTION_SET {
		FUNCTION_0_TO_4,
		FUNCTION_5_TO_8,
		FUNCTION_9_TO_12,
		FUNCTION_13_TO_20,
		FUNCTION_21_TO_28
	};
	void sendLocoCommand();
	void sendLocoFunctionCommand(LOCO_FUNCTION_SET set);

	uint16_t _address;
	uint8_t _direction;
	uint8_t _speed;
	uint8_t _function0to4;
	uint8_t _function5to8;
	uint8_t _function9to12;
	uint8_t _function13to20;
	uint8_t _function21to28;
};

#endif /* LOCOMOTIVE_H_ */
