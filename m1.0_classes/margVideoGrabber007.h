/*
 *  margVideoGrabber.h
 *  emptyExample
 *
 *  Created by André Mintz on 26/03/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofVideoGrabber.h"

class margVideoGrabber : public ofVideoGrabber {

public:
	
	//vector<string>	getDeviceList();
	int				getDeviceID();
	unsigned char*	getGrayscalePixels();	// Deprecated
	
	int				colorlength;
	unsigned char*  grayscalePixels;

};