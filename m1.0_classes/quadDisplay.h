/*
 *  quadDisplay.h
 *  m1.0_proj
 *
 *  Created by André Mintz on 09/06/11.
 *
 */

#include "ofxOpenCv.h"
#include "ofxVectorMath.h"
#include "ofxXmlSettings.h"
#include "coordWarping.h"
#include "guiQuad.h"

class quadDisplay {
	
public:
	
	void			initialize(int dispWidth, int dispHeight, int ID, string root);
	void			initialize(int dispWidth, int dispHeight, int ID);
	
	void			loadID(int ID);
	
	ofxCvColorImage source;
	ofxCvGrayscaleImage gSource;
	ofxCvColorImage	CVWarpedImg;
	ofxCvGrayscaleImage CVWarpedGImg;
	
	void			feedImg(ofxCvColorImage& source);
	void			feedImg(ofxCvGrayscaleImage& source);
	
	void			setFromPixels(unsigned char * pixels, int _w, int _h);
	void			setFromPixels(unsigned char * pixels, int _w, int _h, bool bIsColor);
	
	void			draw(int x, int y, int w, int h);
	
	guiQuad			cvWarpQuadSrc;
	guiQuad			cvWarpQuadDest;
	
	ofxPoint2f		quadSrc[4];
	ofxPoint2f		quadDst[4];
	
	coordWarping	coordWarpSrc;
	coordWarping	coordWarpDest;
	
	ofxPoint2f * warpedPts;
	
	void			clearQuad();
	
	bool			isInQuad(ofxPoint2f point);
	ofxPoint2f		warpTransform(float x, float y);
	
	//Quad files
	void			saveQuad();
	void			selectPoint(float x, float y, float orX, float orY, float orW, float orH, float hitArea);
	void			updatePoint(float x, float y, float orX, float orY, float orW, float orH);
	string			quadFilesRoot;
	
	int				dispID;
	int				dispW;
	int				dispH;
	
	
	
};