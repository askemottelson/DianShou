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


void DianShou::initDB(){
	/// THIS INITS ALL THE GESTURES FROM DB
	ofxSQLiteSelect sel = sqlite->select("id, strokes, char").from("character");
	sel.execute().begin();

	int charId, strokes, strokenumber, pointnumber, x, y;
	int oldstrokenumber = 0;

	string charac;
	MultiStrokeGesture MultiStrokes;
	while(sel.hasNext()) {

		charId = sel.getInt();
		strokes = sel.getInt();
		charac = sel.getString();

		recognizer.addChar(std::to_string(charId), charac);
		// note: select them reversed
		ofxSQLiteSelect sel2 = sqlite->select("strokenumber, pointnumber, x, y").from("stroke").where("charId", charId).order("strokenumber, pointnumber", " ASC ");
		sel2.execute().begin();

		while(sel2.hasNext()) {
			strokenumber = sel2.getInt();
			pointnumber = sel2.getInt();
			x = sel2.getInt();
			y = sel2.getInt();
			
			if(oldstrokenumber != strokenumber){
				recognizer.addLine();
			}

			recognizer.addPoint(x,y);
			
			oldstrokenumber = strokenumber;
			sel2.next();
		}
		recognizer.addLine();
		recognizer.addGesture(std::to_string(charId));

		sel.next();
	}
	recognizer.init();
}


void DianShou::setup(){
	std::cout << "setup();" << std::endl;

	ofEnableSmoothing();
	//ofEnableAlphaBlending();

	// program data

	//ofSetFrameRate(48);

	bgcolor = new int[2];
	// init gray
	bgcolor[0] = 100;
	bgcolor[1] = 100;
	bgcolor[2] = 100;

	cursorcolor = new int[2];
	// init red
	cursorcolor[0] = 255;
	cursorcolor[1] = 0;
	cursorcolor[2] = 0;

	cursor = new int[1];
	cursor[0] = -1;
	cursor[1] = -1;

	circle.push_back(0);
	circle.push_back(0);

	// Leap
	controller.addListener(listener);

	// FTGL
	font.loadFont("simhei.ttf", 20.0f, true,true);

	// Sqlite
	sqlite = new ofxSQLite("data/dianshou.db");
	

	/*sqlite->insert("character")
		.use("strokes", 5)
		.use("char", "你"
	).execute();*/
	
	
	//sqlite->remove("character").where("id",4).execute();
	//sqlite->remove("stroke").where("charId",4).execute();
	//
	//sqlite->remove("character").where("id",5).execute();
	//sqlite->remove("stroke").where("charId",5).execute();

	//sqlite->remove("character").where("id",6).execute();
	//sqlite->remove("stroke").where("charId",6).execute();

	//sqlite->remove("character").where("id",7).execute();
	//sqlite->remove("stroke").where("charId",7).execute();

	//sqlite->remove("character").where("id",8).execute();
	//sqlite->remove("stroke").where("charId",8).execute();

	//sqlite->remove("character").where("id",9).execute();
	//sqlite->remove("stroke").where("charId",9).execute();

	//sqlite->remove("character").where("id",10).execute();
	//sqlite->remove("stroke").where("charId",10).execute();
	

	initDB();
	
	// GUI
	textBox.init();
	

}

//--------------------------------------------------------------
void DianShou::update(){

  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  
  // standard bg color
  bgcolor[0] = 220;
  bgcolor[1] = 220;
  bgcolor[2] = 220;

  if(frame.fingers().count() != 0){
	
	const Leap::HandList& hands = frame.hands();
	const Leap::Hand& hand = hands[0];
	const Leap::FingerList& fingers = hand.fingers();
	const Leap::Finger& finger = fingers[0];

	Vector tipPosition = finger.tipPosition();
	
	int z = tipPosition.z;
	float x = (tipPosition.x * 2) + 300;
	float y = (-tipPosition.y * 2) + 800;
	
	bool evil = (x == 300 && y == 800 && z == 0);

	if(!evil){
		cursor[0] = x;
		cursor[1] = y;
	}

	if(z < 35 && z >= 0){
		if(!evil){
			bgcolor[0] = 255 - z;
			bgcolor[1] = 255 - z;
			bgcolor[2] = 255 - z;

			if(cursor[sizeof(cursor)-1] != 0){
			  circle.push_back(0);
			  circle.push_back(0);
	
			}
		}
	}
	else if(z < 0){
		bgcolor[0] = 255;
		bgcolor[1] = 255;
		bgcolor[2] = 255;

		circle.push_back( x );
		circle.push_back( y );
	}
	
	std::cout << " x: " << cursor[0] << " y: " << cursor[1] << " z: " << " bgcolor: " << bgcolor[0] << std::endl;
  }
  
  else{
	  if(circle[circle.size()-1] != 0){
		  circle.push_back(0);
		  circle.push_back(0);
	  }
  }
  
}

