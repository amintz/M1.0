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
#include "cv.h"
#include "ofxThread.h"
#include <algorithm>



class margTrail : public ofxThread {
	
public:
	
	// Basics
	
	int				totalVidVal;
	int				totalTrailVal;
	
	bool			optimize;
	
	margTrail();
    virtual			~margTrail();
    
	virtual float	getWidth() { return _width; };    //set after first findContours call
	virtual float	getHeight() { return _height; };  //set after first findContours call
	
	virtual void	setAnchorPercent(float xPct, float yPct);
    virtual void	setAnchorPoint(int x, int y);
	virtual void	resetAnchor();  
	
	void			init(int w, int h);
	
	
	// Images
	
	ofxCvGrayscaleImage		trailImg;
	ofxCvColorImage			finalImg;
	ofxCvGrayscaleImage input;
	
	
	// Contour Finding
	
	vector<margBlob>	blobs;
	vector<double>		blobsPtsNormCoords;
	int					nBlobs;
	
	int					minArea;
	int					maxArea;
	int					nConsidered;
	
	bool				bFindHoles;
	bool				bUseAproximation;
	
	
	bool				bFindContoursSet;
	
	void				setFindContours(ofxCvGrayscaleImage& _input,
										int	_minArea,
										int	_maxArea,
										int	_nConsidered,
										bool	_bFindHoles,
										bool	_bUseAproximation,
										bool	_bInterpolate,
										bool	_bBlur);
	
	void				threadedFunction();
	
	int					findContours(ofxCvGrayscaleImage& _input,
									 int	_minArea,
									 int	_maxArea,
									 int	_nConsidered,
									 bool	_bFindHoles,
									 bool	_bUseAproximation,
									 bool	_bInterpolate,
									 bool	_bBlur);
	
	// Geometric Correction
	
	vector<double>		lensCorrectPoints(vector<double> inCoords, CvMat* camera_matrix, CvMat* dist_coeffs);
	vector<double>		persCorrectPoints(vector<double> inCoords, CvMat* translate, double w, double h);
	vector<margBlob>	makeCorrectedBlobs(vector<double>inCoords, vector<margBlob> origBlobs, double w, double h);
	margBlob			lensCorrectBlob(margBlob inBlob, CvMat* camera_matrix, CvMat* dist_coeffs, double w, double h);
	margBlob			persCorrectBlob(margBlob inBlob, CvMat* translate, double w, double h);
	margBlob			scaleBlob(margBlob inBlob, double inW, double inH, double outW, double outH);
	margBlob			fullCorrectBlob(margBlob inBlob, CvMat* camera_matrix, CvMat* dist_coeffs, CvMat* translate,
										double inW, double inH, double outW, double outH);
	vector<margBlob>	fullCorrectBlobs(vector<margBlob> inBlobs, CvMat* camera_matrix, CvMat* dist_coeffs, CvMat* translate,
										 double inW, double inH, double outW, double outH);
	
	// Interpolation
	
	bool				bInterpolate;
	
	int*				findPairs(vector<margBlob> current, vector<margBlob> previous);
	
	vector<margBlob>	interpolate(margBlob& blob1, margBlob& blob2);
	
	float				maxDist;
	float				maxAreaDiff;
	float				maxUnfitness;
	vector<margBlob>	prevBlobs;
	vector<margBlob>	finalBlobs;
	
	
	// Mapping
	
	unsigned char*		trailMap;
	unsigned char*		finalMap;
	
	float				exposureConst;
	float				fadeConst;
	
	bool				bBlur;
	
	void				exposeBlobs(bool _bBlur);
	
	
	// Blending
	
	void				blendVideo(unsigned char* _vidPixels);
	
	
	// Drawing
	
	void				drawInterp (int x, int y, int w, int h);
	void				drawMap(float x, float y, float w, float h);
	void				drawBlend(float x, float y, float w, float h);
	
	virtual void		draw() { draw(0,0, _width, _height); };
    virtual void		draw( float x, float y ) { draw(x,y, _width, _height); };
    virtual void		draw( float x, float y, float w, float h );
	
	
	// Utils
		
	float*	normCart (int x, int y, int orig_x, int orig_y) {
		float* norm_coord = new (nothrow) float[2];
		norm_coord[0] = x-orig_x;
		norm_coord[1] = y-orig_y;
		return norm_coord;
		if(norm_coord != 0) delete[] norm_coord;	
	}
	
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
	
	double absCoord (double val, double range) {
		return (val * (range / 2) + range / 2);
	}
	
	ofPoint absPoint (double normX, double normY, double w, double h) {
		float absX = (normX * (w/2) + w/2);
		float absY = (normY * (h/2) + h/2);
		return ofPoint(absX, absY);
	}
	
protected:
	
	// Basics
    
	int  _width;
    int  _height;
	int size;
	
	ofPoint  anchor;
    bool  bAnchorIsPct;      
	
	virtual void reset();
	
	
	// Contour Finding
    
	ofxCvGrayscaleImage     inputCopy;
    CvMemStorage*           contour_storage;
    CvMemStorage*           storage;
    CvMoments*              myMoments;
    vector<CvSeq*>          cvSeqBlobs;  //these will become blobs
    
  
	
};