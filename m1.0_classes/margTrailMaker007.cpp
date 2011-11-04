/*
 *  margTrailMaker.cpp
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margTrailMaker007.h"

void margTrailMaker::init(int _width, int _height) {
		
	mapWidth = _width;
	mapHeight= _height;
	nMapVals = mapWidth * mapHeight;
	
	trailMap = new unsigned char[nMapVals];
	for (int i = 0; i < nMapVals; i++) {
		trailMap[i] = 0;
	}
	
	bFade = true;
	
	cumulativeFactor.assign(nMapVals, 0);
	
	trailImg.allocate(mapWidth, mapHeight);
	
}

//--------------------------------------------------------------------------------

void margTrailMaker::enlargeBlob(margBlob& inBlob, float factor, int _mapWidth, int _mapHeight) {
	
	for (int i = 0; i < inBlob.nPts; i++) {
		inBlob.pts[i].x = forceInRange(((inBlob.pts[i].x - inBlob.centroid.x) * factor + inBlob.centroid.x),
									   0, mapWidth);
		inBlob.pts[i].y = forceInRange(((inBlob.pts[i].y - inBlob.centroid.y) * factor + inBlob.centroid.y),
									   0, mapHeight);
	}
	
	inBlob.boundingRect.width = forceInRange((((inBlob.boundingRect.width + inBlob.boundingRect.x) - inBlob.centroid.x) * factor + inBlob.centroid.x),
											 0, mapWidth);
	inBlob.boundingRect.height= forceInRange((((inBlob.boundingRect.height+ inBlob.boundingRect.y) - inBlob.centroid.y) * factor + inBlob.centroid.y),
											 0, mapHeight);
	inBlob.boundingRect.x	  = forceInRange(((inBlob.boundingRect.x - inBlob.centroid.x) * factor + inBlob.centroid.x),
											 0, mapWidth);
	inBlob.boundingRect.y	  = forceInRange(((inBlob.boundingRect.y - inBlob.centroid.y) * factor + inBlob.centroid.y),
											 0, mapHeight);
}

// ------------------------------------------------

void margTrailMaker::enlargeBlobs(vector<margBlob>& inBlobs, float factor, int _mapWidth, int _mapHeight) {
	for (int i = 0; i < inBlobs.size(); i++) {
		enlargeBlob(inBlobs[i], factor, _mapWidth, _mapHeight);
	}
}

// -------------------------------------------------

void margTrailMaker::updtMap(vector<margBlob> blobs, float _exposureConst, float _fadeConst, int _blurLevel) {
	
	vector<ofPoint> curCentroids;
	if (bFade) {
		for (int i = 0; i < nMapVals; i++) {
			trailMap[i] = max((int)(trailMap[i] * _fadeConst), 0);
			cumulativeFactor[i] = max((cumulativeFactor[i] * _fadeConst), 0.0f);
		}
	}
	
	for (int i = 0; i < blobs.size(); i++) {
		curCentroids.push_back(blobs[i].centroid);
/*		float factor = 1;
//		for(int j = 0; j < prevCentroids.size(); j++) {
//			if (ofDist(prevCentroids[i].x, prevCentroids[i].y, curCentroids[i].x, curCentroids[i].y) < 20) {
//				int idx = cart2Idx(prevCentroids[i].x, prevCentroids[i].y, mapWidth);
//				cumulativeFactor[idx] += 2*_fadeConst;
//				factor = cumulativeFactor[idx];
//			}
//		}
//		enlargeBlob(blobs[i], min(factor, 2.5f), mapWidth, mapHeight); */
		exposeBlob(blobs[i], _exposureConst);
	}
	
	trailImg.setFromPixels(trailMap, mapWidth, mapHeight);
	
	if (_blurLevel > 0) trailImg.blur(_blurLevel);
	
	trailMap = trailImg.getPixels();
	
	prevCentroids = curCentroids;
	curCentroids.clear();

}

// ------------------------------------------------

void margTrailMaker::updtMap(vector<margBlob> blobs) {
	updtMap(blobs, exposureConst, fadeConst, blurLevel);
}

// ------------------------------------------------

void margTrailMaker::setTrailMaker(float _exposureConst, float _fadeConst, int _blurLevel, bool _bFade) {
	exposureConst = _exposureConst;
	fadeConst = _fadeConst;
	blurLevel = _blurLevel;
	bFade = _bFade;
}

// ------------------------------------------------

void margTrailMaker::setTrailMaker(float _exposureConst, float _fadeConst, int _blurLevel) {
	setTrailMaker(_exposureConst, _fadeConst, _blurLevel, true);
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
					trailMap[cart2Idx(j, pixY, mapWidth)] = min((int)(max((int)trailMap[cart2Idx(j, pixY, mapWidth)], 5) * blob.exposure * exposureConst / fadeConst), 254);
				}
			}
		}
	}
	
	if (nodeX != 0) delete[] nodeX;

}


// --------------------------------------

void margTrailMaker::clearMap() {
	for (int i = 0; i < nMapVals; i++) {
		trailMap[i] = 0;
	}
}

// --------------------------------------

unsigned char* margTrailMaker::getMap() {

	return trailMap;

}

// ------------------------------------

ofxCvGrayscaleImage& margTrailMaker::getTrailImg() {
	return trailImg;
}