/*
 *  margTrailMaker.h
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margBlob007.h"
#include "ofxOpenCv.h"

class margTrailMaker {
	
public:

	ofxCvGrayscaleImage		trailImg;
	
	unsigned char*			trailMap;
	unsigned char*			finalMap;
	
	int						nMapVals;
	int						mapWidth;
	int						mapHeight;
	
	float					exposureConst,
							fadeConst;
	
	int						blurLevel;
	
	bool					bBlur;
	bool					bFade;
	
	void					init(int _width, int _height);
	
	void					enlargeBlob(margBlob& inBlob, float factor, int maxX, int maxY);
	void					enlargeBlobs(vector<margBlob>& inBlobs, float factor, int maxX, int maxY);
	
	void					updtMap(vector<margBlob> blobs, float _exposureConst, float _fadeConst, int _blurLevel);
	void					updtMap(vector<margBlob> blobs);
	void					setTrailMaker(float _exposureConst, float _fadeConst, int _blurLevel, bool _bFade);
	void					setTrailMaker(float _exposureConst, float _fadeConst, int _blurLevel);
	void					exposeBlob(margBlob& blob, float _exposureConst);
	void					exposePix(int idx, float exposure, int iterLevel);
	void					exposePix2(int idx, float exposure, int iterLevel);
	
	unsigned char *			getMap();
	ofxCvGrayscaleImage&	getTrailImg();
	
	vector<ofPoint>			prevCentroids;
	vector<int>				cumulativeFactor;
	
	int cart2Idx(int x, int y, int w) {
		int idx = (y * w) + x;
		return idx;
	}
	
	ofPoint idxToCart(int idx, int width) {
		return ofPoint((idx%width), (idx/width));
	}
	
	float forceInRange(float inVal, float minVal, float maxVal) {
		if (inVal < minVal) return minVal;
		else if (inVal > maxVal) return maxVal;
		else return inVal;		
	}

};