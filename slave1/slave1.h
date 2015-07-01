#ifndef SLAVE1_H
#define SLAVE1_H

#include <Arduino.h>

#include <EEPROM.h>
#include <EEPROMAnything.h>
#include <SPI.h>
#include <SFE_LSM9DS0.h>

#define L1 		A7
#define L2 		A6
#define L3 		A3
#define L4 		A10
#define L5 		A15
#define L6 		A9
#define L7 		A2
#define L8 		A20
#define L9 		A17
#define L10		A16
#define L11		A8
#define L12		A11
#define L13		A12
#define L14		A13
#define L15		A18
#define L16		A19

class LIGHTARRAY{
public:
	uint8_t lightData[16] = {0};	// Light sensor data array
	
	uint8_t refData[16] = {0};
	uint8_t white[16] = {0};
	uint8_t green[16] = {0};

	uint8_t colours[16] = {0};
	uint8_t pColours[16] = {0};	// previous data
	uint8_t ppColours[16] = {0};	// even earlier data
	
	uint8_t armFrontSum = 0;
	uint8_t armBackSum = 0;
	uint8_t armLeftSum = 0;
	uint8_t armRightSum = 0;

	uint8_t lineLocation = 0;

	void init(){
		analogReference(EXTERNAL);

		pinMode(L1,		INPUT);
		pinMode(L2,		INPUT);
		pinMode(L3,		INPUT);
		pinMode(L4,		INPUT);
		pinMode(L5,		INPUT);
		pinMode(L6,		INPUT);
		pinMode(L7,		INPUT);
		pinMode(L8,		INPUT);
		pinMode(L9,		INPUT);
		pinMode(L10,	INPUT);
		pinMode(L11,	INPUT);
		pinMode(L12,	INPUT);
		pinMode(L13,	INPUT);
		pinMode(L14,	INPUT);
		pinMode(L15,	INPUT);
		pinMode(L16,	INPUT);

		loadCalibData();
	}
	void read(){
		analogReadResolution(8);	// 8 bits of resolution is enough for differentiating between black white and green
		analogReadAveraging(0);	// averaging DOES NOT EFFECT PERFORMANCE. It is performed on the ADC, not on the uC

		lightData[0]	= analogRead(L1);
		lightData[1]	= analogRead(L2);
		lightData[2]	= analogRead(L3);
		lightData[3]	= analogRead(L4);
		lightData[4]	= analogRead(L5);
		lightData[5]	= analogRead(L6);
		lightData[6]	= analogRead(L7);
		lightData[7]	= analogRead(L8);
		lightData[8] 	= analogRead(L9);
		lightData[9] 	= analogRead(L10);
		lightData[10]	= analogRead(L11);
		lightData[11]	= analogRead(L12);
		lightData[12]	= analogRead(L13);
		lightData[13]	= analogRead(L14);
		lightData[14]	= analogRead(L15);
		lightData[15]	= analogRead(L16);
	}
	void getColours(){
		for (int i = 0; i < 16; i++){
			colours[i] = lightData[i] > refData[i] ? 1 : 0;
		}
		armFrontSum = colours[0]  + colours[2]  + colours[1]  + colours[3]; // include light sensor 4
		armBackSum  = colours[9]  + colours[11] + colours[13] + colours[8];
		armRightSum = colours[7]  + colours[6]  + colours[5]  + colours[4];
		armLeftSum  = colours[15] + colours[14] + colours[10] + colours[12];

		memcpy(&ppColours, pColours, 16 * sizeof(colours[0]));
		memcpy(&pColours, colours, 16 * sizeof(colours[0]));
	}	
	void calibWhite(){
		read();
		for (int i = 0; i < 16; i++){
			white[i] = lightData[i];
		}
	}
	void calibGreen(){
		read();
		for (int i = 0; i < 16; i++){
			green[i] = lightData[i];
		}
	}
	void endCalib(){
		for (int i = 0; i < 16; i++){
			if (white[i] > green[i] + 2 && (white[i] + green[i])/2 > 150){
				refData[i] = (white[i] + green[i])/2;
			}
			else{
				// data isn't good enough
				refData[i] = 255;
			}
		}
		saveCalibData();
	}
	void saveCalibData(){
		EEPROM_writeAnything(100,  refData);
	}
	void loadCalibData(){
		EEPROM_readAnything(100,  refData);
	}

private:

};


