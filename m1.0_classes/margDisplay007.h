/*
 *  margDisplay.h
 *  m1.0_setup
 *
 *  Created by André Mintz on 01/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxOpenCv.h"
#include "ofVectorMath.h"

class margDisplay {

public:
	
	void	init(int _width, int _height);
	void	init(int _width, int _height, bool bExhibitionMode);
	
	void	feedImg(ofxCvColorImage& _source);
	void	feedImg(ofxCvGrayscaleImage& _source);
	void	feedImg(unsigned char* pixels, int _w, int _h, bool bIsColor);
	void	feedImg(unsigned char* pixels, int _w, int _h);
	
	unsigned char* getPixels();
	
	void	setTranslateMat(CvMat* _translate);
	void	clearTranslateMat();
	
	void	draw(int x, int y, int w, int h);
	void	drawUndistorted(int x, int y, int w, int h);
	
	ofxCvColorImage	image;
	ofxCvColorImage source;
	ofxCvGrayscaleImage gSource;
	
protected:
	
	int		width,
			height;
	
	CvMat*	translate;


};