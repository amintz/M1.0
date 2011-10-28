#include "ofVideoPlayerAdapt.h"
#include "ofUtils.h"

//---------------------------------------------------------------------------
ofVideoPlayerAdapt::ofVideoPlayerAdapt (){
	bUseTexture					= true;
	playerTex					= NULL;
	internalPixelFormat = OF_PIXELS_RGB;
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::setPlayer(ofPtr<ofBaseVideoPlayer> newPlayer){
	player = newPlayer;
}

//---------------------------------------------------------------------------
ofPtr<ofBaseVideoPlayer> ofVideoPlayerAdapt::getPlayer(){
	return player;
}

//--------------------------------------------------------------------
void ofVideoPlayerAdapt::setPixelFormat(ofPixelFormat pixelFormat) {
	internalPixelFormat = pixelFormat;
}

//---------------------------------------------------------------------------
bool ofVideoPlayerAdapt::loadMovie(string name){
	if( player == NULL ){
		
	}
	setPlayer( ofPtr<OF_VID_PLAYER_TYPE>(new OF_VID_PLAYER_TYPE) );
	player->setPixelFormat(internalPixelFormat);
	
	bool bOk = player->loadMovie(name);
	width	 = player->getWidth();
	height	 = player->getHeight();
	
	if( bOk && bUseTexture )
		if(width!=0 && height!=0)
			tex.allocate(width, height, GL_RGB);
	
	return bOk;
}

//---------------------------------------------------------------------------
unsigned char * ofVideoPlayerAdapt::getPixels(){
	if( player != NULL ){
		return player->getPixels();
	}
	return NULL;	
}

//---------------------------------------------------------------------------
ofPixelsRef ofVideoPlayerAdapt::getPixelsRef(){
	return player->getPixelsRef();
}

//
//---------------------------------------------------------------------------
//ofPixels ofVideoPlayerAdapt::getOFPixels(){
//	if( player != NULL ){
//		return player->getOFPixels();
//	}
//	return ofPixels();
//}
//
//---------------------------------------------------------------------------
//ofPixels ofVideoPlayerAdapt::getOFPixels() const{
//	if( player != NULL ){
//		return player->getOFPixels();
//	}
//	return ofPixels();
//}

//---------------------------------------------------------------------------
//for getting a reference to the texture
ofTexture & ofVideoPlayerAdapt::getTextureReference(){
	return tex;
}


//---------------------------------------------------------------------------
bool ofVideoPlayerAdapt::isFrameNew(){
	if( player != NULL ){
		return player->isFrameNew();
	}
	return false;
}

//--------------------------------------------------------------------
void ofVideoPlayerAdapt::update(){
	if(	player != NULL ){

		player->update();
		
		if( bUseTexture && player->isFrameNew() ) {
			
			playerTex = player->getTexture();
			
			if(playerTex == NULL){
				unsigned char *pxls = player->getPixels();

				if(width==0 || height==0) {
					if(player->getWidth() != 0 && player->getHeight() != 0) {
						
						width = player->getWidth();
						height = player->getHeight();
					
						if(tex.bAllocated())
							tex.clear();
					
						tex.allocate(width, height, GL_RGB);
						tex.loadData(pxls, tex.getWidth(), tex.getHeight(), GL_RGB);
					}
				}else{
					tex.loadData(pxls, tex.getWidth(), tex.getHeight(), GL_RGB);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::idleMovie(){
	update();
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::closeMovie(){
	close();
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::close(){
	if( player != NULL ){
		player->close();
	}
	tex.clear();
}

//--------------------------------------------------------
void ofVideoPlayerAdapt::play(){
	if( player != NULL ){
		player->play();
	}
}

//--------------------------------------------------------
void ofVideoPlayerAdapt::stop(){
	if( player != NULL ){
		player->stop();
	}
}

//--------------------------------------------------------
void ofVideoPlayerAdapt::setVolume(int volume){
	if( player != NULL ){
		player->setVolume(volume);
	}
}


//--------------------------------------------------------
void ofVideoPlayerAdapt::setLoopState(ofLoopType state){
	if( player != NULL ){
		player->setLoopState(state);
	}
}

int	ofVideoPlayerAdapt::getLoopState(){
	if( player != NULL ){
		return player->getLoopState();
	}else{
		return OF_LOOP_NONE;
	}
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::setPosition(float pct){
	if( player != NULL ){
		player->setPosition(pct);
	}
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::setFrame(int frame){
	if( player != NULL ){
		player->setFrame(frame);
	}
}


//---------------------------------------------------------------------------
float ofVideoPlayerAdapt::getDuration(){
	if( player != NULL ){
		return player->getDuration();
	}
	
	return 0.0;
}

//---------------------------------------------------------------------------
float ofVideoPlayerAdapt::getPosition(){
	if( player != NULL ){
		return player->getPosition();
	}
	return 0.0;
}

//---------------------------------------------------------------------------
int ofVideoPlayerAdapt::getCurrentFrame(){
	if( player != NULL ){
		return player->getCurrentFrame();
	}
	return 0;
}


//---------------------------------------------------------------------------
bool ofVideoPlayerAdapt::getIsMovieDone(){
	if( player != NULL ){
		return player->getIsMovieDone();
	}
	return false;
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::firstFrame(){
	if( player != NULL ){
		player->firstFrame();
	}
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::nextFrame(){
	if( player != NULL ){
		player->nextFrame();
	}
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::previousFrame(){
	if( player != NULL ){
		player->previousFrame();
	}
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::setSpeed(float _speed){
	if( player != NULL ){
		player->setSpeed(_speed);
	}
}

//---------------------------------------------------------------------------
float ofVideoPlayerAdapt::getSpeed(){
	if( player != NULL ){
		return player->getSpeed();
	}
	return 0.0;
}

//---------------------------------------------------------------------------
void ofVideoPlayerAdapt::setPaused(bool _bPause){
	if( player != NULL ){
		player->setPaused(_bPause);
	}
}

//------------------------------------
void ofVideoPlayerAdapt::setUseTexture(bool bUse){
	bUseTexture = bUse;
}

//----------------------------------------------------------
void ofVideoPlayerAdapt::setAnchorPercent(float xPct, float yPct){
	tex.setAnchorPercent(xPct, yPct);
}

//----------------------------------------------------------
void ofVideoPlayerAdapt::setAnchorPoint(float x, float y){
	tex.setAnchorPoint(x, y);
}

//----------------------------------------------------------
void ofVideoPlayerAdapt::resetAnchor(){
	tex.resetAnchor();
}

//------------------------------------
void ofVideoPlayerAdapt::draw(float _x, float _y, float _w, float _h){
	if(playerTex == NULL)
		tex.draw(_x, _y, _w, _h);
	else
		playerTex->draw(_x, _y, _w, _h);
}

//------------------------------------
void ofVideoPlayerAdapt::draw(float _x, float _y){
	if(playerTex == NULL)
		tex.draw(_x, _y);
	else
	{
		playerTex->draw(_x,_y);
	}
}

//------------------------------------
void ofVideoPlayerAdapt::draw(const ofPoint & p){
	if(playerTex == NULL)
		tex.draw(p);
	else
	{
		playerTex->draw(p);
	}
}

//------------------------------------
void ofVideoPlayerAdapt::draw(const ofRectangle & r){
	if(playerTex == NULL)
		tex.draw(r);
	else
	{
		playerTex->draw(r);
	}
}

//------------------------------------
int ofVideoPlayerAdapt::getTotalNumFrames(){
	if( player != NULL ){
		return player->getTotalNumFrames();
	}
	return 0;
}

//----------------------------------------------------------
float ofVideoPlayerAdapt::getWidth(){
	if(	player != NULL ){
		width = player->getWidth();
	}
	return (float)width;
}

//----------------------------------------------------------
float ofVideoPlayerAdapt::getHeight(){
	if(	player != NULL ){
		height = player->getHeight();
	}
	return (float)height;
}

//----------------------------------------------------------
bool ofVideoPlayerAdapt::isPaused(){
	if(	player != NULL ){
		return player->isPaused();
	}
	return false;
}

//----------------------------------------------------------
bool ofVideoPlayerAdapt::isLoaded(){
	if(	player != NULL ){
		return player->isLoaded();
	}
	return false;
}

//----------------------------------------------------------
bool ofVideoPlayerAdapt::isPlaying(){
	if(	player != NULL ){
		return player->isPlaying();
	}
	return false;
}
