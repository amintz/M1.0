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
	
//	int					id;
//	
//	ofPoint				deltaLoc;
//	ofPoint				deltaLocTotal;
//	ofPoint				predictedPos;
//	float				deltaArea;
//	
//	bool				markedForDeletion;
//	int					framesLeft;
//	vector<float>		error;
//	vector<int>			closest;
	
	float				exposure;
	
	margBlob() {
//		id			= -1;
		area 		= 0.0f;
//		deltaArea	= 0.0f;
		length 		= 0.0f;
		hole 		= false;
//		markedForDeletion = false;
//		framesLeft	= 0;
		nPts        = 0;
		//pts.reserve(300*sizeof(ofPoint));
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
//        id = -1;
//        deltaArea = 0.0f;
//        markedForDeletion = false;
//        framesLeft = 0;
    }
	
//	int getLowestError() {
//        int best=-1;
//        float best_v=99999.0f;
//        for( unsigned int i=0; i<error.size(); i++ ) {
//            if( error[i] < best_v ) {
//                best = i;
//                best_v = error[i];
//            }
//        }
//        return best;
//    }
	
	
	// DEPRECATE THIS!
	
/*	void expose(unsigned char* trailMap, int mapW, int constant) {
		
		int* nodeX = new int[nPts];
		
		int nodes, pixY, pixX, i, j, swp;
		
		int minY = boundingRect.y;
		int maxY = boundingRect.y + boundingRect.height;
		int minX = boundingRect.x;
		int maxX = boundingRect.x + boundingRect.width;
	
		for (pixY = minY; pixY < maxY; pixY++) {
			nodes = 0;
			j = nPts-1;
			for (i=0; i < nPts; i++) {
				if (pts[i].y < (double)pixY && pts[j].y >= (double)pixY ||
					pts[j].y < (double)pixY && pts[i].y >= (double)pixY) {
					nodeX[nodes] = (int) (( (pts[i].x * (pts[j].y - pixY)) + (pts[j].x * (pixY - pts[i].y)) ) / (pts[j].y - pts[i].y));
					nodes++;	
				}
				j = i;
			}
			i=0;
			while (i<nodes-1) {
				if(nodeX[i]>nodeX[i+1]) {
					swp = nodeX[i];
					nodeX[i] = nodeX[i+1];
					nodeX[i+1] = swp;
					if(i) {i--;}
				}
				else {
					i++;
				}
			}

			for(i=0; i<nodes; i+=2) {
				if     (nodeX[i  ] >= maxX) break;
				if     (nodeX[i+1] >  minX) {
					if (nodeX[i  ] <  minX) {nodeX[i] = minX;}
					if (nodeX[i+1] >  maxX) {nodeX[i+1] = maxX;}
					for(j = nodeX[i]; j<nodeX[i+1]; j++) {
						trailMap[cart2Idx(j, pixY, mapW)] = min((int)(trailMap[cart2Idx(j, pixY, mapW)] + exposure * (1 + constant)), 254);
					}
				}
			}
		}
		
		delete[] nodeX;
	}
	
	int cart2Idx(int x, int y, int w) {
		int idx = (y * w) + x;
		return idx;
	} */
	
};

#endif
		

