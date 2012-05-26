#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// XML LOAD ------------------------------------------------*
	
	XML.loadFile("app_settings.xml");
	
	// SET NUMBER OF MODULES HERE ------------------------------*

	XML.pushTag("all", 0);
	modIdx = XML.getValue("modIdx", 0, 0);
	filesPath = XML.getValue("filesPath", "", 0);
	XML.popTag();
	
	// SET OSC HERE --------------------------------------------*
	
	XML.pushTag("all", 0);
		oscSendHost		= XML.getValue("oscHost", "localhost", 0);
		oscSendPort		= XML.getValue("oscPort", 8000, 0);
		oscListenPort	= XML.getValue("oscListenPort", 8001 + modIdx, 0);
		oscMaxMessages	= XML.getValue("oscMaxMessages", 20, 0);
	XML.popTag();
	
	oscSender.setup(oscSendHost, oscSendPort);
	oscReceiver.setup(oscListenPort);
	
	messageStrings  = new string [oscMaxMessages];
	timers			= new float [oscMaxMessages];
	
	// SYNC
	
	bNeedToCheckIn = true;
	
	// MESSAGES ---------------------------------------------*
	
	ctrlMsg = new string[9];
	
	ctrlMsg[ASK_CHECKIN]	= "/ask/checkin";
	ctrlMsg[RCV_CHECKIN]	= "/rcv/checkin";
	ctrlMsg[ASK_NEEDPLAY]	= "/ask/needplay";
	ctrlMsg[RCV_NEEDPLAY]	= "/rcv/needplay";
	ctrlMsg[ASK_PLAY]		= "/ask/play";
	ctrlMsg[RCV_ISPLAYING]	= "/rcv/isPlaying";
	ctrlMsg[ASK_SND_PLAY]	= "/sound/play";
	ctrlMsg[ASK_SHUTDOWN]	= "/ask/quit";
	ctrlMsg[ASK_SND_STOP]	= "/sound/stop";
	
	myMsg = new string[4];
	
	myMsg[MOD_CHECKIN]		= "/m" + ofToString(modIdx) + "/checkin";
	myMsg[MOD_NEEDPLAY]		= "/m" + ofToString(modIdx) + "/needPlay";
	myMsg[MOD_ISPLAYING]	= "/m" + ofToString(modIdx) + "/isPlaying";
	myMsg[MOD_FPS]			= "/m" + ofToString(modIdx) + "/FPS";
	
	
	// DRAWING VARS --------------------------------------------*
	
	camWidth 		= 640;
	camHeight 		= 480;
	
	dispWidth		= 1280;
	dispHeight		= 720;
	
	ofSetWindowPosition(1280 * (modIdx+1), 0);
	ofToggleFullscreen();
	
	// ---------------------------------------------------------*
	
	// MODULE INITIALIZATION -----------------------------------*
	
	module = new margModule[1];
	
	XML.pushTag("module", 0);
		
	int capt = XML.getValue("captDev", -1, 0);
	string moviePath = XML.getValue("moviePath", filesPath + "movies/", 0);
	
	XML.popTag();
	
	module[0].init(camWidth, camHeight, dispWidth, dispHeight, capt, modIdx, filesPath, moviePath, false);
	module[0].setSharedVarsAddresses(&minBlob, &maxBlob, &numBlob,
								  &maxDist, &maxAreaDiff, &maxUnfitness,
								  &blobDefScaleFactor, &blobCondScaleConst, &blobCondScaleMax,
								  &exposureConst, &fadeConst, &blurLevel,
								  &displayMode, &bDynInteractMode,
								  &bDrawBlobs, &whichBlobs,
								  &bAdjQuad, &whichQuad);
	texture.allocate(dispWidth, dispHeight, GL_RGB);
	
	unsigned char* blackPix = new unsigned char[dispWidth * dispHeight * 3];
	for (int i = 0; i < dispWidth * dispHeight * 3; i++) {
		blackPix[i] = 0;
	}
	texture.loadData(blackPix, dispWidth, dispHeight, GL_RGB);
	
	delete[] blackPix;
	
	displayMode			= 5;
	bDynInteractMode	= true;
	bDrawBlobs			= false;
	whichBlobs			= 0;
	bAdjQuad			= false;
	whichQuad			= 0;
	bDrawUndistortBounds= false;
	module[0].interactMode = 2;
	
	bUpdateModule = false;
	bAwareNeedPlay= false;
	bIsPlaying	  = false;
	bGotPlay	  = false;
	
	// LOAD SETTINGS
	
	XML.clear();
	XML.loadFile(filesPath + "Blob Detection_settings.xml");
	
	XML.pushTag("controls", 0);
		XML.pushTag("SliderInt_Blob_Min_Area", 0);
			minBlob = XML.getValue("value", 0, 0);
		XML.popTag();
		XML.pushTag("SliderInt_Blob_Max_Area", 0);
			maxBlob = XML.getValue("value", 100000, 0);
		XML.popTag();
		XML.pushTag("SliderInt_Blob_Max_Number", 0);
			numBlob = XML.getValue("value", 4, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_Max_Area_Diff", 0);
			maxAreaDiff = XML.getValue("value", 10, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_Max_Dist", 0);
			maxDist = XML.getValue("value", 50, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_Def._Scale_Factor", 0);
			blobDefScaleFactor = XML.getValue("value", 1, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_Cond._Scale_Const.", 0);
			blobCondScaleConst = XML.getValue("value", 0.045, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_Cond._Scale_Max.", 0);
			blobCondScaleMax = XML.getValue("value", 2.225, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_Exp_Const", 0);
			exposureConst = XML.getValue("value", 1.2, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_Fade_Const", 0);
			fadeConst = XML.getValue("value", 0.97, 0);
		XML.popTag();
		XML.pushTag("SliderInt_Blur_Level", 0);
			blurLevel = XML.getValue("value", 5, 0);
		XML.popTag();
	XML.popTag();
	
	XML.clear();
	
	XML.loadFile(filesPath + "Capture " + ofToString(modIdx) + "_settings.xml");
	
	XML.pushTag("controls", 0);
		XML.pushTag("SliderInt_Threshold_Min", 0);
			module[0].blobFindThreshMin = XML.getValue("value", 130, 0);
		XML.popTag();
		XML.pushTag("SliderInt_Threshold_Max", 0);
			module[0].blobFindThreshMax = XML.getValue("value", 255, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_RadialDistX", 0);
			module[0].correctRdX = XML.getValue("value", 0.12, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_RadialDistY", 0);
			module[0].correctRdY = XML.getValue("value", 0.02, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_TangentDistX", 0);
			module[0].correctTgX = XML.getValue("value", 0.0, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_TangentDistY", 0);
			module[0].correctTgY = XML.getValue("value", 0.0, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_FocalX", 0);
			module[0].correctFX = XML.getValue("value", 320,	0);
		XML.popTag();
		XML.pushTag("SliderFloat_FocalY", 0);
			module[0].correctFY = XML.getValue("value", 240, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_CenterX", 0);
			module[0].correctCX = XML.getValue("value", 320, 0);
		XML.popTag();
		XML.pushTag("SliderFloat_CenterY", 0);
			module[0].correctCY = XML.getValue("value", 240, 0);
		XML.popTag();
	XML.popTag();
	
	module[0].updateSettings();
	
}

//--------------------------------------------------------------

void testApp::update(){
	
	// hide old messages
	for ( int i=0; i<oscMaxMessages; i++ )
	{
		if ( timers[i] < ofGetElapsedTimef() )
			messageStrings[i] = "";
	}
	
	// check for waiting messages
	while( oscReceiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		oscReceiver.getNextMessage( &m );
		
		cout << "RCV: " << m.getAddress() << endl;
		
		// check for possibilities
		
		// if message asks to play
		
		if (m.getAddress() == ctrlMsg[ASK_PLAY]) {
			if(!bGotPlay) {										// If they haven't told us yet
				module[0].playVideos();							// We play
				bUpdateModule = true;							// We take note to keep it running
				bGotPlay = true;								// We take note we have got this message
				oscMessage.clear();								// Respond to control
				oscMessage.setAddress(myMsg[MOD_ISPLAYING]);	// We are playing
				oscSender.sendMessage(oscMessage);
				cout << "SNT: " << oscMessage.getAddress() << endl;
				bAwareNeedPlay = false;
			}
		}
		
		// If control tells us we are checked in
		else if (m.getAddress() == ctrlMsg[RCV_CHECKIN]) {
			bNeedToCheckIn = false;	// No longer check in
		}
		
		// If control asks us to tell if we need play
		else if (m.getAddress() == ctrlMsg[ASK_NEEDPLAY]) {
			bAwareNeedPlay = false; // Take note to tell them soon enough
		}
		
		// If control tells us they know we need play
		else if (m.getAddress() == ctrlMsg[RCV_NEEDPLAY]) {
			bAwareNeedPlay = true;	// No longer tell them that
		}
		
		// If they tell us to die
		else if (m.getAddress() == ctrlMsg[ASK_SHUTDOWN]) {
			std::exit(0); // Die
		}
	}
	
	// If we need to checkin and if video is loaded
	
	if (bNeedToCheckIn && module[0].getIsVidLoaded()) {
		
		// Tell control we are listening
		
		oscMessage.clear();
		oscMessage.setAddress(myMsg[MOD_CHECKIN]);
		oscSender.sendMessage(oscMessage);
		cout << "SNT: " << oscMessage.getAddress() << endl;
		
	}
	
	// If we are in but they don't know we need play
	
	if (!bNeedToCheckIn && !bAwareNeedPlay) {
		if (module[0].getNeedToPlay()) {				// Find out if we do need play
			oscMessage.clear();
			oscMessage.setAddress(myMsg[MOD_NEEDPLAY]); // Tell them, if we do
			oscSender.sendMessage(oscMessage);
			bIsPlaying = false;							// Take note that we are NOT playing yet
			cout << "SNT: " << oscMessage.getAddress() << endl;
			bGotPlay = false;							// Take note we are waiting for control to respond
		}
	}
	
	
	// Tell control how fast we are going
	
	if (!bNeedToCheckIn) {
		oscMessage.clear();
		oscMessage.setAddress(myMsg[MOD_FPS]);
		oscMessage.addFloatArg(module[0].moduleFPS);
		oscMessage.addFloatArg(ofGetFrameRate());
		oscSender.sendMessage(oscMessage);
	}
		
	// If we must be updated
	
	if(bUpdateModule) {
		if(!module[0].isThreadRunning()) module[0].startThread(true, false);	// Start thread if it isn't running
		if(module[0].getIsPlaying()) texture.loadData(module[0].getPixels(), dispWidth, dispHeight, GL_RGB);	// Load data from module into the texture
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	// DRAW BACKGROUND / CLEAR ---------------------------------*
	
	ofBackground(0, 0, 0);
	
	// DRAW DISPLAY ------------------------------------*
	
	ofSetColor(255, 255, 255);
	
	texture.draw(0, 0, dispWidth, dispHeight);
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == 'F') {
		ofToggleFullscreen();
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
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
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
