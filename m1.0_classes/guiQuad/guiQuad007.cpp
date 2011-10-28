#include "guiQuad007.h"

//----------------------------------------------------
guiQuad::guiQuad(){
	selected = -1;
	quadName = "QUAD_";
}

//----------------------------------------------------
void guiQuad::setup(string _quadName){
	quadName = _quadName;
}

//----------------------------------------------------
void guiQuad::releaseAllPoints(){
	selected = -1;
}

//----------------------------------------------------
//these should be in the range x(0-maxX) y(0-maxH) 
//with 	width  = maxW
//		height = maxH 
void guiQuad::setQuadPoints( ofVec2f * inPts ){
		
	for(int i = 0; i < 4; i++){
		srcZeroToOne[i].x	= inPts[i].x;
		srcZeroToOne[i].y	= inPts[i].y;
		
		if(srcZeroToOne[i].x > 1) srcZeroToOne[i].x = 1;
		if(srcZeroToOne[i].y > 1) srcZeroToOne[i].y = 1;
	}
	
	anchorPoint.x = (least(srcZeroToOne[1].x, srcZeroToOne[2].x) + greatest(srcZeroToOne[0].x, srcZeroToOne[3].x))/2;
	anchorPoint.y = (least(srcZeroToOne[2].y, srcZeroToOne[3].y) + greatest(srcZeroToOne[0].y, srcZeroToOne[1].y))/2;
}

//----------------------------------------------------
void guiQuad::readFromFile(string filePath){
	
	xml.loadFile(filePath);
	loadSettings();	
}

//----------------------------------------------------
void guiQuad::loadSettings(){
	
	string str;
	
	
	for(int i = 0; i < 4; i++){
		
		//These two lines give us:
		// "QUAD_SRC_0"
		// "QUAD_SRC_1" etc...
								
		str = quadName;
		str += ofToString(i);									
																						
		srcZeroToOne[i].x = xml.getValue(str+"_X", srcZeroToOne[i].x);
		srcZeroToOne[i].y = xml.getValue(str+"_Y", srcZeroToOne[i].y);		
		
		if(srcZeroToOne[i].x > 1) srcZeroToOne[i].x = 1;
		if(srcZeroToOne[i].y > 1) srcZeroToOne[i].y = 1;
									
	}
	
	anchorPoint.x = (least(srcZeroToOne[1].x, srcZeroToOne[2].x) + greatest(srcZeroToOne[0].x, srcZeroToOne[3].x))/2;
	anchorPoint.y = (least(srcZeroToOne[2].y, srcZeroToOne[3].y) + greatest(srcZeroToOne[0].y, srcZeroToOne[1].y))/2;
				
}

