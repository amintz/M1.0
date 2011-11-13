/*
 *  margVideoPlayer.cpp
 *  emptyExample
 *
 *  Created by André Mintz on 16/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margVideoPlayer.h"


void margVideoPlayer::init(string xmlPath, string _filePath, int _modIdx) {
	b0IsActive			= true;
	
	bNeedToSetIndex		= true;
	bNeedToBePlayed		= true;	
	bLockCurPixels		= false;
	
	curVidGroup			= 0;
	modIdx				= _modIdx;
	
	XML.loadFile(xmlPath + "m" + ofToString(modIdx) + "vids.xml");
	
	filePath			= _filePath;
	ofVideoPlayer&		curVidPlayer		= vidPlayer0;
	ofVideoPlayer&		bkpVidPlayer		= vidPlayer1;
	
	curVidPlayer.setLoopState(OF_LOOP_NONE);
	curVidPlayer.setUseTexture(false);
	
	while (!curVidPlayer.isLoaded()) {
		curVidPlayer.loadMovie(filePath + "m" + ofToString(modIdx) + "g" + ofToString(curVidGroup) + "v0.mov");
		cout << "loading main movie"<<endl;
	}
	
	
	nGroups = XML.getNumTags("group");
	
	XML.pushTag("group", curVidGroup);
	XML.pushTag("video", 0);
	curInteractMode = XML.getValue("interactMode", 1, 0);
	XML.popTag();
	XML.popTag();
	
	nextVidGroup		= 1;
	
	XML.pushTag("group", nextVidGroup);
	nextVidPossibilities = XML.getNumTags("video");
	XML.popTag();
	
	curPixels = new unsigned char[1280 * 720 * 3];
	bkpPixels = new unsigned char[1280 * 720 * 3];

} 

// ----------------------------------------------

void margVideoPlayer::play() {
	ofVideoPlayer&		curVidPlayer = b0IsActive? vidPlayer0 : vidPlayer1;
	curVidPlayer.play();
	bNeedToBePlayed = false;
}

// ----------------------------------------------

void margVideoPlayer::update() {
	
	ofVideoPlayer&		curVidPlayer = b0IsActive? vidPlayer0 : vidPlayer1;
	
	// IF CURRENT VIDEO IS LOADED, UPDATE
	
	if (curVidPlayer.isLoaded() && curVidPlayer.isPlaying()) {
		curVidPlayer.idleMovie();
		if (curVidPlayer.isFrameNew()) {
			curPixels = curVidPlayer.getPixels();
		}
	}

	
	// IF CURRENT VIDEO IS DONE
	
	if (curVidPlayer.getPosition() > 0.99) {
		curVidPlayer.stop();
		curVidPlayer.close();				// Close it
	
		// SWAP REFERENCES
		if (b0IsActive) {
			b0IsActive			= false;
		} 
		else {
			b0IsActive			= true;
		}
		
		//RESET 'NEXT' VARIABLES AND ASK FOR NEXT INDEX AND TO PLAY
		bNeedToBePlayed = true;
		bNeedToSetIndex	= true;
		curVidGroup		= nextVidGroup;
		curInteractMode = nextInteractMode;
		curVidIndex		= nextVidIndex;
		nextVidGroup	= (curVidGroup+1) > (nGroups-1) ? 0 : curVidGroup + 1;
	}
}

// ----------------------------------------------

void margVideoPlayer::threadedFunction() {
	while (isThreadRunning()!= 0) {
		if(lock()){
		update();
		unlock();
		}
	}
}

// ----------------------------------------------

unsigned char* margVideoPlayer::getPixels() {
	ofVideoPlayer&		curVidPlayer = b0IsActive? vidPlayer0 : vidPlayer1;
	if(curVidPlayer.isLoaded() && curVidPlayer.isPlaying()) {
		return curPixels;
	}
	else {
		return bkpPixels;
	}

}

// ----------------------------------------------

void margVideoPlayer::setNextIndex(int index) {
	ofVideoPlayer&		bkpVidPlayer = b0IsActive? vidPlayer1 : vidPlayer0;
	
	nextVidIndex = index;
	
	bkpVidPlayer.setLoopState(OF_LOOP_NONE);
	bkpVidPlayer.setUseTexture(false);
	
	while (!bkpVidPlayer.isLoaded()) {
		bkpVidPlayer.loadMovie(filePath + "m" + ofToString(modIdx) + "g" + ofToString(nextVidGroup) + "v" + ofToString(nextVidIndex) + ".mov");
		cout << "loading bkpmovie" << endl;
	}
	
	XML.pushTag("group", nextVidGroup);
	XML.pushTag("video", nextVidIndex);
	nextInteractMode = XML.getValue("interactMode", 0, 0);
	XML.popTag();
	XML.popTag();
	
	bNeedToSetIndex = false;
}

// ----------------------------------------------

int margVideoPlayer::getInteractMode() {
	return curInteractMode;
}

// ----------------------------------------------

bool margVideoPlayer::getNeedToSetIndex() {
	return bNeedToSetIndex;
}

// ----------------------------------------------

bool margVideoPlayer::getNeedToBePlayed() {
	return bNeedToBePlayed;
}
