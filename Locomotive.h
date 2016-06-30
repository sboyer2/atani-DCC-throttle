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

	void setAddress(short address);
	short getAddress() {
		return _address;
	}

	void setSpeed(byte speed) {
		_speed = speed;
		sendLocoCommand();
	}
	byte getSpeed() {
		return _speed;
	}
	void setDirection(byte direction) {
		_direction = direction;
	}
	byte getDirection() {
		return _direction;
	}
	byte getFunctions0To4() {
		return _function0to4;
	}
	byte getFunctinos5To8() {
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

	short _address;
	byte _direction;
	byte _speed;
	byte _function0to4;
	byte _function5to8;
	byte _function9to12;
	byte _function13to20;
	byte _function21to28;
};

#endif /* LOCOMOTIVE_H_ */
