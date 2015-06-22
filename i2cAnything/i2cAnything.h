/*
	Simple functions to write any type of variable to i2c using templates.
	TEAM PI 2013
*/

#include <Arduino.h>
#include <i2c_t3.h>

#define I2C_ANYTHING_TIMEOUT 1000	//timout in microseconds

static unsigned long startRequestTime;

template <typename T> uint8_t I2C_write(const T &value)
{
	const uint8_t *p = (const uint8_t*)&value;
	uint8_t i;
	for (i = 0; i < sizeof(value); i++){
		Wire.write(*p++);
	}			
	return i;
}

template <typename T> uint8_t I2C_read(T &value)
{
	uint8_t *p = (uint8_t*)&value;
	uint8_t i;
	for (i = 0; i < sizeof(value); i++){
		*p++ = Wire.read();
	}			
	return i;
}

template <typename T> uint8_t I2CGet(uint8_t address, uint8_t command, size_t len, T &value){
	uint8_t write, end, request;
	Wire.beginTransmission(address);	
	write = Wire.write(command);
	if (write == 0){
		return 5;	//write error
	}
	end = Wire.endTransmission(I2C_STOP, I2C_ANYTHING_TIMEOUT);
	if(Wire.status() == I2C_TIMEOUT) { 
	    Serial.println("timout");
	}
	if (end != 0){
		return end;		
	}
	request = Wire.requestFrom(address, len, I2C_STOP, I2C_ANYTHING_TIMEOUT);
	if (request == 0){
		return 6;	//request error
	}
	startRequestTime = micros();
	while (Wire.available() < len){
		if (micros() - startRequestTime < I2C_ANYTHING_TIMEOUT){
			return 6;
		}
	}
	I2C_read(value);
	return 0;
}

template <typename T> uint8_t I2CGetHL(uint8_t address, uint8_t command, T &value){
	uint8_t write, end, request;
	Wire.beginTransmission(address);
	write = Wire.write(command);
	if (write == 0){
		return 5;	//write error
	}
	end = Wire.endTransmission(I2C_STOP, I2C_ANYTHING_TIMEOUT);
	if (end != 0){
		return end;
	}
	request = Wire.requestFrom(address, 2, I2C_STOP, I2C_ANYTHING_TIMEOUT);
	if (request == 0){
		return 6;	//request error
	}
	startRequestTime = micros();
	while (Wire.available() < 2){
		if (micros() - startRequestTime < I2C_ANYTHING_TIMEOUT){
			return 6;
		}
	}
	value = (Wire.read() << 8) + Wire.read();	
	return 0;
}