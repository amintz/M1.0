/*
 *  margTrail.cpp
 *  m1.0_warp
 *
 *  Created by André Mintz on 24/06/11.
 *
 */

#include "margTrail.h"

// ------------------------------------------------------------

bool marg_sort_carea_compare( const CvSeq* a, const CvSeq* b) {
	// use opencv to calc size, then sort based on size
	float areaa = fabs(cvContourArea(a, CV_WHOLE_SEQ));
	float areab = fabs(cvContourArea(b, CV_WHOLE_SEQ));
	
    //return 0;
	return (areaa > areab);
}

// ------------------------------------------------------------

bool sort_fitness_compare(const vector<int> f, const vector<int> g) {

	return f[2] > g[2];
	
}

//--------------------------------------------------------------------------------

margTrail::margTrail() {
    _width = 0;
    _height = 0;
	myMoments = (CvMoments*)malloc( sizeof(CvMoments) );
	reset();
}

//--------------------------------------------------------------------------------
margTrail::~margTrail() {
	free( myMoments );
}

// ------------------------------------------------------------

void margTrail::init(int w, int h) {
	maxDist = 100;			// implement absolute maximum with variation according to frame rate
	maxAreaDiff = 10;		// relative (10%)
	maxUnfitness = 1 / (maxDist + maxAreaDiff);
	
	bFindContoursSet = false;
	
	exposureConst = 6;
	fadeConst	  = 0.2;
	
	totalVidVal = w*h*3;
	totalTrailVal = w*h;
	
	trailMap = new unsigned char[totalTrailVal];
	finalMap = new unsigned char[totalVidVal];

	for (int i = 0; i < totalTrailVal; i++) {
		trailMap[i] = 0;
		finalMap[i*3] = 0;
		finalMap[i*3+1] = 0;
		finalMap[i*3+2] = 0;
	}
	trailImg.allocate(w, h);
	finalImg.allocate(w, h);
}

// ------------------------------------------------------------

void margTrail::reset() {
	cvSeqBlobs.clear();
    blobs.clear();
	finalBlobs.clear();
	blobsPtsNormCoords.clear();
    nBlobs = 0;
}

// ------------------------------------------------------------

void margTrail::threadedFunction () {
	
	if (input.getWidth()!=0) {
		while ( isThreadRunning() != 0 ) {
			if ( lock() ) {
				findContours(input, minArea, maxArea, nConsidered, bFindHoles, bUseAproximation, bInterpolate, bBlur);
				unlock();
			}
		}
	}
	
	
}

// ------------------------------------------------------------

void margTrail::setFindContours( ofxCvGrayscaleImage&  _input,
							int _minArea,
							int _maxArea,
							int _nConsidered,
							bool _bFindHoles,
							bool _bUseApproximation,
							bool _bInterpolate,
							bool _bBlur) {
	
	if( input.getWidth() == 0 ) {
		input.clear();
		input.setUseTexture(false);
		_width = _input.getWidth();
		_height = _input.getHeight();
		size = _width*_height;
		input.allocate( _width, _height );
	} else if( inputCopy.getWidth() != _width || inputCopy.getHeight() != _height ) {
        // reallocate to new size
        input.clear();
		input.setUseTexture(false);		
        input.allocate( _width, _height );
	}
	
    input.setROI( _input.getROI() );
	
	input = _input;
	minArea = _minArea;
	maxArea = _maxArea;
	nConsidered = _nConsidered;
	bFindHoles = _bFindHoles;
	bUseAproximation = _bUseApproximation;
	bInterpolate = _bInterpolate;
	bBlur = _bBlur;
	bFindContoursSet = true;
	
}


// ------------------------------------------------------------

