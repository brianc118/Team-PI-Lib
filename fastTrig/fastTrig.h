#ifndef FASTTRIG_H
#define FASTTRIG_H

static int16_t sin_table[] = {
	0,
	572,
	1144,
	1715,
	2286,
	2856,
	3425,
	3993,
	4560,
	5126,
	5690,
	6252,
	6813,
	7371,
	7927,
	8481,
	9032,
	9580,
	10126,
	10668,
	11207,
	11743,
	12275,
	12803,
	13328,
	13848,
	14364,
	14876,
	15383,
	15886,
	16383,
	16876,
	17364,
	17846,
	18323,
	18794,
	19260,
	19720,
	20173,
	20621,
	21062,
	21497,
	21925,
	22347,
	22762,
	23170,
	23571,
	23964,
	24351,
	24730,
	25101,
	25465,
	25821,
	26169,
	26509,
	26841,
	27165,
	27481,
	27788,
	28087,
	28377,
	28659,
	28932,
	29196,
	29451,
	29697,
	29934,
	30162,
	30381,
	30591,
	30791,
	30982,
	31163,
	31335,
	31498,
	31650,
	31794,
	31927,
	32051,
	32165,
	32269,
	32364,
	32448,
	32523,
	32587,
	32642,
	32687,
	32722,
	32747,
	32762,
	32767
};

inline int16_t sind(int deg){
	int sign;
	if (deg >= 0){
		sign = 1;
	}
	else{
		deg = -deg;
		sign = -1;
	}
	while (deg >= 360){
		deg -= 360;
	}
	if((deg >= 0) && (deg <= 90)){
		return sign * sin_table[deg];
	} 
	else if((deg > 90) && (deg <= 180)){
		return sign * sin_table[180 - deg];
	} 
	else if((deg > 180) && (deg <= 270)){
		return sign * -sin_table[deg - 180];
	} 
	else if((deg > 270) && (deg <= 360)){
		return sign * -sin_table[360 - deg];
	}
}

inline int16_t cosd(int deg){
	//if (deg < 0){ deg = -deg; }
	deg = (deg > 0 ? deg : -deg);
	while(deg >= 360){
		deg -= 360;
	} 
	if ((deg >= 0) && (deg <= 90)){
		return sin_table[90 - deg];
	}
	else if ((deg > 90) && (deg <= 180)){
		return -sin_table[deg - 90];
	}
	else if ((deg > 180) && (deg <= 270)){
		return -sin_table[270 - deg];
	}
	else{
		return sin_table[deg - 270];
	}
}

#endif