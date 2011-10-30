/*
 *  margVideoPlayer007.cpp
 *  emptyExample
 *
 *  Created by André Mintz on 18/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "margVideoPlayer007.h"


void margVideoPlayer::init(string _xmlPath, string _filePath, int _modIdx) {

	bNeedToSetIndex		= true;
	bNeedToPlay			= true;
	bNeedToLoadNext		= false;
	bReturnBlack		= false;
	
	curVidGroup			= 0;
	modIdx				= _modIdx;
	
	xmlPath = _xmlPath;
	
	XML.loadFile(xmlPath + "m" + ofToString(modIdx) + "vids.xml");
	
	filePath			= _filePath;

	
	bool success = player.loadMovie(filePath + "m" + ofToString(modIdx) + "g" + ofToString(curVidGroup) + "v0.mov", OFXQTVIDEOPLAYER_MODE_PIXELS_ONLY);
	
	if(success) cout << "Module " + ofToString(modIdx) + " successful" << endl;
	
	player.setLoopState(OF_LOOP_NONE);
	
	int w = player.getWidth();
	int h = player.getHeight();
	int pixN = w*h*3;
	
	blackPix = new unsigned char[pixN];
	
	for (int i = 0; i < pixN; i++) {
		blackPix[i] = 0;
	}
	
	nGroups = XML.getNumTags("group");
	
	XML.pushTag("group", curVidGroup);
		XML.pushTag("video", 0);
			curInteractMode = XML.getValue("interactMode", 1, 0);
		XML.popTag();
	XML.popTag();
	
	nextVidGroup		= 1;
	
	XML.pushTag("group", nextVidGroup);
		numNextVidPossibilities = XML.getNumTags("video");
	XML.popTag();
	
}

// ------------------------------------------------

void margVideoPlayer::update() {
	player.update();
	if (player.getIsMovieDone() && !bNeedToLoadNext) {
		bReturnBlack = true;
		bNeedToLoadNext = true;
	}
	if (bNeedToLoadNext) {
		updateVid();
	}
	else if(bReturnBlack && !bNeedToPlay) {
		bReturnBlack = false;
	}
}

// ------------------------------------------------

void margVideoPlayer::updateVid() {
	player.close();
	bool success = player.loadMovie(filePath + "m" + ofToString(modIdx) + "g" + ofToString(nextVidGroup) + "v" + ofToString(nextVidIndex) + ".mov", OFXQTVIDEOPLAYER_MODE_PIXELS_ONLY);
	player.play();
	player.pause();
	if(success) {
		player.setLoopState(OF_LOOP_NONE);
		bNeedToLoadNext		= false;
		bNeedToPlay			= true;
		bNeedToSetIndex		= true;
		curVidGroup			= nextVidGroup;
		curInteractMode		= nextInteractMode;
		curAudioIndex		= nextAudioIndex;
		curVidIndex			= nextVidIndex;
		nextVidGroup		= (curVidGroup+1) > (nGroups-1) ? 0 : curVidGroup + 1;
	}
}

// ------------------------------------------------

void margVideoPlayer::play() {
		player.play();
		bNeedToPlay = false;
}

// ------------------------------------------------

unsigned char* margVideoPlayer::getPixels() {
	if(bReturnBlack){
		return blackPix;
	}
	else {
		return player.getPixels();
	}
}

// ------------------------------------------------

void margVideoPlayer::setNextIndex(int _nextIndex) {
	
	XML.pushTag("group", nextVidGroup);
		numNextVidPossibilities = XML.getNumTags("video");
	nextVidIndex = _nextIndex%numNextVidPossibilities;
		XML.pushTag("video", nextVidIndex);
			nextInteractMode = XML.getValue("interactMode", 0, 0);
			nextAudioIndex = XML.getValue("audioIndex", 0, 0);
		XML.popTag();
	XML.popTag();
	
	bNeedToSetIndex = false;
	
}

// ------------------------------------------------

bool margVideoPlayer::getIsLoaded() {
	return player.isLoaded();
}

// ------------------------------------------------

bool margVideoPlayer::getNeedToLoadNext() {
	return bNeedToLoadNext;
}

// ------------------------------------------------

bool margVideoPlayer::getIsPlaying() {
	//return player.isPlaying();
	cout << endl << "----- margVideoPlayer::getIsPLaying() is not implemented and will be deprecated" << endl;
	return true;
}

// ------------------------------------------------

bool margVideoPlayer::getNeedToPlay() {
	if (player.isLoaded()) {
		return bNeedToPlay;
	}
	else {
		return false;
	}
}

// ------------------------------------------------

int margVideoPlayer::getInteractMode() {
	return curInteractMode;
}

// ------------------------------------------------

int margVideoPlayer::getAudioIndex() {
	return curAudioIndex;
}

// ------------------------------------------------

int margVideoPlayer::getGroupIndex() {
	return curVidGroup;
}

// ------------------------------------------------

bool margVideoPlayer::getNeedToSetIndex() {
	return bNeedToSetIndex;
}

// ------------------------------------------------