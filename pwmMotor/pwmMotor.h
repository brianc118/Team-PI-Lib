/* pwmMotor.h - library for motor control
 * Created by Brian Chen 05/01/2014
 *
 * Last Modified by Brian Chen 16/01/2015 13:09
 * v1.2
 *
 * (C) 2014 Team Pi 
 */

#ifndef pwmMotor_h
#define pwmMotor_h

#include "Arduino.h"

class PMOTOR
{
public:
	PMOTOR();
	PMOTOR(uint8_t pwm_pin, uint8_t direction_pin, uint8_t brk_pin, bool dir, uint8_t cs_pin);
	PMOTOR(uint8_t pwm_pin, uint8_t direction_pin, uint8_t brk_pin, bool dir);
	void begin(uint16_t freq);
	void move(int16_t input);
	uint8_t getCurrent();
private:
	uint8_t pwm;	// pwm pin
	uint8_t d;		// direction pin
	uint8_t b;		// brake pin
	uint8_t cs;	 // current sense pin
	uint8_t DHIGH;	// values to store high/low values for direction control
	uint8_t DLOW;	
};

#endif
