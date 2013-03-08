//
//  textInput.cpp
//  textInput
//
//  Created by Elliot Woods on 12/09/2011.
//  Copyright 2011 Kimchi and Chips.
//	MIT license
//	http://www.opensource.org/licenses/mit-license.php
//

#include "textInput.h"

textInput::textInput() {
	text="";
	position=0;
	cursorx = 10;
	cursory = 0;
	startx = cursorx;
	font.loadFont("arial.ttf", 36.0f, true,true);
}

void textInput::init() {
	ofAddListener(ofEvents().keyPressed, this, &textInput::keyPressedEvent);
}

void textInput::insertChar(string key){
	
	//text = text.substr(0,position) + key + text.substr(position+1,text.length()-1);
	text += key;
	position++;

}

void textInput::draw() {
	ofSetLineWidth(1.0f);
	
	int width = ofGetWindowWidth() - 20;
	int height = 40;
	cursory= ofGetWindowHeight() - (height+10);

	ofSetColor(255);
	ofRect(cursorx+1,cursory,width-1,height-1);


	ofSetColor(0);
	// NW to SW
	ofLine(cursorx, cursory-1, cursorx, cursory+height);
	ofLine(cursorx, cursory, cursorx+width, cursory);
	ofLine(cursorx+width, cursory, cursorx+width, cursory+height);
	ofLine(cursorx, cursory+height, cursorx+width, cursory+height);

	font.drawString(text, startx+5, cursory+30);
	
}

void textInput::keyPressed(int key) {	
	/*if (key==OF_KEY_RETURN) {
		ofNotifyEvent(evtEnter, text, this);
		if (evtEnter.empty()) {
			text.insert(text.begin()+position, '\n');
			position++;
		}
	}*/
	if (key==OF_KEY_BACKSPACE) {
		if (position>0) {
			text.erase(text.begin()+position-1);
			--position;
		}
	}
	else if (key==OF_KEY_DEL) {
		if (text.size() > position) {
			text.erase(text.begin()+position);
		}
	}
	else if (key==OF_KEY_LEFT){
		if (position>0)
			--position;
	}
	else if (key==OF_KEY_RIGHT){
		if (position<text.size()+1)
			++position;	
	}
	else{
		//text.insert(text.begin()+position, key);
		//position++;
	}

	//cursorx = position *12 +startx;

/*	//for multiline:
	cursorx = cursory = 0;
	for (int i=0; i<position; ++i) {
		if (*(text.begin()+i) == '\n') {
			++cursory;
			cursorx = 0;
		} else {
			cursorx++;
		}
	}

*/
}

void textInput::clear() {
	text.clear();
	position=0;
}

void textInput::keyPressedEvent(ofKeyEventArgs &a) {
	keyPressed(a.key);
}