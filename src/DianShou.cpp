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
	MultiStrokeGesture MultiStrokes;
	while(sel.hasNext()) {

		charId = sel.getInt();
		strokes = sel.getInt();
		charac = s2ws(sel.getString());
		allchars.push_back(charac);

		recognizer.addChar(std::to_wstring(charId), charac);
		// note: select them reversed
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

	// make sure that atleast 25 leap registrations says
	// same amount of finger, so that blinks of wrong number of fingers
	// are noget used.
	fingervector.push_back(fingers);
	if(fingervector.size() < 25)
		return;
		
	for(int i = 0; i < fingervector.size(); i++){
		if(fingervector[i] != fingers){
			fingervector.erase(fingervector.begin(), fingervector.end());
			return;
		}
	}

	cout << "You have selected " << std::to_string(fingers) << endl;

	cout << "results = " << results.size() << endl;
	if(fingers == results.size()+2 || fingers == 5){ // new has been selected
		cout << "Add new gesture..." << endl;
			
		string newCharacter = ofSystemTextBoxDialog("Please type a character to identify gesture with");

		if(newCharacter.size() == 0){
			cout << "cancel" << endl;
			return;
		}

		wstring newCharacterW = s2ws(newCharacter);
		MultiStrokeGesture gesture = recognizer.MultiStrokes;

		cout << " ... with " << gesture.size() << " strokes" << endl;

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
		
		writingString += chosen;
		textBox.setValue(writingString);
		recognizer.reset_results();
		fingervector.erase(fingervector.begin(), fingervector.end());

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
		hasRecognized = true;

		// clear screen
		circle.erase(circle.begin(),circle.begin()+circle.size());
		circle.push_back(0);
		circle.push_back(0);
	}
}




// OPENFRAMEWORKS METHODS


void DianShou::setup(){
	// for nicer look - makes program slower
	ofEnableSmoothing();
	ofEnableAlphaBlending();

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

	
	//sqlite->remove("character").where("id",23).execute();
	//sqlite->remove("stroke").where("charId",23).execute();

	/*for(int i = 0; i < 10; i++){
		sqlite->remove("character").where("id",15).execute();
		sqlite->remove("stroke").where("charId",15).execute();
	}*/

	/*sqlite->remove("character").where("id",9).execute();
	sqlite->remove("stroke").where("charId",9).execute();
	sqlite->remove("character").where("id",10).execute();
	sqlite->remove("stroke").where("charId",10).execute();
	sqlite->remove("character").where("id",11).execute();
	sqlite->remove("stroke").where("charId",11).execute();
	*/

	/*

	汉字子了
	sqlite->remove("character").where("id",6).execute();
	sqlite->remove("stroke").where("charId",6).execute();
	sqlite->remove("character").where("id",7).execute();
	sqlite->remove("stroke").where("charId",6).execute();*/

	initDB();
	
	// GUI
	textBox.init();


	//wstring allstring = L"";
	//for(int i = 0; i < allchars.size(); i++){
	//	allstring += allchars[i];
	//}

	//textBox.setValue( allstring );


	hasRecognized = false;
}

void DianShou::update(){

  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  
  // standard bg color
  bgcolor[0] = 220;
  bgcolor[1] = 220;
  bgcolor[2] = 220;

  // draw with one finger
  if(frame.fingers().count() == 1){
	hasRecognized = false;

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
  // select with more than one
  else if(frame.fingers().count() == 0){
	  if(circle[circle.size()-1] != 0){
		  circle.push_back(0);
		  circle.push_back(0);
	  }
	  
	  if(!hasRecognized)
		  startRecognition();
  }
  else if(frame.fingers().count() >= 2){

	  std::cout << " finger count " << std::to_string(frame.fingers().count()) << std::endl;
	  selectChar(frame.fingers().count());
  }
  
}


void DianShou::draw(){
	// set background
	ofBackground(bgcolor[0],bgcolor[1],bgcolor[2]);
	


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
					cout << "line x1 = " << lastPx << " y1 = " << lastPy << " x2 = " << x << " y2 = " << y << endl;
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
	else if(key == OF_KEY_BACKSPACE){
		if(writingString.length() == 0)
			return;

		writingString = writingString.substr(0, writingString.length()-1);
		textBox.setValue(writingString);
	}
	else if(key > 48 && key < 58){
		
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

