#pragma once

#include "ofConstants.h"
#include "ofTexture.h"
#include "ofBaseTypes.h"
#include "ofTypes.h"

#ifdef OF_VIDEO_PLAYER_GSTREAMER
	#include "ofGstVideoPlayer.h"
	#define OF_VID_PLAYER_TYPE ofGstVideoPlayer
#endif

#ifdef OF_VIDEO_PLAYER_QUICKTIME
	#include "ofQuickTimePlayer.h"
	#define OF_VID_PLAYER_TYPE ofQuickTimePlayer
#endif

#ifdef OF_VIDEO_PLAYER_IPHONE
	#include "ofiPhoneVideoPlayer.h"
	#define OF_VID_PLAYER_TYPE ofiPhoneVideoPlayer
#endif

//---------------------------------------------
class ofVideoPlayerAdapt : public ofBaseVideoPlayer,public ofBaseVideoDraws{

	public:

		ofVideoPlayerAdapt ();

		void						setPlayer(ofPtr<ofBaseVideoPlayer> newPlayer);
		ofPtr<ofBaseVideoPlayer>	getPlayer();

		bool 				loadMovie(string name);
		void				setPixelFormat(ofPixelFormat pixelFormat);
		void 				closeMovie();
		void 				close();

		void				update();			//same as idleMovie
		void 				idleMovie();		// rename to updateMovie?
		void 				play();
		void 				stop();

		bool 				isFrameNew();
		unsigned char * 	getPixels();
		ofPixelsRef			getPixelsRef();
		float 				getPosition();
		float 				getSpeed();
		float 				getDuration();
		bool				getIsMovieDone();

		void 				setPosition(float pct);
		void 				setVolume(int volume);
		void 				setLoopState(ofLoopType state);
		int					getLoopState();
		void   				setSpeed(float speed);
		void				setFrame(int frame);  // frame 0 = first frame...

		void 				setUseTexture(bool bUse);
		ofTexture &			getTextureReference();
		void 				draw(float x, float y, float w, float h);
		void 				draw(float x, float y);
		void				draw(const ofPoint & p);
		void				draw(const ofRectangle & r);

		//the anchor is the point the image is drawn around.
		//this can be useful if you want to rotate an image around a particular point.
        void				setAnchorPercent(float xPct, float yPct);	//set the anchor as a percentage of the image width/height ( 0.0-1.0 range )
        void				setAnchorPoint(float x, float y);				//set the anchor point in pixels
        void				resetAnchor();								//resets the anchor to (0, 0)

		void 				setPaused(bool bPause);

		int					getCurrentFrame();
		int					getTotalNumFrames();

		void				firstFrame();
		void				nextFrame();
		void				previousFrame();

		float 				getHeight();
		float 				getWidth();

		bool				isPaused();
		bool				isLoaded();
		bool				isPlaying();

		//this is kept as legacy to support people accessing width and height directly. 
		int					height;
		int					width;

	private:
		ofPtr<ofBaseVideoPlayer>		player;
		
		ofTexture tex;
		ofTexture * playerTex; // a seperate texture that may be optionally implemented by the player to avoid excessive pixel copying.
		bool bUseTexture;
		ofPixelFormat internalPixelFormat;
};





