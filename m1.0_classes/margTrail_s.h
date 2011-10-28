/*
 *  margTrail.h
 *  m1.0_warp
 *
 *  Created by André Mintz on 24/06/11.
 *
 */

#include "ofxCvConstants.h"
#include "margBlob.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxCvColorImage.h"
#include <algorithm>



class margTrail {
	
public:
	
	vector<margBlob>	blobs;
	int					nBlobs;
	
	margTrail();
    virtual  ~margTrail();
    
	virtual float getWidth() { return _width; };    //set after first findContours call
	virtual float getHeight() { return _height; };  //set after first findContours call
	
	vector<margBlob>	prevBlobs;
	vector<margBlob>	finalBlobs;
	
	float				exposureConst;
	float				fadeConst;
	
	float				maxUnfitness;
	
	int					totalVidVal;
	int					totalTrailVal;

	vector<margBlob>	interpolate(margBlob& blob1, margBlob& blob2);
	
	void				blendVideo(unsigned char* _vidPixels);
	
	unsigned char*		trailMap;
	unsigned char*		finalMap;
	
	int* findPairs(vector<margBlob> current, vector<margBlob> previous);
	
	float				maxDist;
	float				maxAreaDiff;
	
	int					findContours(ofxCvGrayscaleImage& input,
									 int	minArea,
									 int	maxArea,
									 int	nConsidered,
									 bool	bFindHoles,
									 bool	bUseAproximation,
									 bool	bInterpolate);
	
	void				exposeBlobs();
	
	void				drawMap(float x, float y, float w, float h);
	void				drawBlend(float x, float y, float w, float h);
	
	virtual void  draw() { draw(0,0, _width, _height); };
    virtual void  draw( float x, float y ) { draw(x,y, _width, _height); };
    virtual void  draw( float x, float y, float w, float h );
	virtual void setAnchorPercent(float xPct, float yPct);
    virtual void setAnchorPoint(int x, int y);
	virtual void resetAnchor();  
	
	void	init(int w, int h);
	
	float* normCart (int x, int y, int orig_x, int orig_y) {
		float* norm_coord = new float[2];
		norm_coord[0] = x-orig_x;
		norm_coord[1] = y-orig_y;
		return norm_coord;
	}
	
	void  drawInterp (int x, int y, int w, int h);
	
	
protected:
	
    int  _width;
    int  _height;
    ofxCvGrayscaleImage     inputCopy;
	ofxCvGrayscaleImage		trailImg;
	ofImage			finalImg;
    CvMemStorage*           contour_storage;
    CvMemStorage*           storage;
    CvMoments*              myMoments;
    vector<CvSeq*>          cvSeqBlobs;  //these will become blobs
    
    ofPoint  anchor;
    bool  bAnchorIsPct;      
	
	virtual void reset();
	
	
};