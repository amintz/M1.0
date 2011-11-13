/*
 *  margInterpolator.h
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margBlob.h"

class margBlobInterpolator {

public:
	
	vector<margBlob>	prevBlobs;
	vector<margBlob>	finalBlobs;
	
	void				init(int _width, int _height);
	
	void				feedBlobs(vector<margBlob> inBlobs);
	
	int*				findPairs(vector<margBlob> current, vector<margBlob> previous, float _maxDist,
								  float _maxAreaDiff, float _maxUnfitness);
	
	int*				findPairs(vector<margBlob> current);
	
	vector<margBlob>	interpolate(margBlob& blob1, margBlob& blob2);
	
	
	// Feed vector of blobs for interpolation.
	// Will interpolate after second feed, adding to blob vector that is fed.
	
	void				makeInterpolated(vector<margBlob> current, float _maxDist, float _maxAreaDiff,
										 float _maxUnfitness);
	
	void				makeInterpolated(vector<margBlob> current);
	
	vector<margBlob>	getInterpolatedBlobs();
	
	void				setInterpolator(float _maxDist, float _maxAreaDiff, float _maxUnfitness);
	
	void				drawInterp (int x, int y, int w, int h);
	
	float				maxDist,
						maxAreaDiff,
						maxUnfitness;
	
protected:
	
	int		width,
			height;
	
	
	ofPoint  anchor;
    bool     bAnchorIsPct;
	
	
	ofPoint	normCart (int x, int y, int orig_x, int orig_y) {
		int normX = x-orig_x;
		int normY = y-orig_y;
		return ofPoint(normX, normY);
	}

};