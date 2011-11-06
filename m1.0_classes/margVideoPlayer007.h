/*
 *  margVideoPlayer007.h
 *  emptyExample
 *
 *  Created by André Mintz on 18/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "ofVideoPlayerAdapt.h"
#include "ofxXmlSettings.h"	

class margVideoPlayer {

public:
	
	void	init(string _xmlPath, string _filePath, int _modIdx);
	void	update();
//	void	updateVid();
	void	play();
	void	setNextIndex(int _nextIndex);
	unsigned char* getPixels();
	bool	getIsPlaying();
	bool	getIsLoaded();
	bool	getNeedToPlay();
	int		getInteractMode();
	
	void	trig();
//	int		getAudioIndex();
//	int		getGroupIndex();
//	bool	getNeedToSetIndex();
	
	
	ofVideoPlayer player;
	
private:
	
	ofxXmlSettings XML;
	
//	unsigned char* blackPix;
	
	string	filePath,
			xmlPath;
	bool	bNeedToPlay;
	int		curInteractMode,
			curTrigger,
			nextTrigger,
			nextInteractMode,
			nextTriggerFrame,
//			curVidGroup,
//			curVidIndex,
//			curAudioIndex,
//			nextVidGroup,
//			nextVidIndex,
//			nextInteractMode,
//			nextAudioIndex,
//			numNextVidPossibilities,
			nTriggers,
			modIdx;
	
};