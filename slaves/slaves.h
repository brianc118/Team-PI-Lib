/*
 * slave.h - slave interface library used to communicate to the 3 slaves on the robot
 *
 * 
 *
 * by Brian Chen
 * (C) Team Pi 2014
 */

#ifndef SLAVES_H
#define SLAVES_H

#include <Arduino.h>
#include <SPI.h>

#include <piCommon.h>

#define SPI_CLOCK 2000000
// #define SLAVE2_CS 2	 //cs1
// #define SLAVE3_CS 9	 //cs2
#define SLAVE2_CS 2 	//cs0
#define SLAVE3_CS 9	   //cs2

#define TIMEOUT 40000	// in uS

#define SPI_SR_TXCTR		0x0000f000 //Mask isolating the TXCTR
#define SPI_WAIT() \
	while ((SPI0_SR & SPI_SR_TXCTR) != 0); \
	while (!(SPI0_SR & SPI_SR_TCF)); \
	SPI0_SR |= SPI_SR_TCF;

enum SERIALSTATUS{
	SERIAL_SUCCESS,
	SERIAL_NOTREADY,
	SERIAL_OVERFLOW,
	SERIAL_UNKNOWN,
	SERIAL_TIMEOUT
};

class SLAVE1 : public HardwareSerial{	// inherit hardware serial to directly write to slave
public:
	// packets are ended with a newline characted '\n'. The newline character is not added to the array
	void sendPacket(uint8_t *dataOut){
		HardwareSerial::println(*dataOut);	   // use println() to add a newline char at the end
	}
	
	uint8_t receivePacket(uint8_t *dataIn, uint8_t maxLength){
		uint8_t available = HardwareSerial::available();
		if (available < maxLength) return SERIAL_NOTREADY;	// data not ready
		if (available > maxLength) return SERIAL_OVERFLOW;	// somethings wrong! need to clear buffer and make a new request

		if (HardwareSerial::readBytes(dataIn, maxLength) <= 0){
			return SERIAL_UNKNOWN;   // this situation should never occur
		}
		return SERIAL_SUCCESS;
	}
	uint8_t receivePacket(uint8_t *dataIn, uint8_t maxLength, bool wait){
		if (wait){
			uint8_t result;
			elapsed = 0;
			while (elapsed < TIMEOUT){
				result = receivePacket(dataIn, maxLength);
				if (result == SERIAL_SUCCESS){
					// Serial.println(elapsed);
					return result;
				}
				else if (result == SERIAL_OVERFLOW){
					// resend request (requestPacket will automatically clear buffer)
					requestPacket(recentReq);
				}
			}
			return SERIAL_TIMEOUT;
		}
		return receivePacket(dataIn, maxLength);
	}
	// request packet from slave. After one request, the slave will return 1 packet
	void requestPacket(uint8_t command){
		clearBuffer();  // clear buffer. It should be empty anyway before a request
		HardwareSerial::write(command);
		recentReq = command;
	}
	void clearBuffer(){
		while(HardwareSerial::available() > 0){
			HardwareSerial::read();
		}
	}
	void startCalibMag(){
		requestPacket(SLAVE1_COMMANDS::CALIB_MAG);
	}
	void endCalibMag(){
		requestPacket(SLAVE1_COMMANDS::END_CALIB_MAG);
	}

private:
	elapsedMicros elapsed;
	uint8_t recentReq;
};

class SPISLAVE{
public:
	uint8_t cs;
	SPISLAVE(uint8_t chip_select){
		cs = chip_select;
	}
	void begin(){;
		pinMode(cs, OUTPUT);
		digitalWriteFast(cs, HIGH);
	}
	
