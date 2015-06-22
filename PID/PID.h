/*
 * pid.h - PID control library
 *
 * PID library for Team Pi.
 *
 * Notes:
 *   - Supports changing the setpoint any time. Uses change in input instead of change in error
 *	 to remove derivative kick
 *   - Allows tuning changes via setTunings().
 * Changelog:
 *	 03/01/2015	Initial commit. Added basic functionality. Not yet tested.
 *	 04/01/2015	Fixed pointer issues and naming issues. Tested to compile in g++
 *
 * by Brian Chen
 * (C) Team Pi 2015
 */

#ifndef PID_H
#define PID_H

#include "Arduino.h"

class PID{

public:
	PID(int32_t* in, int32_t* out, int32_t* set, float p, 
		float i, float d, int min, int max, uint32_t updateInterval);
	uint8_t update();
	bool setTunings(float p, float i, float d);
	bool setOutLimits(int min, int max);
private:
	elapsedMicros elapsed;
	uint32_t interval;	// update interval in uS
	int outMin, outMax;
	int32_t *input;
	int32_t *output;
	int32_t *setpoint;
	float kp;		   // (P)roportional Tuning Parameter
	float ki;		   // (I)ntegral Tuning Parameter
	float kd;		   // (D)erivative Tuning Parameter
	int32_t ITerm;
	int32_t error;
	int32_t dInput, lInput;
	uint8_t _return = 0;
};

PID::PID(int32_t* in, int32_t* out, int32_t* set, float p, 
		 float i, float d, int min, int max, uint32_t updateInterval){
	input = in;
	output = out;
	setpoint = set;
	kp = p;
	ki = i;
	kd = d;
	outMin = min;
	outMax = max;
	interval = updateInterval;
}

uint8_t PID::update(){
	uint32_t deltaT = elapsed;
	// if (deltaT >= interval){
		elapsed = 0;

		_return = 0;

		// update error
		error = *setpoint - *input;

		ITerm += ki * error * deltaT;	// integral
		if (ITerm > outMax){
			ITerm = outMax;
			_return = 3;
		}
		else if (ITerm < outMin){
			ITerm = outMin;
			_return = 4;
		} 
		/* instead of measuring delta(error), we measure
		   delta(input) to prevent derivative kick	*/
		dInput = (*input - lInput) / deltaT;
		
		ITerm = 0;
		kd = 0;
		// compute output
		*output = (kp * error + ITerm - kd * (*input));

		if (*output > outMax){
			*output = outMax;
			_return = 1;
		}
		else if (*output < outMin){
			*output = outMin;
			_return = 2;
		} 
		lInput = *input;
		return _return;
	// }
	return 0;
}

bool PID::setTunings(float p, float i, float d){
	if (p < 0 || i < 0 || d < 0) return false;
	kp = p;
	ki = i * interval / 1000000;
	kd = d / (interval * 1000000);
	return true;
}

bool PID::setOutLimits(int min, int max){
	if (max < min) return false;
	outMin = min;
	outMax = max;

	if (ITerm > outMax){
		ITerm = outMax;
		return false;
	}
	else if (ITerm < outMin){
		ITerm = outMin;
		return false;
	} 
	if (*output > outMax){
		*output = outMax;
		return false;
	}
	else if (*output < outMin){
		*output = outMin;
		return false;
	}
	return true;
}

#endif