//-----------------------------------------------------
bool guiQuad::selectPoint(float x, float y, float offsetX, float offsetY, float width, float height, float hitArea){
	
	//make sure selected is -1 unless we really find a point			
	selected = -1;
	
	if(width == 0 || height == 0 || x < offsetX || x > offsetX + width || y < offsetY ||  y > offsetY + height){
		//then we are out of our possible quad area
		//so we ignore :)
		return false;
	}  
	
	//lets get it in range x(0 - width) y(0 - height)
	float px = x - offsetX;
	float py = y - offsetY;
	
	//now get in 0-1 range
	px /= width;
	py /= height;
	
	hitArea /= width;
	
	//we want to store the smallest distance found
	//because in the case when two points are in the 
	//hit area we want to pick the closet
	float storeDist = 9999999.0;
	
	for(int i = 0; i < 4; i++){
		float dx = fabs(px -  srcZeroToOne[i].x);
		float dy = fabs(py -  srcZeroToOne[i].y);
		
		float dist = sqrt(dx*dx + dy*dy);
		
		if(dist > hitArea)continue;
		
		if(dist < storeDist){
			selected = i;
			storeDist = dist;
		}
	}
	
	if(selected != -1){
		
		getScaledQuadPoints(width, height);	
		
		if ((srcScaled[selected].x - srcScaled[oppVertex(selected)].x) != 0) {
			diag_a = (srcScaled[selected].y - srcScaled[oppVertex(selected)].y) / (srcScaled[selected].x - srcScaled[oppVertex(selected)].x);
			diag_b = srcScaled[selected].y - (diag_a * srcScaled[selected].x);
			diag_v = false;
		} else {
			diag_v = true;
		}
		
		
		
		// side A
		if (srcScaled[0].x - srcScaled[1].x != 0) {
			sideA_m = (srcScaled[0].y - srcScaled[1].y) / (srcScaled[0].x - srcScaled[1].x);
			cout << "sideA_m = " << sideA_m << endl;
			sideA_b = srcScaled[0].y - (sideA_m * srcScaled[0].x);
			cout << "sideA_b = " << sideA_b << endl;
			sideA_v = -1;
		} else {
			sideA_m = 0;
			sideA_b = 0;
			sideA_v = srcScaled[0].x;
			cout << "sideA is vertical" << endl;
			cout << "sideA_v = " << sideA_v << endl;
		}
		
		// side B
		if (srcScaled[1].x - srcScaled[2].x != 0) {
			sideB_m = (srcScaled[1].y - srcScaled[2].y) / (srcScaled[1].x - srcScaled[2].x);
			cout << "sideB_m = " << sideB_m << endl;
			sideB_b = srcScaled[1].y - (sideB_m * srcScaled[1].x);
			cout << "sideB_b = " << sideB_b << endl;
			sideB_v = -1;
		} else {
			sideB_m = 0;
			sideB_b = 0;
			sideB_v = srcScaled[1].x;
			cout << "sideB is vertical" << endl;
			cout << "sideB_v = " << sideB_v << endl;
		}
		
		// side C
		if (srcScaled[2].x - srcScaled[3].x != 0) {
			sideC_m = (srcScaled[2].y - srcScaled[3].y) / (srcScaled[2].x - srcScaled[3].x);
			cout << "sideC_m = " << sideC_m << endl;
			sideC_b = srcScaled[2].y - (sideC_m * srcScaled[2].x);
			cout << "sideC_b = " << sideC_b << endl;
			sideC_v = -1;
		} else {
			sideC_m = 0;
			sideC_b = 0;
			sideC_v = srcScaled[2].x;
			cout << "sideC is vertical" << endl;
			cout << "sideC_v = " << sideC_v << endl;
		}
		
		// side D
		if (srcScaled[3].x - srcScaled[0].x != 0) {
			sideD_m = (srcScaled[3].y - srcScaled[0].y) / (srcScaled[3].x - srcScaled[0].x);
			cout << "srcScaled[3].x = " << srcScaled[3].x << endl;
			cout << "srcScaled[3].y = " << srcScaled[3].y << endl;
			cout << "srcScaled[0].x = " << srcScaled[0].x << endl;
			cout << "srcScaled[0].y = " << srcScaled[0].y << endl;
			cout << "sideD_m = " << sideD_m << endl;
			sideD_b = srcScaled[3].y - (sideD_m * srcScaled[3].x);
			cout << "sideD_b = " << sideD_b << endl;
			sideD_v = -1;
		} else {
			sideD_m = 0;
			sideD_b = 0;
			sideD_v = srcScaled[3].x;
			cout << "sideD is vertical" << endl;
			cout << "sideD_v = " << sideD_v << endl;
		}
		
		
		srcZeroToOne[selected].x	= px;
		srcZeroToOne[selected].y	= py;
		
		srcZeroToOne[selected].x 	= px;
		srcZeroToOne[selected].y 	= py;	
		srcScaled[selected].x		= px;
		srcScaled[selected].y		= py;
		
		if(selected == 0)setCommonText("status: Quad - Top Left");
		else
			if(selected == 1)setCommonText("status: Quad - Top Right");
			else
				if(selected == 2)setCommonText("status: Quad - Bottom Right");
				else 
					if(selected == 3)setCommonText("status: Quad - Bottom Left");
		
		return true;
	}
	
	return false;
}

//----------------------------------------------------


