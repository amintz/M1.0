/*
 *  margModule.h
 *  m1.0_setup
 *
 *  Created by André Mintz on 01/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margCapture007.h"
#include "margBlobFinder007.h"
#include "margBlobCorrector007.h"
#include "margBlobInterpolator007.h"
#include "margTrailMaker007.h"

#include "ofVideoPlayer.h"

#include "margVideoBlender007.h"
#include "margDisplay007.h"
#include "margCoordWarping007.h"

#include "ofThread.h"

// GENERAL MODES

#define SOURCE					0
#define LENSCORRECTED_SOURCE	1
#define WARPED_SOURCE			2
#define THRESHOLD_SOURCE		3
#define	TRAIL_MAP				4
#define	FINAL_IMAGE				5
#define	BYPASS_VIDEO			6

// BLOB DRAWING MODES

#define ORIGINAL_BLOBS			0
#define CORRECTED_BLOBS			1
#define INTERPOLATED_BLOBS		2

// INTERACTION MODES

#define NORMAL_TRAIL			0
#define NO_FADE_TRAIL			1
#define	BYPASS_TRAIL			2

// QUAD MODES

#define DISP_QUAD				0
#define CAPT_QUAD				1


class margModule : public ofThread {
	
public:
	
	// BASIC FUNCTIONS
	
	margModule();
	
	virtual ~margModule();
	
	void	init(int _camWidth, int _camHeight, int _dispWidth, int _dispHeight,
				 int captDev, int _dispID, string settingFilesPath, bool bExhibitionMode);
	
	void	update();
	void	threadedFunction();
	
	void	playVideos();
	
	void	draw(int x, int y, int w, int h);
	void	draw(int x, int y, int w, int h, bool bUndistorted);
	void	drawWhite(int x, int y, int w, int h);
	
	
	// SETUP FUNCTIONS
	
	void	updateSettings(); // Updates settings stored 'remotely'
	
	void	setSharedVarsAddresses(int* _blobMinArea, int* _blobMaxArea, int* _blobNConsidered,							// Shared between modules
								   float* _blobPairMaxDist, float* _blobPairMaxAreaDiff, float* _blobPairMaxUnfitness,	// Shared between modules
								   float* _blobDefScaleFactor,float* _blobCondScaleConst, float* _blobConsScaleMax,		// Shared between modules
								   float* _trailExpConst, float* _trailFadeConst, int* _trailBlurLevel,					// Shared between modules
								   int* _modMode,																		// Shared - for now
								   bool* _modDrawBlobs, int* _modDrawWhichBlobs,										// Shared between modules
								   bool* _modAdjQuad, int* _modAdjWhichQuad);
	
	void	changeCaptDev(int _captID);  // Updates capture device settings
	void	changeDispID(int _dispID);   // Updates display ID settings
	void	setBlobFinder(int _minArea, int _maxArea, int _nConsidered, int _threshMin, int _threshMax); // Sets blob finder parameters
	void	setCameraMatrix(float fX, float fY, float cX, float cY); // Sets Camera intrinsic parameters
	void	setDistCoeffs(float rdX, float rdY, float tgX, float tgY); // Sets Camera extrinsics
	void	setInterpolator(float _maxDist, float _maxAreaDiff, float _maxUnfitness,
							float _blobDefScaleFactor, float _blobCondScaleConst, float _blobCondScaleMax); // Sets interpolator parameters
	void	setTrailMaker(float _exposureConst, float _fadeConst, int _blurLevel); // Sets trailmaker parameters
	
	// Modes
	
	void	setMode(int _drawMode, bool _bDrawBlobs, int _drawWhichBlobs, bool _bAdjQuad, int _adjWhichQuad); // Sets drawing modes
	void	setInteractMode(int _interactMode);
	
	// VidPlayer
	
	void	setNextVidIndex(int idx);
	bool	getNeedToSetIndex();
	bool	getNeedToPlay();
	bool	getIsVidLoaded();
	
	// Quad adjustments
	
	void	selectQuadPoint(int x, int y, int origX, int origY, int w, int h);
	void	updateQuadPoint(int x, int y, int origX, int origY, int w, int h);
	void	clearQuad();
	void	saveQuad();
	
	// Capt Functions
	
	vector<string>& getCaptDevList();
	void	openCaptSettings();
	
	
	// UNIQUE SETTING VARS
	
	int		captID, dispID;
	int		blobFindThreshMin, blobFindThreshMax;
	float	correctFX, correctFY, correctCX, correctCY, correctRdX, correctRdY, correctTgX, correctTgY;
	
	float	moduleFPS;
	float	curModuleFr;
	
	int		interactMode;
	
	// SHARED RESOURCES
	
	unsigned char* originalPix;
	vector<margBlob> originalBlobs;
	vector<margBlob> correctedBlobs;
	vector<margBlob> interpolatedBlobs;
	unsigned char* trailMap;
	unsigned char* finalPixels;
	
	
	// TRIGGERING VARS
	
	bool	bNewOriginalPix,
			bNewOriginalBlobs,
			bNewCorrectedBlobs,
			bNewInterpolatedBlobs,
			bNewTrailMap,
			bNewFinalPixels;
	
	
	// SHARED SETTING VARS - Stored and managed externally
	
	//	int blobMinArea;
	//	int blobMaxArea;
	//	int blobNConsidered;
	//	
	//	float blobPairMaxDist;
	//	float blobPairMaxAreaDiff;
	//	float blobPairMaxUnfitness;
	//	
	//	float trailExpConst;
	//	float trailFadeConst;
	//	int trailBlurLevel;
	//	
	//	int modMode;
	//	
	//	bool modDrawBlobs;
	//	int modDrawWhichBlobs;
	//	
	//	bool modAdjQuad;
	//	int modAdjWhichQuad;
	
private:
	
	//SUBCLASSES

	margCapture				vidCapt;
	margBlobFinder			blobFind;
	margBlobCorrector		blobCorr;
	margBlobInterpolator	blobInterp;
	margTrailMaker			trailMaker;

	ofVideoPlayer			vidPlayer;
	
	margVideoBlender		vidBlender;
	margDisplay				display;
	margDisplay				testDisplay;
	
	margCoordWarping		captQuad;
	margCoordWarping		dispQuad;
	
	ofxCvGrayscaleImage		correctDisp;
	ofxCvGrayscaleImage		tempSource;
	
	
	//SETTING VARS
	
	int						buffCaptID, buffDispID;
	
	int						camWidth,  camHeight,
							dispWidth, dispHeight;
	
	string					filesPath;
	
	int						mode;
	
	bool					bExhibitionMode;
	
	bool					bDrawBlobs;
	int						drawWhichBlobs;
	
	bool					bAdjQuad;
	int						adjWhichQuad;
	
	
	//DYNAMIC VARS ADDRESSES
	
	int* blobMinArea;
	int* blobMaxArea;
	int* blobNConsidered;
	
	float* blobPairMaxDist;
	float* blobPairMaxAreaDiff;
	float* blobPairMaxUnfitness;
	
	float* blobDefScaleFactor;
	float* blobCondScaleConst;
	float* blobCondScaleMax;
	
	float* trailExpConst;
	float* trailFadeConst;
	int* trailBlurLevel;
	
	int* modMode;
	int  bufInteractMode;
	
	bool* modDrawBlobs;
	int* modDrawWhichBlobs;
	
	bool* modAdjQuad;
	int* modAdjWhichQuad;
	
	bool bAddressSet;
	
	
	//RESOURCES
	
	vector<margBlob>		blobs;
	
	unsigned char*			whitePix;
	
	


};