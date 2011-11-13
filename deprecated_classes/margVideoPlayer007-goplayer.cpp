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
	bOKToLoadNext		= false;
	
	curVidGroup			= 0;
	modIdx				= _modIdx;
	
	XML.loadFile(xmlPath + "m" + ofToString(modIdx) + "vids.xml");
	
	filePath			= _filePath;
	
	bool success = player.loadMovie(filePath + "m" + ofToString(modIdx) + "g" + ofToString(curVidGroup) + "v0.mov");
	player.pseudoDraw();
	
	if(success) cout << "Module " + ofToString(modIdx) + " successful" << endl;
	
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
	if (player.getIsMovieDone() && !bNeedToPlay && bOKToLoadNext) {
		updateVid();
	}
	if (player.getPosition() > 0.5) {
		bOKToLoadNext = true;
	} else {
		bOKToLoadNext = false;
	}

	player.pseudoDraw();
}

// ------------------------------------------------

void margVideoPlayer::updateVid() {
	player.loadMovie(filePath + "m" + ofToString(modIdx) + "g" + ofToString(nextVidGroup) + "v" + ofToString(nextVidIndex) + ".mov");
	bNeedToSetIndex		= true;
	//bNeedToPlay			= true;
	curVidGroup			= nextVidGroup;
	curInteractMode		= nextInteractMode;
	curAudioIndex		= nextAudioIndex;
	curVidIndex			= nextVidIndex;
	nextVidGroup		= (curVidGroup+1) > (nGroups-1) ? 0 : curVidGroup + 1;
	while (!player.isLoaded()) {
		sleep(1);
	}
	player.psuedoUpdate();
	player.pseudoDraw();
	if(player.isLoaded()) bNeedToPlay = true;
}

// ------------------------------------------------

void margVideoPlayer::play() {
		player.setPaused(false);
		bNeedToPlay = false;
}

// ------------------------------------------------

unsigned char* margVideoPlayer::getPixels() {
	return player.getPixels();
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

bool margVideoPlayer::getIsPlaying() {
	return player.isPlaying();
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