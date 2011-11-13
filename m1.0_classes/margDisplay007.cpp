/*
 *  margDisplay.cpp
 *  m1.0_setup
 *
 *  Created by André Mintz on 01/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margDisplay007.h"

void margDisplay::init(int _width, int _height) {
	init(_width, _height, true);
}

// -----------------------------------------------

void margDisplay::init(int _width, int _height, bool _bExhibitionMode) {
	width  = _width;
	height = _height;
	
	bExhibitionMode = _bExhibitionMode;
	
	translate = cvCreateMat(3,3,CV_32FC1);
	
	image.setUseTexture(false);
	
	bPixelsLocked = false;
	bPixelsFlushed= true;
	
	image.allocate(width, height);
	
	updatePixN();
	
	bufPixels = new unsigned char[pixN];
	finalPixels=new unsigned char[pixN];
	
	if(bExhibitionMode) source.setUseTexture(false);
	
	if(!bExhibitionMode) gSource.setUseTexture(false);
	if(!bExhibitionMode) gSource.allocate(width, height);

	
	source.allocate(width, height);

}

// --------------------------------------------------------------

void margDisplay::feedImg(ofxCvColorImage& _source) {
	if(!bExhibitionMode) {
		if (image.getWidth() != _source.getWidth()) {
			image.clear();
			image.allocate(source.getWidth(), source.getHeight());
			updatePixN();
		}
	}

	cvWarpPerspective(_source.getCvImage(), image.getCvImage(), translate);
	image.flagImageChanged();
	updatePixels();
}

// --------------------------------------------------------------

void margDisplay::feedImg(ofxCvGrayscaleImage& _source) {
	if(!bExhibitionMode) {
		if (image.getWidth() != _source.getWidth()) {
			image.clear();
			image.allocate(source.getWidth(), source.getHeight());
			updatePixN();
		}
	}
	
	source = _source;
	cvWarpPerspective(source.getCvImage(), image.getCvImage(), translate);
	image.flagImageChanged();
	updatePixels();
}

// --------------------------------------------------------------

void margDisplay::feedImg(unsigned char* pixels, int _w, int _h, bool bIsColor) {
	if(!bExhibitionMode) {
		if (_w != image.getWidth()) {
			if(image.getWidth() != 0)image.clear();
			image.allocate(_w, _h);
			updatePixN();
		}
		if (source.getWidth() != _w) {
			if(source.getWidth() != 0) source.clear();
			source.allocate(_w, _h);
			updatePixN();
		}
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
	updatePixels();
}

// --------------------------------------------------------------

void margDisplay::feedImg(unsigned char* pixels, int _w, int _h) {
	feedImg(pixels, _w, _h, true);
}

// --------------------------------------------------------------

unsigned char* margDisplay::getPixels() {
	if(!bPixelsLocked && !bPixelsFlushed) {
		bPixelsLocked = true;
		memcpy(finalPixels, bufPixels, pixN);
		bPixelsLocked = false;
		bPixelsFlushed= true;
	}
	return finalPixels;
}

// --------------------------------------------------------------

bool margDisplay::tryLockPix() {
	if (bPixelsLocked) {
		return false;
	}
	else {
		bPixelsLocked = true;
		return true;
	}
}

// --------------------------------------------------------------

void margDisplay::updatePixels() {
	if(bPixelsFlushed) {
		bool success = false;
		while (!success) {
			success = tryLockPix();
		}
		memcpy(bufPixels, image.getPixels(), pixN);
		bPixelsLocked = false;
		bPixelsFlushed= false;
	}
}

// --------------------------------------------------------------

void margDisplay::updatePixN() {
	pixN = image.getWidth() * image.getHeight() * 3;
}

// --------------------------------------------------------------

void margDisplay::setTranslateMat(CvMat* _translate) {
	translate = _translate;
}

// --------------------------------------------------------------

void margDisplay::clearTranslateMat() {
	
	CvMat* srcMat;
	CvMat* dstMat;
	
	cvSetZero(translate);
	
	float src[8];
	float dst[8];
	
	src[0] = 0;
	src[1] = 0;
	src[2] = width;
	src[3] = 0;
	src[4] = width;
	src[5] = height;
	src[6] = 0;
	src[7] = height;
	
	for(int i = 0; i < 8; i++) {
		dst[i] = src[i];
	}
	
	cvInitMatHeader(srcMat, 4, 2, CV_32FC1, src);
	cvInitMatHeader(dstMat, 4, 2, CV_32FC1, dst);
	
	
	cvFindHomography(srcMat, dstMat, translate);
	
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