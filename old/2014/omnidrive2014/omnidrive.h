/*
    omnidrive - Library for omnidirectional movement.

    Includes linear acceleration profile to prevent motor slippage.
    Created by Brian Chen 05/01/2014
    Last Modified by Brian Chen 12/08/2014 7:21:09 PM

    Alpha 1.0
*/


#ifndef omnidrive_h
#define omnidrive_h

#include "Arduino.h"
#include "pwmMotor.h"

#define MAX             255
#define MOTOR_MIN_SPEED 0
//BETWEEN 0.0002 - 0.0005
//#define MAX_ACCEL_RATE  0.00035      // MOTOR POWER UNITS/us
//#define ACCEL_GRADIENT  0.0006       // Acceleration gradient for linear acceleration (i.e. parabolic speed)
#define MAX_ACCEL_RATE 0.0005
#define ACCEL_GRADIENT 0.002

class OMNIDRIVE
{
public:
    OMNIDRIVE(PMOTOR &_motorA, PMOTOR &_motorB, PMOTOR &_motorC, PMOTOR &_motorD);
    int16_t move(int16_t angle, uint8_t speed, int16_t angularVelocity);
	//current motor directional speeds (without angular velocity)
	float lpA = 0, lpB = 0, lpC = 0;
	//previous targetr speeds (without angular velocity)
	float lTarget_pA = 0, lTarget_pB = 0, lTarget_pC = 0;
private:
    PMOTOR *motorA, *motorB, *motorC, *motorD;     
	unsigned long lCallTime = 0;
};
#endif

