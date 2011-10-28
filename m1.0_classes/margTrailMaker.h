/*
 *  margTrailMaker.h
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margBlob.h"
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
	
	void					init(int _width, int _height);	
	void					updtMap(vector<margBlob> blobs, float _exposureConst, float _fadeConst, int _blurLevel);
	void					updtMap(vector<margBlob> blobs);
	void					setTrailMaker(float _exposureConst, float _fadeConst, int _blurLevel);
	void					exposeBlob(margBlob& blob, float _exposureConst);
	
	unsigned char *			getMap();
	ofxCvGrayscaleImage&	getTrailImg();
	
	int cart2Idx(int x, int y, int w) {
		int idx = (y * w) + x;
		return idx;
	}
	

};