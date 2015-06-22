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


#include "GenEncoder.h"

//How many instances to allow.  Must match number of interrupt routines
GenEncoder *apEnc[2];	// Pointers back to correct instance

// Instance specific interrupt routines
void encInt1(void){
    apEnc[0]->encIntOrg();
}

void encInt2(void){
    apEnc[1]->encIntOrg();
}

// Array of pointers to interrupt routines
void (*GenEncoder::apInt[])() = {encInt1,encInt2};

// On IntervalTimer interrupt, generate next quadrature signal, 
//   counting up or down 
void GenEncoder::encIntOrg(void){

	// Quadrature counting sequence 
	if (zero){	// Count from zero, keep counting in same direction
		posn=0;
		bigLoop=true; 
		iCycle=BIG_CROSSINGS;
		maxPosn=MAX_POSN;
		minPosn=MIN_POSN;

		zero=false;
	}

	// Set max and min for loops
	if( posn == CYCLE_CENTER){	// Count when cross cycle point
	    if (iCycle-- <= 0){	// Should not get less than zero
		if (bigLoop){
		    bigLoop=false;
		    maxPosn= CYCLE_CENTER+CYCLE_MAX;
		    minPosn= CYCLE_CENTER+CYCLE_MIN;
		    iCycle=CYCLE_CROSSINGS;
		} else {
		    bigLoop=true;
		    maxPosn=MAX_POSN;
		    minPosn=MIN_POSN;
		    iCycle=BIG_CROSSINGS;
		}
	    }
	}
	if (up){
		if (posn < maxPosn){
		    enc++;
		    if (enc > 3){
			enc=0;
		    }
		    setEnc(enc);
	            posn++;
		
		}
		else{	// Max position reached
			up=false;
		}
	}
	else{
		if (posn > minPosn){
		    enc--;
		    if (enc <0){
			enc=3;
		    }
		    setEnc(enc);
			posn--;
		}
		else{	// Min position reached
			up=true;
		}
	}	
		
}

// Quadrature encoder output
void GenEncoder::setEnc(int8_t enc){
    if (enc==3){
	digitalWriteFast(outPinA,LOW);
	digitalWriteFast(outPinB,HIGH);
    }else if (enc==2){
	digitalWriteFast(outPinA,HIGH);
	digitalWriteFast(outPinB,HIGH);
    }else if (enc==1){
	digitalWriteFast(outPinA,HIGH);
	digitalWriteFast(outPinB,LOW);
    }else{		// enc==0 or other
	digitalWriteFast(outPinA,LOW);
	digitalWriteFast(outPinB,LOW);
    }
}


