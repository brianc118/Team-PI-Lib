/* 

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

// Generates quadrature encoder signals, counting up then down with the 
//   option of oscillating about a point.

#ifndef __GENENCODER_H__
#define __GENENCODER_H__

#include <stdint.h>
#include "mk20dx128.h"
#include "IntervalTimer.h"
#include "core_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

class GenEncoder;

// Generate Quadrature encoder signals, counting up to maxPosn, and then 
//   down to minPosn.  Crosses cycleCrossings bigCrossings times and then
//   oscillates around cycleCrossings between cycleMax and cycleMin.  
//   Oscillates for cycleCrossings times, and then goes back to the 
//   larger range.
//
struct GenEncData{
    int32_t maxPosn;
    int32_t minPosn;
    int32_t cycleMax;
    int32_t cycleMin;
    int32_t cycleCenter;
    int cycleCrossings;
    int bigCrossings;
};

extern GenEncoder *apEnc[];	// Pointers to correct instance

class GenEncoder{
    private:
    public:
	const uint32_t outPinA, outPinB;    // Pins to output signal
	const uint32_t periodIT;
	bool zero=true;	
	bool up=true;
	bool bigLoop=true;
	int8_t enc=0;	    // Encoder phase
	int32_t posn=0;	    // Generated position
	int iCycle;
	int32_t maxPosn;
	int32_t minPosn;
	const int32_t MAX_POSN;
	const int32_t MIN_POSN;
	const int32_t CYCLE_MAX;
	const int32_t CYCLE_MIN;
	const int32_t CYCLE_CENTER;
	const int CYCLE_CROSSINGS; 
	const int BIG_CROSSINGS;
	const int8_t nInst; //Instance number
	IntervalTimer newIT;	// Interval timer call int routine when
	                        //    times out
	GenEncoder(int pinA, int pinB,uint32_t per,
		                        GenEncData &d,uint8_t num) : 
		outPinA(pinA),outPinB(pinB),periodIT(per),
		iCycle(d.bigCrossings), maxPosn(d.maxPosn),
	       	minPosn(d.minPosn), 
		MAX_POSN(d.maxPosn),MIN_POSN(d.minPosn),
		CYCLE_MAX(d.cycleMax),CYCLE_MIN(d.cycleMin),
		CYCLE_CENTER(d.cycleCenter),
		CYCLE_CROSSINGS(d.cycleCrossings),
		BIG_CROSSINGS(d.bigCrossings),
		nInst(num){
			// Order of constructor execution
			//  not guaranteed, start with begin
	    apEnc[nInst]=this;
	    	};
	~GenEncoder(){newIT.end();};
	void setZero(){zero=true;};
	uint32_t getPosn(){return posn;};
	void encIntOrg(void);	// Interrupt routine
	void setEnc(int8_t);
	static void (*apInt[])();   // Pointers to correct Interrupt routine
	bool start(){
	    pinMode(outPinA, OUTPUT);
	    pinMode(outPinB, OUTPUT);
	    return newIT.begin(apInt[nInst],periodIT);
	};
};


#ifdef __cplusplus
}
#endif

#endif
	    	    


