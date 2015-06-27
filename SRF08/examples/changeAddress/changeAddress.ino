#include <i2c_t3.h>
#include <SRF08.h>

#define LED 13

/* 
can only change to 0x:
E0		E2		E4		E6	E8	EA	EC	EE	F0	F2	F4	F6	F8	FA	FC	FE
0x70	0x71	0x72 ...
*/

#define CURRENTADDRESS 0x70
#define NEWADDRESS 0xE4
SRF08 srf(CURRENTADDRESS);

void setup(){
	Wire.begin();
	Serial.begin(115200);
	pinMode(LED, OUTPUT);
	switch(NEWADDRESS){
		case CURRENTADDRESS:
			while(true){
				Serial.println("current address same as new!");
				delay(100);
			}
		case 0xE0: break;
		case 0xE2: break;
		case 0xE4: break;
		case 0xE6: break;
		case 0xE8: break;
		case 0xEA: break;
		case 0xEC: break;
		case 0xEE: break;
		case 0xF0: break;
		case 0xF2: break;
		case 0xF4: break;
		case 0xF6: break;
		case 0xF8: break;
		case 0xFA: break;
		case 0xFC: break;
		case 0xFE: break;
		default:
			while(true){
				Serial.println("invalid new address");
				delay(100);
			}
		break;
	}
	srf.setAddress(NEWADDRESS);
}

void loop(){
	finder();
}

void finder()
{
	
	byte error, address;
	int nDevices;
	digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
	Serial.println("Scanning... srf changeAddress.ino");

	nDevices = 0;
	for (address = 1; address < 127; address++)
	{
		// The i2c_scanner uses the return value of
		// the Write.endTransmission to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		if (error == 0)
		{
			Serial.print("I2C device found at address 0x");
			if (address<16)
				Serial.print("0");
			Serial.print(address, HEX);
			Serial.println("  !");

			nDevices++;
		}
		else if (error == 4)
		{
			Serial.print("Unknow error at address 0x");
			if (address<16)
				Serial.print("0");
			Serial.println(address, HEX);
		}
	}
	if (nDevices == 0)
		Serial.println("No I2C devices found\n");
	else
		Serial.println("done\n");

	delay(250);           // wait 5 seconds for next scan
	digitalWrite(LED, LOW);   // turn the LED on (HIGH is the voltage level)
	delay(250);
}