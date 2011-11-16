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
	
	~margVideoPlayer();
	
	void	init(string _xmlPath, string _filePath, int _modIdx);
	void	update();
	void	play();
	void	setNextIndex(int _nextIndex);
	unsigned char* getPixels();
	bool	getIsPlaying();
	bool	getIsLoaded();
	bool	getNeedToPlay();
	int		getInteractMode();
	
	void	trig();
	
	
	ofVideoPlayer player;
	
private:
	
	ofxXmlSettings XML;
	
	string	filePath,
			xmlPath;
	bool	bNeedToPlay;
	int		curInteractMode,
			curTrigger,
			nextTrigger,
			nextInteractMode,
			nextTriggerFrame,
			nTriggers,
			modIdx;
	
	int		pixN;
	
	unsigned char* bufVideoPix;
	unsigned char* finalVideoPix;
	bool		bVideoPixLocked,
				bVideoPixFlushed;
	
	bool	tryLockPix();
	
};