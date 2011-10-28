/*
 *  quadVideoGrabber.cpp
 *  emptyExample
 *
 *  Created by André Mintz on 26/03/11.
 *
 */

#include "quadVideoGrabber.h"

void quadVideoGrabber :: initialize(int camWidth, int camHeight, int devID, string root) {
	
	if (devID>0) {
		vidGrabber.close();
		vidGrabber.setDeviceID(devID);
	}
	
	vidGrabber.initGrabber(camWidth, camHeight, false);
	vidGrabber.grayscalePixels = new unsigned char [camWidth * camHeight];
	vidGrabber.colorlength = camWidth * camHeight * 3;
	fullDeviceList = vidGrabber.getDeviceList();
	vidGrabber.setVerbose(false);
	CVImg.allocate(camWidth, camHeight);
	CVWarpedImg.allocate(camWidth, camHeight);
	
	cvWarpQuad.setup("QUAD_" + ofToString(vidGrabber.getDeviceID()) + "_");
	
	quadFilesRoot = root;
	
	quadSrc[0].set(0, 0);
	quadSrc[1].set(camWidth, 0);
	quadSrc[2].set(camWidth, camHeight);
	quadSrc[3].set(0, camHeight);
	
	quadDst[0].set(0, 0);
	quadDst[1].set(camWidth, 0);
	quadDst[2].set(camWidth, camHeight);
	quadDst[3].set(0, camHeight);
	
	cvWarpQuad.setQuadPoints(quadSrc);
	cvWarpQuad.readFromFile(quadFilesRoot + "warp-cam_" + ofToString(vidGrabber.getDeviceID()) + ".xml");
	warpedPts = cvWarpQuad.getScaledQuadPoints(vidGrabber.width, vidGrabber.height);
	coordWarp.calculateMatrix(warpedPts, quadDst);
	
	_width = camWidth;
	_height = camHeight;
	_area = _width * _height;
	radialDistX = 0;
	radialDistY = 0;
	tangentDistX= 0;
	tangentDistY= 0;
	focalX = 8;
	focalY = 8;
	centerX = camWidth/2;
	centerY = camHeight/2;
	
	bWarpImage = true;
}

void quadVideoGrabber :: initialize(int camWidth, int camHeight, int devID) {
	
	initialize(camWidth, camHeight, devID, "");
	
}

void quadVideoGrabber :: initialize(int camWidth, int camHeight) {

	initialize(camWidth, camHeight, -1);
		
}

void quadVideoGrabber :: setDeviceID(int devID) {
	vidGrabber.setDeviceID(devID);
}

void quadVideoGrabber :: closeGrabber() {
	vidGrabber.close();
}

void quadVideoGrabber :: openGrabberSettings() {
	vidGrabber.videoSettings();
}

void quadVideoGrabber :: updtFrame() {
	vidGrabber.grabFrame();
	if (vidGrabber.isFrameNew()) {
		CVImg.setFromPixels(vidGrabber.getGrayscalePixels(), vidGrabber.width, vidGrabber.height);
	}
}

void quadVideoGrabber :: updtFrame(bool bWarp, bool bLensCorrect) {
	vidGrabber.grabFrame();
	if (vidGrabber.isFrameNew()) {
		CVImg.setFromPixels(vidGrabber.getGrayscalePixels(), vidGrabber.width, vidGrabber.height);
		if (bLensCorrect) {
			CVImg.undistort(radialDistX, radialDistY, tangentDistX, tangentDistY, focalX, focalY, centerX, centerY);
		}
		if (bWarp) {
			if (bWarpImage) {
				CVWarpedImg.warpIntoMe(CVImg, warpedPts, quadDst);
			}
		}
	}
}

void quadVideoGrabber :: threshold(int min, int max) {
	
	unsigned char* prev = CVImg.getPixels();
		for (int i = 0; i<_area; i++) {
			if (prev[i] > min && prev[i]<=max) {
				prev[i] = 255;
			}
			else {
				prev[i] = 0;
			}
		}
		CVImg.setFromPixels(prev, _width, _height);
}