//--------------------------------------------------------------
void DianShou::draw(){
	
	ofBackground(bgcolor[0],bgcolor[1],bgcolor[2]);

	// draw line
	int x,y;

	ofSetColor(0,0,0);
    ofFill();

	int lastPx = 0;
	int lastPy = 0;

	for(int i = 0; i < (int)circle.size(); i+=2){
		x = circle[i];
		y = circle[i+1];
	
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

	// FTGL-wrapper
	
	//ofSetColor(0);
	//font.drawString("Øøøøh", 10, 10);
	
	
	
	// GUI
	
	textBox.draw();

}

//--------------------------------------------------------------
void DianShou::keyPressed(int key){
	
	std::cout << "key pressed " << key << std::endl;
	

	if(key == 'c' || key == 'C'){

		
	}
	else if(key == 32){ // space
		recognizer.reset();

		for(int i = 0; i < circle.size(); i+=2){
			if(circle[i] == 0){
				recognizer.addLine();
			}
			else{
				recognizer.addPoint(circle[i], circle[i+1]);
			}
		}

		if(recognizer.valid()){
			string result = recognizer.recognize();

			if(result == "0"){
				cout << "Score is low - lets add the new gesture to DB" << endl;
						
				string newCharacter = ofSystemTextBoxDialog("Please type a character to identify gesture with", "");
				result = newCharacter;

				if(newCharacter.size() == 0){
					cout << "cancel" << endl;
					return;
				}

				//MultiStrokeGesture gesture = recognizer.mirror();
				MultiStrokeGesture gesture = recognizer.MultiStrokes;
				cout << " mirrored gesture " << gesture.size() << endl;

				// insert

				string sqlstr = std::string("INSERT INTO character (strokes, char) values(5, \"") + newCharacter + std::string("\");");
				cout << "SQL " << sqlstr << endl;
				
				const char * sql = sqlstr.c_str();

				sqlite->simpleQuery(sql);

				/*sqlite->insert("character")
					.use("strokes", (int)gesture.size())
					.use("char",newCharacter)
				.execute();*/

				// lastInsertID
				int charId = sqlite->lastInsertID();

				// add the new gesture
				recognizer.reset_gestures();
				recognizer.addChar(std::to_string(charId), newCharacter);
				recognizer.addGesture(std::to_string(charId));
				recognizer.init();

				// use transactions for a lot of data
				if (SQLITE_OK != sqlite->simpleQuery("BEGIN TRANSACTION;")) {
					cout << "ERROR: cannot begin transaction" << std::endl;
				}


				for(int i = 0; i < gesture.size(); i++){ // lines Path2D
					for(int j = 0; j < gesture[i].size(); j++){ // points Point2D
						int x = gesture[i][j].x;
						int y = gesture[i][j].y;

						sqlite->insert("stroke")
							.use("charId", charId)
							.use("strokenumber", i)
							.use("pointnumber", j)
							.use("x", x)
							.use("y", y)
						.execute();
					}
				}
				
				if (SQLITE_OK != sqlite->simpleQuery("COMMIT;")) {
					cout << "ERROR: cannot commit" << std::endl;
				}
			}

			textBox.insertChar(result);

		}
	}
	else if(key == 'q' || key == 'Q'){
		std::exit(0);
	}
	
	
		// clear screen
		circle.erase(circle.begin(),circle.begin()+circle.size());
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

