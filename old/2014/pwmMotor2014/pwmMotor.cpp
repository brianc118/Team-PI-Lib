/*
    omnidrive - Library for omnidirectional movement
    Created by Brian Chen 05/01/2014
    Last Modified by Brian Chen 06/01/2014 2:26pm

    Beta 0.7
*/


#include "Arduino.h"
#include "pwmMotor.h"

PMOTOR::PMOTOR(uint8_t pwm_pin, uint8_t direction_pin, uint8_t brk_pin, bool dir, uint16_t freq)
{
	pwm = pwm_pin;
	d = direction_pin;
	b = brk_pin;
	if (!freq == 0){
		analogWriteFrequency(pwm, freq);
	}
	pinMode(pwm, OUTPUT);
	pinMode(d, OUTPUT);
	pinMode(b, OUTPUT);

	if (!dir){
		DHIGH = LOW;
		DLOW = HIGH;
	}
	else{
		DHIGH = HIGH;
		DLOW = LOW;
	}
#if(SMOOTHING)
	lCallTime = millis();
#endif
}


//simple move function to drive the motor
void PMOTOR::move(int16_t input){
#if(SMOOTHING)
	unsigned long now = millis();
	uint16_t dt = now - lCallTime;
	lCallTime = now;
	if (input - currentSpeed > SMOOTHING_RATE * dt){
		input = currentSpeed + (int16_t)(SMOOTHING_RATE * dt);
	}
	else if (currentSpeed - input > SMOOTHING_RATE * dt){
		input = currentSpeed - (int16_t)(SMOOTHING_RATE * dt);
	}
	currentSpeed = input;
#endif
	if(input < 0){
		digitalWrite(d,DHIGH);	
		digitalWrite(b,LOW);
		analogWrite(pwm,abs(input));
	}
	else if(input > 0){
		digitalWrite(d,DLOW);
		digitalWrite(b,LOW);
		analogWrite(pwm,abs(input));
	}
	else{
		//following code used to enable brake. Comment out if necessary.
		digitalWrite(b,HIGH);
		analogWrite(pwm,abs(input));
	}	
}