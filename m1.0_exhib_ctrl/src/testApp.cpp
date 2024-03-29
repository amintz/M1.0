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
		oscSoundPort	= XML.getValue("oscSoundPort", 8005, 0);
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
	
	// MESSAGES    ---------------------------------------------*
	
	msg	  = new string [9];
	
	msg[ASK_CHECKIN]	= "/ask/checkin";
	msg[RCV_CHECKIN]	= "/rcv/checkin";
	msg[ASK_NEEDPLAY]	= "/ask/needplay";
	msg[RCV_NEEDPLAY]	= "/rcv/needplay";
	msg[ASK_PLAY]		= "/ask/play";
	msg[RCV_ISPLAYING]	= "/rcv/isPlaying";
	msg[ASK_SND_PLAY]	= "/sound/play";
	msg[ASK_SHUTDOWN]	= "/ask/quit";
	msg[ASK_SND_STOP]	= "/sound/stop";
	
	numLastMessages = 30;
	curMessageIdx = 0;
	lastMessages = new string[numLastMessages];
	
	for(int i = 0; i < numLastMessages; i ++) {
		lastMessages[i] = "";
	}
	
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
	
	oscSoundMessage.setAddress(msg[ASK_SND_PLAY]);
	oscModMessage.setAddress(msg[RCV_CHECKIN]);
	
	bRunAll = false;
	bDrawMessages = false;
	
	// GUI SETUP -----------------------------------------------*
	gui.addToggle("Run All", bRunAll);
	gui.addToggle("Draw Messages", bDrawMessages);
	for (int i = 0; i < numMod; i++) {
		gui.addTitle("Module " + ofToString(i), 50);
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
		
		
		// check for possibilities
		
		for (int i = 0; i < numMod; i++) {
			if (!mod_checkedin[i] && m.getAddress() == "/m" + ofToString(i) + "/checkin") {
				pushNewMessage("RCV: " + m.getAddress());
				mod_checkedin[i] = true;
				oscModMessage.clear();
				oscModMessage.setAddress(msg[RCV_CHECKIN]);
				oscModSender[i].sendMessage(oscModMessage);
				pushNewMessage("SNT: " + oscModMessage.getAddress() + " (" + ofToString(i) + ")");
			}
			else if (!mod_needPlay[i] && m.getAddress() == "/m" + ofToString(i) + "/needPlay") {
				pushNewMessage("RCV: " + m.getAddress());
				mod_needPlay[i] = true;
				mod_isPlaying[i] = false;
				oscModMessage.clear();
				oscModMessage.setAddress(msg[RCV_NEEDPLAY]);
				oscModSender[i].sendMessage(oscModMessage);
				pushNewMessage("SNT: " + oscModMessage.getAddress() + " (" + ofToString(i) + ")");
			}
			else if (!mod_isPlaying[i] && m.getAddress() == "/m" + ofToString(i) + "/isPlaying") {
				pushNewMessage("RCV: " + m.getAddress());
				mod_isPlaying[i] = true;
				mod_needPlay[i] = false;
				oscModMessage.clear();
				oscModMessage.setAddress(msg[RCV_ISPLAYING]);
				oscModSender[i].sendMessage(oscModMessage);
			}
			else if (m.getAddress() == "/m" + ofToString(i) + "/FPS") {
				mod_ThreadFPS[i] = m.getArgAsFloat(0);
				mod_AppFPS[i] = m.getArgAsFloat(1);
			}
		}
	}
	
	// For each Mod
	
	for (int i = 0; i < numMod; i++) {
		
		// If it hasn't checked in yet
		
		if (!mod_checkedin[i]) {
			
			// Ask to checkin, in case it has missed
			
			oscModMessage.clear();
			oscModMessage.setAddress(msg[ASK_CHECKIN]);
			oscModSender[i].sendMessage(oscModMessage);
			pushNewMessage("SNT: " + oscModMessage.getAddress() + " (" + ofToString(i) + ")");
		
		}
	}
	
	
	// If already told everyone to play
	
	if (all_toldToPlay) {
		
		// If all got the message and responded (no mod needs play)
		
		if (checkNoModNeedPlay()) {
			all_toldToPlay = false;	// Forget about it
		}
		
		// If someone didn't get the message or didn't respond yet
		
		else {
			oscModMessage.clear();
			oscModMessage.setAddress(msg[ASK_PLAY]);
			for (int i = 0; i < numMod; i++) {		 
				if(!mod_isPlaying[i]) {										// Find lazy mod
					oscModSender[i].sendMessage(oscModMessage);				// Ask to play once more
					pushNewMessage("SNT: " + oscModMessage.getAddress() + " (" + ofToString(i) + ")");
				}
			}
		}
		
	}
	
	// And if everyone needs a play 
	
	if (checkEveryModNeedPlay()) {
		
		// And if we are go
		
		if (bRunAll && !all_toldToPlay) {
		
			// Tell sound to play
			
			oscSoundMessage.clear();
			oscSoundMessage.setAddress(msg[ASK_SND_PLAY]);
			oscSoundSender.sendMessage(oscSoundMessage);
			pushNewMessage("SNT: " + oscSoundMessage.getAddress() + " (sound)");
			
			oscModMessage.clear();
			oscModMessage.setAddress(msg[ASK_PLAY]);
			
			// Tell each mod to play too
			
			for (int i = 0; i < numMod; i++) {
				if (mod_needPlay[i]) {
					oscModSender[i].sendMessage(oscModMessage);
					pushNewMessage("SNT: " + oscModMessage.getAddress() + " (" + ofToString(i) + ")");
				}
			}
			
			// Take note: we have told them to play already
			
			all_toldToPlay = true;
			
		}
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	// DRAW BACKGROUND / CLEAR ---------------------------------*
	
	ofBackground(0, 0, 0);
	
	// DRAW CONTROL DISPLAY ------------------------------------*
	
	ofSetColor(255, 255, 255);
	
	gui.draw();
	
	if(bDrawMessages) {
		for(int i = 0; i < numLastMessages; i++) {
			ofDrawBitmapString(lastMessages[i], 400, 125 + (i * 20));
		}
	}
	
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
	
	
	for (int i = 0 ; i < numMod; i++) {
		if(!mod_needPlay[i]) return false;
	}
	
	return true;
}

