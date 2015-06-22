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

//  Version 0.9 October 2014

#ifndef __QUADDECODEDEF_H__
#define __QUADDECODEDEF_H__


#include "QuadDecode.h"
#include "wiring.h"
#include "mk20dx128.h"
#include "usb_serial.h"
#include <inttypes.h>

// FTM Interrupt Service routines - on overflow and position compare
void ftm1_isr(void){
    apQDcd1->ftm_isr();
};

void ftm2_isr(void){
    apQDcd2->ftm_isr();
};

//  Constructor execution not guaranteed, need to explictly call setup
template <int N>
void QuadDecode<N>::setup(){
    // Set up input pins
    if (N<2){	//FTM1
	// K20 pin 28,29
	// Bit 8-10 is Alt Assignment
	PORTA_PCR12 = 0x00000712;   //Alt7-QD_FTM1,FilterEnable,Pulldown
	PORTA_PCR13 = 0x00000712;   //Alt7-QD_FTM1,FilterEnable,Pulldown
	//Default is input, don't need to write PDDR (Direction)

	//Can also set up on K20 pin 35, 36 - not tested
	//PORTB_PCR1 = 0x00000612;   //Alt6-QD_FTM1,FilterEnable,Pulldown
	//PORTB_PCR1 = 0x00000612;   //Alt6-QD_FTM1,FilterEnable,Pulldown
	//Default is input, don't need to write PDDR (Direction)

    }else{  // FTM2
	// K20 pin 41,42
	// Bit 8-10 is Alt Assignment
	PORTB_PCR18 = 0x00000612;   //Alt6-QD_FTM2,FilterEnable,Pulldown
	PORTB_PCR19 = 0x00000612;   //Alt6-QD_FTM2,FilterEnable,Pulldown
	//Default is input, don't need to write PDDR (Direction)
    };

    //Set FTMEN to be able to write registers
    FTM_MODE=0x04;	    // Write protect disable - reset value
    FTM_MODE=0x05;	    // Set FTM Enable

    // Set registers written in pins_teensy.c back to default
    FTM_CNT = 0;
    FTM_MOD = 0;
    FTM_C0SC =0;
    FTM_C1SC =0;
    FTM_SC = 0;

    // Set registers to count quadrature
    FTM_FILTER=0x22;	// 2x4 clock filters on both channels
    FTM_CNTIN=0;
    FTM_MOD=0xFFFF;	// Maximum value of counter
    FTM_CNT=0;		// Updates counter with CNTIN

    // Set Registers for output compare mode
    FTM_COMBINE=0;	    // Reset value, make sure
    FTM_C0SC=0x10;	    // Bit 4 Channel Mode
    FTM_C0V= COMP_LOW;	    // Initial Compare Interrupt Value

    FTM_QDCTRL=0b11000001;	    // Quadrature control
    //        Filter enabled, QUADEN set
	    
    // Write Protect Enable
    FTM_FMS=0x40;		// Write Protect, WPDIS=1
};

//Enable interrupts and start counting
template <int N>
void QuadDecode<N>::start(){
    zeroFTM();	// Zero FTM counter

    // Enable global FTMx interrupt
    // Assume global interrupts already enabled 
    //   No individual FTM sources enabled yet
	
    if (N<2){	//FTM1
	NVIC_ENABLE_IRQ(IRQ_FTM1);
    } else {
	NVIC_ENABLE_IRQ(IRQ_FTM2);
    };
	    
};

/*  Interrupt from timer overflow and position compare
 *  These interrupts are used to determine basePosn, the total accumulated
 *    position
 *  The interrupts occur at over/underflow (TOF) or compare interrupts
 *    at two points through 64K count.  This determines direction and also
 *    certain tasks are not done until we know we are far enough away from
 *    TOF position so we won't interrupt during the task.
 *  To generate base position
 *     Interrupt on Timer Overflow (TOV)
 *       Look at current and previous overflow direction
 *       If overflow in same direction, add or subtract counter value (64K)
 *       If overflow in opposite direction, reversed direction
 *         BasePosition count remains the same
 *       Set Interrupt to Compare at low or high value, 
 *         depending on direction
 *       Disable Overflow interrupt.  Don't look at Overflow interrupt
 *         until pass Compare values.  Just oscillating and doesn't affect
 *         Base Position
 *       Leave Overflow bit set 
 *         Will be cleared during Compare Interrupt
 *     Interrupt on Compare
 *       Save last overflow direction to know which way were going
 *       Clear overflow bit 
 *         Far enough away so no oscillation
 *         Need to interrupt again when reach overflow point
 *       Set interrupt to overflow 
 *  The previous overflow direction is set by reading TOFDIR during
 *    the Compare interrupts. This is a bit set indicating the direction
 *    we crossed TOF last.  After zeroing the position, this bit is not
 *    set until we overflow/underflow.   
 */
 
