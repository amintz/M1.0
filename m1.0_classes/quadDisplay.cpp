/*
 *  quadDisplay.cpp
 *  m1.0_proj
 *
 *  Created by André Mintz on 09/06/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "quadDisplay.h"


void quadDisplay :: initialize(int dispWidth, int dispHeight, int ID, string root) {
	
	dispW = dispWidth;
	dispH = dispHeight;
	
	source.allocate(dispW, dispH);
	gSource.allocate(dispW, dispH);
	
	CVWarpedImg.allocate(dispW, dispH);
	CVWarpedGImg.allocate(dispW, dispH);
	
	dispID = ID;
	
	cvWarpQuadDest.setup("QUAD_" + ofToString(dispID) + "_");
	
	quadFilesRoot = root;
	
	quadSrc[0].set(0, 0);
	quadSrc[1].set(dispW, 0);
	quadSrc[2].set(dispW, dispH);
	quadSrc[3].set(0, dispH);
	
	quadDst[0].set(0, 0);
	quadDst[1].set(dispW, 0);
	quadDst[2].set(dispW, dispH);
	quadDst[3].set(0, dispH);
	
	cvWarpQuadDest.setQuadPoints(quadDst);
	cvWarpQuadSrc.setQuadPoints(quadSrc);
	cvWarpQuadDest.readFromFile(quadFilesRoot + "warp-disp_" + ofToString(dispID) + ".xml");
	warpedPts = cvWarpQuadDest.getScaledQuadPoints(CVWarpedGImg.width, CVWarpedGImg.height);
	coordWarpDest.calculateMatrix(quadSrc, warpedPts);
	
}

void quadDisplay :: initialize(int dispWidth, int dispHeight, int ID) {
	
	initialize(dispWidth, dispHeight, ID, "");
	cvWarpQuadDest.setup("QUAD_" + ofToString(dispID) + "_");
	
	
}

void quadDisplay :: loadID(int ID) {
	saveQuad();
	
	dispID = ID;
	
	quadSrc[0].set(0, 0);
	quadSrc[1].set(dispW, 0);
	quadSrc[2].set(dispW, dispH);
	quadSrc[3].set(0, dispH);
	
	quadDst[0].set(0, 0);
	quadDst[1].set(dispW, 0);
	quadDst[2].set(dispW, dispH);
	quadDst[3].set(0, dispH);
	
	cvWarpQuadDest.setQuadPoints(quadDst);
	cvWarpQuadSrc.setQuadPoints(quadSrc);
	cvWarpQuadDest.readFromFile(quadFilesRoot + "warp-disp_" + ofToString(dispID) + ".xml");
	warpedPts = cvWarpQuadDest.getScaledQuadPoints(CVWarpedGImg.width, CVWarpedGImg.height);
	coordWarpDest.calculateMatrix(quadSrc, warpedPts);
}

void quadDisplay :: feedImg(ofxCvColorImage& source) {
	CVWarpedImg.warpIntoMe(source, quadSrc, warpedPts);
}

void quadDisplay :: feedImg(ofxCvGrayscaleImage& source) {
	CVWarpedGImg.warpIntoMe(source, quadSrc, warpedPts);
}

void quadDisplay :: setFromPixels(unsigned char* pixels, int _w, int _h) {
	
	setFromPixels(pixels, _w, _h, true);
	
}

void quadDisplay :: setFromPixels(unsigned char* pixels, int _w, int _h, bool bIsColor) {
	
	if (bIsColor) {
		if (_w != source.getWidth() || _h != source.getHeight()) {
			source.clear();
			source.allocate(_w, _h);
		}
		source.setFromPixels(pixels, _w, _h);
	}
	else {
		if (_w != gSource.getWidth() || _h != gSource.getHeight()) {
			gSource.clear();
			gSource.allocate(_w, _h);
		}
		gSource.setFromPixels(pixels, _w, _h);
		source = gSource;
	}
	CVWarpedImg.warpIntoMe(source, quadSrc, warpedPts);
}

void quadDisplay :: draw(int x, int y, int w, int h) {
	CVWarpedImg.draw(x, y, w, h);
}

void quadDisplay :: clearQuad() {
	
	quadSrc[0].set(0, 0);
	quadSrc[1].set(CVWarpedImg.width, 0);
	quadSrc[2].set(CVWarpedImg.width, CVWarpedImg.height);
	quadSrc[3].set(0, CVWarpedImg.height);
	
	quadDst[0].set(0, 0);
	quadDst[1].set(CVWarpedImg.width, 0);
	quadDst[2].set(CVWarpedImg.width, CVWarpedImg.height);
	quadDst[3].set(0, CVWarpedImg.height);
	
	cvWarpQuadSrc.setQuadPoints(quadSrc);
	cvWarpQuadDest.setQuadPoints(quadDst);
	cvWarpQuadDest.saveToFile(quadFilesRoot + "warp-disp_" + ofToString(dispID) + ".xml");
	
}

void quadDisplay :: saveQuad() {
	cvWarpQuadDest.saveToFile(quadFilesRoot + "warp-disp_" + ofToString(dispID) + ".xml");
	warpedPts = cvWarpQuadDest.getScaledQuadPoints(CVWarpedGImg.width, CVWarpedGImg.height);
	coordWarpDest.calculateMatrix(quadSrc, warpedPts);
}

void quadDisplay :: selectPoint(float x, float y, float orX, float orY, float orW, float orH, float hitArea) {
	cvWarpQuadDest.selectPoint(x, y, orX, orY, orW, orH, hitArea);
}

void quadDisplay :: updatePoint(float x, float y, float orX, float orY, float orW, float orH) {
	cvWarpQuadDest.updatePoint(x, y, orX, orY, orW, orH);
}

bool quadDisplay :: isInQuad(ofxPoint2f point) {
	bool isInQuad = coordWarpDest.bInQuad(point);
	return isInQuad;
}

ofxPoint2f	quadDisplay :: warpTransform(float x, float y) {
	ofxPoint2f output = coordWarpDest.transform(x, y);
	return output;
}






