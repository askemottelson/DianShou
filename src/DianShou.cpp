#include "DianShou.h"

DianShou::DianShou(){

}

DianShou::~DianShou(){
    // Remove the sample listener when done
    controller.removeListener(listener);
}


/**
 * Initializes Database
 * Gets all the characters and points and stores them as gestures in 
 * recognizer
 */
void DianShou::initDB(){
    /// THIS INITS ALL THE GESTURES FROM DB
    ofxSQLiteSelect sel = sqlite->select("id, char").from("character");
    sel.execute().begin();

    // database fields
    int charId, pointnumber, x, y;
    wstring charac;
    
    // get all character entries
    while(sel.hasNext()) {
        charId = sel.getInt();
        charac = s2ws(sel.getString());
        allchars.push_back(charac);

        recognizer.addChar(std::to_wstring(charId), charac);
        ofxSQLiteSelect sel2 = sqlite->select("pointnumber, x, y").from("stroke").where("charId", charId).order("strokenumber, pointnumber", " ASC ");
        sel2.execute().begin();

        // get all points entries
        while(sel2.hasNext()){
            pointnumber = sel2.getInt();
            x = sel2.getInt();
            y = sel2.getInt();
            
            recognizer.addPoint(x,y);
            sel2.next();
        }
        recognizer.addLine();
        recognizer.addGesture(std::to_wstring(charId));

        sel.next();
    }
    recognizer.init();
}

/**
 * selectChar controls which action to be done when a recogntion has been made
 * @param	int fingers		number of fingers that has been recognized
 */
void DianShou::selectChar(int fingers){
    vector<RecognitionResult> results = recognizer.returnResults();
    
    // abort if there's no results
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

/**
 * startRecognition	starts the recognition when a character has been drawn
 */
void DianShou::startRecognition(){
    recognizer.reset();

    for(int i = 0; i < circle.size(); i+=3){
        if(circle[i] != 0)
            recognizer.addPoint(circle[i], circle[i+1]);
    }

    // add a line when all points are added
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
        circle.push_back(1.0f);

        // yellow
        helpColor[0] = 255;
        helpColor[1] = 255;
        helpColor[2] = 102;

        helpText = "Choose your prefered character by showing the number of fingers equal to your character's number";
    }
}

/**
 * processGestures is called by update, and checks if any gestures is done
 * if any gestures are recognized since last call, the prefered action is done
 * supports all arrows on a regular keyboard
 * Even though swipe comes as a vector, this function
 * is implemented in a way so that processGestures() at max
 * produces one arrow emulation click, to avoid pressing the arrow buttons down in infernity.
 */
void DianShou::processGestures(){
    // we dont want right and left clicks when we're drawing
    if(circle.size() > 3)
        return;

    // get frame and gestures
    Leap::Frame frame = controller.frame();
    Leap::GestureList gestures =  frame.gestures();
    
    int numGestures = gestures.count() - 1;

    // currently we only support SWIPE to emulate arrow clicks
    if (gestures[numGestures].type() == Leap::Gesture::TYPE_SWIPE) {

        // if it's the same gesture as last call, ignore it
        if(lastGesture == gestures[numGestures].id())
            return;

        lastGesture = gestures[numGestures].id();

        Leap::SwipeGesture swipe = gestures[numGestures];
        int startX = swipe.startPosition().x;
        int endX = swipe.position().x;
        int startY = swipe.startPosition().y;
        int endY = swipe.position().y;
        int yDiff = startY - endY;
        if(yDiff < 100 && yDiff > -100){
            if(startX > endX) // go left
                v_keyboard.pressKey(VK_LEFT);
            else // go right
                v_keyboard.pressKey(VK_RIGHT);
        }
        else{
            if(startY > endY) // go down
                v_keyboard.pressKey(VK_DOWN);
            else // go up
                v_keyboard.pressKey(VK_UP);
        }
    }
}

/*
 * lineWidth produces a line thickness for openframworks
 * depending on a z-value from the leap controller
 * @param int z		the z-value from the leap controller
 * @return float lineWidth	the lineWith as a floating point. a number between 1.0 - 11.0
 */
float DianShou::lineWidth(int z){
    float lineWidth = 1.0f;
    
    if(z_value < -100)
        lineWidth = 10.0f;
    else if(z_value < 0 && z_value > -100)
        lineWidth = 1.0f + ((float)-z_value / 10.0f);
    
    return lineWidth;
}


// OPENFRAMEWORKS METHODS

