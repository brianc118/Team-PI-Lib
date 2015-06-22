/*
    omnidrive - Library for omnidirectional movement
    Created by Brian Chen 05/01/2014
    Last Modified by Brian Chen 06/01/2014 2:26pm

    Beta 0.7
*/


#ifndef pwmMotor_h
#define pwmMotor_h

#include "Arduino.h"

#define SMOOTHING false
#define SMOOTHING_RATE 5 //smoothing rate in (power units)/ms. Must be >= 1.

class PMOTOR
{
public:
    PMOTOR(uint8_t pwm_pin, uint8_t direction_pin, uint8_t brk_pin, bool dir, uint16_t freq);
    void move(int16_t input);
private:
    uint8_t pwm;	//pwm pin
    uint8_t d;	//direction pin
    uint8_t b;	//brake pin
    uint8_t DHIGH;	//values to store high/low values for direction control
    uint8_t DLOW;
#if(SMOOTHING)
	unsigned long lCallTime = 0;
	int16_t currentSpeed = 0;
#endif
};

#endif