enum PERIPHERALSTATUS{
	NOERRORS,
	IMU_ERR,
	LIGHTARRAY_ERR,
	ADNS_ERR,
	IMU_LIGHTARRAY_ERR,
	IMU_ADNS_ERR,
	LIGHTARRAY_ADNS_ERR,
	IMU_LIGHTARRAY_ADNS_ERR
};

// Magnetic Declination/Inclination - see
// http://autoquad.org/wiki/wiki/configuring-autoquad-flightcontroller/autoquad-calibrations/calibration-faq/magnetic-declination-and-inclination/
// Data acquired from http://magnetic-declination.com
#define DECLINATION  10.92 // (Newmarket, Brisbane, Australia)
#define INCLINATION  57.37 // (Newmarket, Brisbane, Australia)

// Uncomment when in China
// #define DECLINATION  -5.03 // (Hefei, China)
// #define INCLINATION  48.18 // (Hefei, China)

#define LSM9DS0_CSG  2  // CSG connected to Arduino pin 9
#define LSM9DS0_CSXM 10 // CSXM connected to Arduino pin 10

#define INT1XM 5 // INT1XM tells us when accel data is ready
#define INT2XM 9 // INT2XM tells us when mag data is ready
#define DRDYG  8 // DRDYG  tells us when gyro data is ready

class IMU : public LSM9DS0{
public:
	IMU() : LSM9DS0(LSM9DS0_CSG, LSM9DS0_CSXM){};
	
	// imu
	float gScale = 1.16;
	elapsedMicros lastReadG = 0, lastReadM = 0, lastCompFilter = 0;
	uint32_t dtG, dtM;
	float pitch, yaw, roll;
	float abias[3] = {0, 0, 0}, gbias[3] = {0, 0, 0};
	float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
	float MagMinX = 0, MagMaxX = 0,
		  MagMinY = 0, MagMaxY = 0,
		  MagMinZ = 0, MagMaxZ = 0;
	float MagOffsetX, MagOffsetY, MagOffsetZ;
	float MagScaleX = 1, MagScaleY = 1, MagScaleZ = 1;
	float MagYaw, lMagYaw, MagYawRate;
	int16_t bearing;
	int16_t offset;

	void init(){
		// MagMinX = -0.155012;
		// MagMaxX = 0.332781;
		// MagMinY = -0.138394;
		// MagMaxY = 0.311908;
		// MagMinZ = 0;
		// MagMaxZ = 0;
		// MagScaleX = 0.810280932;
		// MagScaleY = 0.877742863;
		readMagCalibrations(); // read magnetometer calibrations from EEPROM
		preCalculateCalibParams();
		// Set up interrupt pins as inputs:
		pinMode(INT1XM, INPUT);
		pinMode(INT2XM, INPUT);
		pinMode(DRDYG,  INPUT);
		
		uint16_t status = LSM9DS0::begin();
		delay(200);
		LSM9DS0::setAccelScale(LSM9DS0::A_SCALE_2G);
		LSM9DS0::setGyroScale(LSM9DS0::G_SCALE_245DPS);
		LSM9DS0::setMagScale(LSM9DS0::M_SCALE_2GS);
		
		LSM9DS0::setAccelODR(LSM9DS0::A_ODR_1600);
		LSM9DS0::setAccelABW(LSM9DS0::A_ABW_50); // Choose lowest filter setting for low noise
		LSM9DS0::setGyroODR(LSM9DS0::G_ODR_760_BW_100);  // Set gyro update rate to 190 Hz with the smallest bandwidth for low noise
		LSM9DS0::setMagODR(LSM9DS0::M_ODR_100); // Set magnetometer to update every 80 ms
		LSM9DS0::calLSM9DS0(gbias, abias); // not there is a 1s delay with this function
		yaw = atan2(mx,my) * 180/PI;
		lMagYaw = yaw;
	}

	void calibOffset(){
		float temp;
		for (int i = 0; i < 1000; i++){
			read();	// read imu
			temp += yaw;
			delayMicroseconds(10);
		}
		offset = temp / 1000;		
		//TOBEARING360(offset);
		// store the offset for potential future use.
		storeOffset();
	}

	void storeOffset(){
		EEPROM.write(510, highByte(bearing));
		EEPROM.write(511, lowByte(bearing));
	}

	void getStoredOffset(){
		bearing = EEPROM.read(511) | (EEPROM.read(510) << 8);
	}

