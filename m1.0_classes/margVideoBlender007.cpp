/*
 *  margVideoBlender.cpp
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margVideoBlender007.h"

void margVideoBlender :: init(int _w, int _h) {
	
	w = _w;
	h = _h;
	
	mapLength = w*h;
	numColorVals = w*h*3;

	finalMap = new unsigned char [mapLength * 3];
	
}

void margVideoBlender :: blendVideo(unsigned char* trailMap, unsigned char* vidPixels) {

	int j = 0;
	
	for (int i = 0; i < numColorVals; i+=3, j++) {
		finalMap[i]  = min(((trailMap[j] * vidPixels[i]  )/ 254 + trailMap[j]), (int)vidPixels[i]);
		finalMap[i+1]= min(((trailMap[j] * vidPixels[i+1])/ 254 + trailMap[j]), (int)vidPixels[i+1]);
		finalMap[i+2]= min(((trailMap[j] * vidPixels[i+2])/ 254 + trailMap[j]), (int)vidPixels[i+2]);
	}

}

unsigned char* margVideoBlender :: getPixels() {

	return finalMap;

}