int margTrail::findContours( ofxCvGrayscaleImage&  _input,
									 int _minArea,
									 int _maxArea,
									 int _nConsidered,
									 bool _bFindHoles,
									 bool _bUseApproximation,
									 bool _bInterpolate,
									 bool _bBlur) {
	
    // get width/height disregarding ROI
    IplImage* ipltemp = _input.getCvImage();
    _width = ipltemp->width;
    _height = ipltemp->height;
	
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
		inputCopy.allocate( _width, _height );
	} else if( inputCopy.getWidth() != _width || inputCopy.getHeight() != _height ) {
        // reallocate to new size
        inputCopy.clear();
		inputCopy.setUseTexture(false);		
        inputCopy.allocate( _width, _height );
	}
	
    inputCopy.setROI( _input.getROI() );
    inputCopy = _input;
	
	CvSeq* contour_list = NULL;
	contour_storage = cvCreateMemStorage( 1000 );
	storage	= cvCreateMemStorage( 1000 );
	
	CvContourRetrievalMode  retrieve_mode
	= (_bFindHoles) ? CV_RETR_LIST : CV_RETR_EXTERNAL;
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
        sort( cvSeqBlobs.begin(), cvSeqBlobs.end(), marg_sort_carea_compare );
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
		blobs[i].boundingRect.x           = rect.x;
		blobs[i].boundingRect.y           = rect.y;
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
            blobs[i].pts.push_back( ofPoint((float)pt.x, (float)pt.y) );
			
			
//			double*	inCoords = normCoords((double)pt.x, (double)pt.y, _width, _height);
//			
//			blobsPtsNormCoords.push_back(inCoords[0]);
//			blobsPtsNormCoords.push_back(inCoords[1]);
			
		}
		blobs[i].nPts = blobs[i].pts.size();
		
	}
	
    nBlobs = blobs.size();
	
	// Free the storage memory.
	// Warning: do this inside this function otherwise a strange memory leak
	if( contour_storage != NULL ) { cvReleaseMemStorage(&contour_storage); }
	if( storage != NULL ) { cvReleaseMemStorage(&storage); }
	
	
	/* INTERPOLATING TRACKED BLOBS */
	
	if(_bInterpolate) {
		
		// If no previous blobs saved, save these for the next frame and go on
		
		if (prevBlobs.size() == 0) {
//			for (int i=0; i < nBlobs; i++) {
//				prevBlobs.push_back(blobs[i]);
//			}
			prevBlobs = blobs;
		}
		
		// If previous blobs are saved, check for good pairs
		
		else {
			int* pairs = findPairs(blobs, prevBlobs);
			
			for (int i = 0; i < nBlobs; i++) {
				if (pairs[i] >= 0) {
					if (prevBlobs[pairs[i]].pts.size() > 0) {
						vector<margBlob> interBlobs = interpolate(prevBlobs[pairs[i]], blobs[i]);
						for (int j = 0; j < interBlobs.size(); j++) {
							finalBlobs.push_back(interBlobs[j]);
						}
					}
				}
				else {
					finalBlobs.push_back(blobs[i]);
				}
			}
			
			prevBlobs.clear();
			
			for (int i = 0; i<nBlobs; i++) {
				prevBlobs.push_back(blobs[i]);
			}
			if(pairs != 0) delete[] pairs;
		}
	}
	
	else {
		for (int i = 0; i < nBlobs; i++) {
			blobs[i].exposure = 1;
			finalBlobs.push_back(blobs[i]);
		}
	}
	
	exposeBlobs(_bBlur);
	
	return nBlobs;
	
}

//--------------------------------------------------------------------------------

vector<double> margTrail::lensCorrectPoints(vector<double> inCoords, CvMat* camera_matrix, CvMat* dist_coeffs) {
	
	
	CvMat*	corrInCoords;
	CvMat*  corrOutCoords;
	
	cvInitMatHeader(corrInCoords, 2, 1, CV_64FC1, &inCoords[0]);
	cvUndistortPoints(corrInCoords, corrOutCoords, camera_matrix, dist_coeffs);
	
	// double* arOutCoords = corrOutCoords->data.db;
		
	vector<double> outCoords(corrOutCoords->data.db, corrOutCoords->data.db + sizeof(corrOutCoords->data.db)/sizeof(double));
	
	return outCoords;

}

//--------------------------------------------------------------------------------

vector<double> margTrail::persCorrectPoints(vector<double> inCoords, CvMat* translate, double w, double h) {

	vector<double> outCoords;
	
	float *data = translate->data.fl;
	
	double a = (double)data[0];
	double b = (double)data[1];
	double c = (double)data[2];
	double d = (double)data[3];
	
	double e = (double)data[4];
	double f = (double)data[5];
	double i = (double)data[6];
	double j = (double)data[7];
	
	int nInCoords = inCoords.size();
	
	for (int k = 0; k < nInCoords ; k += 2) {
		
		double * absInCoords = absCoords(inCoords[k], inCoords[k+1], w, h);
		
		double absOutX = ((a * absInCoords[k] + b * absInCoords[k+1] + c) / (i * absInCoords[k] + j * absInCoords[k+1] + 1));
		double absOutY = ((d * absInCoords[k] + e * absInCoords[k+1] + f) / (i * absInCoords[k] + j * absInCoords[k+1] + 1));
		
		double * normOutCoords= normCoords(absOutX, absOutY, w, h); 
		
		outCoords.push_back(normOutCoords[0]);
		outCoords.push_back(normOutCoords[1]);
		
		if (absInCoords != 0) delete [] absInCoords;
		if (normOutCoords != 0) delete [] normOutCoords;
		
	}
	
	return outCoords;
	
}

