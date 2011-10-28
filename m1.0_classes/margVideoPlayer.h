/*
 *  margVideoPlayer.h
 *  emptyExample
 *
 *  Created by André Mintz on 16/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofVideoPlayer.h"
#include "ofxXmlSettings.h"
#include "ofThread.h"

class margVideoPlayer : public ofThread {
	
public:
	
	void			init(string	xmlPath, string _filePath, int _modIdx);
	void			play();
	void			update();
	void			threadedFunction();
	unsigned char*	getPixels();
	void			setNextIndex(int index);
	int				getInteractMode();
	bool			getNeedToSetIndex();
	bool			getNeedToBePlayed();
	
private:
	
	ofVideoPlayer	vidPlayer0;
	ofVideoPlayer	vidPlayer1;
	
	ofxXmlSettings	XML;
	
	string			filePath;
	
	unsigned char*	curPixels;
	unsigned char*	bkpPixels;
	
	bool			b0IsActive;
	
	bool			bNeedToSetIndex;
	bool			bNeedToBePlayed;
	
	bool			bLockCurPixels;
	
	int				curInteractMode;
	int				curVidGroup;
	int				curVidIndex;
	
	int				nextInteractMode;
	int				nextVidGroup;
	int				nextVidIndex;
	
	int				nextVidPossibilities;
	
	int				nGroups;
	
	int				modIdx;
	
};

