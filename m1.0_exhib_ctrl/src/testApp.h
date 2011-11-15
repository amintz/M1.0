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
		void exit();
	
		bool checkEveryModNeedPlay();
		bool checkEveryModCheckedIn();
		bool checkEveryModIsPlaying();
	
	// GENERAL -----------------------------------*
	
	int					screenWidth,
						screenHeight;
	
	string				oscModHost,
						oscSoundHost;
	int					oscModPort,
						oscSoundPort,
						oscListenPort;
	
	ofxOscMessage		oscModMessage,
						oscSoundMessage;
	
	string				filesPath;
	
	
	// OBJECTS -----------------------------------*

	
	ofxOscSender*		oscModSender;
	ofxOscSender		oscSoundSender;
	ofxOscReceiver		oscReceiver;
	ofxXmlSettings		XML;
	
	int					oscMaxMessages;
	string				curMessage;
	string*				messageStrings;
	float*				timers;
	
	
	// SETUP UTILS -------------------------------*
	
	bool				bRunAll,
						bQuitAll;
	
	// MODULE VARS -------------------------------*
	
	int					numMod;
	
	bool				all_checkedin,
						all_needPlay,
						all_toldToPlay;
	
	bool*				mod_checkedin;
	bool*				mod_needPlay;
	bool*				mod_isPlaying;
	float*				mod_ThreadFPS;
	float*				mod_AppFPS;
	
	
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
	
	// TASK MANAGEMENT AND MODES VARS -----------*
						
	bool				bRunThread,
						bStopThread;
	
	
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