void quadVideoGrabber :: draw(int x, int y, int w, int h, bool bIsWarped) {
	if (bIsWarped && bWarpImage) {
		CVWarpedImg.draw(x, y, w, h);
	}
	else {
		CVImg.draw(x, y, w, h);
	}

}

ofxCvGrayscaleImage& quadVideoGrabber :: getWarpedCVImg() {
	return CVWarpedImg;
}

ofxCvGrayscaleImage& quadVideoGrabber :: getCVImg() {
	return CVImg;
}

ofxCvGrayscaleImage& quadVideoGrabber :: getCorrectedCVImg(bool bWarp, bool bLensCorrect, bool bScale,
														   CvMat* perspMat, int outW, int outH) {
	if (bLensCorrect) {
		cout << "Set function with lens correction parameters"<< endl;
	}
	if (bWarp) {
		cvWarpPerspective(CVImg.getCvImage(), CVWarpedImg.getCvImage(), perspMat);
	}
	else {
		CVWarpedImg = CVImg;
	}

	if (bScale) {
		scaleImg.allocate(outW, outH);
		scaleImg.scaleIntoMe(CVWarpedImg);
		return scaleImg;
		scaleImg.clear();
	}
	else {
		return CVWarpedImg;
	}

}

ofxCvGrayscaleImage& quadVideoGrabber :: getCorrectedCVImg(bool bWarp, bool bLensCorrect, bool bScale, CvMat* perspMat,
														   float rX, float rY, float tX, float tY, float fX, float fY,
														   float cX, float cY, int outW, int outH) {
	if (bLensCorrect) {
		CVImg.undistort(rX, rY, tX, tY, fX, fY, cX, cY);
	}
	if (bWarp) {
		cvWarpPerspective(CVImg.getCvImage(), CVWarpedImg.getCvImage(), perspMat);
	}
	else {
		CVWarpedImg = CVImg;
	}
	
	if (bScale) {
		scaleImg.allocate(outW, outH);
		scaleImg.scaleIntoMe(CVWarpedImg);
		return scaleImg;
		scaleImg.clear();
	}
	else {
		return CVWarpedImg;
	}
	
}

void quadVideoGrabber :: clearQuad() {

	quadSrc[0].set(0, 0);
	quadSrc[1].set(vidGrabber.width, 0);
	quadSrc[2].set(vidGrabber.width, vidGrabber.height);
	quadSrc[3].set(0, vidGrabber.height);
	
	quadDst[0].set(0, 0);
	quadDst[1].set(vidGrabber.width, 0);
	quadDst[2].set(vidGrabber.width, vidGrabber.height);
	quadDst[3].set(0, vidGrabber.height);
	
	cvWarpQuad.setQuadPoints(quadSrc);
	cvWarpQuad.saveToFile(quadFilesRoot + "warp-cam_" + ofToString(vidGrabber.getDeviceID()) + ".xml");
	
}

void quadVideoGrabber :: saveQuad() {

	cvWarpQuad.saveToFile(quadFilesRoot + "warp-cam_" + ofToString(vidGrabber.getDeviceID()) + ".xml");
	warpedPts = cvWarpQuad.getScaledQuadPoints(vidGrabber.width, vidGrabber.height);
	coordWarp.calculateMatrix(warpedPts, quadDst);
	
}

bool quadVideoGrabber :: isInQuad(ofxPoint2f point) {
	bool isInQuad = coordWarp.bInQuad(point);
	return isInQuad;
}

ofxPoint2f	quadVideoGrabber :: warpTransform(float x, float y) {
	ofxPoint2f output = coordWarp.transform(x, y);
	return output;
}

unsigned char* quadVideoGrabber :: getPixels() {
	return vidGrabber.getPixels();
}

unsigned char* quadVideoGrabber :: getWarpedPixels() {
	return CVWarpedImg.getPixels();
}

vector<string> quadVideoGrabber :: getDeviceList() {
	return vidGrabber.getDeviceList();
}

int	quadVideoGrabber :: getDeviceID() {
	return vidGrabber.getDeviceID();
}

void quadVideoGrabber :: setWarpImage (bool on) {
	bWarpImage = on;
}

CvMat* quadVideoGrabber :: getTranslateMat() {
	coordWarp.getTranslateMat();
}