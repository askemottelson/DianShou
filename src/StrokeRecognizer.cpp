#include "StrokeRecognizer.h"

StrokeRecognizer::StrokeRecognizer(){
	
}

StrokeRecognizer::~StrokeRecognizer(){


}

void StrokeRecognizer::init(){
	gm.loadMultistrokeTemplates();
    gm.activateMultiStrokesTemplates(MgestureList);
	cout << "NO of gestures in DB : " << MgestureList.size() << endl;
	
}

void StrokeRecognizer::addChar(string charId, string charac){
	gestureMap[charId] = charac;
}

string StrokeRecognizer::getChar(string charId){
	return gestureMap[charId];
}

void StrokeRecognizer::reset(){
	MultiStrokes.erase(MultiStrokes.begin(),MultiStrokes.begin()+MultiStrokes.size());
}

void StrokeRecognizer::reset_line(){
	line.erase(line.begin(),line.begin()+line.size());
}

bool StrokeRecognizer::valid(){
	return MultiStrokes.size() > 0;
}

void StrokeRecognizer::addPoint(int x, int y){
	line.push_back(Point2D(x,y));
}

void StrokeRecognizer::addLine(){
	if(line.size() > 0){
		Path2D new_line = line;
		MultiStrokes.push_back(new_line);
		
		
		reset_line();
	}
}


// FOR DATABASE
void StrokeRecognizer::addGesture(string name){
	MultiStrokeGesture new_db_gesture = MultiStrokes;

	
	MgestureList.push_back(name);
	gm.addMultiStrokesTemplate(name, new_db_gesture);
	reset();
}


// start recognition

string StrokeRecognizer::recognize(){

	//
    //multi-stroke gesture recognition
    //
    
	gm.results.erase(gm.results.begin(),gm.results.begin()+gm.results.size());
    
	cout << "NO of strokes: " << MultiStrokes.size() << endl;
	cout<< "Results" <<endl;
    RecognitionResult rpm=gm.Multirecognize(MultiStrokes,"normal");
	
	
	/*cout << "$N Recognized gesture: " << getChar(rpm.name) << endl;
    cout << "$N Score:" << rpm.score << endl;*/

	for(int i = 0; i < gm.results.size(); i++){
		cout << "Gesture: " << getChar(gm.results[i].name) << " score: " << gm.results[i].score << endl;
	}

	return getChar(rpm.name);
}

