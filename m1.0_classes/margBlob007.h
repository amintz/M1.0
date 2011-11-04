/*
 *  margBlob.h
 *  m1.0_trail
 *
 *  Created by André Mintz on 25/06/11.
 *
 */

#include "ofxOpenCv.h"
#include "ofxCvBlob.h"

#ifndef MARG_BLOB
#define MARG_BLOB

class margBlob : public ofxCvBlob {
		
public:
	
	float				exposure;
	float				condScaleFactor;
	float				distFromPair;
	
	margBlob() {
		area			= 0.0f;
		length			= 0.0f;
		hole			= false;
		nPts			= 0;
		condScaleFactor = 1.0f;
		distFromPair	= -1;
	}
	
	~margBlob() {
		pts.clear();
	}
	
	margBlob( const ofxCvBlob& b ) {
        area = b.area;
        length = b.length;
        boundingRect = b.boundingRect;
        centroid = b.centroid;
        hole = b.hole;
        pts = b.pts;
		nPts = b.nPts;
    }
	
};

#endif
		

