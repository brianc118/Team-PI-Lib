#ifndef TSOPS_H
#define TSOPS_H

#include <Arduino.h>
#include <piCommon.h>

#define STRENGTHFILTER 50
#define TSOP_COUNT 24

#define UNLOCK_PIN1 25
#define UNLOCK_PIN2 32
#define UNLOCK_PIN3 33

#define TSOP1  4
#define TSOP2  9
#define TSOP3  8
#define TSOP4  7
#define TSOP5  6
#define TSOP6  15
#define TSOP7  18
#define TSOP8  19
#define TSOP9  20
#define TSOP10 28
#define TSOP11 1
#define TSOP12 24
#define TSOP13 31
#define TSOP14 16
#define TSOP15 17
#define TSOP16 5
#define TSOP17 0
#define TSOP18 22
#define TSOP19 23
#define TSOP20 21
#define TSOP21 26
#define TSOP22 27
#define TSOP23 2
#define TSOP24 3

// divisor is 16. i.e. 7 is really 7/16
// K1 + 2*K2 + 2*K3 + 2*K4 = 16
// #define K1 8
// #define K2 2
// #define K3 1
// #define K4 1
#define K1 12
#define K2 3
#define K3 2
#define K4 0

#define MIN_IGNORE_THRESHOLD 5	 //minimum tsop reading to be deemed as valid, anything below is noise
#define MAX_IGNORE_THRESHOLD 180 //maximum tsop reading to be deemed as valid, anything above is noise or a disconnected sensor


// faster method for atan2 ?
#define PI_FLOAT     3.14159265f
#define PIBY2_FLOAT  1.5707963f
// |error| < 0.005
// static float fast_atan2(int y, int x){             
// 	if ( x == 0.0f ){                              
// 		if ( y > 0.0f ) return 180.0f;             
// 		if ( y == 0.0f ) return 0.0f;              
// 		return -90.0f;
// 	}
// 	float atan;
// 	float z = y/x;
// 	if (abs(z) < 1.0f){
// 		atan = z * (45 - 15.66f * (z - 1));
// 		if (x < 0.0f){
// 			if (y < 0.0f){ return atan - 180.0f; }
// 			return atan + 180.0f;
// 		}
// 	}
// 	else{

// 	}
// }

// float fast_atan2f( float y, float x )
// {
// 	if ( x == 0.0f )
// 	{
// 		if ( y > 0.0f ) return PIBY2_FLOAT;
// 		if ( y == 0.0f ) return 0.0f;
// 		return -PIBY2_FLOAT;
// 	}
// 	float atan;
// 	float z = y/x;
// 	if ( fabsf( z ) < 1.0f )
// 	{
// 		atan = z/(1.0f + 0.28f*z*z);
// 		if ( x < 0.0f )
// 		{
// 			if ( y < 0.0f ) return atan - PI_FLOAT;
// 			return atan + PI_FLOAT;
// 		}
// 	}
// 	else
// 	{
// 		atan = PIBY2_FLOAT - z/(z*z + 0.28f);
// 		if ( y < 0.0f ) return atan - PI_FLOAT;
// 	}
// 	return atan;
// }

inline int mod(int x, int m){
    int r = x % m;
    return r < 0 ? r + m : r;
}

class TSOPS{
public:
	uint8_t tempData[TSOP_COUNT] = {0};
	uint8_t data[TSOP_COUNT] = {0};
	uint8_t filteredData[TSOP_COUNT] = {0};
	uint8_t filteredDataSorted[TSOP_COUNT] = {0};
	uint8_t indexes[TSOP_COUNT] = {0};
	int16_t angle = 0;
	uint8_t angleByte = 0;
	uint8_t randomThingy = 55;
	uint8_t strength;
	uint8_t strengths[STRENGTHFILTER] = {0};
	uint8_t averageStrength;

	int32_t x = 0, y = 0;