template <int N>
void QuadDecode<N>::ftm_isr(void){
    
    //First determine which interrupt enabled
    if(FTM_SC == (TOF|TOIE)){	// TOIE and TOF set, TOF interrupt
	v_intStatus=1;

	v_initZeroTOF=false;	// TOF occured, TOFDIR valid

	// If previous direction and current same add or subtract 64K
	//   from base position
	// Otherwise reversed in mid-count, base position stays same
	FTM_SC = 0;	    // Clear TOIE, TOF don't care 
	FTM_C0SC= 0x50;	    // Set CHIE Channel Interrupt Enable
  	if (v_prevInt==prevIntCompHigh){	// Counting up
	    v_intStatus=110;
	    FTM_C0V=COMP_LOW;	    // Set compare to closest value
		// SYNCEN in FTM1_COMBINE 0, update on next counter edge 
	    if (v_prevTOFUp){
		v_intStatus=111;
		v_basePosn+=NUM_64K;    // Up for one counter cycle
	    }
	} else {    // prevIntCompLow, counting down
	    v_intStatus=100;
	    FTM_C0V=COMP_HIGH;	// First expected compare counting down
	    if (!v_prevTOFUp){
		v_intStatus=101;
		v_basePosn-=NUM_64K;
	    }
	}   // Previous and current direction not the same
	    //   indicate reversal, basePosn remains same
    }else { // Channel Compare Int
	v_intStatus=2;
	if (v_initZeroTOF){	// No TOF since zero
	    if (FTM_SC & TOF){  //TOF Set
		v_initZeroTOF=false;	// TOF occured, TOFDIR valid
	    } else {
		v_prevTOFUp = true;	// Reversal will cause TOF
					//   so counting up
	    }
	}
	if (!v_initZeroTOF){	// TOFDIR valid
	    v_prevTOFUp=FTM_QDCTRL & TOFDIR;	// Dir last crossed TOF
	}

	FTM_SC = TOIE;	// Should have been read in if statement
			// Can clear TOF, set TOIE 
	v_read=FTM_C0SC;   // Read to clear CHF Channel Flag
	FTM_C0SC= 0x50;    // Clear Channel Flag, leave CHIE set
	if (FTM_C0V==COMP_LOW){
	    v_intStatus=30;
	    v_prevInt=prevIntCompLow;
	    FTM_C0V=COMP_HIGH;	// Set to other compare value
	} else {    // Compare at high value
	    v_intStatus=40;
	    v_prevInt=prevIntCompHigh;
	    FTM_C0V=COMP_LOW;
	}	

    }
}

