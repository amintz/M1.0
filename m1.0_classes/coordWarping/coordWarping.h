#ifndef _COORD_WARPING_H
#define _COORD_WARPING_H

//we use openCV to calculate our transform matrix

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxVectorMath.h"
#include "ofxXmlSettings.h"

class coordWarping{


	public:
	
		//---------------------------
		coordWarping();

		//first calculate the matrix
		//do this only when it changes - saves cpu!
		void calculateMatrix(ofxPoint2f src[4], ofxPoint2f dst[4]);

		//returns true if the point is inside the src quad
		bool bInQuad(ofxPoint2f pt);
		
		//then when ever you need to warp the coords call this
		ofxPoint2f transform(float xIn, float yIn);
		
		
	
	protected:
		
		ofxPoint2f srcQuad[4];
		ofxPoint2f dstQuad[4];
		
		CvPoint2D32f cvsrc[4];
		CvPoint2D32f cvdst[4];
		CvMat *translate;
				
};

#endif