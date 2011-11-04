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
	
	int w = player.getWidth();
	int h = player.getHeight();
	int pixN = w*h*3;
	
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
		player.setPosition(0);
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
}

// ------------------------------------------------

void margVideoPlayer::trig() {
	
	curTrigger = nextTrigger;
	
	nextTrigger = nextTrigger+1 > nTriggers? 0 : nextTrigger+1;
	
	curInteractMode = nextInteractMode;
	
	XML.pushTag("trigger", nextTrigger);
		nextTriggerFrame= XML.getValue("frame", 0, 0);
		nextInteractMode= XML.getValue("interactMode", 1, 0);
	XML.popTag();
	
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
	return player.getPixels();
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