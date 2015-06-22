/*
	TSOP - Library for array of TSOPS
	Created by Brian Chen 14/09/2013
	Last Modified by Brian Chen 05/01/2014 11:59pm

	This library has been updated for robocup soccer 2014

	Release 1.10
*/

#ifndef TSOP_h
#define TSOP_h

#include "Arduino.h"

#define MAX_COUNT 127	//maximum number of readings per tsop
#define TSOP_COUNT 20	//number of tsops
#define MIN_IGNORE_THRESHOLD 15	//minimum tsop reading to be deemed as valid, anything below is noise
#define MAX_IGNORE_THRESHOLD 200	//maximum tsop reading to be deemed as valid, anything above is noise or a disconnected sensor
#define FAST_MODE false

#if(MAX_COUNT > 255)
	#error "MAX_COUNT cannot exceed the range of uint8_t"
#endif

class TSOP
{
    public:
	    TSOP(const uint8_t tsops[], uint8_t vcc);
        inline void on();
		inline void off();
		void getAll(uint8_t *results);
		void getAllInvert(uint8_t *results);
		void getAllPulseIn(uint8_t *results);
		void getBest(uint8_t *results, uint8_t &index, uint8_t &value);
		void getAngle(uint8_t *results, float &angle);
		void getAngleAdv(uint8_t *results, float &angle);
		void getAngleReg(uint8_t *results, float &angle, uint8_t b);
		void getStrength(uint8_t *results, uint8_t &strength);
		void getAnglePairing(uint8_t *results, float &angle);
		void unlock();
    
        const uint8_t *tsop_pins;
        bool isOn;	//state of tsops, on or off
    private:
        uint8_t _vcc;	
};

#endif

