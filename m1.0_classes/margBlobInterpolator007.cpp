/*
 *  margBlobInterpolator.cpp
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margBlobInterpolator007.h"


bool interpol_sort_fitness_compare(const vector<int> f, const vector<int> g) {
	
	return f[2] > g[2];
	
}

void margBlobInterpolator::init(int _width, int _height) {
	width = _width;
	height = _height;
}

// ---------------------------------------------------------------

void margBlobInterpolator::feedBlobs(vector<margBlob> inBlobs) {
	makeInterpolated(scale(inBlobs, defScaleFactor));
}

// ---------------------------------------------------------------

int* margBlobInterpolator::findPairs(vector<margBlob> current, vector<margBlob> previous, float _maxDist,
									float _maxAreaDiff, float _maxUnfitness) {
	
//  01 -- Get number of blobs (previous and current) and setup to save final matches
	
	int nCurrBlobs = current.size();
	int nPrevBlobs = previous.size();
	
	int nMatches = 0;
	
	int* final_matches = new (nothrow) int [nCurrBlobs];
	
	for (int i = 0; i < nCurrBlobs; i++) {
		final_matches[i] = -1;
	}
	
//	02 -- Setup to save possible matches (which will later be ranked into most likely ones
	
	vector< vector<int> > possible_matches;
	
//	03 -- Find possible matches
	
	// For each current blob
	
	for (int i = 0; i < nCurrBlobs; i++) {
		
		// Go through each previous blobs
		
		for (int j = 0; j < nPrevBlobs; j++) {
			
			// Check their distance
			
			float dist = ofDist(current[i].centroid.x, current[i].centroid.y, previous[j].centroid.x, previous[j].centroid.y);
			
			
			// Check their area difference
			
			float areaDiff = ((float)(current[i].area - previous[j].area) / (float)current[i].area) * (float)100;
			if (areaDiff<0) { areaDiff = areaDiff * (-1); };
			
			// If area difference and distance are smaller than threshold
			
			if ( dist < _maxDist && areaDiff < _maxAreaDiff) {
				
				// Calculate unfitness with bigger weight on distance than area difference
				
				int unfitness = round(10000 / (0.75 * dist + 0.25 *	areaDiff));
				
				// Setup possible match record
				
				vector<int> currPossib;
				currPossib.push_back(i);
				currPossib.push_back(j);
				currPossib.push_back(unfitness);
				
				// Save possible match record
				
				possible_matches.push_back(currPossib);
				nMatches++;
				currPossib.clear();
			}
		}
	}
	
//	04 - Sort possible matches based on unfitness factor and revert order
	
	sort(possible_matches.begin(), possible_matches.end(), interpol_sort_fitness_compare);
	reverse(possible_matches.begin(), possible_matches.end());
	
	
//	05 - Extract final matches	
	
	// Go through possible matches and delete records that can't be used anymore (already used or overlapping with already used)
	// Whilhe there are still possible matches:
	
	while (possible_matches.size() > 0) {
		
		// Get the blobs involved in the most likely of the list
		
		int used_currblob = possible_matches[0][0];
		int used_prevblob = possible_matches[0][1];
		
		// Save them to final matches record
		
		final_matches[used_currblob] = used_prevblob;
		
		// Setup list of elements to erase
				
		vector<int> elements_to_erase;
		
		// For each possible match
				
		for (int j = 0; j < nMatches; j++) {
			
			// If it uses either the current or previous blobs used
			
			if (possible_matches[j][0] == used_currblob ||
				possible_matches[j][1] == used_prevblob) {
				
				// Add them to the list to be erased
				
				elements_to_erase.push_back(j);
			}
		}
		
		// Delete elements to be erased
				
		for (int i = 0; i < elements_to_erase.size(); i++) {
			possible_matches.erase(possible_matches.begin() + elements_to_erase[i]);
		}
		
		// Set up for next run
				
		nMatches -= elements_to_erase.size();
		
		elements_to_erase.clear();
	}
	
	//	06 - Return final matches and finalize
	
	possible_matches.clear();
	current.clear();
	previous.clear();
	
	return final_matches;

}

// -------------------------------------------------------------

int* margBlobInterpolator::findPairs(vector<margBlob> current) {
	
	return findPairs(current, prevBlobs, maxDist, maxAreaDiff, maxUnfitness);
}

// -------------------------------------------------------------

vector<margBlob> margBlobInterpolator::interpolate(margBlob& blob1, margBlob& blob2) {
	
	//  Calculate path's function
    
	int delta_x = (blob2.centroid.x-blob1.centroid.x);
	int delta_y = (blob2.centroid.y-blob1.centroid.y);
	
	double path_angle = (double)delta_y/(double)delta_x;
	double path_orig = (double)blob1.centroid.y - ((double)path_angle * (double)blob1.centroid.x);
	
	int numSteps = ofDist(blob1.centroid.x, blob1.centroid.y, blob2.centroid.x, blob2.centroid.y);
	double step_x = (double)delta_x/(double)numSteps;
	double step_y = (double)delta_y/(double)numSteps;
	
	vector<margBlob> interBlobs(numSteps+2);
	vector< vector<float> > transPts;
	
	
	//  Normalize shape coordinates
	
	vector<ofPoint> blob1_normPts(blob1.nPts);
	vector<ofPoint> blob2_normPts(blob2.nPts);
	
	for(int p = 0; p < blob1.nPts; p++) {
		blob1_normPts[p] = normCart(blob1.pts[p].x, blob1.pts[p].y, blob1.centroid.x, blob1.centroid.y); 
	}
	
	for(int p = 0; p < blob2.nPts; p++) {
		blob2_normPts[p] = normCart(blob2.pts[p].x, blob2.pts[p].y, blob2.centroid.x, blob2.centroid.y);
	}
	
	//  Compare number of vertices and trigger the appropriate methods in each case
	
	// If both shapes have the same number of vertices
	
	if(blob1.nPts == blob2.nPts) {
		
		//  Set transformation paths
		
		for(int p = 0; p < blob1.nPts; p++) {
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
	
	// If blob1 has more vertices than blob2
	
	else if (blob1.nPts > blob2.nPts) {
		
		int diff = blob1.nPts - blob2.nPts;
		int aver_diff = 0;
		int amt_aver  = 1;
		
		if (diff <= blob2.nPts) {
			aver_diff = floor((float)blob2.nPts / (float)diff);
		}
		else {
			aver_diff = 1;
			amt_aver = ceil((float)diff / (float)blob2.nPts);      
		}
		
		int addedExtraPts = 0;
		
		for(int p = 0; p < blob2.nPts; p++) {      
			
			int curSh1Pt = p + addedExtraPts;
			
			int curSh2Pt = p;
			if(curSh2Pt >= blob2.nPts) { curSh2Pt -= blob2.nPts; }
			
			int prevSh2Pt = curSh2Pt - 1;
			if(prevSh2Pt < 0) { prevSh2Pt += blob2.nPts; }
			
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
			
			if (curSh1Pt < blob1.nPts) {
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
	
	// If blob1 has less vertices than blob2
	
	else if (blob1.nPts < blob2.nPts) {
		
		int diff = blob2.nPts - blob1.nPts;
		int aver_diff = 0;
		int amt_aver = 1;
		
		if (diff <= blob1.nPts) {
			aver_diff = floor((float)blob1.nPts / (float)diff);      
		}
		else {
			aver_diff = 1;
			amt_aver = ceil((float)diff / (float)blob1.nPts);  
		}
		
		int removedExtraPts = 0;
		
		for(int p = 0; p < blob1.nPts; p++) { 
			
			int curSh2Pt = p + removedExtraPts;
			if(curSh2Pt >= blob2.nPts) { curSh2Pt -= blob2.nPts;}
			
			int curSh1Pt = p;
			
			int prevSh1Pt = curSh1Pt - 1;
			if(prevSh1Pt < 0) { prevSh1Pt += blob1.nPts;}
			
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
			if (curSh2Pt < blob2.nPts) {
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
	
	//if (numSteps > 0) { exposureLev = 1 / numSteps; }
	
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
		int nTransPts = transPts.size();
		for(int p = 0; p < nTransPts; p++) {
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
		newBlob.nPts = newBlob.pts.size();
		interBlobs.push_back(newBlob);
	}
	
	interBlobs.push_back(blob2);
	
	blob1_normPts.clear();
	blob2_normPts.clear();
	transPts.clear();
	
	return interBlobs;

}


// -------------------------------------------------------------

float margBlobInterpolator::calcScaleFactor(margBlob& blob1, margBlob& blob2) {
	float dist = ofDist(blob1.centroid.x, blob1.centroid.y, blob2.centroid.x, blob2.centroid.y);
	float scaleFactor = 1.0f;
	if (dist < maxDist) {
		float invNormDist = 1.0f - ofMap(dist, 0.0f, maxDist, 0.0f, 1.0f);
		scaleFactor = ofClamp(blob1.condScaleFactor + (invNormDist * condScaleConst), 1.0f, condScaleMax);
	}
	else {
		scaleFactor = 1.0f;
	}
	blob2.condScaleFactor = scaleFactor;
	return scaleFactor;	
}

// -------------------------------------------------------------

margBlob margBlobInterpolator::scale(margBlob& inBlob, float factor) {
	
	// If blob is less than a triangle, dont scale it
	if(inBlob.nPts < 3) return inBlob;
	
	// If it is a triangle or more
	else {
		
		// Set initial bounding rectangle
		float minX = width,
			  maxX = 0,
			  minY = height,
			  maxY = 0;
		
		// For each of the blob's points
		for(int p = 0; p < inBlob.nPts; p++) {
			
			// Normalize X and Y coordinates in relation to Centroid
			// multiply by factor, make them back into absolute coordinates
			// and force them into image size
			inBlob.pts[p].x = forceInRange(((float)(inBlob.pts[p].x-inBlob.centroid.x) * factor) + inBlob.centroid.x, 0, width);
			inBlob.pts[p].y = forceInRange(((float)(inBlob.pts[p].y-inBlob.centroid.y) * factor) + inBlob.centroid.y, 0, height);
			
			// Check if they are set new bounding rectangle
			if(inBlob.pts[p].x < minX) minX = inBlob.pts[p].x;
			else if(inBlob.pts[p].x > maxX) maxX = inBlob.pts[p].x;
			if(inBlob.pts[p].y < minY) minY = inBlob.pts[p].y;
			else if(inBlob.pts[p].y > maxY) maxY = inBlob.pts[p].y;
		}
		
		// Assign new values into blob coordinates
		
		inBlob.boundingRect.x = minX;
		inBlob.boundingRect.y = minY;
		inBlob.boundingRect.width = maxX - minX;
		inBlob.boundingRect.height= maxY - minY;
		return inBlob;
	}
}

// -------------------------------------------------------------

vector<margBlob> margBlobInterpolator::scale(vector<margBlob> inBlobs, float factor) {
	int nBlobs = inBlobs.size();
	
	for(int i = 0; i < nBlobs; i++) {
		scale(inBlobs[i], factor);
	}
	return inBlobs;
}


// -------------------------------------------------------------

void margBlobInterpolator::makeInterpolated(vector<margBlob> current, float _maxDist, float _maxAreaDiff,
										   float _maxUnfitness) {

	if (prevBlobs.size() == 0) {
		// If previous blobs ARE NOT saved. Save current as previous.
		prevBlobs = current;
	}
	else {
		// If previous blobs ARE saved, continue and check for good pairs
		int nBlobs = current.size();
		vector<margBlob> currentCopy = current;
		current.clear();
		
		// Get pair list
		int* pairs = findPairs(currentCopy, prevBlobs, _maxDist, _maxAreaDiff, _maxUnfitness);
		
		// For each current blob
		for (int i = 0; i < nBlobs; i++) {
			
			// If it has another blob assigned to it (a pair)
			if (pairs[i] >= 0) {
				
				// and if such pair is has at least one vertex
				if (prevBlobs[pairs[i]].pts.size() > 0) {
					
					// Generate interpolated blobs
					vector<margBlob> interBlobs = interpolate(prevBlobs[pairs[i]], currentCopy[i]);
					float scaleFactor = calcScaleFactor(prevBlobs[pairs[i]], currentCopy[i]);
					
					// For each interpolated blob
					for (int j = 0; j < interBlobs.size(); j++) {
						
						// Push it into final blob array
						current.push_back(scale(interBlobs[j], scaleFactor));
					}
				}
			}
			
			// If it doesn't have a pair
			else {
				
				// Push single blob into final blob array
				current.push_back(currentCopy[i]);
			}
		}
		
		// Reassign previous blobs to current found blobs
		prevBlobs.clear();
		prevBlobs = currentCopy;
		
		// Delete generated pair list
		if(pairs != 0) delete[] pairs;
	}
	
	
	// Assign final blobs to interpolated blobs, or to just current blobs
	finalBlobs.clear();
	finalBlobs= current;

}

// ------------------------------------------------------

void margBlobInterpolator :: makeInterpolated(vector<margBlob> current) {
	
	makeInterpolated(current, maxDist, maxAreaDiff, maxUnfitness);
	
}

// ------------------------------------------------------

vector<margBlob> margBlobInterpolator :: getInterpolatedBlobs() {
	return finalBlobs;
}

// ------------------------------------------------------

void margBlobInterpolator :: setInterpolator(float _maxDist, float _maxAreaDiff, float _maxUnfitness,
											 float _defScaleFactor, float _condScaleConst, float _condScaleMax) {
	maxDist			= _maxDist;
	maxAreaDiff		= _maxAreaDiff;
	maxUnfitness	= _maxUnfitness;
	defScaleFactor	= _defScaleFactor;
	condScaleConst	= _condScaleConst;
	condScaleMax	= _condScaleMax;
}

// ------------------------------------------------------

void margBlobInterpolator :: drawInterp (int x, int y, int w, int h) {
	
	
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
	for( int i=0; i<(int)finalBlobs.size(); i++ ) {
		ofRect( finalBlobs[i].boundingRect.x, finalBlobs[i].boundingRect.y,
			   finalBlobs[i].boundingRect.width, finalBlobs[i].boundingRect.height );
	}
	
	// ---------------------------- draw the blobs
	
	ofSetHexColor(0x00FFFF);
	
	
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