	// send command and then send data. Returns received byte
	uint8_t txrx(uint8_t command, uint8_t out = 0){
		uint8_t in;
		SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
		digitalWriteFast(cs, LOW);
		//delayMicroseconds(1);
		//Serial.print(
			SPI.transfer(command);
		//	);
		delayMicroseconds(1); // give slave some time to know what it's going to send back next
		in = SPI.transfer(out);

		digitalWriteFast(cs, HIGH);
		SPI.endTransaction();
		// Serial.print('\t');
		// Serial.println(in);
		return in;
	}

	// send and receive 16 bit data
	uint16_t txrx16(uint8_t command, uint8_t out1 = 0, uint8_t out2 = 0){
		uint8_t in1, in2;
		SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
		digitalWriteFast(cs, LOW);

		SPI.transfer(command);
		delayMicroseconds(50);

		in1 = SPI.transfer(out1);
		delayMicroseconds(50);

		in2 = SPI.transfer(out2);
		//SPI.transfer(0);
		digitalWriteFast(cs, HIGH);
		SPI.endTransaction();

		return (in2 << 8) | in1;
	}
	// receive 16 bit data
	int16_t rx16(uint8_t command){
		return (txrx(command) << 8) | txrx(command);
	}

private:
	
};



class SLAVE2 : public SPISLAVE{
public:
	SLAVE2(uint8_t chip_select) : SPISLAVE(chip_select){};
	uint8_t getVBAT_LV(){
		return txrx(SLAVE2_COMMANDS::VBAT_REF_LV);
	}
	uint8_t getVBAT_HV(){
		return txrx(SLAVE2_COMMANDS::VBAT_REF_HV);
	}
	// void getTSOP_DATA(uint8_t *data){
	// 	data[0]  = txrx(SLAVE2_COMMANDS::TSOP_DATA0);
	// 	data[1]  = txrx(SLAVE2_COMMANDS::TSOP_DATA1);
	// 	data[2]  = txrx(SLAVE2_COMMANDS::TSOP_DATA2);
	// 	data[3]  = txrx(SLAVE2_COMMANDS::TSOP_DATA3);
	// 	data[4]  = txrx(SLAVE2_COMMANDS::TSOP_DATA4);
	// 	data[5]  = txrx(SLAVE2_COMMANDS::TSOP_DATA5);
	// 	data[6]  = txrx(SLAVE2_COMMANDS::TSOP_DATA6);
	// 	data[7]  = txrx(SLAVE2_COMMANDS::TSOP_DATA7);
	// 	data[8]  = txrx(SLAVE2_COMMANDS::TSOP_DATA8);
	// 	data[9]  = txrx(SLAVE2_COMMANDS::TSOP_DATA9);
	// 	data[10] = txrx(SLAVE2_COMMANDS::TSOP_DATA10);
	// 	data[11] = txrx(SLAVE2_COMMANDS::TSOP_DATA11);
	// 	data[12] = txrx(SLAVE2_COMMANDS::TSOP_DATA12);
	// 	data[13] = txrx(SLAVE2_COMMANDS::TSOP_DATA13);
	// 	data[14] = txrx(SLAVE2_COMMANDS::TSOP_DATA14);
	// 	data[15] = txrx(SLAVE2_COMMANDS::TSOP_DATA15);
	// 	data[16] = txrx(SLAVE2_COMMANDS::TSOP_DATA16);
	// 	data[17] = txrx(SLAVE2_COMMANDS::TSOP_DATA17);
	// 	data[18] = txrx(SLAVE2_COMMANDS::TSOP_DATA18);
	// 	data[19] = txrx(SLAVE2_COMMANDS::TSOP_DATA19);
	// 	data[20] = txrx(SLAVE2_COMMANDS::TSOP_DATA20);
	// 	data[21] = txrx(SLAVE2_COMMANDS::TSOP_DATA21);
	// 	data[22] = txrx(SLAVE2_COMMANDS::TSOP_DATA22);
	// 	data[23] = txrx(SLAVE2_COMMANDS::TSOP_DATA23);
	// }