//--------------------------------------------------------------------------------

margBlob margTrail::lensCorrectBlob(margBlob inBlob, CvMat* camera_matrix, CvMat* dist_coeffs, double w, double h) {
	
	vector<double> coords;
	
	margBlob corrBlob;
	
	for (int i = 0; i < inBlob.nPts; i++) {
		double* normPt = normCoords((double)inBlob.pts[i].x, (double)inBlob.pts[i].y, w, h);
		coords.push_back(normPt[0]);
		coords.push_back(normPt[1]);
		if (normPt != 0) delete[] normPt;
	}
	
	coords.push_back(normCoord(inBlob.boundingRect.x, w));
	coords.push_back(normCoord(inBlob.boundingRect.y, h));
	coords.push_back(normCoord(inBlob.boundingRect.x+inBlob.boundingRect.width, w));
	coords.push_back(normCoord(inBlob.boundingRect.y+inBlob.boundingRect.height, h));
	coords.push_back(normCoord(inBlob.centroid.x, w));
	coords.push_back(normCoord(inBlob.centroid.y, h));
	
	CvMat* corrInCoords;
	CvMat* corrOutCoords;
	
	cvInitMatHeader(corrInCoords, 2, 1, CV_64FC1, &coords[0]);
	cvUndistortPoints(corrInCoords, corrOutCoords, camera_matrix, dist_coeffs);
	
	vector<double> outCoords(corrOutCoords->data.db, corrOutCoords->data.db + sizeof(corrOutCoords->data.db)/sizeof(double));
	
	for (int i = 0; i < inBlob.nPts; i+=2) {
		corrBlob.pts.push_back(absPoint(outCoords[i], outCoords[i+1], w, h));
	}
	
	corrBlob.boundingRect.x = absCoord(outCoords[inBlob.nPts + 1], w);
	corrBlob.boundingRect.y = absCoord(outCoords[inBlob.nPts + 2], h);
	corrBlob.boundingRect.width = absCoord(inBlob.nPts + 3, w) - corrBlob.boundingRect.x;
	corrBlob.boundingRect.height= absCoord(inBlob.nPts + 4, h) - corrBlob.boundingRect.y;
	
	corrBlob.centroid.x = absCoord(inBlob.nPts + 5, w);
	corrBlob.centroid.y = absCoord(inBlob.nPts + 6, w);
	
	return corrBlob;	
	
}

//--------------------------------------------------------------------------------

margBlob margTrail::persCorrectBlob(margBlob inBlob, CvMat* translate, double w, double h) {
	
	float *data = translate->data.fl;
	
	double a = (double)data[0];
	double b = (double)data[1];
	double c = (double)data[2];
	double d = (double)data[3];
	
	double e = (double)data[4];
	double f = (double)data[5];
	double i = (double)data[6];
	double j = (double)data[7];
	
	margBlob corrBlob;
	
	for (int i = 0; i < inBlob.nPts; i++) {
		int corrX = ((a * inBlob.pts[i].x + b * inBlob.pts[i].y + c) / (i * inBlob.pts[i].x + j * inBlob.pts[i].y + 1));
		int corrY = ((d * inBlob.pts[i].x + e * inBlob.pts[i].y + f) / (i * inBlob.pts[i].x + j * inBlob.pts[i].y + 1));
		corrBlob.pts.push_back(ofPoint(corrX, corrY));
	}
	
	corrBlob.boundingRect.x		= ((a * inBlob.boundingRect.x + b * inBlob.boundingRect.y + c) /
								   (i * inBlob.boundingRect.x + j * inBlob.boundingRect.y + 1));
	corrBlob.boundingRect.y		= ((d * inBlob.boundingRect.x + e * inBlob.boundingRect.y + f) /
								   (i * inBlob.boundingRect.x + j * inBlob.boundingRect.y + 1));
	corrBlob.boundingRect.width = ((a * (inBlob.boundingRect.x + inBlob.boundingRect.width)
										 + b * inBlob.boundingRect.y + c) /
								   (i * (inBlob.boundingRect.x + inBlob.boundingRect.width)
									+ j * inBlob.boundingRect.y + 1)) - corrBlob.boundingRect.x;
	corrBlob.boundingRect.height= ((d * inBlob.boundingRect.x
									+ e * (inBlob.boundingRect.y + inBlob.boundingRect.height) + f) /
								   (i * inBlob.boundingRect.x
									+ j * (inBlob.boundingRect.y + inBlob.boundingRect.height + 1))) - corrBlob.boundingRect.y;
	corrBlob.centroid.x = ((a * inBlob.centroid.x + b * inBlob.centroid.y + c) / (i * inBlob.centroid.x + j * inBlob.centroid.y + 1));
	corrBlob.centroid.y = ((d * inBlob.centroid.x + e * inBlob.centroid.y + f) / (i * inBlob.centroid.x + j * inBlob.centroid.y + 1));
	
	return corrBlob;
}

