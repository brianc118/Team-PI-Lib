/* Send back values to be read by Python pyserial
	Send back 6 values for realtime X,Y,Z and latched X,Y,Z
	On button push, latch X,Y,Z with realtime X,Y,Z
	
	Send lines with
		RX<value>\n		(realtime)
		RY<value>\n
		RZ<value>\n		
		LX<value>\n		(latched)
		LY<value>\n
		LZ<value>\n
		
	Send back <value> as ascii string
	Only send back latched value once after latching
	
	Also send back comments with MS<string>

	Check if any bytes received
	If axis to be zeroed, expect string
	  ZZXY where ZXY are 1 if axis to be zeroed
	       0 otherwise

*/
/*  Use Teensy3.1 FTM (FlexTimerModule) Quadrature Decode 
 *    to determine position on 2 axis
 *    (3rd channel with second Teensy TBD)
 *  Position Counter is 16-bit counter
 *  Read position every 100 mS and determine if counting up 
 *     or down
 *  Count up in interrupt routine for 200 mS GUI update
*/

/* Pinout
 *   Function Arduino  K20ARM	Pin   Direction		Pin Alternative
 *    LED	13	PTC5	 50	Output
 *    ENCXA	 3	PTA12	 28	Input	    FTM1    7
 *    ENCXB	 4	PTA13	 29	Input	    FTM1    7
 *    ENCYA	16/A2	PTB0	 35	Input	    FTM1    6
 *    ENCYB	17/A3	PTB1	 36	Input	    FTM1    6
 *    ENCZA	32	PTB18	 41	Input	    FTM2    6
 *    ENCZB	25	PTB19	 42	Input	    FTM2    6
 *    ENCA	 9	PTC3	 46 	Output
 *    ENCB	10	PTC4	 49	Output
 *    
*/
/*
 * Pin Control Registers PORTx_PCRn
 * Bit 10-8 MUX Pin Mux Control 1-7 alternative
 *   Alternative 1 is GPIO
 * Bit 4 PFE Passive Filter Enable 1 to enable
 * Bit 1 PE Pull Enable 1 to enable
 * Bit 0 PS Pull Select 0 is pulldown 1 is pullup
*/
/* Quadrature Decoding with FTM1 Setup
 *   FTMEM=1  QUADEN=1
 *   Filter A PHAFLTREN bit  CH0FVAL[3:0] in FILTER0
 *   Filter B PHBFLTREN bit  CH1FVAL[3:0] in FILTER0
 *   No channel logic used
 *   FTM channels in input capture or output compare modes
 *   PHAPOL and PHBPOL selects polarity
 *   QUADMODE = 0 for quadrature decode
 *   CNTIN=0 to count from 0
 *   MOD = 64K for maximum range
 *   TOF and TOFDIR indication Top Overflow and Direction
 *   Set CNTIN and MOD before set FTMEM
 *
 *   The following registers are set in pins_teensy.c
 *        and need to be reset.
 *     FTM1_SC
 *     FTM1_CNT
 *     FTM1_MOD
 *     FTM1_C0SC
 *     FTM1_C1SC
 *
 *   FTM1_CNT Counter value
 *      Bit 15-0 is counter value
 *      Writing any value updates counter with CNTIN 
 *   FTM1_MOD Modulo (Max value)
 *	Bit 15-0 is counter value - set to 0xFFFF
 *	Write to CNT first
 *   FTM1_MODE Features Mode Selection
 *      Bit0 FTMEN FTM Enable - [WPDIS must be 1 to write]
 *      Bit2 WPDIS Write Protect to Disable
 *      Set WPDIS, then set FTMEN - must be set to access FTM regs
 *   FTM1_FMS Fault Mode Status
 *       Bit 6 WPEN Write Protect Enable
 *         Write 1 to clear WPDIS
 *   FTM1_FILTER
 *	Filter out pulses shorter than CHxFVALx 4 system clocks
 *	Bit 7-4 CH1FVAL for PHB
 *	Bit 3-0 CH0FVAL for PHA
 *   FTM1_QDCTRL Quadrature Decoder Control and Status
 *	Bit 7 PHAFLTREN Phase A Filter Enable
 *	Bit 6 PHBFLTREN Phase B Filter Enable
 *	Bit 5 PHAPOL Phase A Polarity
 *	Bit 4 PHBPOL Phase B Polarity
 *	Bit 3 QUADMODE Quadrature Decoder Mode
 *	  0 for Quadrature
 *	Bit 2 QUADIR Counting Direction
 *	Bit 1 TOFDIR Timer Overflow Dir
 *	  0 was set on bottom of counting
 *	  1 was set on top of counting
 *	Bit 0 QUADEN Quadrature Mode Enable
 *	  1 is quadrature mode enabled [WPDIS to write]
 */
