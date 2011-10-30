#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// SET NUMBER OF MODULES HERE ------------------------------*
	
	numMod = 1; // -------------------------------------------***
	
	// SET OSC HERE --------------------------------------------*
	
	oscHost = "localhost"; // -------------------------------***
	oscPort = 2222; // ---------------------------------------***
	
	// ---------------------------------------------------------*
	
	// DRAWING VARS --------------------------------------------*
	
	camWidth 		= 640;
	camHeight 		= 480;
	
	dispWidth		= 1280;
	dispHeight		= 720;
	
	drawWidth		= 640;
	drawHeight		= 360;
	drawX			= 500;
	drawY			= 115;
	
	screenWidth		= (numMod + 1) * dispWidth;
	screenHeight	= dispHeight;
	
	ofSetWindowShape(screenWidth, screenHeight);
	
	
	// ---------------------------------------------------------*
	
	// OBJECT ARRAYS -------------------------------------------*
	
	modules = new margModule[numMod];
	
	
	// SETUP VARS ----------------------------------------------*
	
	curMod = -1;
	
	modes[0] = "SOURCE";
	modes[1] = "LENSCORRECTED_SOURCE";
	modes[2] = "WARPED_SOURCE";
	modes[3] = "THRESHOLD_SOURCE";
	modes[4] = "TRAIL_MAP";
	modes[5] = "FINAL_IMAGE";
	modes[6] = "BYPASS_VIDEO";
	
	interactModes[0] = "NORMAL_TRAIL";
	interactModes[1] = "NO_FADE_TRAIL";
	interactModes[2] = "BYPASS_TRAIL";
	
	quads[0] = "DISP_QUAD";
	quads[1] = "CAPT_QUAD";
	
	blobsToDraw[0] = "ORIGINAL_BLOBS";
	blobsToDraw[1] = "CORRECTED_BLOBS";
	blobsToDraw[2] = "INTERPOLATED_BLOBS";
	
	
	// MODULE INITIALIZATION -----------------------------------*
	
	for (int i = 0; i < numMod; i++) {
		modules[i].init(camWidth, camHeight, dispWidth, dispHeight, i+1, i, "../../../../M1.0_data/", false);
		modules[i].setSharedVarsAddresses(&minBlob, &maxBlob, &numBlob,
										  &maxDist, &maxAreaDiff, &maxUnfitness,
										  &blobDefScaleFactor,
										  &exposureConst, &fadeConst, &blurLevel,
										  &displayMode, &bDrawBlobs, &whichBlobs,
										  &bAdjQuad, &whichQuad);
	}
	
	if (checkEveryModNeedPlay()) {
		for (int i = 0; i < numMod; i++) {
			modules[i].playVideos();
		}
	}
	
	// INTERFACE SETUP ----------------------------------------*
	
	gui.addComboBox("Display Mode", displayMode, 7, modes);
	gui.addToggle("Control Display", bControlDisplay);
	gui.addToggle("External Display", bExtDisplay);
	gui.addToggle("Run Thread", bRunThread);
	gui.addToggle("Stop Thread", bStopThread);
	gui.addToggle("Update Settings", bUpdateSettings);
	gui.addToggle("Draw White", bDrawWhite);
	
	for (int i = 0; i < numMod; i++) {
		gui.addSlider("Module " + ofToString(i) + " FPS", (float&)modules[i].moduleFPS, 0, 1);
	}
	
	for (int i = 0 ; i < numMod; i++) {
		gui.addPage("Capture " + ofToString(i));
		//gui.addComboBox("Capture Device", 0, 0, "");
		gui.addButton("Video Settings", opVidSet);
		gui.addComboBox("Interaction Mode", modules[i].interactMode, 3, interactModes);
		gui.addToggle("Adj Quad", bAdjQuad);
		gui.addComboBox("Which Quad to Adj", whichQuad, 2, quads);
		gui.addButton("Clear Quad", clearQuad);
		gui.addSlider("Threshold Min", modules[i].blobFindThreshMin, 0, 255);
		gui.addSlider("Threshold Max", modules[i].blobFindThreshMax, 0, 255);
		gui.addTitle("Lens Correction");
		gui.addSlider("RadialDistX", (float&)modules[i].correctRdX, (float)-1.000, (float)1.000);
		gui.addSlider("RadialDistY", (float&)modules[i].correctRdY, (float)-1.000, (float)1.000);
		gui.addSlider("TangentDistX", (float&)modules[i].correctTgX, (float)-1.000, (float)1.000);
		gui.addSlider("TangentDistY", (float&)modules[i].correctTgY, (float)-1.000, (float)1.000);
		gui.addSlider("FocalX", (float&)modules[i].correctFX, (float)0.000, (float)camWidth);
		gui.addSlider("FocalY", (float&)modules[i].correctFY, (float)0.000, (float)camHeight);
		gui.addSlider("CenterX", (float&)modules[i].correctCX, (float)0.000, (float)camWidth);
		gui.addSlider("CenterY", (float&)modules[i].correctCY, (float)0.000, (float)camHeight);
	}
	
	gui.addPage("Blob Detection");
	gui.addSlider("Blob Min Area", minBlob, 0, 307200);
	gui.addSlider("Blob Max Area", maxBlob, 0, 307200);
	gui.addSlider("Blob Max Number", numBlob, 0, 20);
	
	gui.addTitle("Blob Tracking");
	gui.addSlider("Max Area Diff", maxAreaDiff, 0, 100);
	gui.addSlider("Max Dist", maxDist, 0, 150);
	
	gui.addTitle("Blob Scaling");
	gui.addSlider("Default Scale Factor", blobDefScaleFactor, 0.5, 4.0);
	
	gui.addTitle("Blob Drawing");
	gui.addToggle("Draw Blobs", bDrawBlobs);
	gui.addComboBox("Which Blobs to Draw", whichBlobs, 3, blobsToDraw);
	
	gui.addTitle("Trail");
	gui.addSlider("Exp Const", exposureConst, 0, 15);
	gui.addSlider("Fade Const", fadeConst, 0, 1.0);
	gui.addSlider("Blur Level", blurLevel, 0, 10);
	
	gui.loadFromXML();
	
	gui.show();
	gui.setAutoSave(false);
	
	// ---------------------------------------------------------*
	
	for(int i = 0; i < numMod; i++) {
		modules[i].updateSettings();
	}
	
	curVidGroup = 0;
	nextVidGroup = 1;
	curVidIdx = 0;
	
	oscMessage.setAddress("/sound/group" + ofToString(curVidGroup));
	oscMessage.addIntArg(curVidIdx);

}

