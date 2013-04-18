//
//  textInput.h
//  textInput
//
//  Created by Elliot Woods on 12/09/2011.
//  Copyright 2011 Kimchi and Chips.
//	MIT license
//	http://www.opensource.org/licenses/mit-license.php
//

#include "ofMain.h"
#include "ofEvents.h"
#include "ofxFTGLFont.h"

class textInput {
public:
	textInput();
	void	init();
	void	drawRects();
	void	drawText(string str, int x, int y, int r, int g, int b);
	void	clear();
	void	setValue(wstring key);
	wstring	text;
	int		position;
	
	ofEvent<string> evtEnter;
	ofxFTGLFont font;
	ofxFTGLFont font2;
	
protected:
	int		cursorx, cursory, startx;
};
