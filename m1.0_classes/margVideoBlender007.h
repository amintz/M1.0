/*
 *  margVideoBlender.h
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofThread.h"
#include "ofVideoPlayer.h"

class margVideoBlender {

public:
	
	int w, h, mapLength, numColorVals;
	unsigned char*	finalMap;
	
	void init(int _w, int _h);
	void blendVideo(unsigned char* trailMap, unsigned char* vidPixels);
	unsigned char*	getPixels();
	

};