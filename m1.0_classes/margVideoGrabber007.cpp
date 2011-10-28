/*
 *  margVideoGrabber.cpp
 *  emptyExample
 *
 *  Created by André Mintz on 26/03/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "margVideoGrabber007.h"

//vector<string> margVideoGrabber :: getDeviceList() {
//
//	//---------------------------------
//	#ifdef OF_VIDEO_CAPTURE_QUICKTIME
//	//---------------------------------
//	
//	bool bNeedToInitGrabberFirst = false;
//	
//	if (!bSgInited) bNeedToInitGrabberFirst = true;
//	
////	//if we need to initialize the grabbing component then do it
////	if( bNeedToInitGrabberFirst ){
////		if( !qtInitSeqGrabber() ){
////			vector<string> error;
////			error.push_back("error");
////			return error;
////		}
////	}
//	
//	
//	/*
//	 //input selection stuff (ie multiple webcams)
//	 //from http://developer.apple.com/samplecode/SGDevices/listing13.html
//	 //and originally http://lists.apple.com/archives/QuickTime-API/2008/Jan/msg00178.html
//	 */
//	
//	SGDeviceList deviceList;
//	SGGetChannelDeviceList (gVideoChannel, sgDeviceListIncludeInputs, &deviceList);
//	unsigned char pascalName[64];
//	unsigned char pascalNameInput[64];
//	
//	//this is our new way of enumerating devices
//	//quicktime can have multiple capture 'inputs' on the same capture 'device'
//	//ie the USB Video Class Video 'device' - can have multiple usb webcams attached on what QT calls 'inputs'
//	//The isight for example will show up as:
//	//USB Video Class Video - Built-in iSight ('input' 1 of the USB Video Class Video 'device')
//	//Where as another webcam also plugged whill show up as
//	//USB Video Class Video - Philips SPC 1000NC Webcam ('input' 2 of the USB Video Class Video 'device')
//	
//	//this means our the device ID we use for selection has to count both capture 'devices' and their 'inputs'
//	//this needs to be the same in our init grabber method so that we select the device we ask for
//	int deviceCount = 0;
//	
//	ofLog(OF_LOG_NOTICE, "listing available capture devices");
//	
//	vector <string> myDeviceList;
//	
//	if (!	bGrabberInited) {
//		exit(171);
//	}
//	
//	for(int i = 0 ; i < (*deviceList)->count ; ++i)
//	{
//		SGDeviceName nameRec;
//		nameRec = (*deviceList)->entry[i];
//		SGDeviceInputList deviceInputList = nameRec.inputs;
//		
//		int numInputs = 0;
//		if( deviceInputList ) numInputs = ((*deviceInputList)->count);
//		
//		memcpy(pascalName, (*deviceList)->entry[i].name, sizeof(char) * 64);
//		
//		//this means we can use the capture method
//		if(nameRec.flags != sgDeviceNameFlagDeviceUnavailable){
//			
//			//if we have a capture 'device' (qt's word not mine - I would prefer 'system' ) that is ready to be used
//			//we go through its inputs to list all physical devices - as there could be more than one!
//			for(int j = 0; j < numInputs; j++){
//				
//				
//				//if our 'device' has inputs we get their names here
//				if( deviceInputList ){
//					SGDeviceInputName inputNameRec  = (*deviceInputList)->entry[j];
//					memcpy(pascalNameInput, inputNameRec.name, sizeof(char) * 64);
//				}
//				
//				myDeviceList.push_back(ofToString(deviceCount) + " " + p2cstr(pascalName) + " " + p2cstr(pascalNameInput));
////				printf( "device[%i] %s - %s\n",  deviceCount, p2cstr(pascalName), p2cstr(pascalNameInput) );
//				
//				//we count this way as we need to be able to distinguish multiple inputs as devices
//				deviceCount++;
//			}
//			
//		}else{
//			//printf( "(unavailable) device[%i] %s\n",  deviceCount, p2cstr(pascalName) );
//			myDeviceList.push_back(ofToString(deviceCount) + " unavailable device");
//			deviceCount++;
//		}
//	}
//	
//	//if we initialized the grabbing component then close it
//	if( bNeedToInitGrabberFirst ){
//		qtCloseSeqGrabber();
//	}
//	
//	return myDeviceList;
//	myDeviceList.clear();
//	
//	//---------------------------------
//	#endif
//	//---------------------------------
//	
//}

//int	margVideoGrabber :: getDeviceID() {
//	return deviceID;
//}

unsigned char* margVideoGrabber :: getGrayscalePixels() {
	if (sizeof(grayscalePixels) == 0) grayscalePixels = new unsigned char[width * height];
	if (colorlength == 0) colorlength = width * height * 3;
	unsigned char* pixels = getPixels();
	for (int i = 0; i < colorlength; i+=3) {
		grayscalePixels[i/3] = ((pixels[i]*0.3) + (pixels[i+1]*0.59) + (pixels[i+2]*0.11));
	}
	return grayscalePixels;
}