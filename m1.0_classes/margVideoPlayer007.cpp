/*
 *  margVideoPlayer007.cpp
 *  emptyExample
 *
 *  Created by André Mintz on 18/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "margVideoPlayer007.h"

margVideoPlayer::~margVideoPlayer() {
	player.close();
}

void margVideoPlayer::init(string _xmlPath, string _filePath, int _modIdx) {
	
	modIdx	 = _modIdx;
	
	xmlPath  = _xmlPath;
	filePath = _filePath;
	
	XML.loadFile(xmlPath + "m" + ofToString(modIdx) + "vid-triggers.xml");
	
	player.setUseTexture(false);
	
	
	bool success = false;
	while (!success) {
		success = player.loadMovie(filePath + "m" + ofToString(modIdx) + "g0v0.mov");
	}
	
	player.setLoopState(OF_LOOP_NONE);
	
	bNeedToPlay = true;
	
	pixN = player.getWidth()*player.getHeight()*3;
	
	bufVideoPix = new unsigned char[pixN];
	finalVideoPix = new unsigned char[pixN];
	
	bVideoPixLocked = false;
	bVideoPixFlushed = true;
	
	nTriggers = XML.getNumTags("trigger");
	
	nextTrigger= 0;
	
	XML.pushTag("trigger", nextTrigger);
		nextTriggerFrame= XML.getValue("frame", 0, 0);
		nextInteractMode= XML.getValue("interactMode", 1, 0);
	XML.popTag();
	
}

// ------------------------------------------------

void margVideoPlayer::update() {
	if (player.getIsMovieDone()) {
		player.setFrame(0);
		player.setPaused(true);
		bNeedToPlay = true;
	}
	if(!bNeedToPlay) {
		player.update();
		if (nextTrigger > 0) {
			if (player.getCurrentFrame() >= nextTriggerFrame) {
				trig();
			}
		}
	}
	if (bVideoPixFlushed) {
		bool success = false;
		while (!success) {
			success = tryLockPix();
		}
		memcpy(bufVideoPix, player.getPixels(), pixN);
		bVideoPixFlushed = false;
		bVideoPixLocked = false;
	}
}

// ------------------------------------------------

void margVideoPlayer::trig() {
	
	curTrigger = nextTrigger;
	
	curInteractMode = nextInteractMode;
	
	nextTrigger = nextTrigger+1 >= nTriggers? 0 : nextTrigger+1;
	
	XML.pushTag("trigger", nextTrigger);
		nextTriggerFrame= XML.getValue("frame", 0, 0);
		nextInteractMode= XML.getValue("interactMode", 1, 0);
	XML.popTag();
	
	if (nextTriggerFrame > player.getTotalNumFrames()) {
		nextTrigger = 0;
		XML.pushTag("trigger", nextTrigger);
			nextTriggerFrame= XML.getValue("frame", 0, 0);
			nextInteractMode= XML.getValue("interactMode", 1, 0);
		XML.popTag();
	}
	
}

// ------------------------------------------------

void margVideoPlayer::play() {
	if(player.isLoaded()) {
		trig();
		player.play();
		player.setPaused(false);
		bNeedToPlay = false;
	}
}

// ------------------------------------------------

unsigned char* margVideoPlayer::getPixels() {
	if(!bVideoPixFlushed && !bVideoPixLocked){
		bVideoPixLocked = true;
		memcpy(finalVideoPix, bufVideoPix, pixN);
		bVideoPixLocked = false;
		bVideoPixFlushed = true;
	}
	return finalVideoPix;
}

// ------------------------------------------------

bool margVideoPlayer::tryLockPix() {
	if (bVideoPixLocked) {
		return false;
	}
	else {
		bVideoPixLocked = true;
		return true;
	}

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