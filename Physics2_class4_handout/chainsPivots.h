#ifndef __H_CHAINSPIVOTS__
#define __H_CHAINSPIVOTS__

// Pivot 0, 0
int new_L_Flipper[16] = {
	8, 15,
	49, 13,
	52, 12,
	52, 9,
	49, 7,
	8, 4,
	4, 6,
	4, 13
};

// Pivot 0, 0
int new_R_Flipper[16] = {
	53, 6,
	47, 3,
	7, 7,
	5, 8,
	5, 11,
	7, 12,
	47, 15,
	53, 12
};

// Pivot 0, 0
int topRightWayPoints[30] = {
	279, 107,
	287, 105,
	303, 112,
	316, 123,
	328, 141,
	331, 159,
	328, 181,
	320, 200,
	313, 204,
	319, 184,
	323, 163,
	318, 146,
	307, 138,
	278, 128,
	273, 121
};


// Pivot 0, 0
int topLeftWayPoints[32] = {
	120, 205,
	118, 196,
	112, 175,
	112, 157,
	117, 143,
	133, 135,
	155, 127,
	159, 119,
	156, 108,
	147, 104,
	131, 111,
	118, 120,
	109, 133,
	104, 148,
	103, 170,
	107, 188
};

// Pivot 0, 0
int leftBottomWayPoints[22] = {
	109, 382,
	114, 407,
	122, 423,
	133, 439,
	148, 452,
	149, 458,
	145, 463,
	139, 461,
	127, 450,
	115, 430,
	110, 409
};

// Pivot 0, 0
int topSeparatorPoints[12] = {
	198, 94,
	195, 97,
	195, 118,
	197, 120,
	200, 118,
	200, 97
};

// Pivot 0, 0
int rightBottomWayPoints[28] = {
	321, 382,
	316, 405,
	310, 420,
	300, 437,
	288, 448,
	281, 454,
	280, 460,
	283, 463,
	289, 463,
	299, 454,
	308, 443,
	315, 431,
	319, 418,
	321, 405
};

// Pivot 0, 0
int safetyZonePoints[30] = {
	389, 287,
	389, 434,
	431, 434,
	438, 432,
	442, 428,
	444, 422,
	444, 318,
	441, 312,
	435, 308,
	419, 308,
	419, 405,
	418, 408,
	415, 408,
	414, 405,
	414, 287
};

// Pivot 0, 0
int loopPartPoints[120] = {
	389, 286,
	390, 226,
	393, 201,
	398, 177,
	403, 154,
	403, 140,
	396, 124,
	384, 110,
	366, 105,
	357, 105,
	345, 109,
	336, 118,
	331, 132,
	331, 148,
	335, 160,
	342, 168,
	348, 171,
	361, 171,
	369, 166,
	372, 160,
	374, 145,
	366, 130,
	357, 126,
	347, 120,
	349, 111,
	359, 107,
	367, 107,
	378, 110,
	387, 116,
	393, 126,
	398, 140,
	398, 160,
	395, 174,
	387, 188,
	374, 196,
	366, 199,
	349, 199,
	331, 194,
	317, 182,
	308, 167,
	304, 149,
	304, 133,
	308, 115,
	316, 98,
	324, 90,
	337, 82,
	353, 77,
	372, 77,
	387, 81,
	404, 91,
	416, 104,
	424, 119,
	428, 137,
	428, 155,
	427, 169,
	424, 178,
	421, 191,
	419, 208,
	417, 227,
	417, 303
};

// Pivot 0, 0
int mainBoard[170] = {
	370, 133,
	364, 127,
	356, 127,
	354, 133,
	355, 140,
	357, 153,
	355, 173,
	347, 198,
	338, 223,
	333, 233,
	335, 237,
	339, 237,
	345, 230,
	353, 221,
	366, 218,
	375, 228,
	375, 236,
	370, 242,
	362, 251,
	353, 266,
	345, 281,
	341, 301,
	341, 338,
	345, 356,
	348, 379,
	346, 405,
	342, 426,
	333, 450,
	322, 466,
	307, 483,
	288, 498,
	268, 508,
	245, 512,
	245, 530,
	186, 530,
	186, 515,
	166, 508,
	142, 496,
	121, 480,
	107, 463,
	95, 443,
	87, 422,
	83, 398,
	84, 369,
	88, 350,
	88, 308,
	81, 277,
	71, 254,
	55, 230,
	39, 204,
	18, 173,
	10, 152,
	58, 152,
	60, 183,
	66, 199,
	74, 212,
	83, 225,
	88, 233,
	93, 238,
	98, 238,
	99, 234,
	93, 219,
	86, 205,
	80, 184,
	75, 160,
	76, 131,
	88, 109,
	106, 90,
	132, 74,
	156, 66,
	172, 63,
	180, 49,
	191, 37,
	206, 31,
	223, 31,
	240, 37,
	252, 48,
	260, 63,
	287, 66,
	310, 71,
	334, 79,
	360, 93,
	379, 106,
	389, 118,
	396, 131
};

// Pivot 0, 0
int exitLoopTapPivots[20] = {
	262, 59,
	261, 65,
	278, 69,
	295, 74,
	311, 82,
	326, 91,
	338, 103,
	348, 117,
	353, 129,
	360, 134
};


int Next_To_Flipper_1[54] = {
	277, 455,
	285, 445,
	294, 437,
	302, 425,
	306, 418,
	311, 409,
	314, 398,
	316, 386,
	317, 380,
	319, 377,
	322, 378,
	325, 379,
	326, 386,
	326, 399,
	324, 411,
	323, 419,
	320, 428,
	318, 435,
	312, 444,
	306, 452,
	300, 459,
	294, 465,
	290, 468,
	285, 468,
	280, 466,
	277, 463,
	276, 460

}; 

// Pivot 0, 0
int Flipper_R[20] = {
		278, 481,
		275, 479,
		271, 479,
		237, 498,
		235, 500,
		235, 502,
		237, 503,
		240, 503,
		277, 488,
		279, 485
};

// Pivot 0, 0
int Flipper_L[20] = {
	154, 481,
	158, 479,
	161, 479,
	195, 498,
	197, 500,
	197, 502,
	195, 503,
	192, 502,
	155, 488,
	153, 486
};

/*int Round_Elements[24] = {
	212, 156,
	220, 156,
	224, 158,
	226, 162,
	228, 172,
	225, 175,
	221, 179,
	210, 179,
	207, 176,
	204, 172,
	204, 163,
	206, 160
}; */

#endif // __H_CHAINSPIVOTS__


