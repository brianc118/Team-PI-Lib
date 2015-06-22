/*
	debugSerial - Library for simple serial debugging
    
    (C) 2014 TEAM PI
*/

#ifndef debugSerial_h
#define debugSerial_h

#include "Arduino.h"

#define DEBUGSERIALPORT Serial

class debugSerial
{
public:
    void begin(unsigned int baud);
    void print(String s);
    void println(String s);
    void append(String s);     //add string to buffer for writing
	void writeBuffer();  
	void disable();
	void enable();	
private:
	String buffer = "";
	bool debug = true;
};

#endif