// Initializies FTM for initialization or zero
//   Turn off counting, then no interrupts should be generated
//     and no Overflow bits set
template <int N>
void QuadDecode<N>::zeroFTM(void){

    // Turn off counter to disable interrupts and clear any 
    //   overflow and compare bits

    v_read=FTM_FMS;	// Requires reading before WPDIS can be set

    noInterrupts();
    FTM_MODE=0x05;	// Write Protect Disable with FTMEN set
    FTM_QDCTRL=0xC0;	// Disable QUADEN, filter still enabled
                        //   Turns off counter so no interrupts
    interrupts();

    FTM_CNT=0;
    v_read=FTM_SC;	// Need to read Overflow before it can be set
    FTM_SC=0;		// Disable TOF interrupt, clear Overflow bit
    v_read=FTM_C0SC;	// Need to read Overflow before it can be set
    FTM_C0SC=0x10;	// Disable Channel compare int, clear compare bit
			//   Leaves Mode bit for compare set

   
    v_basePosn=0;
    v_initZeroTOF=true;	// Special case to determine direction 

    //v_prevInt=prevIntTOF;   // Value not used, remove code
    if (FTM_QDCTRL & QUADDIR){	//QuadDir says counting up
	FTM_C0V=COMP_LOW;	// Compare Interrupt Value
    } else {			//   according to last counting value
	FTM_C0V=COMP_HIGH;
    }

    // Enable counter again
    FTM_QDCTRL=0xC1;	// QUADEN
    FTM_FMS=0x40;  	// Write Protect, WPDIS=1
   
    //  Set channel interrupt
    FTM_C0SC=0x50;     // Enable Channel interrupt and Mode 

}
// Calculates current position from Base Position and counter
//  Need to make sure get valid reading without counter values changing
//  Also need to know whether to add or subtract value (depends on dir)
//
//  Cases to look at
//   TOIE (TOF Interrupt Enable) disabled
//     We have not passed a compare interrupt since we hit TOF
//     Add or subtract counter value depending on whether closer to 0 or 64K
//     If far enough away from TOF, use TOFDIR to determine direction
//       and whether to add or subtract. 
//       If no TOF since zero and TOFDIR not valid,
//         need to determine direction from TOF
//   TOIE enabled
//     Direction (prevTOFUp) should have been set in Compare Interrupt
//     Use to determine add or subtract
//     Re-check TOIE
//       If it is set, interrupt just occured, just crossed zero 
//       Re-read basePosition, total position is basePosition
//	


template <int N>
int32_t QuadDecode<N>::calcPosn(void){
    uint16_t count16;	// 16 bit position counter
    uint32_t count32;
    bool cpTOFDirUp;	// TOFDIR in QDCTRL (bit 1)
    int32_t cpBasePosn;
    int32_t currentPosn;
    bool dataConsistent;

    noInterrupts();	    // Variables can change if FTM interrrupt occurs
    if (!(FTM_SC & TOIE)){  // Check TOIE - TOF Interrupt disabled
	count16=FTM_CNT;    // Read current counter 
	count32=count16;
	// TOF Interrupt disabled so no change to basePosn
	if (count16 < LOW_VALUE){
	    currentPosn=v_basePosn + count32;
	    interrupts();   // Turn interrupts back on before return
	    return currentPosn;
	}; 
	if ((count16 > HIGH_VALUE)){
	    currentPosn=v_basePosn + count32 - NUM_64K;
	    interrupts();   // Turn interrupts back on before return
	    return currentPosn;
	}; 
        // Away from TOF, add or subtract depending on DIR last crossed TOF 
	if (v_initZeroTOF){	// No TOF since zero
	    if (FTM_SC & TOF){  //TOF Set
		cpTOFDirUp=FTM_QDCTRL & TOFDIR;	// Away from TOF so stable
	    } else {
		cpTOFDirUp = true;	// Reversal will cause TOF
					//   so counting up
	    }
	}else{
	    cpTOFDirUp=FTM_QDCTRL & TOFDIR; // Away from TOF so stable
	}
	// Use last TOFDIR to determine direction 
	if (cpTOFDirUp){
	    currentPosn=v_basePosn + count32;
	    interrupts();   // Turn interrupts back on before return
	    return currentPosn;
	}; 
	if (!cpTOFDirUp){
	    currentPosn=v_basePosn + count32 - NUM_64K;
	    interrupts();   // Turn interrupts back on before return
	    return currentPosn;
	};
    };
    // TOF Interrupt enabled
    interrupts();   // Allow TOF interrupt if pending
    count16=FTM_CNT;    // Read current counter 
    count32=count16;
    cpBasePosn=v_basePosn;  // Unchanging copy
    dataConsistent=(FTM_SC & TOIE);  // Still TOIE, no TOF
    // prevTOFUp set in Compare int that turns on TOIE
    //   Compare Int could occur but prevTOFUp should remain the same
    if (dataConsistent){
	// v_prevTOFUp changes in compare int, should be stable
	// Need to go thru compare int to set TOIE after zero
	if (v_prevTOFUp){	// Counting up
	    currentPosn=cpBasePosn + count32;
	} else {
	    currentPosn=cpBasePosn + count32 - NUM_64K;
	}
	return currentPosn;
    } else {	// Just crossed over TOF
	return v_basePosn;  // Position is new basePosition, no addtl counts
    }
}

#endif

