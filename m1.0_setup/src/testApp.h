#pragma once

#include "ofxSimpleGuiToo.h"
#include "margModule007.h"
#include "ofxOsc.h"

#include "ofxXmlSettings.h"

#include "ofMain.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
		bool checkEveryModNeedPlay();
		bool checkEveryModNeedVidIndex();
	
	// GENERAL -----------------------------------*
	
	int					screenWidth,
						screenHeight;
	
	string				oscHost;
	int					oscPort;
	
	ofxOscMessage		oscMessage;
	
	string				filesPath;
	
	int					curVidGroup;
	int					nextVidGroup;
	int					curVidIdx;
	int					nextVidIdx;
	
	// OBJECTS -----------------------------------*
	
	margModule*			modules;
	ofxOscSender		oscSender;
	ofxXmlSettings		XML;
	
	
	// SETUP UTILS -------------------------------*
	
	ofTexture			testTex;
	unsigned char*		whitePix;
	
	
	// MODULE VARS -------------------------------*
	
	int					numMod,
						curMod;
	
	// BLOB TRACKING VARS ------------------------*
	
	float				maxAreaDiff,
						maxDist,
						maxUnfitness;
	
	float				blobDefScaleFactor,
						blobCondScaleConst,
						blobCondScaleMax;
	
	// TRAIL MAKING VARS -------------------------*
	
	float				exposureConst,
						fadeConst;
	
	int					blurLevel;
	
	// VIDEO CAPTURING VARS ----------------------*
	
	float 				camWidth;
	float				camHeight;
	
	// DISPLAY VARS ------------------------------*
	
	int					dispWidth,
						dispHeight;
	
	int					drawX,
						drawY,
						drawWidth,
						drawHeight;
	
	// BLOB FINDING VARS -------------------------*
	
	int					minBlob,
						maxBlob,
						numBlob;
	
	// TASK MANAGEMENT AND MODES VARS -----------*
	
	bool				opVidSet,
						bAdjQuad,
						clearQuad,
						bDrawBlobs,
						bExtDisplay,
						bControlDisplay,
						bUpdateSettings;
	
	bool				bDrawWhite,
						bDrawUndistorted;
						
	bool				bRunThread,
						bStopThread;
	
	int					displayMode,
						whichBlobs,
						whichQuad;
	
	string				modes[7],
						interactModes[3],
						quads[2],
						blobsToDraw[3];
	
	
	// UTILS -----------------------------------------------*
	
	string	*vecStr2StrAr(vector<string> vecStr) {
		string* strAr = new (nothrow) string[vecStr.size()];
		for(int i=0; i<vecStr.size(); i++){
			strAr[i] = vecStr[i];
		}
		vecStr.clear();
		return strAr;
		if(strAr != 0) delete[] strAr;
	}
	
	
	int idx2coordX (int idx, int w) {
		int coordX;
		coordX = (idx/3)%w;
		return coordX;
	}
	
	int idx2coordY (int idx, int w) {
		int coordY;
		coordY = (idx/3)/w;
		return coordY;
	}
	
	int coord2idx (int x, int y, int w) {
		int idx;
		idx = 3*((y * w) + x);
		return idx;
	}
	
	int coord2idx (int x, int y, int w, bool bColor) {
		int idx;
		if (bColor) {
			idx = 3*((y * w) + x);
		} else {
			idx = ((y * w) + x);
		}
		return idx;
	}
	
		
};