	uint8_t getTSOP_ANGLE_BYTE(){
		return txrx(SLAVE2_COMMANDS::TSOP_ANGLE_BYTE);
	}
	uint8_t getTSOP_STRENGTH(){
		return txrx(SLAVE2_COMMANDS::TSOP_STRENGTH);
	}
	uint8_t getTSOPAngleStrength(int16_t &angle, uint8_t &strength){
		uint8_t one, two, three, four;

		SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
		digitalWriteFast(cs, LOW);

		one = SPI.transfer(SLAVE2_COMMANDS::TSOP_ANGLE_HIGH);
		delayMicroseconds(150);
		two = SPI.transfer(SLAVE2_COMMANDS::TSOP_ANGLE_LOW);
		delayMicroseconds(50);
		three = SPI.transfer(SLAVE2_COMMANDS::TSOP_STRENGTH);
		delayMicroseconds(50);
		four = SPI.transfer(0);

		digitalWriteFast(cs, HIGH);
		SPI.endTransaction();

		// Serial.print(one); Serial.print('\t');
		// Serial.print(two); Serial.print('\t');
		// Serial.print(three); Serial.print('\t');
		// Serial.println(four);

		angle = three | (two << 8);
		strength = four;
		return four;
	}
};

class SLAVE3 : public SPISLAVE{
public:
	SLAVE3(uint8_t chip_select) : SPISLAVE(chip_select){};
	uint8_t moveMotorA(int16_t velocity){
		txrx16(SLAVE3_COMMANDS::MOVE1, highByte(velocity), lowByte(velocity));
		return 0;
	}
	uint8_t moveMotorB(int16_t velocity){
		txrx16(SLAVE3_COMMANDS::MOVE2, highByte(velocity), lowByte(velocity));
		return 0;
	}
	uint8_t moveMotorC(int16_t velocity){
		txrx16(SLAVE3_COMMANDS::MOVE3, highByte(velocity), lowByte(velocity));
		return 0;
	}
	uint8_t moveMotorD(int16_t velocity){
		txrx16(SLAVE3_COMMANDS::MOVE4, highByte(velocity), lowByte(velocity));
		return 0;
	}
	uint8_t moveMotorE(int16_t velocity){
		txrx16(SLAVE3_COMMANDS::MOVE5, highByte(velocity), lowByte(velocity));
		return 0;
	}
	void getCS1(uint8_t &cs){
		cs = txrx(SLAVE3_COMMANDS::CSENSE1);
	}
	void getCS2(uint8_t &cs){
		cs = txrx(SLAVE3_COMMANDS::CSENSE2);
	}
	void getCS3(uint8_t &cs){
		cs = txrx(SLAVE3_COMMANDS::CSENSE3);
	}
	void getCS4(uint8_t &cs){
		cs = txrx(SLAVE3_COMMANDS::CSENSE4);
	}
	void getCS5(uint8_t &cs){
		cs = txrx(SLAVE3_COMMANDS::CSENSE5);
	}
	void getV1(int16_t &v){
		v = rx16(SLAVE3_COMMANDS::V1);
	}
	void getV2(int16_t &v){
		v = rx16(SLAVE3_COMMANDS::V2);
	}
	void getV3(int16_t &v){
		v = rx16(SLAVE3_COMMANDS::V3);
	}
	void getV4(int16_t &v){
		v = rx16(SLAVE3_COMMANDS::V4);
	}
	uint8_t moveRobot(uint8_t dir, uint8_t velocity, int8_t rotation_velocity){
		SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
		digitalWriteFast(cs, LOW);
		SPI.transfer(SLAVE3_COMMANDS::MOVE);
		SPI.transfer(dir);
		SPI.transfer(velocity);
		SPI.transfer(rotation_velocity);
		digitalWriteFast(cs, HIGH);
		SPI.endTransaction();
		return 0;
	}
};

// create objects
SLAVE1 Slave1;
SLAVE2 Slave2(SLAVE2_CS);
SLAVE3 Slave3(SLAVE3_CS);

#endif