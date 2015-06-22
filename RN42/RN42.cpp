/*
RN42 - Library for RN42 bluetooth modules
Created by Brian Chen 04/04/2013
Last Modified by Brian Chen 04/04/2013 11:21am

Version 0.05
*/

#include "Arduino.h"
#include "RN42.h"


//initiate TSOP
RN42::RN42(){
	
}

void RN42::init(int baud){
	port.begin(baud);
}

void RN42::commandMode(){
	port.print("$$$");
}

void RN42::normalMode(){
	port.println("---");
}

void RN42::connect(String address){
	String s = "C,";
	s += address;
	port.println(s);
}

void RN42::setMode(bool mode){
	if (mode == MASTER){
		port.println("SM,1");
	}
	else{
		port.println("SM,0");
	}
}
