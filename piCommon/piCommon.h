#ifndef PICOMMON_H
#define PICOMMON_H

enum SLAVE1_COMMANDS{
	SLAVE1_CHECK_STATUS,
	CALIB_OFFSET,
	REQUEST_STANDARD_PACKET,
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
	CIRCLE_BOTTOM_LEFT,
	CIRCLE_BOTTOM_RIGHT,
	CIRCLE_TOP_LEFT,
	CIRCLE_TOP_RIGHT,
	SIDE_LEFT,
	SIDE_RIGHT,
	RECT_BOTTOM,
	RECT_BOTTOM_LEFT,
	RECT_BOTTOM_RIGHT,
	RECT_TOP,
	RECT_TOP_LEFT,
	RECT_TOP_RIGHT
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
	if(a > 1800){		\
		a -= 3600;	   \
	}					\
	else if(a <= 1800){  \
		a += 3600;	   \
	}					\
}

#define TOBEARING360(a){ \
	if(a >= 3600){	   \
		a -= 3600;	   \
	}					\
	else if(a < 0){	  \
		a += 3600;	   \
	}					\
}

#endif