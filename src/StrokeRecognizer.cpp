#include "StrokeRecognizer.h"

StrokeRecognizer::StrokeRecognizer(){
    
}

StrokeRecognizer::~StrokeRecognizer(){


}

int StrokeRecognizer::maxXY(Path2D line, string xy){
    int max = INT_MIN;

    for(int i = 0; i < line.size(); i++){
        if(xy == "x"){
            if(line[i].x > max)
                max = line[i].x;
        }
        else{
            if(line[i].y > max)
                max = line[i].y;
        }
    }
    return max;
}

int StrokeRecognizer::minXY(Path2D line, string xy){
    int min = INT_MAX;

    for(int i = 0; i < line.size(); i++){
        if(xy == "x"){
            if(line[i].x < min)
                min = line[i].x;
        }
        else{
            if(line[i].y < min)
                min = line[i].y;
        }
    }
    return min;
}

// mirrors all lines (points) vertically and horizontally
MultiStrokeGesture StrokeRecognizer::mirror(MultiStrokeGesture gesture){
    MultiStrokeGesture result = MultiStrokes;
    
    for(int i = 0; i < result.size(); i++){ // lines Path2D
        int maxX = maxXY(result[i],"x");
        int maxY = maxXY(result[i],"y");
        int minX = minXY(result[i],"x");
        int minY = minXY(result[i],"y");
        
        for(int j = 0; j < result[i].size(); j++){ // points Point2D
            int newX = maxX - (result[i][j].x - minX);
            int newY = maxY - (result[i][j].y - minY);
            //int newX = result[i][j].x;
            
            //int newY = result[i][j].y;
            
            result[i][j].x = newX;
            result[i][j].y = newY;
        }
    }
    return result;
}

void StrokeRecognizer::init(){
    //gm.loadMultistrokeTemplates();
    gm.activateMultiStrokesTemplates(MgestureList);
    cout << "NO of gestures in DB : " << MgestureList.size() << endl;
    
}

void StrokeRecognizer::addChar(wstring charId, wstring charac){
    gestureMap[charId] = charac;
}

wstring StrokeRecognizer::getChar(wstring charId){
    return gestureMap[charId];
}

void StrokeRecognizer::reset(){
    MultiStrokes.erase(MultiStrokes.begin(),MultiStrokes.end());
}

void StrokeRecognizer::reset_line(){
    line.erase(line.begin(),line.begin()+line.size());
}

void StrokeRecognizer::reset_gestures(){
    gm.allMtemplates.erase(gm.allMtemplates.begin(), gm.allMtemplates.end());
}

void StrokeRecognizer::reset_results(){
    results.erase(results.begin(), results.end());
}

vector<RecognitionResult> StrokeRecognizer::returnResults(){
    vector<RecognitionResult> ret;
    for(int i = 0; i < results.size(); i++){
        if(results[i].score > 0.2)
            ret.push_back(results[i]);
    }

    return ret;
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


// FOR DATABASE input
void StrokeRecognizer::addGesture(wstring name){
    MultiStrokeGesture new_db_gesture = MultiStrokes;
    
    MgestureList.push_back(name);
    gm.addMultiStrokesTemplate(name, new_db_gesture);
    reset();
}


// start recognition

void StrokeRecognizer::recognize(){

    //multi-stroke gesture recognition

    MultiStrokeGesture cmp = MultiStrokes;
       
    cout << "NO of strokes: " << cmp.size() << endl;
    cout<< "Results" <<endl;
    gm.Multirecognize(cmp,"normal");
    
    results = gm.getResults();

    /*cout << "$N Recognized gesture: " << getChar(rpm.name) << endl;
    cout << "$N Score:" << rpm.score << endl;*/

    for(int i = 0; i < results.size(); i++){
        cout << "Gesture: " << (int) results[i].name[0] << " score: " << results[i].score << endl;
    }

    /*
    if(results[0].score < 0.8){
        return "0";
    }

    return getChar(results[0].name);
    */
}

