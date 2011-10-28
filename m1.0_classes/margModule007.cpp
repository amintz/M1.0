/*
 *  margMaster.cpp
 *  m1.0_setup
 *
 *  Created by André Mintz on 01/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "margModule007.h"

// ---------------------------------------------------

// --------------------------------------------------- ***
// BASE FUNCTIONS ------------------------------------ ***
// --------------------------------------------------- ***

// ---------------------------------------------------

margModule::margModule() {

}

// ---------------------------------------------------

margModule::~margModule() {
	correctDisp.clear();
	tempSource.clear();
}

// ---------------------------------------------------

void margModule::init(int _camWidth, int _camHeight, int _dispWidth, int _dispHeight,
					  int captDev, int _dispID, string settingFilesPath, bool _bExhibitionMode)				{
	
	dispID = _dispID;
	camWidth  = _camWidth;
	camHeight = _camHeight;
	dispWidth = _dispWidth;
	dispHeight= _dispHeight;
	
	filesPath = settingFilesPath;
	
	bExhibitionMode = _bExhibitionMode;
	
	interactMode = NORMAL_TRAIL;
	
	curModuleFr = 0;
	
	originalPix = new unsigned char[camWidth * camHeight];
	originalBlobs.reserve(10 * sizeof(margBlob));
	correctedBlobs.reserve(10 * sizeof(margBlob));
	interpolatedBlobs.reserve(10 * sizeof(margBlob));
	trailMap	= new unsigned char[dispWidth * dispHeight];
	finalPixels = new unsigned char[dispWidth * dispHeight * 3];
	
	// White Pix
	
	if(!bExhibitionMode) {
		whitePix = new unsigned char[dispWidth * dispHeight];
		
		for (int i = 0; i < dispWidth * dispHeight; i++) {
			whitePix[i] = 255;
		}
	}
	
	// -- CAPTURE
	
	captID = captDev;
	vidCapt.setCaptDev(captDev);
	vidCapt.init(camWidth, camHeight, captDev, bExhibitionMode);
	vidCapt.setGrayscalePixelStorage(originalPix, bNewOriginalPix);
	vidCapt.startThread(false, false);
	
	// -- CAPTURE WARP
	
	captQuad.init(camWidth, camHeight, false);
	captQuad.loadQuad(filesPath + "warp-cam_" + ofToString(captDev) + ".xml");
	
	// -- BLOB FINDER
	
	blobFind.init(camWidth, camHeight);
	blobFind.setPixelStorage(originalPix, bNewOriginalPix);
	blobFind.setBlobStorage(originalBlobs, bNewOriginalBlobs);
	
	// -- BLOB CORRECTOR
	
	blobCorr.init();
	blobCorr.setInDimensions(camWidth, camHeight);
	blobCorr.setOutDimensions(dispWidth, dispHeight);
	blobCorr.loadMats(filesPath + "calibration_matrix.xml");
	blobCorr.setPersCorrection(captQuad.getTranslateMat());
	blobCorr.setMode(true, true, true);
	
	// -- BLOB INTERPOLATOR
	
	blobInterp.init(dispWidth, dispHeight);
	
	// -- TRAIL MAKER
	
	trailMaker.init(dispWidth, dispHeight);
	
	// -- VIDEO PLAYER
	
	//vidPlayer.init(filesPath, filesPath + "movies/", dispID);
	vidPlayer.setLoopState(OF_LOOP_NONE);
	vidPlayer.setUseTexture(false);
	vidPlayer.loadMovie(filesPath + "movies/m" + ofToString(dispID) + "g0v0.mov");
	vidPlayer.play();
	
	// -- VIDEO BLENDER
	
	vidBlender.init(dispWidth, dispHeight);
	
	// -- DISPLAY WARP
	
	dispQuad.init(dispWidth, dispHeight, true);
	dispQuad.loadQuad(filesPath + "warp-disp_" + ofToString(dispID) + ".xml");
	
	// -- DISPLAY
	
	display.init(dispWidth, dispHeight, bExhibitionMode);
	display.setTranslateMat(dispQuad.getTranslateMat());
	
	if(!bExhibitionMode) {
		testDisplay.init(dispWidth, dispHeight);
		testDisplay.setTranslateMat(dispQuad.getTranslateMat());
		testDisplay.feedImg(whitePix, dispWidth, dispHeight, false);
	}

	// -- DEFAULTS
	
	mode = FINAL_IMAGE;
	bDrawBlobs = false;
	bAddressSet = false;
}

// -------------------------------------------------

void margModule::update() {
	
	// --
	
	vidPlayer.update();
	
	//setInteractMode(vidPlayer.getInteractMode());
	
	if (interactMode == NORMAL_TRAIL || interactMode == NO_FADE_TRAIL) {
		blobFind.feedPixels(originalPix);
		blobFind.findContours();
		blobs = blobFind.getFoundBlobs();
		blobCorr.feedBlobs(blobs);
		blobs = blobCorr.getCorrectBlobs();
		blobInterp.feedBlobs(blobs);
		blobs = blobInterp.getInterpolatedBlobs();
		if(mode == TRAIL_MAP || mode == FINAL_IMAGE || bExhibitionMode) trailMaker.updtMap(blobs);
		if (vidPlayer.isPlaying() && (mode == FINAL_IMAGE || bExhibitionMode)) {
			vidBlender.blendVideo(trailMaker.getMap(), vidPlayer.getPixels());
		}
	}

	// --
	
	if (bExhibitionMode) {
		if (interactMode == NORMAL_TRAIL || interactMode == NO_FADE_TRAIL) {
			display.feedImg(vidBlender.getPixels(), dispWidth, dispHeight);
		}
		else {
			display.feedImg(vidPlayer.getPixels(), dispWidth, dispHeight);
		}
	}
	else {
		switch (mode) {
			case SOURCE:
				display.feedImg(vidCapt.getPixels(), camWidth, camHeight, false);
				break;
			case LENSCORRECTED_SOURCE:
				if (correctDisp.getWidth() == 0) correctDisp.allocate(camWidth, camHeight);
				correctDisp.setFromPixels(vidCapt.getPixels(), camWidth, camHeight);
				correctDisp.undistort(correctRdX, correctRdY, correctTgX, correctTgY, correctFX, correctFY, correctCX, correctCY);
				display.feedImg(correctDisp.getPixels(), camWidth, camHeight, false);
				break;
			case WARPED_SOURCE:
				if (correctDisp.getWidth() == 0) correctDisp.allocate(camWidth, camHeight);
				tempSource.allocate(camWidth, camHeight);
				tempSource.setFromPixels(vidCapt.getPixels(), camWidth, camHeight);
				tempSource.undistort(correctRdX, correctRdY, correctTgX, correctTgY, correctFX, correctFY, correctCX, correctCY);
				cvWarpPerspective(tempSource.getCvImage(), correctDisp.getCvImage(), blobCorr.getPerspMat());
				tempSource.clear();
				display.feedImg(correctDisp.getPixels(), camWidth, camHeight, false);
				break;
			case THRESHOLD_SOURCE:
				display.feedImg(blobFind.getInputImg().getPixels(), camWidth, camHeight, false);
				break;
			case TRAIL_MAP:
				display.feedImg(trailMaker.getMap(), dispWidth, dispHeight, false);
				break;
			case FINAL_IMAGE:
				if(interactMode == NORMAL_TRAIL || interactMode == NO_FADE_TRAIL) {
					display.feedImg(vidBlender.getPixels(), dispWidth, dispHeight);
				}
				else if (interactMode == BYPASS_TRAIL) {
					display.feedImg(vidPlayer.getPixels(), dispWidth, dispHeight);
				}
				break;
			case BYPASS_VIDEO:
				if(vidPlayer.getWidth()!=0) display.feedImg(vidPlayer.getPixels(), dispWidth, dispHeight);
				break;
			default:
				//display.feedImg(vidBlender.getPixels(), dispWidth, dispHeight);
				break;
		}
	}

	if(!bExhibitionMode) {
		curModuleFr++;
		moduleFPS = curModuleFr / ofGetElapsedTimef();
	}
}

// ------------------------------------------------

void margModule::threadedFunction() {
	while (isThreadRunning()!= 0) {
		if (lock()) {
			update();
			unlock();
		}
	}
}

// ------------------------------------------------

void margModule::playVideos() {
	vidPlayer.play();
}

// ------------------------------------------------

void margModule::draw(int x, int y, int w, int h) {
	ofSetColor(255, 255, 255);
	display.draw(x, y, w, h);
	if (bAdjQuad) {
		switch (adjWhichQuad) {
			case DISP_QUAD:
				dispQuad.drawQuad(x, y, w, h);
				break;
			case CAPT_QUAD:
				captQuad.drawQuad(x, y, w, h);
				break;
			default:
				captQuad.drawQuad(x, y, w, h);
				break;
		}
	}
	if (bDrawBlobs) {
		switch (drawWhichBlobs) {
			case ORIGINAL_BLOBS:
				blobFind.drawBlobs(x, y, w, h);
				break;
			case CORRECTED_BLOBS:
				blobCorr.drawBlobs(x, y, w, h);
				break;
			case INTERPOLATED_BLOBS:
				blobInterp.drawInterp(x, y, w, h);
				break;
			default:
				break;
		}
	}
}

// ------------------------------------------------

void margModule::drawUndistorted(int x, int y, int w, int h) {

}

// ------------------------------------------------

void margModule::drawWhite(int x, int y, int w, int h) {
	ofSetColor(255, 255, 255);
	testDisplay.draw(x, y, w, h);
	if (bAdjQuad) {
		switch (adjWhichQuad) {
			case DISP_QUAD:
				dispQuad.drawQuad(x, y, w, h);
				break;
			case CAPT_QUAD:
				captQuad.drawQuad(x, y, w, h);
				break;
			default:
				dispQuad.drawQuad(x, y, w, h);
				break;
		}
	}
}

// ------------------------------------------------

// ------------------------------------------------ ***
// SETUP FUNCTIONS -------------------------------- ***
// ------------------------------------------------ ***

// ------------------------------------------------

void margModule::updateSettings() {
	if(bAddressSet) {
		setBlobFinder(*blobMinArea, *blobMaxArea, *blobNConsidered, blobFindThreshMin, blobFindThreshMax);
		setCameraMatrix(correctFX, correctFY, correctCX, correctCY);
		setDistCoeffs(correctRdX, correctRdY, correctTgX, correctTgY);
		setInterpolator(*blobPairMaxDist, *blobPairMaxAreaDiff, *blobPairMaxUnfitness);
		setTrailMaker(*trailExpConst, *trailFadeConst, *trailBlurLevel);
		setMode(*modMode, *modDrawBlobs, *modDrawWhichBlobs, *modAdjQuad, *modAdjWhichQuad);
		if (bufInteractMode != interactMode) setInteractMode(interactMode);
	}
}

// ------------------------------------------------

void margModule::setSharedVarsAddresses(int* _blobMinArea, int* _blobMaxArea, int* _blobNConsidered,							// Shared between modules
										float* _blobPairMaxDist, float* _blobPairMaxAreaDiff, float* _blobPairMaxUnfitness,		// Shared between modules
										float* _trailExpConst, float* _trailFadeConst, int* _trailBlurLevel,					// Shared between modules
										int* _modMode,																			// Shared - for now
										bool* _modDrawBlobs, int* _modDrawWhichBlobs,											// Shared between modules
										bool* _modAdjQuad, int* _modAdjWhichQuad)												// Shared between modules
{ 
	
	blobMinArea = _blobMinArea;
	blobMaxArea = _blobMaxArea;
	blobNConsidered = _blobNConsidered;
	
	blobPairMaxDist = _blobPairMaxDist;
	blobPairMaxAreaDiff = _blobPairMaxAreaDiff;
	blobPairMaxUnfitness = _blobPairMaxUnfitness;
	
	trailExpConst = _trailExpConst;
	trailFadeConst = _trailFadeConst;
	trailBlurLevel = _trailBlurLevel;
	
	modMode = _modMode;
	
	modDrawBlobs = _modDrawBlobs;
	modDrawWhichBlobs = _modDrawWhichBlobs;
	
	modAdjQuad = _modAdjQuad;
	modAdjWhichQuad = _modAdjWhichQuad;
	
	bAddressSet = true;
}
								  
// ------------------------------------------------

void margModule::changeCaptDev(int _captID) {
	if (_captID != buffCaptID) {
		buffCaptID = captID = _captID;
		vidCapt.changeDevice(captID);
		captQuad.loadQuad(filesPath + "warp-cam_" + ofToString(captID) + ".xml");
	}
}

// ------------------------------------------------

void margModule::changeDispID(int _dispID) {
	if (_dispID != buffDispID) {
		buffDispID = dispID = _dispID;
		dispQuad.loadQuad(filesPath + "warp-disp_" + ofToString(dispID) + ".xml");
	}
}

// ------------------------------------------------

void margModule::setBlobFinder(int _minArea, int _maxArea, int _nConsidered,
							   int _threshMin, int _threshMax) {
	blobFind.setFindContours(_minArea, _maxArea, _nConsidered, false, true, _threshMin, _threshMax);
}

// ------------------------------------------------

void margModule::setCameraMatrix(float fX, float fY, float cX, float cY) {
	blobCorr.setCameraMatrix(fX, fY, cX, cY);
}

// -----------------------------------------------

void margModule::setDistCoeffs(float rdX, float rdY, float tgX, float tgY) {
	blobCorr.setDistCoeffs(rdX, rdY, tgX, tgY);
}

// -----------------------------------------------

void margModule::setInterpolator(float _maxDist, float _maxAreaDiff, float _maxUnfitness) {
	blobInterp.setInterpolator(_maxDist, _maxAreaDiff, _maxUnfitness);
}

// -----------------------------------------------

void margModule::setTrailMaker(float _exposureConst, float _fadeConst, int _blurLevel) {
	trailMaker.setTrailMaker(_exposureConst, _fadeConst, _blurLevel);
}

// -----------------------------------------------

vector<string>& margModule::getCaptDevList() {
	//return vidCapt.getDeviceList();
}

// -----------------------------------------------

void margModule::openCaptSettings() {
	vidCapt.openGrabberSettings();
}

// -----------------------------------------------

void margModule::clearQuad() {
	if (bAdjQuad) {
		if(adjWhichQuad == CAPT_QUAD) captQuad.clearQuad();
		else dispQuad.clearQuad();
	}
}

// -----------------------------------------------

void margModule::selectQuadPoint(int x, int y, int origX, int origY, int w, int h) {
	if (bAdjQuad) {
		if (adjWhichQuad == CAPT_QUAD) captQuad.selectPoint(x, y, origX, origY, w, h, 20);
		else dispQuad.selectPoint(x, y, origX, origY, w, h, 20);
	}
}
	
// -----------------------------------------------

void margModule::updateQuadPoint(int x, int y, int origX, int origY, int w, int h) {
	if (bAdjQuad) {
		if (adjWhichQuad == CAPT_QUAD) {
			captQuad.updatePoint(x, y, origX, origY, w, h);
			blobCorr.setPersCorrection(captQuad.getTranslateMat());
		}
		else {
			dispQuad.updatePoint(x, y, origX, origY, w, h);
			display.setTranslateMat(dispQuad.getTranslateMat());
			if(!bExhibitionMode) {
				testDisplay.setTranslateMat(dispQuad.getTranslateMat());
				testDisplay.feedImg(whitePix, dispWidth, dispHeight, false);
			}
		}
	}
}

// -----------------------------------------------

void margModule::saveQuad() {
	captQuad.saveQuad(filesPath + "warp-cam_" + ofToString(captID) + ".xml");
	dispQuad.saveQuad(filesPath + "warp-disp_" + ofToString(dispID) + ".xml");
}

// -----------------------------------------------
void margModule::setMode(int _mode, bool _bDrawBlobs, int _drawWhichBlobs, bool _bAdjQuad, int _adjWhichQuad) {
	mode = _mode;
	bDrawBlobs = _bDrawBlobs;
	drawWhichBlobs = _drawWhichBlobs;
	bAdjQuad = _bAdjQuad;
	adjWhichQuad = _adjWhichQuad;
}

// -----------------------------------------------

void margModule::setInteractMode(int _interactMode) {
	interactMode = _interactMode;
	switch (interactMode) {
		case NORMAL_TRAIL:
			trailMaker.setTrailMaker(*trailExpConst, *trailFadeConst, *trailBlurLevel, true);
			break;
		case NO_FADE_TRAIL:
			trailMaker.setTrailMaker(*trailExpConst, *trailFadeConst, *trailBlurLevel, false);
			break;
		case BYPASS_TRAIL:
			break;
		default:
			trailMaker.setTrailMaker(*trailExpConst, *trailFadeConst, *trailBlurLevel, true);
			break;
	}	
}

// -----------------------------------------------

void margModule::setNextVidIndex(int idx) {
	//vidPlayer.setNextIndex(idx);
}

// -----------------------------------------------

bool margModule::getNeedToSetIndex() {
	//return vidPlayer.getNeedToSetIndex();
	return false;
}

// -----------------------------------------------

bool margModule::getNeedToPlay() {
	//return vidPlayer.getNeedToPlay();
	return false;
}

// -----------------------------------------------

bool margModule::getIsVidLoaded() {
	return vidPlayer.isLoaded();
}