// -------------------------------------------------------------

bool testApp::checkNoModNeedPlay() {
	
	
	for (int i = 0 ; i < numMod; i++) {
		if(mod_needPlay[i]) return false;
	}
	
	return true;
	
}

// -------------------------------------------------------------

bool testApp::checkEveryModCheckedIn() {	
	
	for (int i = 0 ; i < numMod; i++) {
		if(!mod_checkedin[i]) return false;
	}
	
	return true;

}

// -------------------------------------------------------------

bool testApp::checkEveryModIsPlaying() {
	
	for (int i = 0; i < numMod; i++) {
		if(!mod_isPlaying[i]) return false;
	}
	
	return true;
	
}

// -----------------------------------------------------------

void testApp::pushNewMessage(string message) {

	for(int i = 1; i < numLastMessages; i++) {
		lastMessages[numLastMessages-i] = lastMessages[numLastMessages-i-1];
	}
	lastMessages[0] = message;
	
}

// -----------------------------------------------------------

void testApp::exit() {
	oscModMessage.clear();
	oscModMessage.setAddress(msg[ASK_SHUTDOWN]);
	for (int i = 0; i < numMod; i++) {
		oscModSender[i].sendMessage(oscModMessage);
		cout << "SNT: " << oscModMessage.getAddress() << endl;
	}
	oscSoundMessage.clear();
	oscSoundMessage.setAddress(msg[ASK_SND_STOP]);
	oscSoundSender.sendMessage(oscSoundMessage);
	cout << "SNT: " << oscSoundMessage.getAddress() << endl;
}