void DianShou::setup(){
    // for nicer look - makes program slower
    ofEnableSmoothing();
    ofSetFrameRate(48);
    ofSetWindowTitle("DianShou");

    //ofEnableAlphaBlending();

    
    // init gray
    bgcolor.push_back(220);
    bgcolor.push_back(220);
    bgcolor.push_back(220);

    // init red
    cursorcolor.push_back(255);
    cursorcolor.push_back(0);
    cursorcolor.push_back(0);

    // init cursor position
    cursor.push_back(-1);
    cursor.push_back(-1);

    // strokes
    circle.push_back(0);
    circle.push_back(0);
    circle.push_back(1.0f);

    // stroke thickness
    z_value = 0;

    // help
    enableHelp = false;

    helpColor.push_back(0);
    helpColor.push_back(0);
    helpColor.push_back(0);
    helpText = "Draw a character using a finger";
    helpColor[0] = 50;
    helpColor[1] = 205;
    helpColor[2] = 50;

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
    return readyToRecognize && circle.size() > 3;
}

void DianShou::update(){
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();

  lastfinger.push_back(frame.fingers().count());
  // clean a little bit when too much data
  if(lastfinger.size() > 100){
      lastfinger.erase(lastfinger.begin(), lastfinger.end()-25);
  }

  

  // draw with one finger
  if(frame.fingers().count() == 1 || (frame.fingers().count() > 0 && circle.size() > 3)){
    readyToRecognize = true;
    const Leap::HandList& hands = frame.hands();
    const Leap::Hand& hand = hands[0];
    const Leap::FingerList& fingers = hand.fingers();
    const Leap::Finger& finger = fingers[0];

    Vector tipPosition = finger.tipPosition();
    
    z_value = tipPosition.z;
    float x = (tipPosition.x * 2) + 300;
    float y = (-tipPosition.y * 2) + 800;

    float linewidth = lineWidth(z_value);

    // I'm not sure this ever happens but it's made
    // to avoid zero-data from Leap to force recognition while drawing 
    if(x == 300 && y == 800 && z_value == 0)
        return;
    
    cursor[0] = x;
    cursor[1] = y;
    
    if(z_value < 35 && z_value >= 0){
        bgcolor[0] = 255 - z_value;
        bgcolor[1] = 255 - z_value;
        bgcolor[2] = 255 - z_value;

        if(cursor[0] != 0){
            circle.push_back(0);
            circle.push_back(0);
            circle.push_back(1.0f);
        }
    }
    else if(z_value < 0){
        bgcolor[0] = 255;
        bgcolor[1] = 255;
        bgcolor[2] = 255;

        circle.push_back( x );
        circle.push_back( y );
        circle.push_back( lineWidth(z_value));
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
      
      if(circle[circle.size()-2] != 0){
          circle.push_back(0);
          circle.push_back(0);
          circle.push_back(1.0f);
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
      helpText = "Draw a character using a finger";
      helpColor[0] = 50;
      helpColor[1] = 205;
      helpColor[2] = 50;
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
    float thickness;

    ofSetColor(0,0,0);
    ofFill();

    int lastPx = 0;
    int lastPy = 0;

    for(int i = 0; i < (int)circle.size(); i+=3){
        x = circle[i];
        y = circle[i+1];
        thickness = circle[i+2];
        
        if(!(x == 0 && y == 0) && lastPx != 0){
            ofSetLineWidth(thickness);
            ofLine(lastPx,lastPy,x,y);
        }
        lastPx = x;
        lastPy = y;
    }

    if(enableHelp)
        drawHelp();

    // set cursor
    ofSetColor(255,0,0);
    ofFill();
    ofCircle(cursor[0], cursor[1], 4+lineWidth(z_value));

    // suggestions on screen
    textBox.drawRects();
}

void DianShou::drawHelp(){
    vector<string> arr = ofSplitString(helpText," ");
    
    string first, second;

    for(int i = 0; i < arr.size(); i++){
        if(i > 6){
            first += arr[i] + " ";
        }
        else{
            second += arr[i] + " ";
        }
    }

    if(arr.size() > 10){
        textBox.drawText(first, 20, 60, helpColor[0], helpColor[1], helpColor[2]);
        textBox.drawText(second, 20, 40, helpColor[0], helpColor[1], helpColor[2]);
    }
    else{
        textBox.drawText(helpText, 20, 40, helpColor[0], helpColor[1], helpColor[2]);
    }
}

void DianShou::keyPressed(int key){
    cout << "You pressed key: " << key << endl;
    drawDB = 0;

    if(key == 'q' || key == 'Q'){
        std::exit(0);
    }
    else if(key == 'h' || key == 'H'){
        enableHelp = !enableHelp;
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