//--------------------------------------------------------------
void testApp::update(){
	// CURRENT MODULE INDEX UPDATE -----------------------------*
	
	if ((gui.currentPageIndex - 2) >= 0 && (gui.currentPageIndex -2) < numMod) {
		curMod = gui.currentPageIndex -2;
	}
	else if (curMod == -1) {
		curMod = 0;
	}
	
	// ---------------------------------------------------------*
	
	// RESPONSE TO DEMANDS -------------------------------------*
	
	// VIDEO SETTINGS
	
	if (opVidSet) {
		modules[curMod].openCaptSettings();
		opVidSet = false;
	}
	
	// QUAD RESET
	
	if (clearQuad) {
		modules[curMod].clearQuad();
		clearQuad = false;
	}
	
	// ---------------------------------------------------------*
	
	// EACH MODULE UPDATE --------------------------------------*
	
	
	if (bRunThread) {
		for (int i = 0; i < numMod; i++) {
			modules[i].startThread(true, false);
		}
		bRunThread = false;
	}
	if (bStopThread) {
		for (int i = 0; i < numMod; i++) {
			modules[i].stopThread(false);
		}
		bStopThread = false;
	}
	for (int i = 0; i < numMod; i++) {
		if (!modules[i].isThreadRunning()) {
			modules[i].update();
			if(bUpdateSettings)modules[i].updateSettings();
		}
	}
	
	if (checkEveryModNeedPlay()) {
		//curVidIdx = nextVidIdx;
		//curVidGroup = nextVidGroup;
		//nextVidGroup = nextVidGroup + 1 > 2? 0 : nextVidGroup + 1; 
		
		//oscSender.sendMessage(oscMessage);
		
		oscMessage.clear();
		oscMessage.setAddress("/sound"+ofToString(curVidGroup));
		oscMessage.addIntArg(curVidIdx);
		
		for (int i = 0; i < numMod; i++) {
			modules[i].playVideos();
		}
		
		//oscSender.sendMessage(oscMessage);
	}
	
//	if (checkEveryModNeedVidIndex()) {
//		nextVidIdx = ofRandom(0, 200);
//		for (int i = 0; i < numMod; i++) {
//			modules[i].setNextVidIndex(nextVidIdx);
//		}
//	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	// DRAW BACKGROUND / CLEAR ---------------------------------*
	
	ofBackground(0, 0, 0);
	
	// DRAW CONTROL DISPLAY ------------------------------------*
	
	ofSetColor(255, 255, 255);
	
	if(bControlDisplay) modules[curMod].draw(drawX, drawY, drawWidth, drawHeight);
	
	// DRAW EXTERNAL DISPLAYS IF APPLICABLE --------------------*
	
	if (bExtDisplay) {
		for (int i = 0; i < numMod; i++) {
			modules[i].draw(dispWidth * (i + 1), 0, dispWidth, dispHeight);
		}
	}
	
	if(bDrawWhite) {
		modules[curMod].drawWhite(dispWidth * (curMod + 1), 0, dispWidth, dispHeight);
	}
	
	// DRAW PERSPECTIVE CONTROLS IF APPLICABLE -----------------*
	
	gui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	// CHANGE GUI PAGE BY THE NUMBER
	if(key>='0' && key<='9') {
		gui.setPage(key - '0');
		gui.show();
	} 
	
	// OTHER CALLS
	else {
		switch(key) {
			case ' ': gui.toggleDraw(); break;
			case '[': gui.prevPage(); break;
			case ']': gui.nextPage(); break;
			case 'p': gui.nextPageWithBlank(); break;
			case 'f':
				ofToggleFullscreen();
				ofSetWindowShape(screenWidth, screenHeight);
				break;
			case 's':
				modules[curMod].saveQuad();
				gui.saveToXML();
				break;
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if(bAdjQuad && bUpdateSettings) {
		if (x < dispWidth) {
			modules[curMod].updateQuadPoint(x, y, drawX, drawY, drawWidth, drawHeight);
		}
		else {
			for (int i = 0; i < numMod; i++) {
				modules[i].updateQuadPoint(x, y, dispWidth * (i+1), 0, dispWidth, dispHeight);
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if(bAdjQuad && bUpdateSettings) {
		if (x < dispWidth) {
			modules[curMod].selectQuadPoint(x, y, drawX, drawY, drawWidth, drawHeight);
		}
		else {
			for (int i = 0; i < numMod; i++) {
				modules[i].selectQuadPoint(x, y, dispWidth * (i+1), 0, dispWidth, dispHeight);
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
// -------------------------------------------------------------

bool testApp::checkEveryModNeedPlay() {
	
	bool need = true;
	
	for (int i = 0 ; i < numMod; i++) {
		need = modules[i].getNeedToPlay();
	}
	
	return need;
}

// -------------------------------------------------------------

bool testApp::checkEveryModNeedVidIndex() {	
	
	bool need = true;
	
	for (int i = 0; i < numMod; i++) {
		if(!modules[i].getNeedToSetIndex()) need = false;
	}
	
	return need;
}
