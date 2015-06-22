/*
	TSOP - Library for array of TSOPS
	Created by Brian Chen 14/09/2013
	Last Modified by Brian Chen 05/01/2014 11:59pm

	This library has been updated for robocup soccer 2014

	Release 1.10
*/

#include "Arduino.h"
#include "TSOP.h"

#define SHIFT_INC 1

inline void fabls(uint8_t *py, float &tp, uint8_t b_value)
{
	uint8_t px[TSOP_COUNT] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 16, 170, 180, 190 };
	int16_t dy, b_shift = 0;
	int32_t s = 0, bs = 30000;
	uint8_t i;

	for (uint8_t shift = 0; shift < TSOP_COUNT * 10; shift += SHIFT_INC){
		s = 0;

		for (i = 0; i < TSOP_COUNT; i++)
		{
			// find dy to get variance
			//dy = py[i] + 3 * px[i] * px[i] / 200 - 3 * px[i];
		
			dy = -1 * b_value * px[i] * px[i] / 10000 + 200 * b_value * px[i] / 10000;
			dy = py[i] - dy;
			//dy = py[i] + 5 * px[i] * px[i] / 303 - 33 * px[i] / 10;

			s += dy * dy;	
			// get ready for next iteration
			px[i] += SHIFT_INC;
			if (px[i] >= 200)
				px[i] -= 200;				
		}
		s /= 100;
		if (s < bs){
			bs = s;
			b_shift = shift;
		}
	}

	tp = 100 - b_shift;
	tp /= 10;
	if (tp < 0)
		tp += 20;
	else if (tp >= 20)
		tp -= 20;
}

inline float getMidIndex(float a, float b){
	float index;
	if (abs(a - b) > TSOP_COUNT / 2){
		index = ((a + b) / 2 + TSOP_COUNT / 2);
		if (index >= TSOP_COUNT){
			index -= TSOP_COUNT;
		}
	}
	else{
		index = (a + b) / 2;
	}
	return index;
}

inline void sortIndex(uint8_t *a, uint8_t size, int8_t *index) {
	for (uint8_t i = 0; i<(size - 1); i++) {
		for (uint8_t o = 0; o<(size - (i + 1)); o++) {
			if (a[o] > a[o + 1]) {
				uint8_t t = a[o];
				a[o] = a[o + 1];
				a[o + 1] = t;
				uint8_t tt = index[o];
				index[o] = index[o + 1];
				index[o + 1] = tt;
			}
		}
	}
}

inline void sort(uint8_t *a, uint8_t size) {
	for (uint8_t i = 0; i<(size - 1); i++) {
		for (uint8_t o = 0; o<(size - (i + 1)); o++) {
			if (a[o] > a[o + 1]) {
				uint8_t t = a[o];
				a[o] = a[o + 1];
				a[o + 1] = t;
			}
		}
	}
}

// initiate TSOP
TSOP::TSOP(const uint8_t *tsops, uint8_t vcc){
	tsop_pins = tsops;
	_vcc = vcc;

	pinMode(_vcc, OUTPUT);
  	on();

	//make sure all tsop pins are set as input
	for(uint8_t i = 0; i < TSOP_COUNT; i++){
		pinMode(tsop_pins[i],INPUT);
	}		
}

void TSOP::getAll(uint8_t *results){
	uint8_t r[TSOP_COUNT] = { 0 };
	//make sure tsops are on	
	on();
	//read each tsop for MAX_COUNT amount of times
	for(uint8_t z = 0; z < MAX_COUNT; z++){
		for(uint8_t i = 0; i < TSOP_COUNT; i++){			
			//if HIGH, set result as 0, otherwise 1
            #if(FAST_MODE)  //fast mode
                r[i] += (digitalReadFast(tsop_pins[i]) == HIGH ? 0 : 2); 	            
            #else           //normal mode
                r[i] += (digitalRead(tsop_pins[i]) == HIGH ? 0 : 2); 	            
            #endif					
		}	
		delayMicroseconds(20); //short delay
	}	
	//if a tsop is faulty and constantly returning high, MAX_COUNT of "hits" will be achieved. Do this to remove error.
	for(uint8_t i = 0; i < TSOP_COUNT; i++){
		if(r[i] < MIN_IGNORE_THRESHOLD || r[i] > MAX_IGNORE_THRESHOLD){
			r[i] = 0;
		}
	}
	memcpy(results, r, TSOP_COUNT);
}

void TSOP::getAllInvert(uint8_t *results){
	uint8_t r[TSOP_COUNT] = { 0 };
	//make sure tsops are on	
	on();
	//read each tsop for MAX_COUNT amount of times
	for (uint8_t z = 0; z < TSOP_COUNT; z++){
		for (uint8_t i = 0; i < MAX_COUNT; i++){
			//if HIGH, set result as 0, otherwise 1
#if(FAST_MODE)  //fast mode
			r[z] += (digitalReadFast(tsop_pins[z]) == HIGH ? 0 : 1);
#else           //normal mode
			r[z] += (digitalRead(tsop_pins[z]) == HIGH ? 0 : 1);
#endif					
		}
	}
	//if a tsop is faulty and constantly returning high, MAX_COUNT of "hits" will be achieved. Do this to remove error.
	for (uint8_t i = 0; i < TSOP_COUNT; i++){
		if (r[i] < MIN_IGNORE_THRESHOLD || r[i] > MAX_IGNORE_THRESHOLD){
			r[i] = 0;
		}
	}
	memcpy(results, r, TSOP_COUNT);
}

