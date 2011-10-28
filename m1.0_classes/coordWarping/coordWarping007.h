#ifndef _COORD_WARPING_H
#define _COORD_WARPING_H

//we use openCV to calculate our transform matrix

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofVectorMath.h"
#include "ofxXmlSettings.h"

class coordWarping{


	public:
	
		//---------------------------
		coordWarping();

		//first calculate the matrix
		//do this only when it changes - saves cpu!
		void calculateMatrix(ofVec2f src[4], ofVec2f dst[4]);

		//returns true if the point is inside the src quad
		bool bInQuad(ofVec2f pt);
		
		//then when ever you need to warp the coords call this
		ofVec2f transform(float xIn, float yIn);
		
		
	
	protected:
		
		ofVec2f srcQuad[4];
		ofVec2f dstQuad[4];
		
		CvPoint2D32f cvsrc[4];
		CvPoint2D32f cvdst[4];
		CvMat *translate;
				
};

#endif