/*
 *  margBlobCorrectionUtils.h
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofThread.h"
#include "ofxXmlSettings.h"
#include "margBlob007.h"
//#include "cv.h"


class margBlobCorrector {

public:
	
	// FUNCTIONS ------------------------------------*
	
	void	init();
	
	void	setInDimensions(double _inW, double _inH);
	void	setOutDimensions(double _outW, double _outH);
	void	setPersCorrection(CvMat* _translate);
	void	setCameraMatrix(float fX, float fY, float cX, float cY);
	void	setDistCoeffs(float rdX, float rdY, float tgX, float tgY);
	void	loadMats(string xmlFile);
	void	setAll(CvMat* _camera_matrix, CvMat* _dist_coeffs, CvMat* _translate, double _inW, double _inH,
				   double _outW, double _outH);
	
	void	setMode(bool _correctLens, bool _correctPerspective, bool _scale);
	
	
	margBlob		  lensCorrectBlob(margBlob& inBlob);
	margBlob		  persCorrectBlob(margBlob& inBlob);
	margBlob		  scaleBlob(margBlob& inBlob);
	
	void			  enlargeBlob(margBlob& inBlob, float factor);
	
	vector<margBlob>&  fullCorrectBlobs(vector<margBlob>& inBlobs);
	
	void			  feedBlobs(vector<margBlob>& inBlobs);
	vector<margBlob>&  getCorrectBlobs();
	
	
	ofPoint	 undistortPoint(float _x, float _y);
	
	void	 drawBlobs(int x, int y, int w, int h);
	
	CvMat*	 getPerspMat();
	CvMat*   getCamMat();
	CvMat*	 getDistMat();

	
	// RESOURCES -----------------------------------*
	
	CvMat*	 camera_matrix;
	CvMat*	 dist_coeffs;
	CvMat*	 translate;
	
	float*	 data;
	
	vector<margBlob> correctBlobs;
	
	double	 inW,  inH,
			 outW, outH;
	
	double	 fX,  fY,
			 ifX, ifY,
			 cX,  cY,
			 rX,  rY,
			 tX,  tY;
	
	bool	 bCorrectLens,
			 bCorrectPerspective,
			 bScale;
	
	margBlob lensUndistBounds;
	
	void	 calculateLensUndistBounds();
	ofPoint	 bestIfOutOfBounds(ofPoint inPt);

	// UTILS ---------------------------------------------------- *
	
	double* normCoords (double x, double y, double w, double h) {
		double* norm_coord = new (nothrow) double[2];
		norm_coord[0] = ((x - w/2) / w/2);
		norm_coord[1] = ((y - h/2) / h/2);
		return norm_coord;
		if(norm_coord != 0) delete[] norm_coord;
	}
	
	double* normCoords (ofPoint inPt, double w, double h) {
		double* norm_coord = new (nothrow) double[2];
		norm_coord[0] = ((inPt.x - w/2) / w/2);
		norm_coord[1] = ((inPt.y - h/2) / h/2);
		return norm_coord;
		if(norm_coord != 0) delete[] norm_coord;
	}
	
	double	normCoord (double val, double range) {
		return ((val - range/2) / range/2);
	}
	
	double* absCoords (double x, double y, double w, double h) {
		double* abs_coord = new (nothrow) double[2];
		abs_coord[0] = (x * (w / 2) + w/2);
		abs_coord[1] = (y * (h / 2) + h/2);
		return abs_coord;		
		if(abs_coord != 0) delete[] abs_coord;
	}
	
	double absCoord (double val, double range, bool bForceInRange) {
		if(bForceInRange) return forceInRange((val * (range / 2) + range / 2), 0, range);
		else return (val * (range / 2) + range / 2);
	}
	
	ofPoint absPoint (double normX, double normY, double w, double h, bool bForceInRange) {
		
		float absX, absY;
		
		if(bForceInRange) {
			absX = forceInRange((normX * (w/2) + w/2), 0, w);
			absY = forceInRange((normY * (h/2) + h/2), 0, h);
		}
		else {
			absX = (normX * (w/2) + w/2);
			absY = (normY * (h/2) + h/2);
		}
		return ofPoint(absX, absY);
	}
	
	double	forceInRange (double val, double min, double max) {
		if (val < min) return min;
		else if (val > max) return max;
		else return val;

	}
	
	ofPoint perspTransform(ofPoint inPoint, CvMat* perspMat, bool bForceInRange){
		
		data = perspMat->data.fl;
		
		//from Myler & Weeks - so fingers crossed!
		float outX = ((data[0]*inPoint.x + data[1]*inPoint.y + data[2]) / (data[6]*inPoint.x + data[7]*inPoint.y + 1));
		float outY = ((data[3]*inPoint.x + data[4]*inPoint.y + data[5]) / (data[6]*inPoint.x + data[7]*inPoint.y + 1));
		
		if (bForceInRange) {
			outX = forceInRange(outX, 0, inW);
			outY = forceInRange(outY, 0, inH);
		}
		
		return ofPoint(outX, outY);
	}
	
	float* perspTransformCoords(float inX, float inY, bool bForceInRange) {

		float outX = ((data[0]*inX + data[1]*inY + data[2]) / (data[6]*inX + data[7]*inY + 1));
		float outY = ((data[3]*inX + data[4]*inY + data[5]) / (data[6]*inX + data[7]*inY + 1));
		
		float* outCoords = new (nothrow) float[2];
		
		if (bForceInRange) {
			outCoords[0] = forceInRange(outX, 0, inW);
			outCoords[1] = forceInRange(outY, 0, inH);
		} else {
			outCoords[0] = outX;
			outCoords[1] = outY;
		}
		
		return outCoords;
		delete[] outCoords;
	}
	
	ofPoint perspTransform(ofPoint inPoint, bool bForceInRange){
		
		float outX = ((data[0]*inPoint.x + data[1]*inPoint.y + data[2]) / (data[6]*inPoint.x + data[7]*inPoint.y + 1));
		float outY = ((data[3]*inPoint.x + data[4]*inPoint.y + data[5]) / (data[6]*inPoint.x + data[7]*inPoint.y + 1));
		
		if (bForceInRange) {
			outX = forceInRange(outX, 0, inW);
			outY = forceInRange(outY, 0, inH);
		}
		
		return ofPoint(outX, outY);
	}
	
	
protected:
	
	ofPoint  anchor;
    bool     bAnchorIsPct;
	
	
};

//--------------------------------------------------------------------------------