/* Generate 32 bit position value from 16 bit counter
 *   Keep track of BasePosition which indicates how many times wrapped
 *      around
 *   Generate TotalPosition by reading counter and adjusting from 
 *     Base Position
 *   To generate Base Position interrupt at every quadrant
 *   This is to avoid issues if sitting and oscillating around either point
 *   Use every quadrant interrupt to determine direction
 *   To generate base position
 *     Interrupt on Timer Overflow (TOV)
 *       Look at current and previous overflow direction
 *       If overflow in same direction, add or subtract counter value (64K)
 *       If overflow in opposite direction, reversed direction
 *         BasePosition count remains the same
 *       Set Interrupt to Compare at quarter points +-(64K/2)
 *       Disable Overflow interrupt
 *       Leave Overflow bit set 
 *         Will be cleared during Compare Interrupt or Position reading
 *         Need to check during Position reading
 *     Interrupt on Compare
 *       Save last overflow direction to know which way were going
 *       Clear overflow bit 
 *         Far enough away so no oscillation
 *         Need to interrupt again when reach overflow point
 *       Set interrupt to overflow
 *
 *   To get TotalPosition read counter and adjust from Base Position
 *     Read counter as unsigned integer
 *     Read overflow direction
 *     If last overflowed counting up, add to Base Position
 *     If last overflowed counting down, subtract 64K and add to
 *     Base Position
 *   Issues with Overflow interrupt occuring during reading
 *   To deal with this:
 *     If Interrupt on Compare clear Overflow bit
 *     Store Base Count
 *     Read Counter
 *     Read Overflow Direction
 *     Read Overflow bit
 *   If Overflow bit set re-do sequence 3x or until Overflow bit not set
 *     If 3x and still getting overflow, oscillating around zero
 *     Default to zero, and add zero to Base Count
 *     BaseCount should be remaining the same       
 */
/*  Registers for 16 to 32 conversion interrupts
 *
 *   FTM1_SC FTM1 Status and Control
 *     Bit 7 TOF Timer Overflow Flag
 *     Bit 6 TOIE Timer Overflow Interrupt Enable
 *     Bit 5 CPWMS Center Aligned PWM Select
 *       Resets to 0 (Write when WPDIS is 1)
 *     Rest of bits are 0 (Write when WPDIS is 1)
 *
 *   FTM1_C0SC  FTM1 Channel 0 Status and Control
 *     Set WPDIS before writing control bits
 *     Bit 7 CHF Channel Flag
 *       Channel event occured
 *       Read and write 0 to clear
 *     Bit 6 CHIE Channel Interrupt Enable
 *       Set for compare interrupt
 *     Bit 5 MSB Channel Mode Select B
 *       Set to 0
 *     Bit 4 MSA Channel Mode Select A
 *       Set to 1
 *     Bit 3:2  ELS Edge or Level Select
 *       Set to 0:0
 *
 *   FTM1_COMBINE 
 *     Set WPDIS to 1 before writing
 *     DECAPEN (Dual Edge Capture Enable)
 *     COMBINE (Combine Channels)
 *     Resets to all zero
 *
 *   FTM1_C0V Channel 0 Value
 *     Channel Compare Value
 *     Set to 0x80 - halfway thru count
 *
 *   FTM1_STATUS 
 *     Duplicate of CHnF bit for all channels
 *     Bit 0 CH0F
 *
 */
/* To zero and initialize channel
 *   Zero counter
 *   Set last Overflow Dir to Counting Direction
 */


#define GEN_ENCODER		// Include encoder test signals code

#include "WProgram.h"
#include "QuadDecode_def.h"
#ifdef GEN_ENCODER
#include "GenEncoder.h"
#endif
#include <stdbool.h>

// Timing for GUI update time, how often send back data values
#define GUI_UPDATE_TIME	 250000	// 200 mSec update

// Command headers for X,Y,Z data channels
const char cmd1[] = "RX";
const char cmd2[] = "RY";
const char cmd3[] = "RZ";
const char cmd4[] = "LX";
const char cmd5[] = "LY";
const char cmd6[] = "LZ";
const char* cmdPointers[]= {cmd1,cmd2,cmd3,cmd4,cmd5,cmd6};

const uint8_t num_cmds = 6;	// Generated Postition in latched position
//const uint8_t num_cmds = 3;		// Only send up RXYZ continually

// Variables from CMM program
volatile int32_t rtX=0, rtY=0, rtZ=0;	// Realtime values of X,Y,Z
volatile int32_t ltX=0, ltY=0, ltZ=0;	// Latched values of X,Y,Z