//--------------------------------------------------------------------------------

margBlob margTrail::scaleBlob(margBlob inBlob, double inW, double inH, double outW, double outH) {

	margBlob corrBlob;
	
	for (int i = 0; i < inBlob.nPts; i++) {
		int corrX = ofMap(inBlob.pts[i].x, 0, inW, 0, outW);
		int corrY = ofMap(inBlob.pts[i].y, 0, inH, 0, outH);
		corrBlob.pts.push_back(ofPoint(corrX, corrY));
	}
	
	corrBlob.boundingRect.x		= ofMap(inBlob.boundingRect.x, 0, inW, 0, outW);
	corrBlob.boundingRect.y		= ofMap(inBlob.boundingRect.y, 0, inH, 0, outH);
	corrBlob.boundingRect.width = ofMap(inBlob.boundingRect.width, 0, inW, 0, outW);
	corrBlob.boundingRect.height= ofMap(inBlob.boundingRect.height, 0, inH, 0, outH);
	corrBlob.centroid.x = ofMap(inBlob.centroid.x, 0, inW, 0, outW);
	corrBlob.centroid.y = ofMap(inBlob.centroid.y, 0, inH, 0, outH);
	
	return corrBlob;
	
}

//--------------------------------------------------------------------------------

vector<margBlob> margTrail::fullCorrectBlobs(vector<margBlob> inBlobs, CvMat* camera_matrix, CvMat* dist_coeffs,
											 CvMat* translate, double inW, double inH, double outW, double outH) {
	vector<margBlob> outBlobs;
	
	int nInBlobs = inBlobs.size();
	
	for (int i = 0; i < nInBlobs; i++) {
		outBlobs.push_back(scaleBlob(persCorrectBlob(lensCorrectBlob(inBlobs[i], camera_matrix, dist_coeffs, inW, inH),
													 translate, inW, inH), inW, inH, outW, outH));
	}
	
	return outBlobs;
	
}

//--------------------------------------------------------------------------------

vector<margBlob> margTrail::makeCorrectedBlobs(vector<double>inCoords, vector<margBlob> origBlobs, double w, double h) {
	
	vector<margBlob> corrBlobs;
	
	int nOrigBlobs = origBlobs.size();
	
	int coordIdx;
	
	for (int i = 0; i < nOrigBlobs; i++) {
		margBlob newBlob;
		for (int j = 0; j < origBlobs[i].nPts; i++) {
			double * _absCoords = absCoords(inCoords[coordIdx], inCoords[coordIdx+1], w, h);
			newBlob.pts.push_back(ofPoint(_absCoords[0], _absCoords[1]));
			if (_absCoords != 0) delete[] _absCoords;
			coordIdx += 2;
		}
		
	}
	
}


//--------------------------------------------------------------------------------

void margTrail::exposeBlobs(bool bBlur) {
	
	for (int i = 0; i < size; i++) {
		trailMap[i] = max((int)(trailMap[i] - fadeConst), 0);
	}
	
	for (int i = 0; i < finalBlobs.size(); i++) {
		finalBlobs[i].expose(trailMap, _width, exposureConst);
	}
	
	trailImg.setFromPixels(trailMap, _width, _height);
	if(bBlur) trailImg.blur(4);
	trailMap = trailImg.getPixels();
	
}

