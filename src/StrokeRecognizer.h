#include "$1\lib\GeometricRecognizer.h"
#include "$1\lib\PathWriter.h"

#include <map> 
#include <iostream>

using namespace DollarRecognizer;

class StrokeRecognizer{

	public:

		StrokeRecognizer();
		virtual ~StrokeRecognizer();

		string recognize();
		void addPoint(int x, int y);
		void addLine();
		void reset();
		void reset_line();
		void addGesture(string name);
		bool valid();
		void addChar(string charId, string charac);
		string getChar(string charId);
		void init();
		MultiStrokeGesture mirror();
		MultiStrokeGesture MultiStrokes;
		void reset_gestures();

		

	private:
		Path2D line;
		utils util;
		vector<string> MgestureList;
		GeometricRecognizer gm;
		std::map<string,string> gestureMap;
		MultiStrokeGesture mirror(MultiStrokeGesture gesture);
		int maxXY(Path2D line, string xy);
		int minXY(Path2D line, string xy);
		
		
		
};
