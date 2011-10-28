/*
 *  margCapture.h
 *  m1.0_setup
 *
 *  Created by André Mintz on 30/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "margVideoGrabber007.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxThread.h"

class margCapture : public ofxThread {

public:
	
	// FUNCTIONS
	
	//Initializing
	
	void			init(int camWidth, int camHeight, int devID, bool bExhibition);
	void			init(int camWidth, int camHeight, int devID);
	void			init(int camWidth, int camHeight);
	
	void			setGrayscalePixelStorage(unsigned char* _pixelStorage, bool& pixelStorageFlag);
	
	void			changeDevice(int devID);
	
	void			openGrabberSettings();
	
	void			closeGrabber();
	
	
	//Updating
	
	void			updtFrame();
	void			threadedFunction();
	void			makeGrayscale();
	
	
	//Access data
	
	unsigned char*			getPixels();
	
	bool					arePixelsUpdated();
	
	int						getWidth();
	int						getHeight();
	int						getNumPixVals();
	int						getNumPixels();
	
	vector<string>&			getDeviceList();
	

	// RESOURCES
	
	// General
	margVideoGrabber		vidGrabber;
	vector<string>			deviceList;
	
	
protected:
	
	int		width,
			height,
			numPixels,
			numPixVals;	
	
	unsigned char* pixels;
	unsigned char* grayscalePixels;
	
	bool	bPixelsUpdated;

};