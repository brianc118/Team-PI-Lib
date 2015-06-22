/*
HCTL2032 Dual Axis Decoder IC (Avago) Library
Runs off an 8-bit tristate interface (parallel)

Optimised for the Teensy 3.x for high speed IO. Only compatible with Teensy 3.x.

To run on other Arduinos, replace all Teensy specific functions (such as digitalWriteFast) with 
Arduino standard functions.



Brian Chen 2014

Based off Andrew Jalics origina library HCTL2032:
http://www.robogaia.com/two-axis-encoder-counter-mega-shield-version-2.html
*/
/* Please see header file (HCTL2032.h) and Avago HCTL-2032 Datasheet for more technical details */

#include "HCTL2032.h"

HCTL2032::HCTL2032(unsigned char countMode)
{
   pinMode(HCTL2032_PIN_D0,   INPUT);
   pinMode(HCTL2032_PIN_D1,   INPUT);
   pinMode(HCTL2032_PIN_D2,   INPUT);
   pinMode(HCTL2032_PIN_D3,   INPUT);
   pinMode(HCTL2032_PIN_D4,   INPUT);
   pinMode(HCTL2032_PIN_D5,   INPUT);
   pinMode(HCTL2032_PIN_D6,   INPUT);
   pinMode(HCTL2032_PIN_D7,   INPUT);

   pinMode(HCTL2032_PIN_XY,   OUTPUT);
   pinMode(HCTL2032_PIN_OE,   OUTPUT);
   pinMode(HCTL2032_PIN_EN1,  OUTPUT);
   pinMode(HCTL2032_PIN_EN2,  OUTPUT);
   pinMode(HCTL2032_PIN_SEL1, OUTPUT);
   pinMode(HCTL2032_PIN_SEL2, OUTPUT);
   pinMode(HCTL2032_PIN_RSTX, OUTPUT);
   pinMode(HCTL2032_PIN_RSTY, OUTPUT);

   // XY LOW  X Axis AKA 1st Axis
   // XY HIGH Y Axis AKA 2nd Axis
   digitalWriteFastFast(HCTL2032_PIN_XY, LOW);

   digitalWriteFastFast(HCTL2032_PIN_OE, HIGH);  // Active LOW

   switchCountMode(countMode);

   // Byte Selected MSB SEL1  LOW SEL2 HIGH
   // Byte Selected 2nd SEL1 HIGH SEL2 HIGH
   // Byte Selected 3rd SEL1  LOW SEL2 LOW
   // Byte Selected LSB SEL1 HIGH SEL2 LOW
   digitalWriteFastFast(HCTL2032_PIN_SEL1, LOW);
   digitalWriteFastFast(HCTL2032_PIN_SEL2, HIGH);

   digitalWriteFastFast(HCTL2032_PIN_RSTX, HIGH);  // Active LOW
   digitalWriteFastFast(HCTL2032_PIN_RSTY, HIGH);  // Active LOW

   XAxisReset( );
   YAxisReset( );
}

// Communicates with a HCTL-2032 IC to get reset the X encoder count
// see Avago/Agilent/HP HCTL-2032 PDF for details
void HCTL2032::XAxisReset( )
{
   digitalWriteFast(HCTL2032_PIN_RSTX, LOW);
   delayMicroseconds(1);
   digitalWriteFast(HCTL2032_PIN_RSTX, HIGH);
   delayMicroseconds(1);
}

