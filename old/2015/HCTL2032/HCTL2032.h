/*
HCTL2032 Dual Axis Decoder IC (Avago) Library

Optimised for the Teensy 3.1
Runs off an 8-bit tristate interface (parallel)

Brian Chen 2014

Based off Andrew Jalics origina library HCTL2032:
http://www.robogaia.com/two-axis-encoder-counter-mega-shield-version-2.html
*/

#ifndef __HCTL2032__
#define __HCTL2032__

#include "Arduino.h"

class HCTL2032
{
   public:
      HCTL2032( unsigned char countMode = 4 );

      void          XAxisReset( );
      unsigned long XAxisGetCount( );
      void          YAxisReset( );
      unsigned long YAxisGetCount( );

      void          switchCountMode( unsigned char countMode );

   private:
      unsigned long count;
      unsigned char busByte;
};

#define HCTL2032_PIN_D0   0
#define HCTL2032_PIN_D1   1
#define HCTL2032_PIN_D2   2
#define HCTL2032_PIN_D3   3
#define HCTL2032_PIN_D4   4
#define HCTL2032_PIN_D5   5
#define HCTL2032_PIN_D6   6
#define HCTL2032_PIN_D7   7

#define HCTL2032_PIN_XY   37
#define HCTL2032_PIN_OE   36 // Active LOW
#define HCTL2032_PIN_EN1  35
#define HCTL2032_PIN_EN2  34
#define HCTL2032_PIN_SEL1 33
#define HCTL2032_PIN_SEL2 32
#define HCTL2032_PIN_RSTX 31 // Active LOW
#define HCTL2032_PIN_RSTY 30 // Active LOW

// HCTL-2032 Count Mode Info
// Count Mode Illegal Mode EN1 LOW  EN2 LOW
// Count Mode   4X         EN1 HIGH EN2 LOW
// Count Mode   2X         EN1 LOW  EN2 HIGH
// Count Mode   1X         EN1 HIGH EN2 HIGH

// HCTL-2032 Byte Selected Info
// Byte Selected MSB SEL1  LOW SEL2 HIGH
// Byte Selected 2nd SEL1 HIGH SEL2 HIGH
// Byte Selected 3rd SEL1  LOW SEL2 LOW
// Byte Selected LSB SEL1 HIGH SEL2 LOW

// HCTL-2032 X/Y Info
// XY LOW  X Axis AKA 1st Axis
// XY HIGH Y Axis AKA 2nd Axis

#endif
