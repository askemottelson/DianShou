#include "DianShou.h"

#include <iostream>
#include <vector>


// OPENFRAMEWORKS FUNCTIONS

//--------------------------------------------------------------

DianShou::DianShou(){

}

DianShou::~DianShou(){
	// Remove the sample listener when done
	controller.removeListener(listener);
}



void DianShou::setup(){
	std::cout << "setup();" << std::endl;

	ofEnableSmoothing();
	ofEnableAlphaBlending();

	myString = " ASKE ER SØD ";
	font.loadFont("arial.ttf", 15.0f, true,true);
	

	
	bgcolor = new float[2];
	// init gray
	bgcolor[0] = 100;
	bgcolor[1] = 100;
	bgcolor[2] = 100;

	cursorcolor = new float[2];
	// init red
	cursorcolor[0] = 255;
	cursorcolor[1] = 0;
	cursorcolor[2] = 0;

	cursor = new float[1];
	circle.push_back(0);
	circle.push_back(0);
	circle.push_back(10);


	// Have the sample listener receive events from the controller
	controller.addListener(listener);
	
}


//--------------------------------------------------------------
void DianShou::update(){
	 // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  
  // standard bg color
  bgcolor[0] = 220;
  bgcolor[1] = 220;
  bgcolor[2] = 220;

  // cursor position if no data
  cursor[0] = -100;
  cursor[1] = -100;

  if(frame.fingers().count() != 0){

	const Leap::HandList& hands = frame.hands();
	const Leap::Hand& hand = hands[0];
	const Leap::FingerList& fingers = hand.fingers();
	const Leap::Finger& finger = fingers[0];

	Vector tipPosition = finger.tipPosition();

	int z = tipPosition.z;
	float x = (tipPosition.x * 2) + 300;
	float y = (-tipPosition.y * 2) + 800;

	cursor[0] = x;
	cursor[1] = y;

	if(z < 35 && z > 0){
		bgcolor[0] = 255 - z;
		bgcolor[1] = 255 - z;
		bgcolor[2] = 255 - z;

		if(cursor[sizeof(cursor)-1] != 0){
		  circle.push_back(0);
		  circle.push_back(0);
		  circle.push_back(0);
		}

	}
	else if(z < 0){
		bgcolor[0] = 255;
		bgcolor[1] = 255;
		bgcolor[2] = 255;

		circle.push_back( x );
		circle.push_back( y );
		circle.push_back( 10 );
	}

	std::cout << " x: " << x << " y: " << y << " z: " << z  << " bgcolor: " << bgcolor[0] << std::endl;
  
  }
  else{
	  if(cursor[sizeof(cursor)-1] != 0){
		  circle.push_back(0);
		  circle.push_back(0);
		  circle.push_back(0);
	  }
  }
}

//--------------------------------------------------------------
void DianShou::draw(){
	
	ofBackground(bgcolor[0],bgcolor[1],bgcolor[2]);

	// draw line
	int x,y,r;

	ofSetColor(0,0,0);
    ofFill();

	int lastPx = 0;
	int lastPy = 0;

	for(int i = 0; i < (int)circle.size() / 3; i++){
		x = circle[i*3];
		y = circle[i*3+1];
		r = circle[i*3+2];
	
		if(!(x == 0 && y == 0) && lastPx != 0){
			ofLine(lastPx,lastPy,x,y);
		}

		lastPx = x;
		lastPy = y;
	}

	// set cursor
	ofSetColor(255,0,0);
    ofFill();
    ofCircle(cursor[0], cursor[1], 10);


	/////////// FTGL
	
	ofSetColor(0);
	font.drawString(myString, 10.0f, 100.0f);

}

//--------------------------------------------------------------
void DianShou::keyPressed(int key){
	std::cout << "key pressed " << key << std::endl;
	
	if(key == 'c' || key == 'C'){
		
	}
	else if(key == 32){ // space
		//drawStr = L"你好";
	}

	// clear screen
	ofBackground(bgcolor[0],bgcolor[1],bgcolor[2]);
	circle.erase(circle.begin(),circle.begin()+circle.size());
	circle.push_back(0);
	circle.push_back(0);
	circle.push_back(0);
	
}

//--------------------------------------------------------------
void DianShou::keyReleased(int key){

}

//--------------------------------------------------------------
void DianShou::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void DianShou::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void DianShou::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void DianShou::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void DianShou::windowResized(int w, int h){

}

//--------------------------------------------------------------
void DianShou::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void DianShou::dragEvent(ofDragInfo dragInfo){ 

}