// Communicates with a HCTL-2032 IC to get the X Axis encoder count via an 8bit parallel bus
// see Avago/Agilent/HP HCTL-2032 Datasheet PDF for details
unsigned long HCTL2032::XAxisGetCount( )
{
   digitalWriteFast(HCTL2032_PIN_XY,   LOW);
   digitalWriteFast(HCTL2032_PIN_OE,   LOW);
   digitalWriteFast(HCTL2032_PIN_SEL1, LOW);
   digitalWriteFast(HCTL2032_PIN_SEL2, HIGH);
   delayMicroseconds(1);
   busByte = PINA;
   count   = busByte;
   count <<= 8;

   digitalWriteFast(HCTL2032_PIN_SEL1, HIGH);
   digitalWriteFast(HCTL2032_PIN_SEL2, HIGH);
   delayMicroseconds(1);
   busByte = PINA;
   count  += busByte;
   count <<= 8;

   digitalWriteFast(HCTL2032_PIN_SEL1, LOW);
   digitalWriteFast(HCTL2032_PIN_SEL2, LOW);
   delayMicroseconds(1);
   busByte = PINA;
   count  += busByte;
   count <<= 8;

   digitalWriteFast(HCTL2032_PIN_SEL1, HIGH);
   digitalWriteFast(HCTL2032_PIN_SEL2, LOW);
   delayMicroseconds(1);
   busByte = PINA;
   count  += busByte;

   digitalWriteFast(HCTL2032_PIN_OE,  HIGH);

   return count;
}

// Communicates with a HCTL-2032 IC to get reset the Y encoder count
// see Avago/Agilent/HP HCTL-2032 PDF for details
void HCTL2032::YAxisReset( )
{
   digitalWriteFast(HCTL2032_PIN_RSTY, LOW);
   delayMicroseconds(1);
   digitalWriteFast(HCTL2032_PIN_RSTY, HIGH);
   delayMicroseconds(1);
}

// Communicates with a HCTL-2032 IC to get the Y Axis encoder count via an 8bit parallel bus
// see Avago/Agilent/HP HCTL-2032 PDF for details
unsigned long HCTL2032::YAxisGetCount( )
{
   digitalWriteFast(HCTL2032_PIN_XY,   HIGH);
   digitalWriteFast(HCTL2032_PIN_OE,   LOW);
   digitalWriteFast(HCTL2032_PIN_SEL1, LOW);
   digitalWriteFast(HCTL2032_PIN_SEL2, HIGH);
   delayMicroseconds(1);
   busByte = PINA;
   count   = busByte;
   count <<= 8;

   digitalWriteFast(HCTL2032_PIN_SEL1, HIGH);
   digitalWriteFast(HCTL2032_PIN_SEL2, HIGH);
   delayMicroseconds(1);
   busByte = PINA;
   count  += busByte;
   count <<= 8;

   digitalWriteFast(HCTL2032_PIN_SEL1, LOW);
   digitalWriteFast(HCTL2032_PIN_SEL2, LOW);
   delayMicroseconds(1);
   busByte = PINA;
   count  += busByte;
   count <<= 8;

   digitalWriteFast(HCTL2032_PIN_SEL1, HIGH);
   digitalWriteFast(HCTL2032_PIN_SEL2, LOW);
   delayMicroseconds(1);
   busByte = PINA;
   count  += busByte;

   digitalWriteFast(HCTL2032_PIN_OE,  HIGH);

   return count;
}

// Communicates with a HCTL-2032 IC to set the count mode
// see Avago/Agilent/HP HCTL-2032 PDF for details
void HCTL2032::switchCountMode( unsigned char countMode )
{
   // Count Mode Illegal Mode EN1 LOW  EN2 LOW
   // Count Mode   4X         EN1 HIGH EN2 LOW
   // Count Mode   2X         EN1 LOW  EN2 HIGH
   // Count Mode   1X         EN1 HIGH EN2 HIGH
   switch(countMode)
   {
      case 1: // 1X Count Mode
         digitalWriteFast(HCTL2032_PIN_EN1, HIGH);
         digitalWriteFast(HCTL2032_PIN_EN2, HIGH);
         break;
      case 2: // 2X Count Mode
         digitalWriteFast(HCTL2032_PIN_EN1, LOW);
         digitalWriteFast(HCTL2032_PIN_EN2, HIGH);
         break;
      case 4: // 4X Count Mode is the default
      default:
         digitalWriteFast(HCTL2032_PIN_EN1, HIGH);
         digitalWriteFast(HCTL2032_PIN_EN2, LOW);
         break;
   }
   delayMicroseconds(1);
}