void TSOP::getAllPulseIn(uint8_t *results){
	uint8_t r[TSOP_COUNT] = { 0 };
	for (uint8_t i = 0; i < TSOP_COUNT; i++){
		r[i] = pulseIn(tsop_pins[i], LOW, 1000) / 3;
	}
	//if a tsop is faulty and constantly returning high, MAX_COUNT of "hits" will be achieved. Do this to remove error.
	for (uint8_t i = 0; i < TSOP_COUNT; i++){
		if (r[i] < MIN_IGNORE_THRESHOLD || r[i] > MAX_IGNORE_THRESHOLD){
			r[i] = 0;
		}
	}
	memcpy(results, r, TSOP_COUNT);
}

//function to get the tsop with the most "hits" and returns its index and number of "hits" via the parameters (value and index)
void TSOP::getBest(uint8_t *results, uint8_t& index, uint8_t& value){
	index = 0;
	value = 0;
	for(uint8_t i = 0; i < TSOP_COUNT; i++){
  		if(results[i] > value){ 
			if(results[i] != MAX_COUNT){
  				value = results[i];			
				index = i;	
			}
  		}		
  	}
}

//function to get approximated angle, using basic method (get index and convert to angle)
void TSOP::getAngle(uint8_t *results, float &angle){
	uint8_t b_index, b_value;	//variables to store getBest() results
	getBest(results, b_index, b_value);
	angle = b_index * 360 / TSOP_COUNT; //convert index to angle in 360 degrees
}

void TSOP::getAngleAdv(uint8_t *results, float &angle){
	int8_t indexes[TSOP_COUNT] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

	uint8_t r[TSOP_COUNT];

	for (uint8_t i = 0; i < TSOP_COUNT; i++){
		r[i] = results[i];
	}
	sortIndex(r, TSOP_COUNT, indexes);

	float index = (float)indexes[19];

	if (abs(r[19] - r[18]) < 30 && abs(r[19] - r[17] < 30 * 2)){
		//index = getMidIndex(indexes[18], indexes[19]);
		index = getMidIndex(getMidIndex(indexes[18], indexes[19]), getMidIndex(indexes[17], indexes[19]));
		//Serial.println("indexe: " + String(index));
	}
	else if (abs(r[19] - r[18]) < 30){
		index = getMidIndex(indexes[18], indexes[19]);
	}
	
	angle = index * 360 / TSOP_COUNT;
}

void TSOP::getAngleReg(uint8_t *results, float &angle, uint8_t b){
	float index;
	fabls(results, index, b);
	angle = index * 360 / TSOP_COUNT;
}


//function to get signal strength
void TSOP::getStrength(uint8_t *results, uint8_t &strength){
	uint8_t s[TSOP_COUNT];
	for (int i = 0; i < TSOP_COUNT; i++){
		s[i] = results[i];
	}
	//uint8_t temp[TSOP_COUNT] = *results;
	sort(s, TSOP_COUNT);
	
	strength = (s[19] + s[18]) / 2;	//get highest 2 values and average. This is the strength.
}


//advanced function for angle estimation using pairing algorith. Currently does not work.
void TSOP::getAnglePairing(uint8_t *results, float &angle){
	int8_t indexes[TSOP_COUNT] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -9, -8, -7, -6, -5, -4, -3, -2, -1 };
	int8_t indexesPos[TSOP_COUNT] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
	uint8_t r[TSOP_COUNT];
	float tempAngle;
	angle = 0;

	uint8_t unusedPairs = 0;

	for (uint8_t i = 0; i < TSOP_COUNT; i++){
		r[i] = results[i];
	}
	sortIndex(r, TSOP_COUNT, indexes);
	sortIndex(r, TSOP_COUNT, indexesPos);
	//sorted indexes in order of strength. Now pair them in strength.
	for (uint8_t i = TSOP_COUNT; i > 1; i--){
		if (r[indexesPos[i]] == 0 || r[indexesPos[i - 1]] == 0){
			unusedPairs++;
		}
		else{
			//tempAngle = (indexes[i] + indexes[i - 1]) / 2;
			//if (indexes[i] - indexes[i - 1] > TSOP_COUNT / 2){
			//	//tempAngle += TSOP_COUNT / 2;
			//}
			//if (indexes[i - 1] - indexes[i] > TSOP_COUNT / 2){
			//	//tempAngle += TSOP_COUNT / 2;
			//}
			tempAngle = getMidIndex(indexesPos[i], indexesPos[i - 1]);
			angle += tempAngle; //avereage all tempAngles
		}
	}
	angle /= (TSOP_COUNT - 1 - unusedPairs);
	//angle *= 360/TSOP_COUNT; //scale to 360
}



//function to turn on tsops
void TSOP::on(){
	digitalWrite(_vcc, HIGH);
	isOn = true;
}

//function to turn off tsops
void TSOP::off(){
	digitalWrite(_vcc, LOW);
	isOn = false;
}

void TSOP::unlock(){
	off();
	delayMicroseconds(500); //small delay to make sure the tsops are actually off.
	on();
	delayMicroseconds(500);
}