//----------------------------------------------------
bool guiQuad::updatePoint(float x, float y, float offsetX, float offsetY, float width, float height){
	
	//nothing to update
	if(selected == -1) return false;
	
	if(width == 0 || height == 0){
		//dangerous so we ignore :)
		return false;
	}  
	
	if( x < offsetX ) 			x = offsetX;
	if( x > offsetX + width ) 	x = offsetX + width;
	if( y < offsetY ) 			y = offsetY;
	if( y > offsetY + height) 	y = offsetY + height;
	
	//lets get it in range x(0 - width) y(0 - height)
	float px = x - offsetX;
	float py = y - offsetY;
				
	//now get in 0-1 range
	px /= width;
	py /= height;
	
	srcZeroToOne[selected].x 	= px;
	srcZeroToOne[selected].y 	= py;	
	
	srcScaled[selected].x		= px;
	srcScaled[selected].y		= py;
	
	anchorPoint.x = (least(srcZeroToOne[1].x, srcZeroToOne[2].x) + greatest(srcZeroToOne[0].x, srcZeroToOne[3].x))/2;
	anchorPoint.y = (least(srcZeroToOne[2].y, srcZeroToOne[3].y) + greatest(srcZeroToOne[0].y, srcZeroToOne[1].y))/2;
	
	return true;
}

//----------------------------------------------------

bool guiQuad::updatePoint(float x, float y, float offsetX, float offsetY, float width, float height, bool noDistorsion){
	
	if(!noDistorsion) {
		updatePoint(x, y, offsetX, offsetY, width, height);
	} else {
		
		getScaledQuadPoints(width, height);
		
		//nothing to update
		if(selected == -1) return false;
		
		if(width == 0 || height == 0){
			//dangerous so we ignore :)
			return false;
		}  
		
		if( x < offsetX ) 			x = offsetX;
		if( x > offsetX + width ) 	x = offsetX + width;
		if( y < offsetY ) 			y = offsetY;
		if( y > offsetY + height) 	y = offsetY + height;
		
		//lets get it in range x(0 - width) y(0 - height)
		float px = x - offsetX;
		float py = y - offsetY;
		
		if (!diag_v){
			py = diag_a*px + diag_b;
			if (py < 0) { py = 0; px = ((py - diag_b)/diag_a); }
			else if (py > height) { py = height; px = ((py - diag_b)/diag_a); }
		} else {
			px = selX;
			if (px < 0) { px = 0; py = diag_b; }
			else if (px > width) { px = width; py = diag_a * width + diag_b; }
		}
		
		cout << "px = " << px << endl;
		cout << "py = " << py << endl;
		
		if (selected == 0) {
			if (sideA_v == -1) {
				sideA_b = py - (sideA_m * px);
				cout << "sideA_b = " << sideA_b << endl;
				if (sideB_v == -1) {
					float temp_x;
					if (sideB_m < 0 || sideB_m > 1) {
						temp_x= (((sideA_m * temp_x) + sideA_b - sideB_b) / sideB_m) / width;
					} else {
						temp_x= (((sideB_m * temp_x) + sideB_b - sideA_b) / sideA_m) / width;
					}
					srcZeroToOne[1].x = temp_x;
					srcZeroToOne[1].y = ((sideA_m * (srcZeroToOne[1].x * width)) + sideA_b)/height;
				} else {
					srcZeroToOne[1].y = ((sideA_m * (srcZeroToOne[1].x * width)) + sideA_b) / height;
				}
			} else {
				srcZeroToOne[1].x = px/width;
				srcZeroToOne[1].y = ((sideB_m * px) + sideB_b) / height;
			}
			
			if (sideD_v == -1) {
				sideD_b = py - (sideD_m * px);
				cout << "sideD_b = " << sideD_b << endl;
				if (sideC_v == -1) {
					float temp_x;
					if (sideC_m < 0 || sideC_m > 1) {
						temp_x = (((sideD_m * temp_x) + sideD_b - sideC_b) / sideC_m) / width;
					} else {
						temp_x = (((sideC_m * temp_x) + sideC_b - sideD_b) / sideD_m) / width;
					}
					srcZeroToOne[3].x = temp_x;
					cout << "3x = " << temp_x * width << endl;
					srcZeroToOne[3].y = ((sideD_m * (srcZeroToOne[3].x * width)) + sideD_b)/height;
					cout << "3y = " << srcZeroToOne[3].y * height << endl;
				} else {
					srcZeroToOne[3].y = ((sideD_m * (srcZeroToOne[3].x * width)) + sideD_b) / height;
				}
			} else {
				srcZeroToOne[3].x = px/width;
				srcZeroToOne[3].y = ((sideC_m * px) + sideC_b) / height;
				
			}
			
			if (srcZeroToOne[1].x < 0) { srcZeroToOne[1].x = 0; }
			else if (srcZeroToOne[1].x > 1) { srcZeroToOne[1].x = 1; }
			if (srcZeroToOne[1].y < 0) { srcZeroToOne[1].y = 0; }
			else if (srcZeroToOne[1].y > 1) { srcZeroToOne[1].y = 1; }
			
			if (srcZeroToOne[3].x < 0) { srcZeroToOne[3].x = 0; }
			else if (srcZeroToOne[3].x > 1) { srcZeroToOne[3].x = 1; }
			if (srcZeroToOne[3].y < 0) { srcZeroToOne[3].y = 0; }
			else if (srcZeroToOne[3].y > 1) { srcZeroToOne[3].y = 1; }
			
		}
			
		
		//now get in 0-1 range
		px /= width;;
		py /= height;
		
		srcZeroToOne[selected].x 	= px;
		srcZeroToOne[selected].y 	= py;
		
		anchorPoint.x = (least(srcZeroToOne[1].x, srcZeroToOne[2].x) + greatest(srcZeroToOne[0].x, srcZeroToOne[3].x))/2;
		anchorPoint.y = (least(srcZeroToOne[2].y, srcZeroToOne[3].y) + greatest(srcZeroToOne[0].y, srcZeroToOne[1].y))/2;		
	
	}
	
	return true;
}

