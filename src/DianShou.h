#pragma once

#include "ofMain.h"
#include "LeapListener.h"
#include "StrokeRecognizer.h"
#include "VirtualKeyboard.h"
#include "TextInput/textInput.h"

#include "ofxFTGLFont.h"
#include "ofxSQLiteHeaders.h"

#include <vector>


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

		
		// global variables
		int *bgcolor;
		int *cursorcolor;
		int *cursor;
		std::vector<float> circle;

		// sqlite
		ofxSQLite* sqlite;

		// $N
		StrokeRecognizer recognizer;

		// GUI
		textInput textBox;

	private:
		int drawDB;
		void initDB();
		wstring writingString;
		void selectChar(int fingers);
		vector<int> fingervector;
		VirtualKeyboard v_keyboard;
		void startRecognition();
		bool readyToRecognize;
		vector<wstring> allchars;
		void processGestures();
		
		// Create a sample listener and controller
		LeapListener listener;
		Leap::Controller controller;
		int32_t lastGesture;

		// string to wstring
		std::wstring s2ws(const std::string& s)
		{
			int len;
			int slength = (int)s.length() + 1;
			len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, 0, 0); 
			wchar_t* buf = new wchar_t[len];
			MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, buf, len);
			std::wstring r(buf);
			delete[] buf;
			return r;
		};
};