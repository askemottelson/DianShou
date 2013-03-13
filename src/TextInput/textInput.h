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
	void	draw();
	void	keyPressed(int key);
	void	clear();
	void	setValue(wstring key);
	wstring	text;
	int		position;
	
	ofEvent<string> evtEnter;

	ofxFTGLFont font;
	
protected:
	void	keyPressedEvent(ofKeyEventArgs &a);
	int		cursorx, cursory, startx;
};
