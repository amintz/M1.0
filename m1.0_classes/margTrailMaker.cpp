/*
 *  margTrailMaker.cpp
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margTrailMaker.h"

void margTrailMaker::init(int _width, int _height) {
		
	mapWidth = _width;
	mapHeight= _height;
	nMapVals = mapWidth * mapHeight;
	
	trailMap = new unsigned char[nMapVals];
	for (int i = 0; i < nMapVals; i++) {
		trailMap[i] = 0;
	}
	
	trailImg.allocate(mapWidth, mapHeight);
	
}

// -------------------------------------------------

void margTrailMaker::updtMap(vector<margBlob> blobs, float _exposureConst, float _fadeConst, int _blurLevel) {

	for (int i = 0; i < nMapVals; i++) {
		trailMap[i] = max((int)(trailMap[i] - _fadeConst), 0);
	}
	
	for (int i = 0; i < blobs.size(); i++) {
		exposeBlob(blobs[i], _exposureConst);
	}
	
	trailImg.setFromPixels(trailMap, mapWidth, mapHeight);
	
	if (_blurLevel > 0) trailImg.blur(_blurLevel);
	
	trailMap = trailImg.getPixels();

}

// ------------------------------------------------

void margTrailMaker::updtMap(vector<margBlob> blobs) {
	updtMap(blobs, exposureConst, fadeConst, blurLevel);
}

// ------------------------------------------------

void margTrailMaker::setTrailMaker(float _exposureConst, float _fadeConst, int _blurLevel) {
	exposureConst = _exposureConst;
	fadeConst = _fadeConst;
	blurLevel = _blurLevel;
}

// ------------------------------------------------

void margTrailMaker::exposeBlob(margBlob& blob, float exposureConst) {

	int* nodeX = new (nothrow) int[blob.nPts];
	
	int nodes, pixY, pixX, i, j, swp;
	
	int minY = blob.boundingRect.y;
	int maxY = blob.boundingRect.y + blob.boundingRect.height;
	int minX = blob.boundingRect.x;
	int maxX = blob.boundingRect.x + blob.boundingRect.width;
	
	for (pixY = minY; pixY < maxY; pixY++) {
		nodes = 0;
		j = blob.nPts-1;
		for (i=0; i < blob.nPts; i++) {
			if (blob.pts[i].y < (double)pixY && blob.pts[j].y >= (double)pixY ||
				blob.pts[j].y < (double)pixY && blob.pts[i].y >= (double)pixY) {
				nodeX[nodes] = (int) (( (blob.pts[i].x * (blob.pts[j].y - pixY)) + (blob.pts[j].x * (pixY - blob.pts[i].y)) ) / (blob.pts[j].y - blob.pts[i].y));
				nodes++;	
			}
			j = i;
		}
		i=0;
		while (i<nodes-1) {
			if(nodeX[i]>nodeX[i+1]) {
				swp = nodeX[i];
				nodeX[i] = nodeX[i+1];
				nodeX[i+1] = swp;
				if(i) {i--;}
			}
			else {
				i++;
			}
		}
		
		for(i=0; i<nodes; i+=2) {
			if     (nodeX[i  ] >= maxX) break;
			if     (nodeX[i+1] >  minX) {
				if (nodeX[i  ] <  minX) {nodeX[i] = minX;}
				if (nodeX[i+1] >  maxX) {nodeX[i+1] = maxX;}
				for(j = nodeX[i]; j<nodeX[i+1]; j++) {
					trailMap[cart2Idx(j, pixY, mapWidth)] = min((int)(trailMap[cart2Idx(j, pixY, mapWidth)] + blob.exposure * (1 + exposureConst)), 254);
				}
			}
		}
	}
	
	if (nodeX != 0) delete[] nodeX;

}

// --------------------------------------

unsigned char* margTrailMaker::getMap() {

	return trailMap;

}

// ------------------------------------

ofxCvGrayscaleImage& margTrailMaker::getTrailImg() {
	return trailImg;
}