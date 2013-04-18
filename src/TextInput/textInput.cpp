#include "textInput.h"

textInput::textInput() {
	text=L"";
	position=0;
	cursorx = 10;
	cursory = 0;
	startx = cursorx;
	font.loadFont("arial_uni.ttf", 36.0f, true,true);
	font2.loadFont("arial_uni.ttf", 18.0f, true,true);
}

void textInput::init() {
	
}

void textInput::setValue(wstring key){
	text = key;
	position = 0;
}

void textInput::drawRects() {
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

	font.drawWString(text, startx+10, cursory+33);
}

void textInput::drawText(string str, int x, int y, int r, int g, int b){
	ofSetColor(r,g,b);
	font2.drawString(str, x, y);
}

void textInput::clear() {
	text.clear();
	position=0;
}

