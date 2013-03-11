#pragma once

#include "ofMain.h"
#include "LeapListener.h"

#include "ofxFTGLFont.h"
#include "ofxSQLiteHeaders.h"
#include "StrokeRecognizer.h"
#include "TextInput/textInput.h"





using namespace Leap;

class DianShou : public ofBaseApp{

	public:

		DianShou();
		virtual ~DianShou();

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		// Create a sample listener and controller
		LeapListener listener;
		Controller controller;

		// global variables
		int *bgcolor;
		int *cursorcolor;
		int *cursor;

		std::vector<float> circle;

		// ftgl
		ofxFTGLFont font;
		string myString;

		// sqlite
		ofxSQLite* sqlite;
		

		// $1
		StrokeRecognizer recognizer;

		// GUI
		textInput textBox;

	private:
		void initDB();
	

		
	
};
