/*
	debugSerial - Library for simple serial debugging
    
    (C) 2014 TEAM PI
*/
#include "Arduino.h"
#include "debugSerial.h"

void debugSerial::begin(unsigned int baud){  
	if (debug){
		DEBUGSERIALPORT.begin(baud);
	}	
}

void debugSerial::print(String s){
	if (debug){
		DEBUGSERIALPORT.print(s);
	}    
}

void debugSerial::println(String s){
	if (debug){
		DEBUGSERIALPORT.println(s);
	}    
}

void debugSerial::append(String s){
	if (debug){
		buffer += s;
	}	
}

void debugSerial::writeBuffer(){
	if (debug){
		DEBUGSERIALPORT.println(buffer);
		buffer = "";
	}	
}

void debugSerial::disable(){
	debug = false;
}
void debugSerial::enable(){
	debug = true;
}