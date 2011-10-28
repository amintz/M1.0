/*
 *  margCoordWarping.cpp
 *  m1.0_warp
 *
 *  Created by André Mintz on 02/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margCoordWarping.h"


void margCoordWarping :: init(float width, float height, bool doReverse) {
	w = width;
	h = height;
	bReverse = doReverse;
	
	quadSrc[0].set(0, 0);
	quadSrc[1].set(w, 0);
	quadSrc[2].set(w, h);
	quadSrc[3].set(0, h);
	
	quadDst[0].set(0, 0);
	quadDst[1].set(w, 0);
	quadDst[2].set(w, h);
	quadDst[3].set(0, h);
	
	if(bReverse) quadGui.setQuadPoints(quadDst);
	else quadGui.setQuadPoints(quadSrc);
	
	warpedPts = quadGui.getScaledQuadPoints(w, h);
	
	if(bReverse) calculateMatrix(quadSrc, warpedPts);
	else calculateMatrix(warpedPts, quadSrc);
}

// -----------------------------------------------------------

void margCoordWarping :: updateMatrix() {
	if (bReverse) {
		warpedPts = quadGui.getScaledQuadPoints(w, h);
		calculateMatrix(quadSrc, warpedPts);
	}
	else {
		warpedPts = quadGui.getScaledQuadPoints(w, h);
		calculateMatrix(warpedPts, quadDst);
	}
}

//------------------------------------------------------------

CvMat* margCoordWarping :: getTranslateMat() {
	return translate;
}

//------------------------------------------------------------

void  margCoordWarping :: setTranslateMat(CvMat* _translate) {
	translate = _translate;
}

// -----------------------------------------------------------

void margCoordWarping :: loadQuad(string filePath) {
	
	quadFilePath = filePath;
	
	quadSrc[0].set(0, 0);
	quadSrc[1].set(w, 0);
	quadSrc[2].set(w, h);
	quadSrc[3].set(0, h);
	
	quadDst[0].set(0, 0);
	quadDst[1].set(w, 0);
	quadDst[2].set(w, h);
	quadDst[3].set(0, h);
	

	quadGui.readFromFile(quadFilePath);
	
	warpedPts = quadGui.getScaledQuadPoints(w, h);
	
	if(bReverse) calculateMatrix(quadSrc, warpedPts);
	else calculateMatrix(warpedPts, quadSrc);
	
}

// -----------------------------------------------------------

void margCoordWarping :: saveQuad() {
	quadGui.saveToFile(quadFilePath);
}

// -----------------------------------------------------------

void margCoordWarping :: saveQuad(string filePath) {
	quadGui.saveToFile(filePath);
}

// -----------------------------------------------------------

void margCoordWarping :: clearQuad() {
	quadSrc[0].set(0, 0);
	quadSrc[1].set(w, 0);
	quadSrc[2].set(w, h);
	quadSrc[3].set(0, h);
	
	quadDst[0].set(0, 0);
	quadDst[1].set(w, 0);
	quadDst[2].set(w, h);
	quadDst[3].set(0, h);
	
	quadGui.setQuadPoints(quadSrc);
}

// -----------------------------------------------------------

void margCoordWarping :: selectPoint(float x, float y, float offsetX, float offsetY,
									 float width, float height, float hitArea) {
	quadGui.selectPoint(x, y, offsetX, offsetY, width, height, hitArea);
}

// -----------------------------------------------------------

void margCoordWarping :: updatePoint(float x, float y, float offsetX, float offsetY,
									 float width, float height) {
	quadGui.updatePoint(x, y, offsetX, offsetY, width, height);
	warpedPts = quadGui.getScaledQuadPoints(w, h);
	
	if(bReverse) calculateMatrix(quadSrc, warpedPts);
	else calculateMatrix(warpedPts, quadSrc);
}

// -----------------------------------------------------------

void margCoordWarping :: drawQuad(float x, float y, float width, float height) {
	quadGui.draw(x, y, width, height);
}
