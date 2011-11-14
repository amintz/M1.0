#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(60);
	
	
	// XML LOAD ------------------------------------------------*
	
	XML.loadFile("app_settings.xml");
	
	// SET NUMBER OF MODULES HERE ------------------------------*

	XML.pushTag("all", 0);
	numMod = XML.getValue("numMod", 2, 0); // -------------------------------------------***
	filesPath = XML.getValue("filesPath", "", 0);
	XML.popTag();
	
	// SET OSC HERE --------------------------------------------*
	
	XML.pushTag("all", 0);
		oscModHost		= XML.getValue("oscModHost", "localhost", 0);
		oscModPort		= XML.getValue("oscModPort", 8001, 0);
		oscSoundHost	= XML.getValue("oscSoundHost", "localhost", 0);
		oscSoundPort	= XML.getValue("oscSoundPort", 8000, 0);
		oscListenPort	= 8000; //XML.getValue("oscListenPort", 8000, 0);
		oscMaxMessages	= XML.getValue("oscMaxMessages", 20, 0);
	XML.popTag();
	
	oscSoundSender.setup(oscSoundHost, oscSoundPort);
	
	oscReceiver.setup(oscListenPort);
	
	messageStrings  = new string [oscMaxMessages];
	timers			= new float [oscMaxMessages];
	
	oscModSender = new ofxOscSender[numMod];
	
	for (int i = 0; i < numMod; i++) {
		oscModSender[i].setup(oscModHost, oscModPort + i);
	}
	
	// ---------------------------------------------------------*
	
	// DRAWING VARS --------------------------------------------*
	
	camWidth 		= 640;
	camHeight 		= 480;
	
	dispWidth		= 1280;
	dispHeight		= 720;
	
	// ---------------------------------------------------------*

	
	// MODULE INITIALIZATION -----------------------------------*
	
	all_checkedin = false;
	all_toldToPlay= false;
	
	mod_checkedin = new bool[numMod];
	mod_needPlay  = new bool[numMod];
	mod_isPlaying = new bool[numMod];
	mod_ThreadFPS = new float[numMod];
	mod_AppFPS	  = new float[numMod];
	
	for (int i = 0; i < numMod; i++) {
		mod_checkedin[i] = false;
		mod_needPlay[i] = false;
		mod_isPlaying[i] = false;
		mod_ThreadFPS[i] = 0.0f;
		mod_AppFPS[i] = 0.0f;
	}
	
	oscSoundMessage.setAddress("/sound/play");
	oscModMessage.setAddress("/mod0/checkin_received");
	
	bRunAll = false;
	
	// GUI SETUP -----------------------------------------------*
	gui.addToggle("Run All", bRunAll);
	for (int i = 0; i < numMod; i++) {
		gui.addToggle("Mod " + ofToString(i) + " checked in", mod_checkedin[i]);
		gui.addSlider("Mod " + ofToString(i) + " Thread FPS", (float&)mod_ThreadFPS[i], 0, 60);
		gui.addSlider("Mod " + ofToString(i) + " App FPS", (float&)mod_AppFPS[i], 0, 60);
	}
	gui.show();
	
	// ---------------------------------------------------------*
	
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
		
		cout << "RCV: " << m.getAddress() << endl	;
		
		// check for possibilities
		
		// module check-in
		for (int i = 0; i < numMod; i++) {
			if (!mod_checkedin[i] && m.getAddress() == "/m" + ofToString(i) + "/checkin") {
				mod_checkedin[i] = true;
				oscModMessage.clear();
				oscModMessage.setAddress("/m" + ofToString(i) + "/checkedIn");
				oscModSender[i].sendMessage(oscModMessage);
				cout << "SNT: " << oscModMessage.getAddress() << endl;
			}
			if (!mod_needPlay[i] && m.getAddress() == "/m" + ofToString(i) + "/needPlay") {
				mod_needPlay[i] = true;
				mod_isPlaying[i] = false;
				oscModMessage.clear();
				oscModMessage.setAddress("/m" + ofToString(i) + "/awareNeedPlay");
				oscModSender[i].sendMessage(oscModMessage);
				cout << "SNT: " << oscModMessage.getAddress() << endl;
			}
			if (!mod_isPlaying[i] && m.getAddress() == "/m" + ofToString(i) + "/isPlaying") {
				mod_isPlaying[i] = true;
				mod_needPlay[i] = false;
			}
			if (m.getAddress() == "/m" + ofToString(i) + "/FPS") {
				mod_ThreadFPS[i] = m.getArgAsFloat(0);
				mod_AppFPS[i] = m.getArgAsFloat(1);
			}
		}
	}
	
	for (int i = 0; i < numMod; i++) {
		if (!mod_checkedin[i]) {
			oscModMessage.clear();
			oscModMessage.setAddress("/m" + ofToString(i) + "/please_checkin");
			oscModSender[i].sendMessage(oscModMessage);
			cout << "SNT: " << oscModMessage.getAddress() << endl;
		}
		else {
			if (!mod_isPlaying[i]) {
				if (!mod_needPlay[i]) {
					oscModMessage.clear();
					oscModMessage.setAddress("/m"+ ofToString(i) + "/do_you_need_play");
					oscModSender[i].sendMessage(oscModMessage);
					cout << "SNT: " << oscModMessage.getAddress() << endl;
				}
			}
		}
	}
		
	if (checkEveryModCheckedIn()) {
		if (bRunAll) {
			if (checkEveryModNeedPlay()) {
				oscSoundMessage.clear();
				oscSoundMessage.setAddress("/sound/stop");
				oscSoundSender.sendMessage(oscSoundMessage);
				cout << "SNT: " << oscSoundMessage.getAddress() << endl;
				
				oscSoundMessage.clear();
				oscSoundMessage.setAddress("/sound/stop");
				oscSoundSender.sendMessage(oscSoundMessage);
				cout << "SNT: " << oscSoundMessage.getAddress() << endl;
				
				oscSoundMessage.clear();
				oscSoundMessage.setAddress("/sound/play");
				oscSoundSender.sendMessage(oscSoundMessage);
				cout << "SNT: " << oscSoundMessage.getAddress() << endl;
				
				oscModMessage.clear();
				oscModMessage.setAddress("/all/play");
				for (int i = 0; i < numMod; i++) {
					oscModSender[i].sendMessage(oscModMessage);
					cout << "SNT: " << oscModMessage.getAddress() << endl;
				}
				all_toldToPlay = true;
			}
		}
	}
	else {
		cout << "waiting for every mod to check-in" << endl;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	// DRAW BACKGROUND / CLEAR ---------------------------------*
	
	ofBackground(0, 0, 0);
	
	// DRAW CONTROL DISPLAY ------------------------------------*
	
	ofSetColor(255, 255, 255);
	
	gui.draw();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch(key) {
		case 'f':
			ofToggleFullscreen();
			ofSetWindowShape(screenWidth, screenHeight);
			break;
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
// -------------------------------------------------------------

bool testApp::checkEveryModNeedPlay() {
	
	bool result = true;
	
	for (int i = 0 ; i < numMod; i++) {
		result = mod_needPlay[i];
	}
	
	return result;
}

// -------------------------------------------------------------

bool testApp::checkEveryModCheckedIn() {	
	
	bool result = true;
	
	for (int i = 0 ; i < numMod; i++) {
		result = mod_checkedin[i];
	}
	
	return result;

}

// -------------------------------------------------------------

bool testApp::checkEveryModIsPlaying() {

	bool result = true;
	
	for (int i = 0; i < numMod; i++) {
		result = mod_isPlaying[i];
	}
	
	return result;
	
}

// -----------------------------------------------------------

void testApp::exit() {
	oscModMessage.clear();
	oscModMessage.setAddress("/all/quit");
	for (int i = 0; i < numMod; i++) {
		oscModSender[i].sendMessage(oscModMessage);
		cout << "SNT: " << oscModMessage.getAddress() << endl;
	}
}