#ifndef _GUI_QUAD_
#define _GUI_QUAD_

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofVectorMath.h"
#include "ofxXmlSettings.h"

#include "baseGui007.h"

//#include "miscUtils.h"

class guiQuad : public baseGui{
	
public:
	guiQuad();
	
	void setup(string _quadName);
	void readFromFile(string filePath);
	void loadSettings();
	
	void releaseAllPoints();
	//these should be in the range x(0-maxX) y(0-maxH) 
	//with 	width  = maxW
	//		height = maxH 
	void setQuadPoints( ofVec2f * inPts );		
	
	bool selectPoint(float x, float y, float offsetX, float offsetY, float width, float height, float hitArea);
	bool selectPoint(float x, float y, float offsetX, float offsetY, float width, float height, float hitArea, bool noDistorsion);
	bool updatePoint(float x, float y, float offsetX, float offsetY, float width, float height);
	bool updatePoint(float x, float y, float offsetX, float offsetY, float width, float height, bool noDistorsion);
	
	//returns pts to width by height range 
	ofVec2f * getScaledQuadPoints(float width, float height);
	//returns pts in 0-1 scale
	ofVec2f * getQuadPoints();		
	void saveToFile(string filePath, string newQuadName);
	void saveToFile(string filePath);
	void draw(float x, float y, float width, float height, int red, int green, int blue, int thickness);
	void draw(float x, float y, float width, float height);
	
	
protected:
	ofxXmlSettings	xml;
	ofVec2f srcZeroToOne[4];
	ofVec2f srcScaled[4];
	ofVec2f anchorPoint;
	ofVec2f anchorPointScaled;
	string quadName;
	int selected;
	int neighborVertex1;
	int neighborVertex2;
	float selX;
	float selY;
	float sideA_m;
	float sideA_b;
	float sideA_v;
	float sideB_m;
	float sideB_b;
	float sideB_v;
	float sideC_m;
	float sideC_b;
	float sideC_v;
	float sideD_m;
	float sideD_b;
	float sideD_v;
	float diag_a;
	float diag_b;
	bool diag_v;
	int oppVertex(int vertex);
	
	float greatest(float a, float b) {
		float c = b - a;
		
		if (c<0) { 
			return a;
		} else {
			return b;
		}
	}
	
	float least(float a, float b) {
		float c = b - a;
		
		if (c<0) { 
			return b;
		} else {
			return a;
		}
	}
	
	int nextVertex (int currVertex) {
		
		int next = currVertex + 1;
		if (next > 3) {
			next = 0;
		}
		return next;
		
	}
	
	int prevVertex (int currVertex) {
		
		int prev = currVertex - 1;
		if (prev < 0) {
			prev = 3;
		}
		return prev;
	}
	
};

#endif	



