#include "$1\lib\GeometricRecognizer.h"
#include "$1\lib\PathWriter.h"

#include <map> 
#include <string>
#include <iostream>
#include <vector>

using namespace DollarRecognizer;

class StrokeRecognizer{

    public:

        StrokeRecognizer();
        virtual ~StrokeRecognizer();

        void recognize();
        void addPoint(int x, int y);
        void addLine();
        void reset();
        void reset_line();
        void addGesture(wstring name);
        bool valid();
        void addChar(wstring charId, wstring charac);
        wstring getChar(wstring charId);
        void init();
        MultiStrokeGesture mirror(MultiStrokeGesture gesture);
        MultiStrokeGesture MultiStrokes;
        void reset_gestures();
        void reset_results();
        vector<RecognitionResult> returnResults();
        GeometricRecognizer gm;
        

    private:
        Path2D line;
        utils util;
        vector<wstring> MgestureList;
        std::map<wstring,wstring> gestureMap;
        
        int maxXY(Path2D line, string xy);
        int minXY(Path2D line, string xy);
        vector<RecognitionResult> results;
        
        
        
};
