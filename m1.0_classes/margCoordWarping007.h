/*
 *  margCoordWarping.h
 *  m1.0_warp
 *
 *  Created by André Mintz on 02/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "coordWarping007.h"
#include "guiQuad007.h"

class margCoordWarping : public coordWarping {

public:
	
	// FUNCTIONS
	
	void			init(float width, float height, bool doReverse);
	
	// Translation Matrix
	void			updateMatrix();
	void			updateMatrix(bool reverse);
	CvMat*			getTranslateMat();
	void			setTranslateMat(CvMat* _translate); // deprecated
	
	// Quad Files
	void			loadQuad(string filePath);
	void			saveQuad();
	void			saveQuad(string filePath);
	
	// Quad Adjusting
	void			clearQuad();
	void			selectPoint(float x, float y, float offsetX, float offsetY,
								float width, float height, float hitArea);
	void			updatePoint(float x, float y, float offsetX, float offsetY,
								float width, float height);
	
	// Drawing
	void			drawQuad(float x, float y, float width, float height);
	
	ofVec2f*		getScaledQuadPts(float width, float height);
	
	
	// RESOURCES
	
	guiQuad			quadGui;
	
	ofVec2f*		warpedPts;
	
	ofVec2f		quadSrc[4];
	ofVec2f		quadDst[4];
	
	string			quadFilePath;
	
protected:
	float			w, h;
	bool			bReverse;
	
};