	void begin(){
		pinMode(UNLOCK_PIN1, OUTPUT);
		pinMode(UNLOCK_PIN2, OUTPUT);

		pinMode(TSOP1, INPUT);
		pinMode(TSOP2, INPUT);
		pinMode(TSOP3, INPUT);
		pinMode(TSOP4, INPUT);
		pinMode(TSOP5, INPUT);
		pinMode(TSOP6, INPUT);
		pinMode(TSOP7, INPUT);
		pinMode(TSOP8, INPUT);
		pinMode(TSOP9, INPUT);
		pinMode(TSOP10, INPUT);
		pinMode(TSOP11, INPUT);
		pinMode(TSOP12, INPUT);
		pinMode(TSOP13, INPUT);
		pinMode(TSOP14, INPUT);
		pinMode(TSOP15, INPUT);
		pinMode(TSOP16, INPUT);
		pinMode(TSOP17, INPUT);
		pinMode(TSOP18, INPUT);
		pinMode(TSOP19, INPUT);
		pinMode(TSOP20, INPUT);
		pinMode(TSOP21, INPUT);
		pinMode(TSOP22, INPUT);
		pinMode(TSOP23, INPUT);
		pinMode(TSOP24, INPUT);

		// determine cos table
		double temp;
		double temp_angle;
		
		for (int i = 0; i < TSOP_COUNT; i++){
			// remember, angle = -bearing + 90 = -bearing + PI/2
			// angle = i * 2 * PI / TSOP_COUNT
			temp_angle = 1.5707963f - 6.283185307f * i / TSOP_COUNT;

			temp = cos(temp_angle) * 4095.0f;
			scaledCos[i] = temp;

			temp = sin(temp_angle) * 4095.0f;			
			scaledSin[i] = temp;
		}
		
		unlock();
	}
	// read sensors
	void read(){
		// digitalRead(Fast) returns an uint8_t. HIGH and LOW are just definitions
		tempData[0]  += digitalReadFast(TSOP1)  ^ 1;
		tempData[1]  += digitalReadFast(TSOP2)  ^ 1;
		tempData[2]  += digitalReadFast(TSOP3)  ^ 1;
		tempData[3]  += digitalReadFast(TSOP4)  ^ 1;
		tempData[4]  += digitalReadFast(TSOP5)  ^ 1;
		tempData[5]  += digitalReadFast(TSOP6)  ^ 1;
		tempData[6]  += digitalReadFast(TSOP7)  ^ 1;
		tempData[7]  += digitalReadFast(TSOP8)  ^ 1;
		tempData[8]  += digitalReadFast(TSOP9)  ^ 1;
		tempData[9]  += digitalReadFast(TSOP10) ^ 1;
		tempData[10] += digitalReadFast(TSOP11) ^ 1;
		tempData[11] += digitalReadFast(TSOP12) ^ 1;
		tempData[12] += digitalReadFast(TSOP13) ^ 1;
		tempData[13] += digitalReadFast(TSOP14) ^ 1;
		tempData[14] += digitalReadFast(TSOP15) ^ 1;
		tempData[15] += digitalReadFast(TSOP16) ^ 1;
		tempData[16] += digitalReadFast(TSOP17) ^ 1;
		tempData[17] += digitalReadFast(TSOP18) ^ 1;
		tempData[18] += digitalReadFast(TSOP19) ^ 1;
		tempData[19] += digitalReadFast(TSOP20) ^ 1;
		tempData[20] += digitalReadFast(TSOP21) ^ 1;
		tempData[21] += digitalReadFast(TSOP22) ^ 1;
		tempData[22] += digitalReadFast(TSOP23) ^ 1;
		tempData[23] += digitalReadFast(TSOP24) ^ 1;
		// tempData[23] = 100;
	}
	void finishRead(){
		// copy tempData to data. data[] contains the complete readings
		// memcpy(data, tempData, sizeof(tempData));
		for (int i = 0; i < TSOP_COUNT; i++){
			data[i] = tempData[i];
		}
		// reset the other arrays
		// CLEARARRAY(data);
		CLEARARRAY(tempData);
		CLEARARRAY(filteredDataSorted);
		// memset(tempData, 0, sizeof(tempData));
		// memset(filteredDataSorted, 0, sizeof(filteredDataSorted));
		// memset(indexes, 0, sizeof(indexes));
	}
	/* For some odd reason, TSOPs "lock" to the ball and become extremely sensative after a few seconds.
	   This is why they are powered by the Teensy, to allow the Teensy to turn them on/off */
	void on(){
		digitalWriteFast(UNLOCK_PIN1, HIGH);
		digitalWriteFast(UNLOCK_PIN2, HIGH);
	}
	void off(){
		digitalWriteFast(UNLOCK_PIN1, LOW);
		digitalWriteFast(UNLOCK_PIN2, LOW);
	}
	void unlock(){
		off();
		//delay(500);
		delayMicroseconds(10000);  // since we have a pullup now, this will need to be longer
		on();
	}
	void filterData(){
		int i;
		//if a tsop is faulty and constantly returning high, MAX_COUNT of "hits" will be achieved. Do this to remove error.
		for(i = 0; i < TSOP_COUNT; i++){
			if(data[i] < MIN_IGNORE_THRESHOLD || data[i] > MAX_IGNORE_THRESHOLD){
				data[i] = 0;
			}
		}
		for (i = 0; i < TSOP_COUNT; i++){
			/* a rather efficient way to filter data by scoring each data by the tsop by it's
			adjacent tsops. Scoring 3 extra pairs is enough (perhaps even 2) 
			We have to use our own mod function! the % operator is actually remainder in c++. i.e.
			doesn't work for negatives! */
			uint16_t tempData = K1 *  data[i]
						      + K2 * (data[mod(i - 1, TSOP_COUNT)] + data[mod(i - 1, TSOP_COUNT)])
						      + K3 * (data[mod(i - 2, TSOP_COUNT)] + data[mod(i - 2, TSOP_COUNT)])
						      + K4 * (data[mod(i - 3, TSOP_COUNT)] + data[mod(i - 3, TSOP_COUNT)]);

			filteredData[i] = tempData >> 4; // divide by 16 (2^4)

			// sort data from highest to lowest
			for (int j = 0; j < TSOP_COUNT; j++){
				if (filteredData[i] > filteredDataSorted[j]){
					// we've found our place!
					// shift elements from index j down
					if (j <= i){
						// make sure we only shift what is needed
						ARRAYSHIFTDOWN(filteredDataSorted, j, i);
						ARRAYSHIFTDOWN(indexes, j, i);
					}
					filteredDataSorted[j] = filteredData[i];
					indexes[j] = i;
					break;
				}
			}
		}
	}
	void getStrength(){
		strength = data[indexes[0]];
		ARRAYSHIFTDOWN(strengths, 0, STRENGTHFILTER - 2);
		strengths[0] = strength;
		ARRAYAVERAGE(strengths, averageStrength);
	}
	void getAngle(){
		angle = 360 * indexes[0] / TSOP_COUNT; // get angle
		angleByte = 255 * indexes[0] / TSOP_COUNT;
	}
	/* gets an estimate of the ball angle using circular averaging of indexes
	   http://en.wikipedia.org/wiki/Mean_of_circular_quantities.
	   Note: too much averaging may impair response time and accuracy */
	void getAngle(uint8_t n){
		x = 0;
		y = 0;
		for (int i = 0; i < n; i++){
			// convert vector to cartesian (remember that each bitshift << is *2)
			x += filteredDataSorted[i] * scaledCos[indexes[i]];
			y += filteredDataSorted[i] * scaledSin[indexes[i]];
		}
		if (x == 0 && y == 0){
			// When vectors sum to (0, 0), we're in trouble. We've got some dodgy data
			angle = 0;
		}
		else{
			angle = 90 - (57.3 * atan2(y, x));
		}
		if (angle < 0) angle += 360;
		
		if (angle < 0){
			angle += 360;
		}
		angleByte = angle * 255 / 360;
		
	}
private:
	// variable to store trig table. Note that this is for bearings!
	int16_t scaledCos[TSOP_COUNT] = {0};
	int16_t scaledSin[TSOP_COUNT] = {0};
};

#endif