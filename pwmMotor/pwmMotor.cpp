/* pwmMotor.cpp - library for motor control
 * Created by Brian Chen 05/01/2014
 *
 * Last Modified by Brian Chen 16/01/2015 13:09
 * v1.2
 *
 * (C) 2014 Team Pi 
 */


#include "Arduino.h"
#include "pwmMotor.h"

PMOTOR::PMOTOR(uint8_t pwm_pin, uint8_t direction_pin, uint8_t brk_pin, bool dir, uint8_t cs_pin){
	pwm = pwm_pin;
	d = direction_pin;
	b = brk_pin;
	
	if (!dir){
		DHIGH = LOW;
		DLOW = HIGH;
	}
	else{
		DHIGH = HIGH;
		DLOW = LOW;
	}
	cs = cs_pin;
}
PMOTOR::PMOTOR(uint8_t pwm_pin, uint8_t direction_pin, uint8_t brk_pin, bool dir)
{
	PMOTOR(pwm_pin, direction_pin, brk_pin, dir, 255);
}

void PMOTOR::begin(uint16_t freq){
	if (!freq == 0){
		analogWriteFrequency(pwm, freq);
	}
	pinMode(pwm, OUTPUT);
	pinMode(d, OUTPUT);
	pinMode(b, OUTPUT);
}

//simple move function to drive the motor
void PMOTOR::move(int16_t input){
	if(input < 0){
		digitalWriteFast(d,DHIGH);	
		digitalWriteFast(b,LOW);
		analogWrite(pwm,abs(input));
	}
	else if(input > 0){
		digitalWriteFast(d,DLOW);
		digitalWriteFast(b,LOW);
		analogWrite(pwm,abs(input));
	}
	else{
		//following code used to enable brake. Comment out if necessary.
		digitalWrite(b,HIGH);
		analogWrite(pwm,abs(input));
	}	
}

uint8_t PMOTOR::getCurrent(){
	if (cs == 255){
		// current sense pin not set.
		return 0;
	}
	analogReadResolution(8);
	return analogRead(0);
}

/*
#define MOVEMOTOR(input, d, b, pwm){    \
	if (input < 0){                     \
		digitalWriteFast(d, HIGH);      \
		digitalWriteFast(b, LOW);       \
		analogWrite(pwm, abs(input));   \
	} else if(input > 0){               \
		digitalWriteFast(d,DLOW);       \
		digitalWriteFast(b,LOW);        \
		analogWrite(pwm,abs(input));    \
	} else{                             \
		digitalWrite(b,HIGH);           \
		analogWrite(pwm,abs(input)); }} \
		*/