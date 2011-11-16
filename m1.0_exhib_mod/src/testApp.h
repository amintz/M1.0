#pragma once

#include "ofxSimpleGuiToo.h"
#include "margModule007.h"
#include "ofxOsc.h"

#include "ofxXmlSettings.h"

#include "ofMain.h"

#define ASK_CHECKIN		0
#define RCV_CHECKIN		1
#define ASK_NEEDPLAY	2
#define RCV_NEEDPLAY	3
#define ASK_PLAY		4
#define RCV_ISPLAYING	5
#define ASK_SND_PLAY	6
#define ASK_SHUTDOWN	7
#define ASK_SND_STOP	8

#define MOD_CHECKIN		0
#define MOD_NEEDPLAY	1
#define MOD_ISPLAYING	2
#define MOD_FPS			3

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
	
	string*				ctrlMsg;
	string*				myMsg;
	
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
						bGotPlay,
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
