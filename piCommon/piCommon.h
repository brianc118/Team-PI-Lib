#ifndef PICOMMON_H
#define PICOMMON_H

enum SLAVE1_COMMANDS{
	SLAVE1_CHECK_STATUS,
	CALIB_OFFSET,
	REQUEST_STANDARD_PACKET,
	CALIB_MAG,
	END_CALIB_MAG,
	CALIB_DATA,
	CALIB_GREEN,
	CALIB_WHITE,
	END_CALIB_LIGHT,
	LIGHT_DATA,
	LIGHT_DATA_REFS,
	LIGHT_DATA_COLOURS,
	LIGHT_DATA_GREEN,
	LIGHT_DATA_WHITE,
};

enum SLAVE2_COMMANDS{
	SLAVE2_CHECK_STATUS,
	TSOP_ANGLE_HIGH,	// action: nothing ; returns: tsop angle
	TSOP_ANGLE_LOW, // action: nothing ; returns: tsop angle
	TSOP_ANGLE_BYTE, // action: nothing; returns: tsop angle as byte (0 - 255)
	TSOP_STRENGTH,
	VBAT_REF_LV,   // action: nothing ; returns: batt voltage
	VBAT_REF_HV,   // action: nothing ; returns: batt voltage  
	
	TSOP_DATA0,	   // action: nothing ; returns: tsop data 0
	TSOP_DATA1,	   // action: nothing ; returns: tsop data 1
	TSOP_DATA2,	   // action: nothing ; returns: tsop data 2
	TSOP_DATA3,	   // action: nothing ; returns: tsop data 3
	TSOP_DATA4,	   // action: nothing ; returns: tsop data 4
	TSOP_DATA5,	   // action: nothing ; returns: tsop data 5
	TSOP_DATA6,	   // action: nothing ; returns: tsop data 6
	TSOP_DATA7,	   // action: nothing ; returns: tsop data 7
	TSOP_DATA8,	   // action: nothing ; returns: tsop data 8
	TSOP_DATA9,	   // action: nothing ; returns: tsop data 9
	TSOP_DATA10,   // action: nothing ; returns: tsop data 10
	TSOP_DATA11,   // action: nothing ; returns: tsop data 11
	TSOP_DATA12,   // action: nothing ; returns: tsop data 12
	TSOP_DATA13,   // action: nothing ; returns: tsop data 13
	TSOP_DATA14,   // action: nothing ; returns: tsop data 14
	TSOP_DATA15,   // action: nothing ; returns: tsop data 15
	TSOP_DATA16,   // action: nothing ; returns: tsop data 16
	TSOP_DATA17,   // action: nothing ; returns: tsop data 17
	TSOP_DATA18,   // action: nothing ; returns: tsop data 18
	TSOP_DATA19,   // action: nothing ; returns: tsop data 19
	TSOP_DATA20,   // action: nothing ; returns: tsop data 20
	TSOP_DATA21,   // action: nothing ; returns: tsop data 21
	TSOP_DATA22,   // action: nothing ; returns: tsop data 22
	TSOP_DATA23,   // action: nothing ; returns: tsop data 23 
};


enum SLAVE3_COMMANDS{
	SLAVE3_CHECK_STATUS,

	MOVE1,	  // action: moves motors ; returns: current sense
	MOVE2,	  // action: moves motors ; returns: current sense
	MOVE3,	  // action: moves motors ; returns: current sense
	MOVE4,	  // action: moves motors ; returns: current sense
	MOVE5,	  // action: moves motors ; returns: null (current sense connected to master)

