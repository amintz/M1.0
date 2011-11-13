/*
 *  margVideoPlayer007.h
 *  emptyExample
 *
 *  Created by André Mintz on 18/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "goThreadedVideo.h"
#include "ofxXmlSettings.h"	

class margVideoPlayer {

public:
	
	void	init(string _xmlPath, string _filePath, int _modIdx);
	void	update();
	void	updateVid();
	void	play();
	void	setNextIndex(int _nextIndex);
	unsigned char* getPixels();
	bool	getIsPlaying();
	bool	getIsLoaded();
	bool	getNeedToPlay();
	int		getInteractMode();
	int		getAudioIndex();
	int		getGroupIndex();
	bool	getNeedToSetIndex();
	
	
	goThreadedVideo player;
	
private:
	
	ofxXmlSettings XML;
	
	string	filePath,
			xmlPath;
	bool	bNeedToSetIndex,
			bNeedToPlay,
			bOKToLoadNext;
	int		curInteractMode,
			curVidGroup,
			curVidIndex,
			curAudioIndex,
			nextVidGroup,
			nextVidIndex,
			nextInteractMode,
			nextAudioIndex,
			numNextVidPossibilities,
			nGroups,
			modIdx;
	
};