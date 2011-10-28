/*
 *  margDisplay.cpp
 *  m1.0_setup
 *
 *  Created by André Mintz on 01/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margDisplay.h"

void margDisplay::init(int _width, int _height, bool bExhibitionMode) {
	width  = _width;
	height = _height;
	
	translate = cvCreateMat(3,3,CV_32FC1);
	
	image.allocate(width, height);
	
	if(bExhibitionMode) source.setUseTexture(false);
	gSource.setUseTexture(false);
	
	source.allocate(width, height);
	gSource.allocate(width, height);
}

// --------------------------------------------------------------

void margDisplay::feedImg(ofxCvColorImage& _source) {
	if (image.getWidth() != _source.getWidth()) {
		image.clear();
		image.allocate(source.getWidth(), source.getHeight());
	}

	cvWarpPerspective(_source.getCvImage(), image.getCvImage(), translate);
	image.flagImageChanged();
}

// --------------------------------------------------------------

void margDisplay::feedImg(ofxCvGrayscaleImage& _source) {
	if (image.getWidth() != _source.getWidth()) {
		image.clear();
		image.allocate(source.getWidth(), source.getHeight());
	}
	
	source = _source;
	cvWarpPerspective(source.getCvImage(), image.getCvImage(), translate);
	image.flagImageChanged();
}

// --------------------------------------------------------------

void margDisplay::feedImg(unsigned char* pixels, int _w, int _h, bool bIsColor) {
	if (_w != image.getWidth()) {
		if(image.getWidth() != 0)image.clear();
		image.allocate(_w, _h);
	}
	if (source.getWidth() != _w) {
		if(source.getWidth() != 0) source.clear();
		source.allocate(_w, _h);
	}
	if (bIsColor) {
		source.setFromPixels(pixels, _w, _h);
	}
	else {
		if (gSource.getWidth() != _w) {
			if(gSource.getWidth() != 0) gSource.clear();
			gSource.allocate(_w, _h);
		}
		gSource.setFromPixels(pixels, _w, _h);
		source = gSource;
	}
	cvWarpPerspective(source.getCvImage(), image.getCvImage(), translate);
	image.flagImageChanged();
}

// --------------------------------------------------------------

void margDisplay::feedImg(unsigned char* pixels, int _w, int _h) {
	feedImg(pixels, _w, _h, true);
}

// --------------------------------------------------------------

void margDisplay::setTranslateMat(CvMat* _translate) {
	translate = _translate;
}

// --------------------------------------------------------------

void margDisplay::clearTranslateMat() {
	
	CvPoint2D32f src[4];
	CvPoint2D32f dst[4];
	
	src[0].x = 0;
	src[0].y = 0;
	src[1].x = width;
	src[1].y = 0;
	src[2].x = width;
	src[2].y = height;
	src[3].x = 0;
	src[3].y = height;
	
	for(int i = 0; i < 4; i++) {
		dst[i] = src[i];
	}
	
	cvWarpPerspectiveQMatrix(src, dst, translate);
	
}

// -------------------------------------------------------------

void margDisplay::draw(int x, int y, int w, int h) {
	image.draw(x, y, w, h);
}

// --------------------------------------------------------------

void margDisplay::drawUndistorted(int x, int y, int w, int h) {
	source.draw(x, y, w, h);
}

// --------------------------------------------------------------