//--------------------------------------------------------------------------------

void margTrail::drawMap(float x, float y, float w, float h) {
	trailImg.draw(x, y);
}

//--------------------------------------------------------------------------------

void margTrail::drawBlend(float x, float y, float w, float h) {
	finalImg.draw(x, y);
}

//--------------------------------------------------------------------------------
void margTrail::draw( float x, float y, float w, float h ) {
	
    float scalex = 0.0f;
    float scaley = 0.0f;
    if( _width != 0 ) { scalex = w/_width; } else { scalex = 1.0f; }
    if( _height != 0 ) { scaley = h/_height; } else { scaley = 1.0f; }
	
    if(bAnchorIsPct){
        x -= anchor.x * w;
        y -= anchor.y * h;
    }else{
        x -= anchor.x;
        y -= anchor.y;
    }
	
    ofPushStyle();
	// ---------------------------- draw the bounding rectangle
	ofSetColor(0xDD00CC);
    glPushMatrix();
    glTranslatef( x, y, 0.0 );
    glScalef( scalex, scaley, 0.0 );
	
	ofNoFill();
	for( int i=0; i<(int)blobs.size(); i++ ) {
		ofRect( blobs[i].boundingRect.x, blobs[i].boundingRect.y,
			   blobs[i].boundingRect.width, blobs[i].boundingRect.height );
	}
	
	// ---------------------------- draw the blobs
	ofSetColor(0x00FFFF);
	
	for( int i=0; i<(int)blobs.size(); i++ ) {
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

//--------------------------------------------

void margTrail :: blendVideo(unsigned char* _vidPixels) {
	 //exposeBlobs();
	 for (int i = 0; i < totalTrailVal; i++) {
		int j = i*3;
		finalMap[j]  = min(((trailMap[i] * _vidPixels[j]  )/ 254 + trailMap[i]), (int)_vidPixels[j]);
		finalMap[j+1]= min(((trailMap[i] * _vidPixels[j+1])/ 254 + trailMap[i]), (int)_vidPixels[j+1]);
		finalMap[j+2]= min(((trailMap[i] * _vidPixels[j+2])/ 254 + trailMap[i]), (int)_vidPixels[j+2]);
	 }
	 finalImg.setFromPixels(finalMap, 640, 480);
}

//--------------------------------------------

vector<margBlob> margTrail :: interpolate(margBlob& blob1, margBlob& blob2) {
	
	vector<margBlob> interBlobs;
	vector< vector<float> > transPts;
	
	//  Calculate path's function
    
	int delta_x = (blob2.centroid.x-blob1.centroid.x);
	int delta_y = (blob2.centroid.y-blob1.centroid.y);
	
	double path_angle = (double)delta_y/(double)delta_x;
	double path_orig = (double)blob1.centroid.y - ((double)path_angle * (double)blob1.centroid.x);
	
	int numSteps = ofDist(blob1.centroid.x, blob1.centroid.y, blob2.centroid.x, blob2.centroid.y);
	double step_x = (double)delta_x/(double)numSteps;
	double step_y = (double)delta_y/(double)numSteps;
	
	
	//  Normalize shape coordinates
	
	vector<ofPoint> blob1_normPts;
	vector<ofPoint> blob2_normPts;
	
	for(int p = 0; p < blob1.pts.size(); p++) {
		float* nor1 = normCart(blob1.pts[p].x, blob1.pts[p].y, blob1.centroid.x, blob1.centroid.y);
		blob1_normPts.push_back(ofPoint());
		blob1_normPts[p].x = nor1[0];
		blob1_normPts[p].y = nor1[1];
		if(nor1 != 0) delete[] nor1;
	}
	
	for(int p = 0; p < blob2.pts.size(); p++) {
		float* nor2 = normCart(blob2.pts[p].x, blob2.pts[p].y, blob2.centroid.x, blob2.centroid.y);
		blob2_normPts.push_back(ofPoint());
		blob2_normPts[p].x = nor2[0];
		blob2_normPts[p].y = nor2[1];
		if(nor2 != 0) delete[] nor2;
	}
	
	//  Compare number of vertices and trigger the appropriate methods in each case
	
	// If both shapes have the same number of vertices
	
	if(blob1.pts.size() == blob2.pts.size()) {
		
		//  Set transformation paths
		
		for(int p = 0; p < blob1.pts.size(); p++) {
			int delta_pt_x = round(blob2_normPts[p].x-blob1_normPts[p].x);
			int delta_pt_y = round(blob2_normPts[p].y-blob1_normPts[p].y);
			float step_pt_x= (float)((double)delta_pt_x/(double)numSteps);
			float step_pt_y= (float)((double)delta_pt_y/(double)numSteps);
			float curPts[] = {blob1_normPts[p].x, blob1_normPts[p].y, step_pt_x, step_pt_y};
			vector<float> newPt (curPts, curPts + sizeof(curPts) / sizeof(float));
			transPts.push_back(newPt);
			newPt.clear();
		}
	}
	
	else if (blob1.pts.size() > blob2.pts.size()) {
	
		int diff = blob1.pts.size() - blob2.pts.size();
		int aver_diff = 0;
		int amt_aver  = 1;
		
		if (diff <= blob2.pts.size()) {
			aver_diff = floor((float)blob2.pts.size() / (float)diff);
		}
		else {
			aver_diff = 1;
			amt_aver = ceil((float)diff / (float)blob2.pts.size());      
		}
		
		int addedExtraPts = 0;
		
		for(int p = 0; p < blob2.pts.size(); p++) {      
			
			int curSh1Pt = p + addedExtraPts;
			
			int curSh2Pt = p;
			if(curSh2Pt >= blob2.pts.size()) { curSh2Pt -= blob2.pts.size(); }
			
			int prevSh2Pt = curSh2Pt - 1;
			if(prevSh2Pt < 0) { prevSh2Pt += blob2.pts.size(); }
			
			if((p+1) % aver_diff == 0 && addedExtraPts < diff) {
				for(int amt = 0; amt < amt_aver; amt ++) {
					if (addedExtraPts < diff) {
						int final_pt_x = round(((blob2_normPts[curSh2Pt].x - blob2_normPts[prevSh2Pt].x) / 2.0) + blob2_normPts[prevSh2Pt].x);  //Stablish final coord for extra point in shape 1
						int final_pt_y = round(((blob2_normPts[curSh2Pt].y - blob2_normPts[prevSh2Pt].y) / 2.0) + blob2_normPts[prevSh2Pt].y);  //as the middle of one of the sides of shape 2
						int delta_pt_x = round(final_pt_x - blob1_normPts[curSh1Pt].x);
						int delta_pt_y = round(final_pt_y - blob1_normPts[curSh1Pt].y);
						float step_pt_x = (float)((double)delta_pt_x/(double)numSteps);
						float step_pt_y = (float)((double)delta_pt_y/(double)numSteps);
						float curPts[] = {blob1_normPts[curSh1Pt].x, blob1_normPts[curSh1Pt].y, step_pt_x, step_pt_y};
						vector<float> newPt (curPts, curPts + sizeof(curPts) / sizeof(float));
						transPts.push_back(newPt);
						newPt.clear();
						addedExtraPts ++;
						curSh1Pt ++;
					}
				}
			}
			
			if (curSh1Pt < blob1.pts.size()) {
				int delta_pt_x = round(blob2_normPts[curSh2Pt].x-blob1_normPts[curSh1Pt].x);
				int delta_pt_y = round(blob2_normPts[curSh2Pt].y-blob1_normPts[curSh1Pt].y);
				float step_pt_x= (float)((double)delta_pt_x/(double)numSteps);
				float step_pt_y= (float)((double)delta_pt_y/(double)numSteps);
				float curPts[] = {blob1_normPts[curSh1Pt].x, blob1_normPts[curSh1Pt].y, step_pt_x, step_pt_y};
				vector<float> newPt (curPts, curPts + sizeof(curPts) / sizeof(float));
				transPts.push_back(newPt);
				newPt.clear();
			}
		}  
	}
	
	else if (blob1.pts.size() < blob2.pts.size()) {
		
		int diff = blob2.pts.size() - blob1.pts.size();
		int aver_diff = 0;
		int amt_aver = 1;
		
		if (diff <= blob1.pts.size()) {
			aver_diff = floor((float)blob1.pts.size() / (float)diff);      
		}
		else {
			aver_diff = 1;
			amt_aver = ceil((float)diff / (float)blob1.pts.size());  
		}
		
		int removedExtraPts = 0;
		
		for(int p = 0; p < blob1.pts.size(); p++) { 
			
			int curSh2Pt = p + removedExtraPts;
			if(curSh2Pt >= blob2.pts.size()) { curSh2Pt -= blob2.pts.size();}
			
			int curSh1Pt = p;
			
			int prevSh1Pt = curSh1Pt - 1;
			if(prevSh1Pt < 0) { prevSh1Pt += blob1.pts.size();}
			
			if((p+1) % aver_diff == 0 && removedExtraPts < diff) { 
				for(int amt = 0; amt < amt_aver; amt++) {
					if (removedExtraPts < diff) {
						int init_pt_x = round((blob1_normPts[curSh1Pt].x - blob1_normPts[prevSh1Pt].x)/2.0 + blob1_normPts[prevSh1Pt].x);  //Stablish initial coord for extra point of shape 2
						int init_pt_y = round((blob1_normPts[curSh1Pt].y - blob1_normPts[prevSh1Pt].y)/2.0 + blob1_normPts[prevSh1Pt].y);   //as the middle of one of the sides of shape 1
						int delta_pt_x = round(blob2_normPts[curSh2Pt].x-init_pt_x);
						int delta_pt_y = round(blob2_normPts[curSh2Pt].y-init_pt_y);
						float step_pt_x= (float)((double)delta_pt_x/(double)numSteps);
						float step_pt_y= (float)((double)delta_pt_y/(double)numSteps);
						float curPts[] = {init_pt_x, init_pt_y, step_pt_x, step_pt_y};
						vector<float> newPt (curPts, curPts + sizeof(curPts) / sizeof(float));
						transPts.push_back(newPt);
						newPt.clear();
						removedExtraPts ++;
						curSh2Pt++;
						if(curSh2Pt >= blob2.pts.size()) { curSh2Pt -= blob2.pts.size();}
					}
				}
			}
			if (curSh2Pt < blob2.pts.size()) {
				int delta_pt_x = round(blob2_normPts[curSh2Pt].x-blob1_normPts[curSh1Pt].x);
				int delta_pt_y = round(blob2_normPts[curSh2Pt].y-blob1_normPts[curSh1Pt].y);
				float step_pt_x= (float)((double)delta_pt_x/(double)numSteps);
				float step_pt_y= (float)((double)delta_pt_y/(double)numSteps);
				float curPts[] = {blob1_normPts[curSh1Pt].x, blob1_normPts[curSh1Pt].y, step_pt_x, step_pt_y};
				vector<float> newPt (curPts, curPts + sizeof(curPts) / sizeof(float));
				transPts.push_back(newPt);
				newPt.clear();
			}      
		}
	}
	
	float exposureLev = 1;
	
	if (numSteps > 0) { exposureLev = 1 / numSteps; }
	
	blob1.exposure = exposureLev;
	blob2.exposure = exposureLev;
	
	interBlobs.push_back(blob1);
	
	for(int i = 0; i < numSteps; i++) {
		margBlob newBlob;
		newBlob.centroid.x=round(blob1.centroid.x+(i*(float)step_x));
		newBlob.centroid.y=round(blob1.centroid.y+(i*(float)step_y));
		newBlob.exposure = exposureLev;
		float minX = -1;
		float minY = -1;
		float maxX = -1;
		float maxY = -1;
		for(int p = 0; p < transPts.size(); p++) {
			float x = newBlob.centroid.x + transPts[p][0] + (i * transPts[p][2]);
			float y = newBlob.centroid.y + transPts[p][1] + (i * transPts[p][3]);
			if (x < minX || minX == -1) {
				minX = x;
			}
			if (x > maxX || maxX == -1) {
				maxX = x;
			}
			if (y < minY || minY == -1) {
				minY = y;
			}
			if (y > maxY || maxY == -1) {
				maxY = y;
			}
			newBlob.pts.push_back(ofPoint(x, y));
		}
		newBlob.boundingRect.x = minX;
		newBlob.boundingRect.y = minY;
		newBlob.boundingRect.width = (maxX - minX);
		newBlob.boundingRect.height = (maxY - minY);
		interBlobs.push_back(newBlob);
	}
	
	interBlobs.push_back(blob2);
	
	blob1_normPts.clear();
	blob2_normPts.clear();
	transPts.clear();
	
	return interBlobs;

}

// ----------------------------------------------------------

void margTrail :: drawInterp (int x, int y, int w, int h) {


	float scalex = 0.0f;
    float scaley = 0.0f;
    if( _width != 0 ) { scalex = w/_width; } else { scalex = 1.0f; }
    if( _height != 0 ) { scaley = h/_height; } else { scaley = 1.0f; }
	
    if(bAnchorIsPct){
        x -= anchor.x * w;
        y -= anchor.y * h;
    }else{
        x -= anchor.x;
        y -= anchor.y;
    }
	
    ofPushStyle();
	
	// ---------------------------- draw the bounding rectangle
	
	ofSetColor(0xDD00CC);
    glPushMatrix();
    glTranslatef( x, y, 0.0 );
    glScalef( scalex, scaley, 0.0 );
	
	ofNoFill();
	for( int i=0; i<(int)finalBlobs.size(); i++ ) {
		ofRect( finalBlobs[i].boundingRect.x, finalBlobs[i].boundingRect.y,
			   finalBlobs[i].boundingRect.width, finalBlobs[i].boundingRect.height );
	}
	
	// ---------------------------- draw the blobs
	
	ofSetColor(0x00FFFF);
	
	
	for( int i=0; i<finalBlobs.size(); i++ ) {
		ofNoFill();
		ofBeginShape();
		for( int j=0; j<finalBlobs[i].pts.size(); j++ ) {
			ofVertex( finalBlobs[i].pts[j].x, finalBlobs[i].pts[j].y );
		}
		ofEndShape();
	}
	glPopMatrix();
	ofPopStyle();
}

// --------------------------------------

int* margTrail::findPairs(vector<margBlob> current, vector<margBlob> previous) {
	
	int nCurrBlobs = current.size();
	int nPrevBlobs = previous.size();
	
	int nMatches = 0;
	
	int* final_matches = new (nothrow) int [nCurrBlobs];
	
	for (int i = 0; i < nCurrBlobs; i++) {
		final_matches[i] = -1;
	}
	
	vector< vector<int> > possible_matches;
	
	for (int i = 0; i < nCurrBlobs; i++) {
		for (int j = 0; j < nPrevBlobs; j++) {
			float dist = ofDist(current[i].centroid.x, current[i].centroid.y, previous[j].centroid.x, previous[j].centroid.y);
			float areaDiff = ((float)(current[i].area - previous[j].area) / (float)current[i].area) * (float)100;
			if (areaDiff<0) { areaDiff = areaDiff * (-1); };
			if ( dist < maxDist && areaDiff < maxAreaDiff) {
				int unfitness = round(10000 / (0.75 * dist + 0.25 *	areaDiff));
				vector<int> currPossib;
				currPossib.push_back(i);
				currPossib.push_back(j);
				currPossib.push_back(unfitness);
				possible_matches.push_back(currPossib);
				nMatches++;
				currPossib.clear();
			}
		}
	}
	sort(possible_matches.begin(), possible_matches.end(), sort_fitness_compare);
	reverse(possible_matches.begin(), possible_matches.end());
	
	bool finished = false;
	
	while (possible_matches.size() > 0) {
		int used_currblob = possible_matches[0][0];
		int used_prevblob = possible_matches[0][1];
		final_matches[used_currblob] = used_prevblob;
		
		vector<int> elements_to_erase;
		
		for (int j = 0; j < nMatches; j++) {
			if (possible_matches[j][0] == used_currblob ||
				possible_matches[j][1] == used_prevblob) {
				elements_to_erase.push_back(j);
			}
		}
		
		for (int i = 0; i < elements_to_erase.size(); i++) {
			possible_matches.erase(possible_matches.begin() + elements_to_erase[i]);
		}
		
		
		nMatches -= elements_to_erase.size();
		
		elements_to_erase.clear();
	}


			
	
	possible_matches.clear();
	current.clear();
	previous.clear();
	
	return final_matches;
}

//--------------------------------------------------------------------------------
void margTrail::setAnchorPercent( float xPct, float yPct ){
    anchor.x = xPct;
    anchor.y = yPct;
    bAnchorIsPct = true;
}

//--------------------------------------------------------------------------------
void margTrail::setAnchorPoint( int x, int y ){
    anchor.x = x;
    anchor.y = y;
    bAnchorIsPct = false;
}

//--------------------------------------------------------------------------------
void margTrail::resetAnchor(){
    anchor.set(0,0);
    bAnchorIsPct = false;
}


