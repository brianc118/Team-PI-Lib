#include <i2c_t3.h>
#include <SRF08.h>

SRF08 srf1(0x70);
SRF08 srf2(0x71);
SRF08 srf3(0x72);

int16_t range1, range2, range3;

void setup(){
	Serial.begin(115200);
	Wire.begin();
	Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
	//Wire.setDefaultTimeout(1000);

	delay(1000);
	srf1.setRange(255);
	srf2.setRange(255);
	srf3.setRange(255);
}

void loop(){
	if (false){
		srf1.getRange(range1);
		srf2.getRange(range2);
		srf3.getRange(range3);
	}
	else{
		srf1.getRangeIfCan(range1);
		srf2.getRangeIfCan(range2);
			
		srf3.getRangeIfCan(range3);
	}
	
	Serial.print(range1);
	Serial.print('\t');
	Serial.print(range2);
	Serial.print('\t');
	Serial.println(range3);
	//delay(100);
}