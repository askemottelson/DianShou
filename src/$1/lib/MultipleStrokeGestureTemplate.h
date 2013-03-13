#ifndef MULTISTROKEGESTURETEMPLATE_H
#define MULTISTROKEGESTURETEMPLATE_H
#include <string>

using namespace std;

namespace DollarRecognizer
{
class MultipleStrokeGestureTemplate
{
public:
        wstring name;
        MultiStrokeGesture paths;

        MultipleStrokeGestureTemplate(wstring name, MultiStrokeGesture paths)
        {
                this->name   = name;
                this->paths = paths;

        }
};

typedef vector<MultipleStrokeGestureTemplate> MultipleStrokeGestureTemplates;
}
#endif // MULTISTROKEGESTURETEMPLATE_H
