/*
 *  margCapture.cpp
 *  m1.0_setup
 *
 *  Created by André Mintz on 30/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margCapture.h"


void margCapture::init(int camWidth, int camHeight, int devID, bool bExhibition) {

	width = camWidth;
	height = camHeight;
	
	numPixels = width * height;
	numPixVals = numPixels * 3;
	
	//pixels = new unsigned char[numPixVals];
	grayscalePixels = new unsigned char[numPixels];
	
	bPixelsUpdated = false;
	
	// Initializing grabber
	vidGrabber.setVerbose(true);
	vidGrabber.setDesiredFrameRate(120);
	vidGrabber.initGrabber(camWidth, camHeight, false);
	
	// Device Settings
	deviceList = vidGrabber.getDeviceList();
	if (devID > 0 && devID < deviceList.size()) vidGrabber.setDeviceID(devID);
	
}

// ----------------------------------------------------------

void margCapture::init(int camWidth, int camHeight, int devID) {
	init(camWidth, camHeight, devID, false);
}

// ----------------------------------------------------------

void margCapture::init(int camWidth, int camHeight) {
	init(camWidth, camHeight, -1);
}

// ----------------------------------------------------------

void margCapture::setGrayscalePixelStorage(unsigned char* pixelStorage, bool& pixelStorageFlag) {
	grayscalePixels = pixelStorage;
	pixelStorageFlag = true;
}

// ----------------------------------------------------------

void margCapture::changeDevice(int devID) {
	if (devID != vidGrabber.getDeviceID()) {
		vidGrabber.close();
		vidGrabber.setDeviceID(devID);
		vidGrabber.setVerbose(false);
		vidGrabber.setDesiredFrameRate(120);
		vidGrabber.initGrabber(width, height, false);
	}
}

// ----------------------------------------------------------

void margCapture::openGrabberSettings() {
	vidGrabber.videoSettings();
}


// ----------------------------------------------------------

void margCapture::closeGrabber() {
	vidGrabber.close();
}

// ----------------------------------------------------------

void margCapture::updtFrame() {
	vidGrabber.grabFrame();
	if (vidGrabber.isFrameNew()) {
		pixels = vidGrabber.getPixels();
		makeGrayscale();
		bPixelsUpdated = true;
	}
}

// ----------------------------------------------------------

void margCapture::threadedFunction() {
	while (isThreadRunning() != 0) {
		if (lock()) {
			updtFrame();
			unlock();
		}
	}
}

// ----------------------------------------------------------

void margCapture::makeGrayscale() {
	for (int i = 0; i < numPixVals; i+=3) {
		grayscalePixels[i/3] = ((pixels[i]*0.3) + (pixels[i+1]*0.59) + (pixels[i+2]*0.11));
	}
}

// ----------------------------------------------------------

unsigned char* margCapture::getPixels() {
	bPixelsUpdated = false;
	return grayscalePixels;
}

// ----------------------------------------------------------

bool margCapture::arePixelsUpdated() {
	return bPixelsUpdated;
}

// ----------------------------------------------------------

int margCapture::getWidth() {
	return width;
}

// ----------------------------------------------------------

int margCapture::getHeight() {
	return height;
}

// ----------------------------------------------------------

int margCapture::getNumPixVals() {
	return numPixVals;
}

// ----------------------------------------------------------

int margCapture::getNumPixels() {
	return numPixels;
}

// ----------------------------------------------------------

vector<string>& margCapture::getDeviceList() {
	return deviceList;
}