	void read(){
		
		if(digitalReadFast(DRDYG) == HIGH){  // When new gyro data is ready
			LSM9DS0::readGyro();		   // Read raw gyro data

			dtG = lastReadG;
			lastReadG = 0;
			gx = LSM9DS0::calcGyro(LSM9DS0::gx) - gbias[0];   // Convert to degrees per seconds, remove gyro biases
			gy = LSM9DS0::calcGyro(LSM9DS0::gy) - gbias[1];
			gz = LSM9DS0::calcGyro(LSM9DS0::gz) - gbias[2];
			gx *= gScale;
			gy *= gScale;
			gz *= gScale;
			
		}
		
		if(digitalReadFast(INT1XM) == HIGH){  // When new accelerometer data is ready
			LSM9DS0::readAccel();		 // Read raw accelerometer data
			ax = LSM9DS0::calcAccel(LSM9DS0::ax) - abias[0];   // Convert to g's, remove accelerometer biases
			ay = LSM9DS0::calcAccel(LSM9DS0::ay) - abias[1];
			az = LSM9DS0::calcAccel(LSM9DS0::az) - abias[2];
		}
	  
		if(digitalReadFast(INT2XM) == HIGH){  // When new magnetometer data is ready
			LSM9DS0::readMag();		   // Read raw magnetometer data
			mx = LSM9DS0::calcMag(LSM9DS0::mx);	 // Convert to Gauss and correct for calibration
			my = LSM9DS0::calcMag(LSM9DS0::my);
			mz = LSM9DS0::calcMag(LSM9DS0::mz);
			// Serial.print(MagOffsetX);
			// Serial.print('\t');
			// Serial.print(MagOffsetY);
			// Serial.print('\t');
			// Serial.print(MagScaleX);
			// Serial.print('\t');
			// Serial.print(MagScaleY);
			// Serial.print('\t');
			// Serial.print(mx, 3);
			// Serial.print('\t');
			// Serial.println(my, 3);
			mx -= MagOffsetX;
			my -= MagOffsetY;
			mz -= MagOffsetZ;
			mx *= MagScaleX;
			my *= MagScaleY;
			mz *= MagScaleZ;

			dtM = lastReadM;
			lastReadM = 0;

			MagYaw = atan2(mx,my) * 180 / PI;

			// float dMagYaw = MagYaw - lMagYaw;
			// if (dMagYaw > 180){
			// 	// change in yaw is wrong!
			// 	dMagYaw -= 360;
			// }
			// else if (dMagYaw < -180){
			// 	dMagYaw += 360;
			// }
			// Serial.print(dMagYaw);
			// Serial.print('\t');
			// Serial.print(lMagYaw);
			// Serial.print('\t');
			// Serial.println(MagYaw);
			
			// MagYawRate = dMagYaw / dtM * 1000000; // change in magnetometer yaw rate in degrees per second
			lMagYaw = MagYaw;
		}
	}

	// function to get yaw with complementary filter.
	void complementaryFilterBearing(float aa){
		uint32_t dtLastCompFilter = lastCompFilter;
		lastCompFilter = 0;

		yaw += gz * dtLastCompFilter / 1000000;

		// the following fixes issues when say yaw is like -175 degrees
		// while MagYaw is 175 degrees (comp filter should give around 180 degrees)
		// but it'll move it towards zero
		if (yaw - MagYaw > 180){
			MagYaw += 360;
		}
		else if (yaw - MagYaw < -180){
			MagYaw -= 360;
		}

		// Serial.print(yaw);
		// Serial.print('\t');
		// Serial.print(MagYaw);

		// Serial.print('\t');
		// Serial.print(gz);

		yaw = aa * yaw + (1.0 - aa) * MagYaw;
		
		TOBEARING180(yaw);

		// Serial.print('\t');
		// Serial.println(yaw);
	}	

	void preCalculateCalibParams(){
		MagOffsetX = (MagMinX + MagMaxX) / 2.0;
		MagOffsetY = (MagMinY + MagMaxY) / 2.0;
		MagOffsetZ = (MagMinZ + MagMaxZ) / 2.0;
		// soft iron error correction
		float VMaxX = MagMaxX - MagOffsetX;
		float VMaxY = MagMaxY - MagOffsetY;
		float VMaxZ = MagMaxZ - MagOffsetZ;

		float VMinX = MagMinX - MagOffsetX;
		float VMinY = MagMinY - MagOffsetY;
		float VMinZ = MagMinZ - MagOffsetZ;

		float avgX = (VMaxX - VMinX)/2;
		float avgY = (VMaxY - VMinY)/2;
		float avgZ = (VMaxZ - VMinZ)/2;

		float avgRadius = avgX + avgY; // scrap the Z axis
		avgRadius /= 2;

		MagScaleX = avgRadius/avgX;
		MagScaleY = avgRadius/avgY;
		MagScaleZ = avgRadius/avgZ;

		
	}