volatile bool zero_rtX=0, zero_rtY=0, zero_rtZ=0;   // Zero values of X,Y,Z

volatile bool doOutput=false;	// Run output routine

IntervalTimer serialTimer;	// How often to update serial

// Generate test encoder signals
#ifdef GEN_ENCODER
/*struct GenEncData{
    int32_t maxPosn;
    int32_t minPosn;
    int32_t cycleMax;
    int32_t cycleMin;
    int32_t cycleCenter;
    int cycleCrossings;
    int bigCrossings;
};*/

const int ENCOUT1_A = 9;    // Pinouts
const int ENCOUT1_B = 10;
const int32_t ENC_UPDATE_TIME1 = 433;    // uSec 

GenEncData genEncData1 ={95000,-25000,10,-10,0,100,2};

GenEncoder genEnc1(ENCOUT1_A,ENCOUT1_B,ENC_UPDATE_TIME1,genEncData1,0);

const int ENCOUT2_A = 15;   // Pinouts
const int ENCOUT2_B = 14;
const int32_t ENC_UPDATE_TIME2 = 70;    // uSec
GenEncData genEncData2 ={120000,-66000,10000,-25000,24576,4,3};

GenEncoder genEnc2(ENCOUT2_A,ENCOUT2_B,ENC_UPDATE_TIME2,genEncData2,1);
#endif

void timerInt(void);	// Main timing loop interrupt

QuadDecode<1> xPosn;	// Template using FTM1
QuadDecode<2> yPosn;	// Template using FTM2


extern "C" int main(void)
{   
	//bool LED_ON=false;

	char inbuf[3];  // Characters to be received
	char inByte;
	int32_t buffer[6];  // Values to send, X,Y,Z realtime and latched
	uint8_t n;	    // Number of characters received
	uint8_t j;

	Serial.begin(9600); // USB is always 12 Mbit/sec

	xPosn.setup();	    // Start Quad Decode position count
	yPosn.setup();	    // Start Quad Decode position count

	// Start encoder test signal generation
#ifdef GEN_ENCODER
	genEnc1.start();
	genEnc2.start();
#endif

	serialTimer.begin(timerInt,GUI_UPDATE_TIME);	// GUI Update time


	xPosn.start();	    // Start Quad Decode position count
	yPosn.start();	    // Start Quad Decode position count

	    while (1) {
		if (doOutput){
		    doOutput=false;

		    // Read in string to zero axis
		    if (Serial.available()){
			while (Serial.available()){
			    inByte=Serial.read();
			    if (inByte == 'Z'){	    // Start byte detected
				for(n=0;n<3;n++){  //    read in string
				    inbuf[n]=Serial.read();
				}
			    }
			}
			// Echo value back for diagnostics
			Serial.print("MSByteReceived is ");
			for(int8_t ib =0; ib < 3; ib++){
			    Serial.write(inbuf[ib]);
			}
			Serial.println(" ");
		    };

		    // Determine if need to zero input

		    // Zero axis posn from GUI
		    if (inbuf[0]=='1'){
		       	zero_rtX=true;
			inbuf[0]='0';
		    }
		    else zero_rtX=false;

		    if (inbuf[1]=='1'){
		       	zero_rtY=true;
			inbuf[1]='0';
		    }
		    else zero_rtY=false;

		    if (inbuf[2]=='1'){
		       	zero_rtZ=true;
			inbuf[2]='0';
		    }
		    else zero_rtZ=false;
		    
		    if (zero_rtX ){
#ifdef GEN_ENCODER
			genEnc1.setZero();
#endif
			xPosn.zeroFTM(); 			
		    }
		    rtX=xPosn.calcPosn();

		    if (zero_rtY ){
#ifdef GEN_ENCODER
			genEnc2.setZero();
#endif
			yPosn.zeroFTM(); 			
		    }
		    rtY=yPosn.calcPosn();
		    
		    if (zero_rtZ ){
			rtZ=0;
		    }

		    // Set Generated value in latched position for debug
		    ltX=genEnc1.getPosn();  // X generated position
		    ltY=genEnc2.getPosn();  // Y generated position

		    // Send out axis values
		    buffer[0] = rtX;
		    buffer[1] = rtY;
		    buffer[2] = rtZ;
		    buffer[3] = ltX;
		    buffer[4] = ltY;			
		    buffer[5] = ltZ;
    
		    Serial.println("MSTeensy Loop");	
		    
		    // Send out rtX, rtY, rtZ values
		    for (j=0;j<num_cmds;++j){
			    Serial.print(cmdPointers[j]);
			    Serial.println(buffer[j]);

		    }

		    Serial.println("MSEnd of Loop");	

		}
	    }

}
void timerInt(void){
    doOutput=true;
}


