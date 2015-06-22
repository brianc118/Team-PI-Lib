/*
RN42 - Library for RN42 bluetooth modules
Created by Brian Chen 04/04/2013
Last Modified by Brian Chen 04/04/2013 11:21am

Version 0.05
*/

#ifndef RN42_h
#define RN42_h

#include "Arduino.h"

#define port Serial1

#define MASTER true
#define SLAVE false

class RN42
{
public:
	RN42();
	void init(int baud);
	void commandMode();
	void normalMode();
	void connect(String address);
	void setMode(bool mode);

private:
};

#endif