	void initCalibMagRoutine(){
		// don't want compensation/calibrated readings when calibrating!

		MagOffsetX = 0;
		MagOffsetY = 0;
		MagOffsetZ = 0;

		MagScaleX = 1;
		MagScaleY = 1;
		MagScaleZ = 1;

		// wait for magnetometer data to come
		// while(digitalReadFast(INT2XM) == LOW){};

		// LSM9DS0::readMag();		   // Read raw magnetometer data
		// MagMinX = LSM9DS0::calcMag(LSM9DS0::mx);
		// MagMinY = LSM9DS0::calcMag(LSM9DS0::my);
		// MagMinZ = LSM9DS0::calcMag(LSM9DS0::mz);
		// MagMaxX = MagMinX;
		// MagMaxY = MagMinY;
		// MagMaxZ = MagMinZ;
		MagMinX = 100;
		MagMinY = 100;
		MagMinZ = 100;
		MagMaxX = -100;
		MagMaxY = -100;
		MagMaxZ = -100;
	}
	// calibrate magnetomer. Please just rotate the magnetometer around
	// until you get pretty much every angle!
	void calibMagRoutine(){
		read();
		if (mx < MagMinX) MagMinX = mx;
		if (mx > MagMaxX) MagMaxX = mx;
		
		if (my < MagMinY) MagMinY = my;
		if (my > MagMaxY) MagMaxY = my;

		if (mz < MagMinZ) MagMinZ = mz;
		if (mz > MagMaxZ) MagMaxZ = mz;
	}

	void storeMagCalibrations(){
		EEPROM_writeAnything(0,  MagMinX);
		EEPROM_writeAnything(4,  MagMaxX);
		EEPROM_writeAnything(8,  MagMinY);
		EEPROM_writeAnything(12, MagMaxY);
		EEPROM_writeAnything(16, MagMinZ);
		EEPROM_writeAnything(20, MagMaxZ);
	}

	void readMagCalibrations(){
		EEPROM_readAnything(0,  MagMinX);
		EEPROM_readAnything(4,  MagMaxX);
		EEPROM_readAnything(8,  MagMinY);
		EEPROM_readAnything(12, MagMaxY);
		EEPROM_readAnything(16, MagMinZ);
		EEPROM_readAnything(20, MagMaxZ);
	}

	// calibrate gyro offset (gbias). Keep robot still for 1s.
	void calibGyroDrift(){
		float dx = 0, dy = 0, dz = 0;
		float temp = gScale;
		gScale = 1;
		yaw = 0;

		gbias[0] = 0; gbias[1] = 0; gbias[2] = 0;
		while(!Serial.available()){};
		while(Serial.available()){Serial.read();}

		elapsedMicros calibDriftTime = 0;
		calibDriftTime = 0;
		while (calibDriftTime < 1000000){
			read();
			complementaryFilterBearing(1); // full gyro
		}
		//gbias[2] = yaw;
		Serial.print(gbias[0]);
		Serial.print('\t');
		Serial.print(gbias[1]);
		Serial.print('\t');
		Serial.println(gbias[2]);
	}
};

class SLAVE1 : public HardwareSerial{
public:
	enum PERIPHERALSTATUS{
		NOERRORS,
		IMU_ERR,
		LIGHTARRAY_ERR,
		ADNS_ERR,
		IMU_LIGHTARRAY_ERR,
		IMU_ADNS_ERR,
		LIGHTARRAY_ADNS_ERR,
		IMU_LIGHTARRAY_ADNS_ERR
	};

	LIGHTARRAY lightArray;
	IMU imu;

	PERIPHERALSTATUS peripheralStatus = PERIPHERALSTATUS::NOERRORS;
	LOCATION location = LOCATION::FIELD;
	LINELOCATION lineLocation = LINELOCATION::UNKNOWN;

	// mouse sensor
	uint8_t x, y;
	
	uint8_t checkIfRequested(){
    	if(HardwareSerial::available() <= 0) return 255;
    	else return HardwareSerial::read();
    }

	// packets are ended with a newline characted '\n'. The newline character is not added to the array
    void sendPacket(uint8_t *dataOut, uint8_t length){
        HardwareSerial::write(dataOut, length);
    }
    
    bool receivePacket(uint8_t *dataIn, uint8_t maxLength){
        if (HardwareSerial::readBytesUntil('\n', dataIn, maxLength) <= 0){
            return false;   // this situation should never occur
        }
        return true;
    }
};

SLAVE1 slave1;

#endif