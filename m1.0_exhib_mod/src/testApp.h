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
	
	// GENERAL -----------------------------------*
	
	int					screenWidth,
						screenHeight;
	
	string				oscSendHost;
	int					oscSendPort;
	int					oscListenPort;
	
	ofxOscMessage		oscMessage;
	
	string				filesPath;
	
	int					modIdx;
	
	// OBJECTS -----------------------------------*
	
	margModule*			module;
	ofTexture			texture;
	
	ofxOscSender		oscSender;
	ofxOscReceiver		oscReceiver;
	
	string*				messageStrings;
	float*				timers;
	
	int					oscMaxMessages;
	
	ofxXmlSettings		XML;
	
	
	// SETUP UTILS -------------------------------*
	
	bool				bNeedToCheckIn,
						bAwareNeedPlay,
						bIsPlaying;
	
	bool				bUpdateModule;
	
	int					displayMode,
						whichBlobs,
						whichQuad;
	
	bool				bDynInteractMode,
						bDrawBlobs,
						bAdjQuad,
						bDrawUndistortBounds;
	
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
	
	// BLOB FINDING VARS -------------------------*
	
	int					minBlob,
						maxBlob,
						numBlob;
	
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
