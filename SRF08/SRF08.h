/*
	SRF08 - Library for array of TSOPS
	Created by Brian Chen 15/06/2014

*/

#ifndef SRF08_h
#define SRF08_h

#include "Arduino.h"
#include "i2c_t3.h"

#define CMD                 0x00 // Command byte, values of 0 being sent with write have to be masked as a byte to stop them being misinterpreted as NULL this is a bug with arduino 1.0
#define LIGHTBYTE           0x01 // Byte to read light sensor
#define RANGEBYTE           0x02 // Byte for start of ranging data
#define CHANGERANGEBYTE		0x03 // Byte to change range

class SRF08
{
    public:
	    SRF08(uint8_t add);
		void getRange(int16_t &range);
		void startRange();
		bool rangeDone();
		bool getRangeIfCan(int16_t &range);
		void getLight(int16_t &lightReading);
		void getSoft(uint8_t &softRev);
		bool autoGetStartIfCan(int16_t &range);
		void setRange(uint16_t cm);
		void setAddress(uint8_t address);
    private:
		uint8_t address;
		uint8_t highByte = 0x00;                             // Stores high byte from ranging
		uint8_t lowByte = 0x00;                              // Stored low byte from ranging
		bool isRanging = false;
		unsigned long startRequestTime;
		elapsedMillis lastReadTime;
};

#endif

