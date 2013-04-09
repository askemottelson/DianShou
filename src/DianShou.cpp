#include "DianShou.h"

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

	wstring charac;
	while(sel.hasNext()) {
		charId = sel.getInt();
		strokes = sel.getInt();
		charac = s2ws(sel.getString());
		allchars.push_back(charac);

		recognizer.addChar(std::to_wstring(charId), charac);
		ofxSQLiteSelect sel2 = sqlite->select("strokenumber, pointnumber, x, y").from("stroke").where("charId", charId).order("strokenumber, pointnumber", " ASC ");
		sel2.execute().begin();

		while(sel2.hasNext()) {
			strokenumber = sel2.getInt();
			pointnumber = sel2.getInt();
			x = sel2.getInt();
			y = sel2.getInt();
			
			if(oldstrokenumber != strokenumber){
				//recognizer.addLine();
			}

			recognizer.addPoint(x,y);
			oldstrokenumber = strokenumber;
			sel2.next();
		}

		recognizer.addLine();
		recognizer.addGesture(std::to_wstring(charId));

		sel.next();
	}
	
	recognizer.init();
}


void DianShou::selectChar(int fingers){
	vector<RecognitionResult> results = recognizer.returnResults();
	if(results.size() == 0) return;

	cout << "You have selected " << std::to_string(fingers) << endl;

	if(fingers == results.size()+2 || fingers == 5){ // new has been selected
		cout << "Add new gesture..." << endl;
			
		string newCharacter = ofSystemTextBoxDialog("Please type a character to identify gesture with");

		// cancel button pressed
		if(newCharacter.size() == 0){
			return;
		}

		wstring newCharacterW = s2ws(newCharacter);
		MultiStrokeGesture gesture = recognizer.MultiStrokes;

		// insert
		string sqlstr = std::string("INSERT INTO character (strokes, char) values(5, \"") + newCharacter + std::string("\");");
				
		const char *sql = sqlstr.c_str();
		sqlite->simpleQuery(sql);

		// lastInsertID
		int charId = sqlite->lastInsertID();

		// add the new gesture
		recognizer.reset_gestures();
		recognizer.addChar(std::to_wstring(charId), newCharacterW);
		recognizer.addGesture(std::to_wstring(charId));
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
	else{
		// 2 finges means first character, that has 0th place in vector
		wstring chosen = recognizer.getChar(results[fingers-2].name);
		
		//writingString += chosen;
		//textBox.setValue(writingString);
		textBox.setValue(L" ");
		recognizer.reset_results();

		// copy chosen character to clipboard and paste where cursor is
		v_keyboard.message(chosen);
	}
}

void DianShou::startRecognition(){
	recognizer.reset();

	for(int i = 0; i < circle.size(); i+=2){
		if(circle[i] == 0){
			//recognizer.addLine();
		}
		else{
			recognizer.addPoint(circle[i], circle[i+1]);
		}
	}

	recognizer.addLine();
	if(recognizer.valid()){
		
		wstring finalstring = L"";
		recognizer.recognize();

		vector<DollarRecognizer::RecognitionResult> results = recognizer.returnResults();

		cout << "Results... " << endl;

		int i;
		int j = 2;
		for(i = 0; i < results.size(); i++){
			if(j == 5) // we want to be able to do everything with one hand
				break;
				
			finalstring += std::to_wstring(j) + L" " + recognizer.getChar(results[i].name) + L"   ";
			j++;
		}

		finalstring += std::to_wstring(j) + L" ?";

		textBox.setValue(finalstring);
		readyToRecognize = false;

		// clear screen
		circle.erase(circle.begin(),circle.begin()+circle.size());
		circle.push_back(0);
		circle.push_back(0);
	}
}


void DianShou::processGestures(){
	// we dont want right and left clicks when we're drawing
	if(circle.size() > 2)
		return;

	Leap::Frame frame = controller.frame();

	Leap::GestureList gestures =  frame.gestures();
	int numGestures = gestures.count() - 1;

	if (gestures[numGestures].type() == Leap::Gesture::TYPE_SWIPE) {

		if(lastGesture == gestures[numGestures].id() )
			return;

		lastGesture = gestures[numGestures].id() ;

		Leap::SwipeGesture swipe = gestures[numGestures];
		int startX = swipe.startPosition().x;
		int endX = swipe.position().x;
		int startY = swipe.startPosition().y;
		int endY = swipe.position().y;
		int yDiff = startY - endY;
		if(yDiff < 100 && yDiff > -100){
			if(startX > endX){
				// go left
				v_keyboard.pressKey(VK_LEFT);
			}
			else{
				// go right
				v_keyboard.pressKey(VK_RIGHT);
			}
		}
		else{
			if(startY > endY){
				// go down
				v_keyboard.pressKey(VK_DOWN);
			}
			else{
				// go up
				v_keyboard.pressKey(VK_UP);
			}
		}
	}
	
}



// OPENFRAMEWORKS METHODS


void DianShou::setup(){
	// for nicer look - makes program slower
	ofEnableSmoothing();
	//ofEnableAlphaBlending();

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
	
	drawDB = 0;

	// Leap
	controller.addListener(listener);

	// Sqlite
	sqlite = new ofxSQLite("data/dianshou.db");
	
	/*for(int i = 0; i < 25; i++){
		sqlite->remove("character").where("id",i).execute();
		sqlite->remove("stroke").where("charId",i).execute();
	}*/

	initDB();
	
	// GUI
	textBox.init();

	readyToRecognize = true;
}

bool DianShou::ready(){
	return readyToRecognize && circle.size() > 2;
}

void DianShou::update(){

  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  
  // standard bg color
  bgcolor[0] = 220;
  bgcolor[1] = 220;
  bgcolor[2] = 220;

  lastfinger.push_back(frame.fingers().count());
  // clean a little bit when too much data
  if(lastfinger.size() > 100){
	  lastfinger.erase(lastfinger.begin(), lastfinger.end()-25);
  }

  // draw with one finger
  if(frame.fingers().count() == 1 || (frame.fingers().count() > 0 && circle.size() > 2)){
	readyToRecognize = true;
	const Leap::HandList& hands = frame.hands();
	const Leap::Hand& hand = hands[0];
	const Leap::FingerList& fingers = hand.fingers();
	const Leap::Finger& finger = fingers[0];

	Vector tipPosition = finger.tipPosition();
	
	int z = tipPosition.z;
	float x = (tipPosition.x * 2) + 300;
	float y = (-tipPosition.y * 2) + 800;

	// I'm not sure this ever happens but it's made
	// to avoid zero-data from Leap to force recognition while drawing 
	if(x == 300 && y == 800 && z == 0)
		return;
	
	cursor[0] = x;
	cursor[1] = y;
	
	if(z < 35 && z >= 0){
		bgcolor[0] = 255 - z;
		bgcolor[1] = 255 - z;
		bgcolor[2] = 255 - z;

		if(cursor[sizeof(cursor)-1] != 0){
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
	}
	
  }
  // recognize when no fingers around
  else if(frame.fingers().count() == 0){
	  
	  if(lastfinger.size() < 25)
		  return;
		
	  for(int i = lastfinger.size()-25; i < lastfinger.size(); i++){
		  if(lastfinger[i] != frame.fingers().count()){
			  return;
		  }
	  }
	  
	  if(circle[circle.size()-1] != 0){
		  circle.push_back(0);
		  circle.push_back(0);
	  }
	  
	  if(ready()){
		  lastfinger.erase(lastfinger.begin(), lastfinger.end());
		  startRecognition();
	  }
  }
  // select with 2 or more fingers
  else if(frame.fingers().count() >= 2){

	   if(lastfinger.size() < 25)
			return;
		
		for(int i = lastfinger.size()-25; i < lastfinger.size(); i++){
			if(lastfinger[i] != frame.fingers().count()){
				return;
			}
		}

	  // finally select the character corrosponding to number of fingers
	  selectChar(frame.fingers().count());
  }

  processGestures();
}


void DianShou::draw(){
	// set background
	ofBackground(bgcolor[0],bgcolor[1],bgcolor[2]);
	
	// this is a debug thing
	if(drawDB != 0){
		ofBackground(bgcolor[0],bgcolor[1],bgcolor[2]);
		ofSetColor(0,0,0);
	    ofFill();

		int x,y;
		int lastPx = 0;
		int lastPy = 0;
		
		for(int i = 0; i < recognizer.gm.allMtemplates[drawDB-49].paths[0].size(); i++){
			x = recognizer.gm.allMtemplates[drawDB-49].paths[0][i].x;
			y = recognizer.gm.allMtemplates[drawDB-49].paths[0][i].y;

			if(lastPx != 0){
				if(i == recognizer.gm.allMtemplates[drawDB-49].paths[0].size() - 1)
					ofLine(lastPx,lastPy,x,y);
			}
			lastPx = x;
			lastPy = y;
		}
		return;
	}

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

	// suggestions on screen
	textBox.draw();
}

void DianShou::keyPressed(int key){
	cout << "You pressed key: " << key << endl;
	drawDB = 0;

	if(key == 'q' || key == 'Q'){
		std::exit(0);
	}
	else if(key > 48 && key < 58){ // these keys represents 0-9
		// a debug function
		drawDB = key;
	}
}

void DianShou::keyReleased(int key){

}

void DianShou::mouseMoved(int x, int y ){

}

void DianShou::mouseDragged(int x, int y, int button){

}

void DianShou::mousePressed(int x, int y, int button){

}

void DianShou::mouseReleased(int x, int y, int button){

}

void DianShou::windowResized(int w, int h){

}

void DianShou::gotMessage(ofMessage msg){

}

void DianShou::dragEvent(ofDragInfo dragInfo){ 

}

