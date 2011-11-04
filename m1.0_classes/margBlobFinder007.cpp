/*
 *  margBlobFinder.cpp
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margBlobFinder007.h"

// -------------------------------------------

bool marg2_sort_carea_compare( const CvSeq* a, const CvSeq* b) {
	// use opencv to calc size, then sort based on size
	float areaa = fabs(cvContourArea(a, CV_WHOLE_SEQ));
	float areab = fabs(cvContourArea(b, CV_WHOLE_SEQ));
	
    //return 0;
	return (areaa > areab);
}

// -------------------------------------------

margBlobFinder::margBlobFinder() {
    width = 0;
    height = 0;
	myMoments = (CvMoments*)malloc( sizeof(CvMoments) );
	reset();
}

//--------------------------------------------------------------------------------
margBlobFinder::~margBlobFinder() {
	free( myMoments );
}

// ------------------------------------------------------------

void margBlobFinder::init(int w, int h) {
	
	bFindContoursSet = false;

	size = w*h;
	
	width = w;
	height = h;
	
	input.setUseTexture(false);
	input.allocate(width, height);
	
	threshPix = new unsigned char[size];
	
	bNewFrame = false;
	
}

// ------------------------------------------------------------

void margBlobFinder::setPixelStorage(unsigned char* pixelStorage, bool& pixelStorageFlag) {
	if(pixelStorageFlag) {
		inPix = pixelStorage;
		pixelStorageFlag = false;
	}
}

// ------------------------------------------------------------

void margBlobFinder::setBlobStorage(vector<margBlob>& blobStorage, bool& blobStorageFlag) {
	blobs = blobStorage;
	blobStorageFlag = true;
}

// ------------------------------------------------------------

void margBlobFinder::reset() {
	cvSeqBlobs.clear();
    blobs.clear();
	nBlobs = 0;
}

// ------------------------------------------------------------

void margBlobFinder::setROI(const ofRectangle& _rectROI) {
	rectROI = _rectROI;
}

// ------------------------------------------------------------

void margBlobFinder::threadedFunction () {
	if (input.getWidth()!=0) {
		while ( isThreadRunning() != 0 ) {
			if ( lock() ) {
				threshold(inPix);
				input.setFromPixels(threshPix, width, height);
				bNewFrame = true;
				findContours();
				unlock();
			}
		}
	}
}

// ------------------------------------------------------------

void margBlobFinder::feedPixels(unsigned char* inPix) {
	threshold(inPix);
	input.setROI(rectROI);
	input.setFromPixels(threshPix, width, height);
	bNewFrame = true;
}

// ------------------------------------------------------------

void margBlobFinder::setFindContours( ofxCvGrayscaleImage&  _input,
								int _minArea,
								int _maxArea,
								int _nConsidered,
								bool _bFindHoles,
								bool _bUseApproximation) {
	
//	if( input.getWidth() == 0 ) {
//		width = _input.getWidth();
//		height = _input.getHeight();
//		size = width*height;
//		input.clear();
//		input.setUseTexture(false);
//		input.allocate( width, height );
//	} else if( input.getWidth() != _input.getWidth() || input.getHeight() != _input.getHeight() ) {
//        // reallocate to new size
//		width = _input.getWidth();
//		height = _input.getHeight();
//		size = width*height;
//        input.clear();
//		input.setUseTexture(false);		
//        input.allocate( width, height );
//	}
//	
//    input.setROI( _input.getROI() );
	
	input = _input;
	minArea = _minArea;
	maxArea = _maxArea;
	nConsidered = _nConsidered;
	bFindHoles = _bFindHoles;
	bUseAproximation = _bUseApproximation;
	bFindContoursSet = true;
	
}

// ------------------------------------------------------------

void margBlobFinder::setFindContours(int _minArea,
									 int _maxArea,
									 int _nConsidered,
									 bool _bFindHoles,
									 bool _bUseApproximation,
									 int _threshMin,
									 int _threshMax) {
	
	minArea = _minArea;
	maxArea = _maxArea;
	nConsidered = _nConsidered;
	bFindHoles = _bFindHoles;
	bUseAproximation = _bUseApproximation;
	threshMin = _threshMin;
	threshMax = _threshMax;
	
	bFindContoursSet = true;
}


// ------------------------------------------------------------

int margBlobFinder::findContours(ofxCvGrayscaleImage&  _input,
								 int _minArea,
								 int _maxArea,
								 int _nConsidered,
								 bool _bFindHoles,
								 bool _bUseApproximation) {
	
	blobs.reserve(_nConsidered*sizeof(margBlob));
	
    // get width/height disregarding ROI
    IplImage* ipltemp = _input.getCvImage();
    width = ipltemp->width;
    height = ipltemp->height;
	
	reset();
	
	// opencv will clober the image it detects contours on, so we want to
    // copy it into a copy before we detect contours.  That copy is allocated
    // if necessary (necessary = (a) not allocated or (b) wrong size)
	// so be careful if you pass in different sized images to "findContours"
	// there is a performance penalty, but we think there is not a memory leak
    // to worry about better to create mutiple contour finders for different
    // sizes, ie, if you are finding contours in a 640x480 image but also a
    // 320x240 image better to make two ofxCvContourFinder objects then to use
    // one, because you will get penalized less.
	
	if( inputCopy.getWidth() == 0 ) {
		inputCopy.clear();
		inputCopy.setUseTexture(false);
		inputCopy.allocate( width, height );
	} else if( inputCopy.getWidth() != width || inputCopy.getHeight() != height ) {
        // reallocate to new size
        inputCopy.clear();
		inputCopy.setUseTexture(false);		
        inputCopy.allocate( width, height );
	}

	inputCopy.setROI( _input.getROI() );

    inputCopy = _input;
	
	CvSeq* contour_list = NULL;
	contour_storage = cvCreateMemStorage( 1000 );
	storage	= cvCreateMemStorage( 1000 );
	
	CvContourRetrievalMode  retrieve_mode = (_bFindHoles) ? CV_RETR_LIST : CV_RETR_EXTERNAL;
	
	cvFindContours( inputCopy.getCvImage(), contour_storage, &contour_list,
				   sizeof(CvContour), retrieve_mode, _bUseApproximation ? CV_CHAIN_APPROX_SIMPLE : CV_CHAIN_APPROX_NONE );
	
	CvSeq* contour_ptr = contour_list;
	
	// put the contours from the linked list, into an array for sorting
	while( (contour_ptr != NULL) ) {
		float area = fabs( cvContourArea(contour_ptr, CV_WHOLE_SEQ) );
		if( (area > _minArea) && (area < _maxArea) ) {
            cvSeqBlobs.push_back(contour_ptr);
		}
		contour_ptr = contour_ptr->h_next;
	}
	
	
	// sort the pointers based on size
	if( cvSeqBlobs.size() > 1 ) {
        sort( cvSeqBlobs.begin(), cvSeqBlobs.end(), marg2_sort_carea_compare );
	}
	
	
	// now, we have cvSeqBlobs.size() contours, sorted by size in the array
    // cvSeqBlobs let's get the data out and into our structures that we like
	for( int i = 0; i < MIN(_nConsidered, (int)cvSeqBlobs.size()); i++ ) {
		
		blobs.push_back( margBlob() );
		float area = cvContourArea( cvSeqBlobs[i], CV_WHOLE_SEQ );
		CvRect rect	= cvBoundingRect( cvSeqBlobs[i], 0 );
		cvMoments( cvSeqBlobs[i], myMoments );
		
		blobs[i].area                     = fabs(area);
		blobs[i].hole                     = area < 0 ? true : false;
		blobs[i].length 			      = cvArcLength(cvSeqBlobs[i]);
		blobs[i].boundingRect.x           = rect.x + rectROI.x;
		blobs[i].boundingRect.y           = rect.y + rectROI.y;
		blobs[i].boundingRect.width       = rect.width;
		blobs[i].boundingRect.height      = rect.height;
		blobs[i].centroid.x 			  = (myMoments->m10 / myMoments->m00);
		blobs[i].centroid.y 			  = (myMoments->m01 / myMoments->m00);
		
		// get the points for the blob:
		CvPoint           pt;
		CvSeqReader       reader;
		cvStartReadSeq( cvSeqBlobs[i], &reader, 0 );
		
    	for( int j=0; j < cvSeqBlobs[i]->total; j++ ) {
			CV_READ_SEQ_ELEM( pt, reader );
            blobs[i].pts.push_back( ofPoint((float)pt.x + rectROI.x, (float)pt.y + rectROI.y) );
		}
		blobs[i].nPts	  = blobs[i].pts.size();
		blobs[i].exposure = 1;
		
	}
	
    nBlobs = blobs.size();
	
	// Free the storage memory.
	// Warning: do this inside this function otherwise a strange memory leak
	if( contour_storage != NULL ) { cvReleaseMemStorage(&contour_storage); }
	if( storage != NULL ) { cvReleaseMemStorage(&storage); }
	
	return nBlobs;
	
}

// -------------------------------------------

int	margBlobFinder::findContours() {
	if (bNewFrame) {
		bNewFrame = false;
		return findContours(input, minArea, maxArea, nConsidered, bFindHoles, bUseAproximation);
	}
	else {
		return 0;
	}
}

// -------------------------------------------

void margBlobFinder::drawBlobs(int x, int y, int w, int h) {
	
	float scalex = 0.0f;
    float scaley = 0.0f;
    if( width != 0 ) { scalex = (float)w/(float)width; } else { scalex = 1.0f; }
    if( height != 0 ) { scaley = (float)h/(float)height; } else { scaley = 1.0f; }
	
    if(bAnchorIsPct){
        x -= anchor.x * w;
        y -= anchor.y * h;
    }else{
        x -= anchor.x;
        y -= anchor.y;
    }
	
	
	ofPushStyle();
	
	// ---------------------------- draw the bounding rectangle
	
	ofSetHexColor(0xDD00CC);
	glPushMatrix();
	glTranslatef( x, y, 0.0 );
	glScalef( scalex, scaley, 0.0 );
	
	ofNoFill();
	for( int i=0; i<(int)blobs.size(); i++ ) {
		ofRect( blobs[i].boundingRect.x, blobs[i].boundingRect.y,
			   blobs[i].boundingRect.width, blobs[i].boundingRect.height );
	}
	
	// ---------------------------- draw the blobs
	
	ofSetHexColor(0x00FFFF);


	for( int i=0; i<blobs.size(); i++ ) {
		ofNoFill();
		ofBeginShape();
		for( int j=0; j<blobs[i].nPts; j++ ) {
			ofVertex( blobs[i].pts[j].x, blobs[i].pts[j].y );
		}
		ofEndShape();
	}
	glPopMatrix();
	ofPopStyle();
	
}


// -------------------------------------------

vector<margBlob>& margBlobFinder::getFoundBlobs() {

	return blobs;
	
}

// -------------------------------------------

void margBlobFinder::threshold(unsigned char* pix) {	
	for (int i = 0; i < size; i++) {
		if(pix[i] < threshMax && pix[i] > threshMin) threshPix[i] = 255;
		else threshPix[i] = 0;
	}
}

// ------------------------------------------

ofxCvGrayscaleImage& margBlobFinder::getInputImg() {
	return input;
}

// -----------------------------------------

unsigned char* margBlobFinder::getThreshPix() {
	return threshPix;
}
