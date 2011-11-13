/*
 *  margBlobInterpolator.cpp
 *  m1.0_trail
 *
 *  Created by André Mintz on 19/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margBlobInterpolator.h"


bool interpol_sort_fitness_compare(const vector<int> f, const vector<int> g) {
	
	return f[2] > g[2];
	
}

void margBlobInterpolator::init(int _width, int _height) {
	width = _width;
	height = _height;
}

// ---------------------------------------------------------------

void margBlobInterpolator::feedBlobs(vector<margBlob> inBlobs) {
	makeInterpolated(inBlobs);
}

// ---------------------------------------------------------------

int* margBlobInterpolator::findPairs(vector<margBlob> current, vector<margBlob> previous, float _maxDist,
									float _maxAreaDiff, float _maxUnfitness) {
	
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
			if ( dist < _maxDist && areaDiff < _maxAreaDiff) {
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
	sort(possible_matches.begin(), possible_matches.end(), interpol_sort_fitness_compare);
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

// -------------------------------------------------------------

int* margBlobInterpolator::findPairs(vector<margBlob> current) {
	int nCurrBlobs = current.size();
	int nPrevBlobs = prevBlobs.size();
	
	int nMatches = 0;
	
	int* final_matches = new (nothrow) int [nCurrBlobs];
	
	for (int i = 0; i < nCurrBlobs; i++) {
		final_matches[i] = -1;
	}
	
	vector< vector<int> > possible_matches;
	possible_matches.reserve(3 * nCurrBlobs * nPrevBlobs * sizeof(int));
	
	for (int i = 0; i < nCurrBlobs; i++) {
		for (int j = 0; j < nPrevBlobs; j++) {
			float dist = ofDist(current[i].centroid.x, current[i].centroid.y, prevBlobs[j].centroid.x, prevBlobs[j].centroid.y);
			float areaDiff = ((float)(current[i].area - prevBlobs[j].area) / (float)current[i].area) * (float)100;
			if (areaDiff<0) { areaDiff = areaDiff * (-1); };
			if ( dist < maxDist && areaDiff < maxAreaDiff) {
				int unfitness = round(10000 / (0.75 * dist + 0.25 *	areaDiff));
				vector<int> currPossib(3);
				currPossib[0] = i;
				currPossib[1] = j;
				currPossib[2] = unfitness;
				possible_matches.push_back(currPossib);
				nMatches++;
				currPossib.clear();
			}
		}
	}
	sort(possible_matches.begin(), possible_matches.end(), interpol_sort_fitness_compare);
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
	
	return final_matches;
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
		interBlobs.push_back(newBlob);
	}
	
	interBlobs.push_back(blob2);
	
	blob1_normPts.clear();
	blob2_normPts.clear();
	transPts.clear();
	
	return interBlobs;

}

// -------------------------------------------------------------

void margBlobInterpolator::makeInterpolated(vector<margBlob> current, float _maxDist, float _maxAreaDiff,
										   float _maxUnfitness) {

	if (prevBlobs.size() == 0) {
		// If previous blobs ARE NOT saved. Save current and return them.
		prevBlobs = current;
	}
	else {
		int nBlobs = current.size();
		vector<margBlob> currentCopy = current;
		current.clear();

		// If previous blobs ARE saved, continue and check for good pairs
		int* pairs = findPairs(currentCopy, prevBlobs, _maxDist, _maxAreaDiff, _maxUnfitness);
		for (int i = 0; i < nBlobs; i++) {
			if (pairs[i] >= 0) {
				if (prevBlobs[pairs[i]].pts.size() > 0) {
					vector<margBlob> interBlobs = interpolate(prevBlobs[pairs[i]], currentCopy[i]);
					for (int j = 0; j < interBlobs.size(); j++) {
						current.push_back(interBlobs[j]);
					}
				}
			}
			else {
				current.push_back(currentCopy[i]);
			}
		}
			
		prevBlobs.clear();
		prevBlobs = currentCopy;
		finalBlobs.clear();
		finalBlobs= current;
		if(pairs != 0) delete[] pairs;
	}

}

// ------------------------------------------------------

void margBlobInterpolator :: makeInterpolated(vector<margBlob> current) {
	
	if (prevBlobs.size() == 0) {
		// If previous blobs ARE NOT saved. Save current and return them.
		prevBlobs = current;
	}
	else {
		int nBlobs = current.size();
		vector<margBlob> currentCopy = current;
		current.clear();
		
		// If previous blobs ARE saved, continue and check for good pairs
		int* pairs = findPairs(currentCopy);
		for (int i = 0; i < nBlobs; i++) {
			if (pairs[i] >= 0) {
				if (prevBlobs[pairs[i]].pts.size() > 0) {
					vector<margBlob> interBlobs = interpolate(prevBlobs[pairs[i]], currentCopy[i]);
					for (int j = 0; j < interBlobs.size(); j++) {
						current.push_back(interBlobs[j]);
					}
				}
			}
			else {
				current.push_back(currentCopy[i]);
			}
		}
		
		prevBlobs.clear();
		prevBlobs = currentCopy;
		finalBlobs.clear();
		finalBlobs= current;
		if(pairs != 0) delete[] pairs;
	}
	
}

// ------------------------------------------------------

vector<margBlob> margBlobInterpolator :: getInterpolatedBlobs() {
	return finalBlobs;
}

// ------------------------------------------------------

void margBlobInterpolator :: setInterpolator(float _maxDist, float _maxAreaDiff, float _maxUnfitness) {
	maxDist = _maxDist;
	maxAreaDiff = _maxAreaDiff;
	maxUnfitness = _maxUnfitness;
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
