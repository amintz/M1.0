/*
 *  margVideoGrabber.h
 *  emptyExample
 *
 *  Created by André Mintz on 26/03/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxOpenCv.h"
#include "ofxVectorMath.h"
#include "ofxXmlSettings.h"
#include "margCoordWarping.h"
#include "guiQuad.h"

#include "margVideoGrabber.h"

class quadVideoGrabber {

public:
	
	//Initializing
	
	void			initialize(int camWidth, int camHeight, int devID, string root);
	void			initialize(int camWidth, int camHeight, int devID);
	void			initialize(int camWidth, int camHeight);
	
	//Updating
	
	void			updtFrame();
	void			updtFrame(bool bWarp, bool bLensCorrect);
	void			threshold(int min, int max);
	
	// To pass videoGrabber functions
	
	vector<string>	getDeviceList();
	int				getDeviceID();
	void			closeGrabber();
	void			setDeviceID(int devID);
	void			openGrabberSettings();
	
	//To capture
	
	margVideoGrabber vidGrabber;
	vector<string>	 fullDeviceList;
	
	int				_width, _height, _area;
	
	//To draw
	
	void			draw(int x, int y, int w, int h, bool bIsWarped);
	
	//To pixels
	unsigned char*	getPixels();
	unsigned char*	getWarpedPixels();
	ofxCvGrayscaleImage& getCVImg();
	ofxCvGrayscaleImage& getWarpedCVImg();
	ofxCvGrayscaleImage& getCorrectedCVImg(bool bWarp, bool bLensCorrect, bool bScale,
										   CvMat* perspMat, int outW, int outH);
	ofxCvGrayscaleImage& getCorrectedCVImg(bool bWarp, bool bLensCorrect, bool bScale, CvMat* perspMat,
										   float rX, float rY, float tX, float tY, float fX, float fY,
										   float cX, float cY, int outW, int outH);
	
	
	//For warping purposes
	
	ofxCvGrayscaleImage	CVImg;
	ofxCvGrayscaleImage	CVWarpedImg;
	ofxCvGrayscaleImage scaleImg;
	
	guiQuad			cvWarpQuad;
	
	ofxPoint2f * warpedPts;
	
	ofxPoint2f		quadSrc[4];
	ofxPoint2f		quadDst[4];
	
	margCoordWarping	coordWarp;
	
	CvMat*			getTranslateMat();
	
	void			clearQuad();
	
	bool			isInQuad(ofxPoint2f point);
	ofxPoint2f		warpTransform(float x, float y);
	
	bool			bWarpImage;
	void			setWarpImage(bool on);
	
	//Quad files
	void			saveQuad();
	string			quadFilesRoot;
	
	//For Lens Correction
	
	float				radialDistX;
	float				radialDistY;
	float				tangentDistX;
	float				tangentDistY;
	float				focalX;
	float				focalY;
	float				centerX;
	float				centerY;
	
	//ROI
	void				updtROI();

};