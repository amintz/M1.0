/*
 *  margBlobFinder.h
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MARG_BLOB_FINDER
#define	 MARG_BLOB_FINDER

#include "ofxOpenCv.h"
#include "margBlob007.h"
#include "ofThread.h"
#include <algorithm>
//#include "opencv.hpp"

class margBlobFinder : public ofThread {
	
public:
	
	
	// FUNCTIONS ------------------------------------*
		
	
	margBlobFinder();
	
    virtual				~margBlobFinder();
    
	virtual float		getWidth() { return width; };    //set after first findContours call
	virtual float		getHeight() { return height; };  //set after first findContours call
	
	void				init(int w, int h);
	
	void				setPixelStorage(unsigned char* pixelStorage, bool& pixelStorageFlag);
	
	void				setBlobStorage(vector<margBlob>& blobStorage, bool& blobStorageFlag);
	
	void				feedPixels(unsigned char* inPix); // input must be grayscale
	
	void				setFindContours(ofxCvGrayscaleImage& _input,
										int		_minArea,
										int		_maxArea,
										int		_nConsidered,
										bool	_bFindHoles,
										bool	_bUseAproximation);
	
	void				setFindContours(int		_minArea,
										int		_maxArea,
										int		_nConsidered,
										bool	_bFindHoles,
										bool	_bUseAproximation,
										int		_threshMin,
										int		_threshMax);
	
	void				setROI(const ofRectangle& _rectROI);
	
	void				threadedFunction();
	
	int					findContours(ofxCvGrayscaleImage& _input,
									 int	_minArea,
									 int	_maxArea,
									 int	_nConsidered,
									 bool	_bFindHoles,
									 bool	_bUseAproximation);
	
	int					findContours();
	
	vector<margBlob>&	getFoundBlobs();
	
	void				drawBlobs (int x, int y, int w, int h);
	
	void				threshold(unsigned char* pix);
	
	ofxCvGrayscaleImage& getInputImg();
	
	unsigned char*		getThreshPix();
		
	/*	virtual void	setAnchorPercent(float xPct, float yPct);
	 //  virtual void	setAnchorPoint(int x, int y);
	 //	virtual void	resetAnchor(); */
	
	// RESOURCES ---------------------------------*	
	
	// Contour Finding
	
	vector<margBlob>	blobs;
	int					nBlobs;
	
	int					minArea;
	int					maxArea;
	int					nConsidered;
	
	bool				bFindHoles;
	bool				bUseAproximation;
	
	bool				bFindContoursSet;
	
	
	// Threshold	
	
	int					threshMin,
						threshMax;
	
	
protected:
	
	// Basics
    
	int				width,
					height,
					size;
	
	ofRectangle		rectROI;
	
	ofPoint			anchor;
    bool			bAnchorIsPct;      
	
	virtual void	reset();
	
	bool			bNewFrame;
	
	unsigned char*	inPix;
	
	unsigned char*  threshPix;
	
	
	// Contour Finding
    
	ofxCvGrayscaleImage     inputCopy;
	ofxCvGrayscaleImage		input;
    CvMemStorage*           contour_storage;
    CvMemStorage*           storage;
    CvMoments*              myMoments;
    vector<CvSeq*>          cvSeqBlobs;  //these will become blobs
	
};

#endif

