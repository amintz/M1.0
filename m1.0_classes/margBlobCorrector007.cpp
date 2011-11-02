/*
 *  margBlobCorrector.cpp
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margBlobCorrector007.h"


void	margBlobCorrector::init() {
	camera_matrix = cvCreateMat(3, 3, CV_32FC1);
	dist_coeffs	  = cvCreateMat(1, 4, CV_32FC1);
	
	float cam_mat[] = { 0, 0, 0,
						0, 0, 0,
						0, 0, 1 };
	
	cvInitMatHeader(camera_matrix, 3, 3, CV_32FC1, cam_mat);
	
	float dist_c[] = {0, 0, 0, 0};
	
	cvInitMatHeader(dist_coeffs, 1, 4, CV_32FC1, dist_c);
	
	persQuadPoints = new ofVec2f[4];
}

// ------------------------------------------------------------

void	margBlobCorrector::setInDimensions(double _inW, double _inH) {
	
	inW = _inW;
	inH = _inH;
	
}

// ------------------------------------------------------------

void	margBlobCorrector::setOutDimensions(double _outW, double _outH) {
	
	outW = _outW;
	outH = _outH;
	
}

// ------------------------------------------------------------

void	margBlobCorrector::setPersCorrection(CvMat* _translate) {
	translate = _translate;
	data = translate->data.fl;
	calculatePersUndistBounds();
}

// ------------------------------------------------------------

void	margBlobCorrector::setCameraMatrix(float _fX, float _fY, float _cX, float _cY){
	float* _camera_matrix = new float[9];
		
	_camera_matrix[0] = _fX;
	_camera_matrix[1] = 0;
	_camera_matrix[2] = _cX;
	_camera_matrix[3] = 0;
	_camera_matrix[4] = _fY;
	_camera_matrix[5] = _cY;
	_camera_matrix[6] = 0;
	_camera_matrix[7] = 0;
	_camera_matrix[8] = 1;
	
	fX = _fX;
	cX = _cX;
	fY = _fY;
	cY = _cY;
	
	ifX= 1.0/fX;
	ifY= 1.0/fY;
	
	cvInitMatHeader(camera_matrix, 3, 3, CV_32FC1, _camera_matrix);
	
	calculateLensUndistBounds();
	calculatePersUndistBounds();
	
	delete[] _camera_matrix;
}

// ------------------------------------------------------------

void	margBlobCorrector::setDistCoeffs(float _rX, float _rY, float _tX, float _tY){
	
	float* _dist_coeffs = new float[4];
	
	_dist_coeffs[0] = _rX;
	_dist_coeffs[1] = _rY;
	_dist_coeffs[2] = _tX;
	_dist_coeffs[3] = _tY;
	
	rX = _rX;
	rY = _rY;
	tX = _tX;
	tY = _tY;
	
	cvInitMatHeader(dist_coeffs, 1, 4, CV_32FC1, _dist_coeffs);
	
	calculateLensUndistBounds();
	calculatePersUndistBounds();
	
	delete[] _dist_coeffs;
	
}

// ------------------------------------------------------------

void	margBlobCorrector::loadMats(string xmlFile) {
	
	ofxXmlSettings xml;
	
	float* _camera_matrix = new float[9];
	float* _dist_coeffs   = new float[4];
	
	if (xml.loadFile(xmlFile)) {
		if (xml.pushTag("cam_intr", 0)) {
			_camera_matrix[0] = xml.getValue("intr0", 0.0, 0);
			_camera_matrix[1] = xml.getValue("intr1", 0.0, 0);
			_camera_matrix[2] = xml.getValue("intr2", 0.0, 0);
			_camera_matrix[3] = xml.getValue("intr3", 0.0, 0);
			_camera_matrix[4] = xml.getValue("intr4", 0.0, 0);
			_camera_matrix[5] = xml.getValue("intr5", 0.0, 0);
			_camera_matrix[6] = xml.getValue("intr6", 0.0, 0);
			_camera_matrix[7] = xml.getValue("intr7", 0.0, 0);
			_camera_matrix[8] = xml.getValue("intr8", 0.0, 0);
			xml.popTag();
		}
		if (xml.pushTag("dist_coeffs", 0)) {
			_dist_coeffs[0] = xml.getValue("dist0", 0.0, 0);
			_dist_coeffs[1] = xml.getValue("dist1", 0.0, 0);
			_dist_coeffs[2] = xml.getValue("dist2", 0.0, 0);
			_dist_coeffs[3] = xml.getValue("dist3", 0.0, 0);
			xml.popTag();
		}
	}
	
	fX = (double)_camera_matrix[0];
	cX = (double)_camera_matrix[2];
	fY = (double)_camera_matrix[4];
	cY = (double)_camera_matrix[5];
	
	ifX= 1.0/fX;
	ifY= 1.0/fY;
	
	rX = (double)_dist_coeffs[0];
	rY = (double)_dist_coeffs[1];
	tX = (double)_dist_coeffs[2];
	tY = (double)_dist_coeffs[3];
	
	cvInitMatHeader(camera_matrix, 3, 3, CV_32FC1, _camera_matrix);
	
	float* cm = camera_matrix->data.fl;
	
	for (int i = 0; i < 9; i++) {
		cout << "cm["<< i << "] = "<<cm[i]<<endl;
	}
	
	cvInitMatHeader(dist_coeffs, 1, 4, CV_32FC1, _dist_coeffs);
	
	float* dc = dist_coeffs->data.fl;
	
	for (int i = 0; i < 4; i++) {
		cout << "dc[" << i << "] = "<<dc[i]<<endl;
	}
	
	calculateLensUndistBounds();
	calculatePersUndistBounds();
	
	delete [] _camera_matrix;
	delete [] _dist_coeffs;
	
	xml.clear();
}

// ------------------------------------------------------------

void	margBlobCorrector::setAll(CvMat* _camera_matrix, CvMat* _dist_coeffs, CvMat* _translate,
								  double _inW, double _inH, double _outW, double _outH) {
	
	camera_matrix = _camera_matrix;
	dist_coeffs	  = _dist_coeffs;
	translate	  = _translate;
	data		  =  translate->data.fl;
	inW			  = _inW;
	inH			  = _inH;
	outW		  = _outW;
	outH		  = _outH;
	
	calculateLensUndistBounds();
	calculatePersUndistBounds();
	
}

// ------------------------------------------------------------

void	margBlobCorrector::setMode(bool _bCorrectLens, bool _bCorrectPerspective, bool _bScale) {
	
	bCorrectLens = _bCorrectLens;
	bCorrectPerspective = _bCorrectPerspective;
	bScale = _bScale;
	
}

// ------------------------------------------------------------

ofPoint margBlobCorrector::undistortPoint(float _x, float _y) {
	
	int j;
	double x = _x,
		   y = _y;  
	double k1 = rX,  
		   k2 = rY,
		   k3 = 0,
		   p1 = tX,  
		   p2 = tY;
	
	double x0 = x = (x - inW/2) * ifX;
	double y0 = y = (y - inH/2) * ifY;
	
	for (j = 0 ; j < 5; j++) {
		
		double r2 = x*x + y*y;
		double icdist = 1./(1 + ((k3 * r2 + k2) * r2 + k1) * r2);
		double deltaX = 2*p1*x*y + p2*(r2 + 2*x*x);
		double deltaY = p1 *(r2 + 2*y*y) + 2*p2*x*y;
		
		x = ((x0 - deltaX)*icdist);
		y = ((y0 - deltaY)*icdist);

	}
	
	x = x * fX + cX;
	y = y * fY + cY;
	
	return ofPoint(x,y);
	
	//return bestIfOutOfLensBounds(ofPoint(x,y));
	
}

// ------------------------------------------------------------

margBlob margBlobCorrector::lensCorrectBlob(margBlob& inBlob) {
		
	margBlob corrBlob;									// Blob to be returned
	
	float minX = inW;
	float minY = inH;
	float maxX = 0;
	float maxY = 0;
	
	// Adapted UndistortPoint Method
	
	for (int i = 0; i < inBlob.nPts; i++) {
		corrBlob.pts.push_back(undistortPoint(inBlob.pts[i].x, inBlob.pts[i].y));
		minX = MIN(minX, corrBlob.pts[i].x);
		minY = MIN(minY, corrBlob.pts[i].y);
		maxX = MAX(maxX, corrBlob.pts[i].x);
		maxY = MAX(maxY, corrBlob.pts[i].y);
	}
	
	corrBlob.boundingRect.x = minX;
	corrBlob.boundingRect.y = minY;
	corrBlob.boundingRect.width = maxX - corrBlob.boundingRect.x;
	corrBlob.boundingRect.height= maxY - corrBlob.boundingRect.y;
	
	corrBlob.centroid = undistortPoint(inBlob.centroid.x, inBlob.centroid.y);
	
	corrBlob.nPts		= inBlob.nPts;
	corrBlob.exposure	= inBlob.exposure;
	corrBlob.area		= inBlob.area;
	
	return corrBlob;	
	
}

//--------------------------------------------------------------------------------

margBlob margBlobCorrector::persCorrectBlob(margBlob& inBlob) {
	
	
	margBlob corrBlob;
	
	float minX = inW;
	float minY = inH;
	float maxX = 0;
	float maxY = 0;
	
	for (int i = 0; i < inBlob.nPts; i++) {
		corrBlob.pts.push_back(perspTransform(inBlob.pts[i], true));
		minX = MIN(minX, corrBlob.pts[i].x);
		minY = MIN(minY, corrBlob.pts[i].y);
		maxX = MAX(maxX, corrBlob.pts[i].x);
		maxY = MAX(maxY, corrBlob.pts[i].y);
	}
	
	corrBlob.boundingRect.x = minX;
	corrBlob.boundingRect.y = minY;
	corrBlob.boundingRect.width = maxX - corrBlob.boundingRect.x;
	corrBlob.boundingRect.height= maxY - corrBlob.boundingRect.y;
	
	corrBlob.centroid = perspTransform(inBlob.centroid, true);
	
	corrBlob.nPts	  = inBlob.nPts;
	corrBlob.exposure = inBlob.exposure;
	corrBlob.area	  = inBlob.area;
	
	return corrBlob;
}

//--------------------------------------------------------------------------------

margBlob margBlobCorrector::scaleBlob(margBlob& inBlob) {
	
	margBlob corrBlob;
	
	float minX = outW;
	float minY = outH;
	float maxX = 0;
	float maxY = 0;
	
	for (int i = 0; i < inBlob.nPts; i++) {
		float corrX = ofMap(inBlob.pts[i].x, 0, inW, 0, outW);
		float corrY = ofMap(inBlob.pts[i].y, 0, inH, 0, outH);
		minX = MIN(minX, corrX);
		minY = MIN(minY, corrY);
		maxX = MAX(maxX, corrX);
		maxY = MAX(maxY, corrY);
		corrBlob.pts.push_back(ofPoint(corrX, corrY));
	}
	
	corrBlob.boundingRect.x		= minX;
	corrBlob.boundingRect.y		= minY;
	corrBlob.boundingRect.width = maxX - minX;
	corrBlob.boundingRect.height= maxY - minY;
	
	corrBlob.centroid.x = ofMap(inBlob.centroid.x, 0, inW, 0, outW);
	corrBlob.centroid.y = ofMap(inBlob.centroid.y, 0, inH, 0, outH);
	
	corrBlob.nPts	  = inBlob.nPts;
	corrBlob.exposure = inBlob.exposure;
	corrBlob.area	  = inBlob.area;
	
	return corrBlob;
	
}


//--------------------------------------------------------------------------------

vector<margBlob>& margBlobCorrector::fullCorrectBlobs(vector<margBlob>& inBlobs) {
	feedBlobs(inBlobs);
	return correctBlobs;
}

//--------------------------------------------------------------------------------

void margBlobCorrector::feedBlobs(vector<margBlob>& inBlobs) {
	correctBlobs.clear();
	int nInBlobs = inBlobs.size();
	correctBlobs.reserve(nInBlobs * sizeof(margBlob));
	
	margBlob correctBlob;
	
	for (int i = 0; i < nInBlobs; i++) {
		correctBlob = inBlobs[i];
		if (bCorrectLens) correctBlob = lensCorrectBlob(correctBlob);
		if (bCorrectPerspective) correctBlob = persCorrectBlob(correctBlob);
		if (bScale) correctBlob = scaleBlob(correctBlob);
		correctBlobs.push_back(correctBlob);
	}
}

// ----------------------------------------------------------

vector<margBlob>& margBlobCorrector::getCorrectBlobs() {
	return correctBlobs;
}

// ----------------------------------------------------------

void	margBlobCorrector::drawBlobs(int x, int y, int w, int h) {
	
	float scalex = 0.0f;
    float scaley = 0.0f;
    if( outW != 0 ) { scalex = w/outW; } else { scalex = 1.0f; }
    if( outH != 0 ) { scaley = h/outH; } else { scaley = 1.0f; }
	
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
	for( int i=0; i<(int)correctBlobs.size(); i++ ) {
		ofRect( correctBlobs[i].boundingRect.x, correctBlobs[i].boundingRect.y,
			   correctBlobs[i].boundingRect.width, correctBlobs[i].boundingRect.height );
	}
	
	// ---------------------------- draw the blobs
	
	ofSetHexColor(0x00FFFF);
	
	
	for( int i=0; i<correctBlobs.size(); i++ ) {
		ofNoFill();
		ofBeginShape();
		for( int j=0; j<correctBlobs[i].nPts; j++ ) {
			ofVertex( correctBlobs[i].pts[j].x, correctBlobs[i].pts[j].y );
		}
		ofEndShape();
	}
	
	glPopMatrix();
	ofPopStyle();
	
}

void margBlobCorrector::drawUndistortBounds(int x, int y, int w, int h) {
	
	float scalex = 0.0f;
    float scaley = 0.0f;
    if( outW != 0 ) { scalex = w/outW; } else { scalex = 1.0f; }
    if( outH != 0 ) { scaley = h/outH; } else { scaley = 1.0f; }
	
    if(bAnchorIsPct){
        x -= anchor.x * w;
        y -= anchor.y * h;
    }else{
        x -= anchor.x;
        y -= anchor.y;
    }
	
	
	ofPushStyle();
	glPushMatrix();
	glTranslatef( x, y, 0.0 );
	glScalef( scalex, scaley, 0.0 );
	
	// ---------------------------- draw lens undistort bounds
	
	margBlob scaledLens = scaleBlob(lensUndistBounds);
	
	ofSetHexColor(0xFF0000);
	
	ofNoFill();
	ofBeginShape();
	for(int j = 0; j<lensUndistBounds.nPts; j++) {
		ofVertex( scaledLens.pts[j].x, scaledLens.pts[j].y);
	}
	ofEndShape();
	
	ofSetHexColor(0x00FF00);
	ofRect(scaledLens.boundingRect.x, scaledLens.boundingRect.y, scaledLens.boundingRect.width, scaledLens.boundingRect.height);
	
	// ---------------------------- draw perspective undistort bounds
	
	margBlob scaledPers = scaleBlob(persUndistBounds);
	
	ofSetHexColor(0xFF00FF);
	
	ofNoFill();
	ofBeginShape();
	for(int j = 0; j<persUndistBounds.nPts; j++) {
		ofVertex( scaledPers.pts[j].x, scaledPers.pts[j].y);
	}
	ofEndShape();
	
	ofSetHexColor(0x0000FF);
	ofRect(scaledPers.boundingRect.x, scaledPers.boundingRect.y, scaledPers.boundingRect.width, scaledPers.boundingRect.height);
	
	glPopMatrix();
	ofPopStyle();
	
}


// ----------------------------------------------------------

CvMat* margBlobCorrector::getPerspMat() {
	
	return translate;
	
}

// ----------------------------------------------------------

CvMat* margBlobCorrector::getCamMat() {

	return camera_matrix;
	
}

// ----------------------------------------------------------

CvMat* margBlobCorrector::getDistMat() {
	
	return dist_coeffs;
	
}

// ----------------------------------------------------------

void margBlobCorrector::calculateLensUndistBounds() {
	
	lensUndistBounds.pts.clear();
	
	int nPts = 0;
	
	int minX = inW;
	int maxX = 0;
	int minY = inH;
	int maxY = 0;
	
	int u, v, i;
	float x0 = (float)inW/2.0f,
		  y0 = float(inH)/2.0f;
	float k1 = rX,  
		  k2 = rY,
	      k3 = 0,
	      p1 = tX,  
	      p2 = tY;
	
	v = 0;
	
	for( u = 0; u < inW; u++ )	{
		float y = (v - cY)*ifY, y2 = y*y;
		float x = (u - cX)*ifX,
		x2 = x*x,
		r2 = x2 + y2,
		_2xy = 2*x*y;
		float kr = 1 + ((k3*r2 + k2)*r2 + k1)*r2;
		float _x = fX*(x*kr + p1*_2xy + p2*(r2 + 2*x2)) + x0;
		float _y = fY*(y*kr + p1*(r2 + 2*y2) + p2*_2xy) + y0;
		int   ix = cvFloor(_x),
		iy = cvFloor(_y);
		
		lensUndistBounds.pts.push_back(ofPoint(ix, iy));
		minX = ix < minX? ix : minX;
		maxX = ix > maxX? ix : maxX;
		minY = iy < minY? iy : minY;
		maxY = iy > maxY? iy : maxY;
		nPts++;
	}
	
	u = inW-1;
	
	for (v = 0; v < inH; v++) {
		float y = (v - cY)*ifY, y2 = y*y;
		float x = (u - cX)*ifX,
		x2 = x*x,
		r2 = x2 + y2,
		_2xy = 2*x*y;
		float kr = 1 + ((k3*r2 + k2)*r2 + k1)*r2;
		float _x = fX*(x*kr + p1*_2xy + p2*(r2 + 2*x2)) + x0;
		float _y = fY*(y*kr + p1*(r2 + 2*y2) + p2*_2xy) + y0;
		int   ix = cvFloor(_x),
		iy = cvFloor(_y);
		
		lensUndistBounds.pts.push_back(ofPoint(ix, iy));
		minX = ix < minX? ix : minX;
		maxX = ix > maxX? ix : maxX;
		minY = iy < minY? iy : minY;
		maxY = iy > maxY? iy : maxY;
		nPts++;
	}
	
	v = inH-1;
	
	for( u = inW-1; u > -1; u-- ) {
		float y = (v - cY)*ifY, y2 = y*y;
		float x = (u - cX)*ifX,
		x2 = x*x,
		r2 = x2 + y2,
		_2xy = 2*x*y;
		float kr = 1 + ((k3*r2 + k2)*r2 + k1)*r2;
		float _x = fX*(x*kr + p1*_2xy + p2*(r2 + 2*x2)) + x0;
		float _y = fY*(y*kr + p1*(r2 + 2*y2) + p2*_2xy) + y0;
		int   ix = cvFloor(_x),
		iy = cvFloor(_y);
		
		lensUndistBounds.pts.push_back(ofPoint(ix, iy));
		minX = ix < minX? ix : minX;
		maxX = ix > maxX? ix : maxX;
		minY = iy < minY? iy : minY;
		maxY = iy > maxY? iy : maxY;
		nPts++;
	}
	
	u = 0;
	
    for( v = inH-1; v > -1; v--) {
        float y = (v - cY)*ifY, y2 = y*y;
		float x = (u - cX)*ifX,
		x2 = x*x,
		r2 = x2 + y2,
		_2xy = 2*x*y;
		float kr = 1 + ((k3*r2 + k2)*r2 + k1)*r2;
		float _x = fX*(x*kr + p1*_2xy + p2*(r2 + 2*x2)) + x0;
		float _y = fY*(y*kr + p1*(r2 + 2*y2) + p2*_2xy) + y0;
		int   ix = cvFloor(_x),
		iy = cvFloor(_y);
		
		lensUndistBounds.pts.push_back(ofPoint(ix, iy));
		minX = ix < minX? ix : minX;
		maxX = ix > maxX? ix : maxX;
		minY = iy < minY? iy : minY;
		maxY = iy > maxY? iy : maxY;
		nPts++;
    }
	
	lensUndistBounds.nPts = nPts;
	
	lensUndistBounds.boundingRect.x = minX;
	lensUndistBounds.boundingRect.y = minY;
	lensUndistBounds.boundingRect.width = maxX - minX;
	lensUndistBounds.boundingRect.height = maxY - minY;
	
	//lensUndistBounds = scaleBlob(lensUndistBounds);
}

// -----------------------------------------------------------------

// -----------------------------------------------------------------

ofPoint	margBlobCorrector::bestIfOutOfLensBounds(ofPoint inPt) {
	int j = lensUndistBounds.nPts-1;
	int minDist = 300000;
	int minDistIdx = -1;
	bool bIsInBounds = false;
	
	for (int i=0; i < lensUndistBounds.nPts; i++) {
		int curDist = ofDist(inPt.x, inPt.y, lensUndistBounds.pts[i].x, lensUndistBounds.pts[i].y);
		if(curDist < minDist) {
			minDist = curDist;
			minDistIdx = i;
		}
		if((lensUndistBounds.pts[i].y < (double)inPt.y && lensUndistBounds.pts[j].y >= (double)inPt.y ||
			lensUndistBounds.pts[j].y < (double)inPt.y && lensUndistBounds.pts[i].y >= (double)inPt.y)
			&& (lensUndistBounds.pts[i].x<=inPt.x || lensUndistBounds.pts[j].x<=inPt.x)){
			if((lensUndistBounds.pts[i].x+(inPt.y-lensUndistBounds.pts[i].y)/
				(lensUndistBounds.pts[j].y-lensUndistBounds.pts[i].y)*(lensUndistBounds.pts[j].x-lensUndistBounds.pts[i].x)<inPt.x)) {
				bIsInBounds = !bIsInBounds;
			}
		}
		j = i;
	}
	if(bIsInBounds) return inPt;
	else return lensUndistBounds.pts[minDist];
}

// ----------------------------------------------------------------


void margBlobCorrector::setPersQuadPoints(ofVec2f* quadPoints) {
	memcpy(persQuadPoints, quadPoints, 8*sizeof(int));
	for (int i = 0; i < 4; i++) {
		cout << "QuadPoints[" << i << "].x = " << quadPoints[i].x << endl;
		cout << "QuadPoints[" << i << "].y = " << quadPoints[i].y << endl;
	}
	calculatePersUndistBounds();
}

// ----------------------------------------------------------------

void margBlobCorrector::calculatePersUndistBounds() {
	
	persUndistBounds.pts.clear();
	
	int minX = inW;
	int maxX = 0;
	int minY = inH;
	int maxY = 0;
	
	for (int v = 0; v < 4; v++) {
		
		int nextV = v + 1 < 4 ? v + 1 : 0;
		
		int d = round(ofDist(persQuadPoints[v].x, persQuadPoints[v].y, persQuadPoints[nextV].x, persQuadPoints[nextV].y));
		float x_st = (persQuadPoints[nextV].x - persQuadPoints[v].x)/d;
		float y_st = (persQuadPoints[nextV].y - persQuadPoints[v].y)/d;
		for (int i = 0; i < d; i++) {
			float x = ceil(persQuadPoints[v].x + (x_st * i));
			float y = ceil(persQuadPoints[v].y + (y_st * i));
			persUndistBounds.pts.push_back(ofVec2f( x, y ));
			minX = x < minX ? x : minX;
			maxX = x > maxX ? x : maxX;
			minY = y < minY ? y : minY;
			maxY = y > maxY ? y : maxY;
		}
	
	}
	
	persUndistBounds.nPts = persUndistBounds.pts.size();
	
	persUndistBounds.boundingRect.x = minX;
	persUndistBounds.boundingRect.y = minY;
	persUndistBounds.boundingRect.width = maxX - minX;
	persUndistBounds.boundingRect.height= maxY - minY;
	
	persUndistBounds = lensCorrectBlob(persUndistBounds);
	
}