//----------------------------------------------------
//returns pts to width by height range 
ofVec2f * guiQuad::getScaledQuadPoints(float width, float height){

	for(int i = 0; i < 4; i++){
		srcScaled[i].x = srcZeroToOne[i].x * width;
		srcScaled[i].y = srcZeroToOne[i].y * height;
	}
	
	anchorPointScaled.x = anchorPoint.x * width;
	anchorPointScaled.y = anchorPoint.y * height;
	
	return srcScaled;
}


//----------------------------------------------------
//returns pts in 0-1 scale
ofVec2f * guiQuad::getQuadPoints(){
	return srcZeroToOne;
}


//----------------------------------------------------
void guiQuad::saveToFile(string filePath, string newQuadName){
	string str;
		
	for(int i = 0; i < 4; i++){
	
		str = newQuadName;
		str += ofToString(i);

		xml.setValue(str+"_X", srcZeroToOne[i].x);
		xml.setValue(str+"_Y", srcZeroToOne[i].y);		
	}
	
	xml.saveFile(filePath);
}		

//----------------------------------------------------
void guiQuad::saveToFile(string filePath){
	saveToFile(filePath, quadName);
}

//----------------------------------------------------
void guiQuad::draw(float x, float y, float width, float height, int red, int green, int blue, int thickness){
	
	getScaledQuadPoints(width, height);
	glPushMatrix();
		glTranslatef(x, y, 0);
		
		ofNoFill();
		
		ofSetColor(red, green, blue);
		glLineWidth(thickness);
		glBegin(GL_LINE_LOOP);
		for(int i = 0; i < 4; i++){
			glVertex2f(srcScaled[i].x, srcScaled[i].y);
		}
		glEnd();
		
		glLineWidth(1);
		ofSetRectMode(OF_RECTMODE_CENTER);
		for(int i = 0; i < 4; i++){
			
			if(i == 0)ofSetColor(255, 0, 0);
			if(i == 1)ofSetColor(0, 255, 0);
			if(i == 2)ofSetColor(0, 0, 255);
			if(i == 3)ofSetColor(0, 255, 255);

			ofRect(srcScaled[i].x, srcScaled[i].y, 8, 8);
		}
		
		ofSetColor(255, 0, 0);
		ofCircle(anchorPointScaled.x, anchorPointScaled.y, 4);
		ofDrawBitmapString("+", anchorPointScaled.x - 4, anchorPointScaled.y + 4);
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofFill();
	glPopMatrix();
}

//----------------------------------------------------
void guiQuad::draw(float x, float y, float width, float height){
	
	//default to a think yellow line
	draw(x, y, width, height, 255, 255, 0, 1);
}

//----------------------------------------------------
int guiQuad::oppVertex(int vertex) {
	
	if (vertex == 0) {
		return 2;
	} else if (vertex == 1) {
		return 3;
	} else if (vertex == 2) {
		return 0;
	} else if (vertex == 3) {
		return 1;
	}
}