	CSENSE1,  // action: nothing	  ; returns: current sense
	CSENSE2,  // action: nothing	  ; returns: current sense
	CSENSE3,  // action: nothing	  ; returns: current sense
	CSENSE4,  // action: nothing	  ; returns: current sense
	CSENSE5,  // action: nothing	  ; returns: current sense
	V1,	      // action: nothing	  ; returns: motor velocity
	V2,	      // action: nothing	  ; returns: motor velocity
	V3,	      // action: nothing	  ; returns: motor velocity
	V4,	      // action: nothing	  ; returns: motor velocity
	BRAKE1,   // action: brake		  ; returns: motor velocity
	BRAKE2,   // action: brake		  ; returns: motor velocity
	BRAKE3,   // action: brake		  ; returns: motor velocity
	BRAKE4,   // action: brake		  ; returns: motor velocity
	BRAKE5,   // action: brake		  ; returns: motor velocity
	BRAKEALL, // action: brake all	  ; returns: motor velocity
	MOVE	  // action: move		  ; returns: omnidrive.move() output
	/* note. first 2 bytes are for alpha (direction), second byte is velocity
	   third byte is rotational velocity */
};

enum LINELOCATION{
	UNKNOWN,
	CORNER_BOTTOM_LEFT,
	CORNER_BOTTOM_RIGHT,
	CORNER_TOP_LEFT,
	CORNER_TOP_RIGHT,
	SIDE_TOP,
	SIDE_BOTTOM,
	SIDE_LEFT,
	SIDE_RIGHT,
};

enum LOCATION{
	FIELD,
	CIRCLE_LEFT,
	CIRCLE_RIGHT,
	CIRCLE_TOP,
	CIRCLE_BOTTOM,
	EDGE_LEFT,  
	EDGE_RIGHT, 
	EDGE_TOP,   
	EDGE_BOTTOM,
};

#define TOBEARING180(a){ \
	if(a > 180){		\
		a -= 360;	   \
	}					\
	else if(a <= -180){  \
		a += 360;	   \
	}					\
}

#define TOBEARING360(a){ \
	if(a >= 360){	   \
		a -= 360;	   \
	}					\
	else if(a < 0){	  \
		a += 360;	   \
	}					\
}

/* macros to shift array. Parameters are inclusive. For example:
   int array[] =                  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
   ARRAYSHIFTUP(array, 2, 6)   => { 1, 2, 3, 3, 4, 5, 6, 7, 9, 10 };
   ARRAYSHIFTDOWN(array, 2, 6) => { 1, 3, 4, 5, 6, 7, 7, 8, 9, 10 }; */

#define ARRAYSHIFTUP(a, lower, upper){            \
    if (lower == 0){                              \
        for (int q = lower; q < upper; q++){      \
            *(a + q) = *(a + q + 1); }            \
    } else{                                       \
        for (int q = lower - 1; q < upper; q++){  \
            *(a + q) = *(a + q + 1); }}}          \

#define ARRAYSHIFTDOWN(a, lower, upper){          \
    if (upper == (sizeof(a)/sizeof(a[0])) - 1){   \
        for (int q = upper - 1; q >= lower; q--){ \
            *(a + q + 1) = *(a + q); }            \
    } else{                                       \
        for (int q = upper; q >= lower; q--){     \
            *(a + q + 1) = *(a + q); }}}          \

#define ARRAYAVERAGE(a, out){                         \
    int average = 0;                                  \
    for (int i = 0; i < sizeof(a)/sizeof(a[0]); i++){ \
    	average += a[i];}                             \
    out = average/(sizeof(a)/sizeof(a[0]));}          \

// get mid index of two indexes in the domain [0, TSOP_COUNT - 1]
#define GETMIDINDEX(a, b, index){               \
	if (abs(a - b) > TSOP_COUNT / 2){           \
		index = ((a + b) / 2 + TSOP_COUNT / 2); \
		if (index >= TSOP_COUNT){               \
			index -= TSOP_COUNT; }              \
	} else{                                     \
		index = (a + b) / 2; }}                 \

#define CLEARARRAY(a){                                \
	for (int q = 0; q < sizeof(a)/sizeof(a[0]); q++){ \
		a[q] = 0; }}                                  \
		
#endif